/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include <iostream>
#include <iterator>
#include <string>

#include "zmq_addon.hpp"
#include "../common/xmiddleware_impl.hpp"
#include "xheartbeat.hpp"

namespace xeus
{
    xheartbeat::xheartbeat(zmq::context_t& context,
                           const xkernel_configuration& config)
        : m_heartbeat(context, zmq::socket_type::router)
        , m_controller(context, zmq::socket_type::rep)
    {
        if (config.m_curve_public_key != "")
        {
            set_curve_encryption(m_heartbeat, config.m_curve_public_key, config.m_curve_secret_key);
        }
        init_socket(m_heartbeat, config.m_transport, config.m_ip, config.m_hb_port);
        init_socket(m_controller, get_controller_end_point("heartbeat"));
    }

    xheartbeat::~xheartbeat()
    {
    }

    std::string xheartbeat::get_port() const
    {
        return get_socket_port(m_heartbeat);
    }

    void xheartbeat::run()
    {
        zmq::pollitem_t items[] = {
            { m_heartbeat, 0, ZMQ_POLLIN, 0 },
            { m_controller, 0, ZMQ_POLLIN, 0 }
        };

        while (true)
        {
            try
            {
                zmq::poll(&items[0], 2, std::chrono::milliseconds(-1));
            }
            catch (const zmq::error_t& e)
            {
                std::cerr << "Heartbeat: " << e.what() << std::endl;
                if (e.num() == EINTR) continue;
                if (e.num() == ETERM) break;
                throw e;
            }

            if (items[0].revents & ZMQ_POLLIN)
            {
                zmq::multipart_t wire_msg;
                wire_msg.recv(m_heartbeat);
                wire_msg.send(m_heartbeat);
            }

            if (items[1].revents & ZMQ_POLLIN)
            {
                // stop message
                zmq::multipart_t wire_msg;
                wire_msg.recv(m_controller);
                wire_msg.send(m_controller);
                break;
            }
        }
    }
}
