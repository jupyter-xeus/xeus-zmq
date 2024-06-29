/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include <iostream>

#include "xheartbeat_client.hpp"
#include "xclient_zmq_impl.hpp"
#include "../common/xmiddleware_impl.hpp"

namespace xeus
{

    xheartbeat_client::xheartbeat_client(zmq::context_t& context,
                                 const xeus::xconfiguration& config,
                                 const std::size_t max_retry,
                                 const long timeout)
        : m_heartbeat(context, zmq::socket_type::req)
        , m_controller(context, zmq::socket_type::rep)
        , m_max_retry(max_retry)
        , m_heartbeat_timeout(timeout)
        , m_heartbeat_end_point("")
        , m_request_stop(false)
    {
        m_heartbeat_end_point = get_end_point(config.m_transport, config.m_ip, config.m_hb_port);
        m_heartbeat.connect(m_heartbeat_end_point);
        init_socket(m_controller, get_controller_end_point("heartbeat"));
    }

    xheartbeat_client::~xheartbeat_client()
    {
        m_heartbeat.disconnect(m_heartbeat_end_point);
    }

    void xheartbeat_client::send_heartbeat_message()
    {
        zmq::message_t ping_msg("ping", 4);
        m_heartbeat.send(ping_msg, zmq::send_flags::none);
    }

    bool xheartbeat_client::wait_for_answer(long timeout)
    {
        zmq::pollitem_t items[] = {
            { m_heartbeat, 0, ZMQ_POLLIN, 0 }, { m_controller, 0, ZMQ_POLLIN, 0 }
        };

        zmq::poll(&items[0], 2, std::chrono::milliseconds(timeout));
        try
        {
            if (items[0].revents & ZMQ_POLLIN)
            {
                zmq::multipart_t wire_msg;
                wire_msg.recv(m_heartbeat);
            }

            if (items[1].revents & ZMQ_POLLIN)
            {
                // stop message
                zmq::multipart_t wire_msg;
                wire_msg.recv(m_controller);
                wire_msg.send(m_controller);
                m_request_stop = true;
            }

            return true;
        }
        catch (std::exception& e)
        {
            std::cerr << e.what() << std::endl;
        }
        return false;
    }

    void xheartbeat_client::register_kernel_status_listener(const kernel_status_listener& l)
    {
        m_kernel_status_listener = l;
    }

    void xheartbeat_client::notify_kernel_dead(bool status)
    {
        m_kernel_status_listener(status);
    }

    void xheartbeat_client::run()
    {
        std::size_t retry_count = 0;

        while(!m_request_stop)
        {
            send_heartbeat_message();
            if(!wait_for_answer(m_heartbeat_timeout))
            {
                if (retry_count < m_max_retry)
                {
                    ++retry_count;
                }
                else
                {
                    notify_kernel_dead(true);
                    break;
                }
            }
            else
            {
                retry_count = 0;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
}
