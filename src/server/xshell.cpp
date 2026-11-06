/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include <chrono>
#include <iostream>
#include <thread>

#include "xserver_zmq_split_impl.hpp"
#include "xshell.hpp"
#include "../common/xmiddleware_impl.hpp"

namespace xeus
{
    xshell::xshell(zmq::context_t& context,
                   const xkernel_configuration& config,
                   xserver_zmq_split_impl* server)
        : m_shell(context, zmq::socket_type::router)
        , m_stdin(context, zmq::socket_type::router)
        , m_publisher_pub(context, zmq::socket_type::pub)
        , m_controller(context, zmq::socket_type::rep)
        , p_server(server)
    {
        if (config.m_curve_public_key != "")
        {
            set_curve_encryption(m_shell, config.m_curve_public_key, config.m_curve_secret_key);
            set_curve_encryption(m_stdin, config.m_curve_public_key, config.m_curve_secret_key);
        }
        init_socket(m_shell, config.m_transport, config.m_ip, config.m_shell_port);
        init_socket(m_stdin, config.m_transport, config.m_ip, config.m_stdin_port);

        m_publisher_pub.set(zmq::sockopt::linger, get_socket_linger());
        m_publisher_pub.connect(get_publisher_end_point());
        
        m_controller.set(zmq::sockopt::linger, get_socket_linger());
        m_controller.bind(get_controller_end_point("shell"));
    }

    std::string xshell::get_shell_port() const
    {
        return get_socket_port(m_shell);
    }

    std::string xshell::get_stdin_port() const
    {
        return get_socket_port(m_stdin);
    }

    fd_t xshell::get_shell_fd() const
    {
        return m_shell.get(zmq::sockopt::fd);
    }

    fd_t xshell::get_controller_fd() const
    {
        return m_controller.get(zmq::sockopt::fd);
    }
    
    std::optional<channel> xshell::poll_channels(long timeout)
    {
        zmq::pollitem_t items[] = {
            { m_shell, 0, ZMQ_POLLIN, 0 },
            { m_controller, 0, ZMQ_POLLIN, 0 }
        };

        try
        {
            zmq::poll(&items[0], 2, std::chrono::milliseconds(timeout));

            if (items[0].revents & ZMQ_POLLIN)
            {
                return channel::SHELL;
            }

            if (items[1].revents & ZMQ_POLLIN)
            {
                return channel::CONTROL;
            }
        }
        catch (const zmq::error_t& e)
        {
            std::cerr << "Shell: " << e.what() << std::endl;
            if (e.num() != EINTR && e.num() != ETERM)
                throw e;
        }

        return std::nullopt;
    }

    std::optional<xmessage> xshell::read_shell(int flags)
    {
        zmq::multipart_t wire_msg;
        if (wire_msg.recv(m_shell, flags))
        {
            return p_server->deserialize(wire_msg);
        }
        return std::nullopt;
    }

    std::optional<std::string> xshell::read_controller(int flags)
    {
        zmq::multipart_t wire_msg;
        if (wire_msg.recv(m_controller, flags))
        {
            return wire_msg.popstr();
        }
        return std::nullopt;
    }

    void xshell::send_shell(zmq::multipart_t& message)
    {
        message.send(m_shell);
    }

    std::optional<xmessage> xshell::send_stdin(zmq::multipart_t& message)
    {
        message.send(m_stdin);
        zmq::multipart_t wire_msg;
        wire_msg.recv(m_stdin);
        return p_server->deserialize(wire_msg);
    }

    void xshell::send_controller(std::string message)
    {
        zmq::multipart_t wire_msg(std::move(message));
        wire_msg.send(m_controller);
    }

    void xshell::publish(zmq::multipart_t& message)
    {
        message.send(m_publisher_pub);
    }

    void xshell::abort_queue(const listener& l, long polling_interval)
    {
        while (true)
        {
            zmq::multipart_t wire_msg;
            bool received = wire_msg.recv(m_shell, ZMQ_NOBLOCK);
            if (!received)
            {
                return;
            }

            auto msg = p_server->deserialize(wire_msg);
            if (msg)
            {
                l(std::move(msg.value()));
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(polling_interval));
        }
    }
}

