/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include <thread>
#include <chrono>
#include <iostream>

#include "xeus-zmq/xmiddleware.hpp"
#include "xeus-zmq/xserver_uv_shell_main.hpp"
#include "xcontrol_uv.hpp"

namespace xeus
{
    xcontrol_uv::xcontrol_uv(zmq::context_t& context,
                       const std::string& transport,
                       const std::string& ip,
                       const std::string& control_port,
                       xserver_uv_shell_main* server)
        : m_control(context, zmq::socket_type::router)
        , m_publisher_pub(context, zmq::socket_type::pub)
        , m_messenger(context)
        , p_server(server)
        , m_request_stop(false)
    {
        init_socket(m_control, transport, ip, control_port);
        m_publisher_pub.set(zmq::sockopt::linger, get_socket_linger());
        m_publisher_pub.connect(get_publisher_end_point());
    }

    xcontrol_uv::~xcontrol_uv()
    {
    }

    std::string xcontrol_uv::get_port() const
    {
        return get_socket_port(m_control);
    }

    void xcontrol_uv::connect_messenger()
    {
        m_messenger.connect();
    }

    xcontrol_messenger& xcontrol_uv::get_messenger()
    {
        return m_messenger;
    }

    void xcontrol_uv::run()
    {
        m_request_stop = false;

        while (!m_request_stop)
        {
            zmq::multipart_t wire_msg;
            wire_msg.recv(m_control);
            try
            {
                xmessage msg = p_server->deserialize(wire_msg);
                p_server->notify_control_listener(std::move(msg));
            }
            catch (std::exception& e)
            {
                std::cerr << e.what() << std::endl;
            }
        }

        m_messenger.stop_channels();
        p_server->notify_control_stopped();
    }

    void xcontrol_uv::stop()
    {
        m_request_stop = true;
    }

    void xcontrol_uv::send_control(zmq::multipart_t& message)
    {
        message.send(m_control);
    }

    void xcontrol_uv::publish(zmq::multipart_t& message)
    {
        message.send(m_publisher_pub);
    }
}

