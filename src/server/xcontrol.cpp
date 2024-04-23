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

#include "xcontrol.hpp"
#include "xserver_zmq_split_impl.hpp"
#include "../common/xmiddleware_impl.hpp"

namespace xeus
{
    xcontrol::xcontrol(zmq::context_t& context,
                       const std::string& transport,
                       const std::string& ip,
                       const std::string& control_port,
                       xserver_zmq_split_impl* server)
        : m_control(context, zmq::socket_type::router)
        , m_publisher_pub(context, zmq::socket_type::pub)
        , m_messenger(context)
        , p_server(server)
    {
        init_socket(m_control, transport, ip, control_port);
        m_publisher_pub.set(zmq::sockopt::linger, get_socket_linger());
        m_publisher_pub.connect(get_publisher_end_point());
    }

    std::string xcontrol::get_port() const
    {
        return get_socket_port(m_control);
    }

    fd_t xcontrol::get_fd() const
    {
        return m_control.get(zmq::sockopt::fd);
    }

    void xcontrol::connect_messenger()
    {
        m_messenger.connect();
    }

    void xcontrol::stop_channels()
    {
        m_messenger.stop_channels();
    }

    xcontrol_messenger& xcontrol::get_messenger()
    {
        return m_messenger;
    }

    std::optional<xmessage> xcontrol::read_control(int flags)
    {
        zmq::multipart_t wire_msg;
        if (wire_msg.recv(m_control, flags))
        {
            try
            {
                return p_server->deserialize(wire_msg);
            }
            catch (std::exception& e)
            {
                std::cerr << e.what() << std::endl;
            }
        }
        return std::nullopt;
    }

    void xcontrol::send_control(zmq::multipart_t& message)
    {
        message.send(m_control);
    }

    void xcontrol::publish(zmq::multipart_t& message)
    {
        message.send(m_publisher_pub);
    }
}

