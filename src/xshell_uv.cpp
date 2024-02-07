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

#define UVW_AS_LIB
#include <uvw.hpp>

#include "xeus-zmq/xmiddleware.hpp"
#include "xeus-zmq/xserver_uv_shell_main.hpp"
#include "xshell_uv.hpp"

namespace xeus
{
    xshell_uv::xshell_uv(zmq::context_t& context,
                   const std::string& transport,
                   const std::string& ip,
                   const std::string& shell_port,
                   const std::string& stdin_port,
                   xserver_uv_shell_main* server)
        : m_shell(context, zmq::socket_type::router)
        , m_stdin(context, zmq::socket_type::router)
        , m_publisher_pub(context, zmq::socket_type::pub)
        , m_controller(context, zmq::socket_type::rep)
        , p_server(server)
    {
        init_socket(m_shell, transport, ip, shell_port);
        init_socket(m_stdin, transport, ip, stdin_port);
        m_publisher_pub.set(zmq::sockopt::linger, get_socket_linger());
        m_publisher_pub.connect(get_publisher_end_point());

        m_controller.set(zmq::sockopt::linger, get_socket_linger());
        m_controller.bind(get_controller_end_point("shell"));
    }

    xshell_uv::~xshell_uv()
    {
    }

    std::string xshell_uv::get_shell_port() const
    {
        return get_socket_port(m_shell);
    }

    std::string xshell_uv::get_stdin_port() const
    {
        return get_socket_port(m_stdin);
    }

    void xshell_uv::run()
    {
        // Initialize the default loop
        auto loop = uvw::loop::get_default();

        using poll_h = uvw::poll_handle;

        // m_shell and m_controller are zmq sockets

        // Create a resource and bind it to the loop
        std::shared_ptr<poll_h> shell_resource = loop->resource<poll_h>();
        std::shared_ptr<poll_h> control_resource = loop->resource<poll_h>();

        // Resources are event emitters to which listeners are attached
        shell_resource->on<uvw::error_event>(
            [](const uvw::error_event&, poll_h&)
            {
                // TODO: handle errors
                std::cerr << "Something wrong.\n";
            }
        );

        shell_resource->on<uvw::listen_event>(
            [this](const uvw::listen_event& /*event*/, poll_h&)
            {
                zmq::multipart_t wire_msg;
                wire_msg.recv(m_shell);
                try
                {
                    xmessage msg = p_server->deserialize(wire_msg);
                    p_server->notify_shell_listener(std::move(msg));
                }
                catch(std::exception& e)
                {
                    std::cerr << e.what() << std::endl;
                }
            }
        );

        control_resource->on<uvw::listen_event>(
            [this](const uvw::listen_event&, poll_h&)
            {
                // stop message
                zmq::multipart_t wire_msg;
                wire_msg.recv(m_controller);
                std::string msg { wire_msg.peekstr(0) };
                if(msg == "stop")
                {
                    // TODO: close handles
                    wire_msg.send(m_controller);
                }
                else
                {
                    zmq::multipart_t wire_reply = p_server->notify_internal_listener(wire_msg);
                    wire_reply.send(m_controller);
                }
            }
        );

        // TODO: connect the server to the client?
        shell_resource->bind(get_shell_port(), 4242);
        shell_resource->listen();
        // control_resource->listen();

        // loop->run();
        loop->run(uvw::loop::run_mode::DEFAULT); // ONCE, NOWAIT
    }

    void xshell_uv::send_shell(zmq::multipart_t& message)
    {
        message.send(m_shell);
    }

    void xshell_uv::send_stdin(zmq::multipart_t& message)
    {
        message.send(m_stdin);
        zmq::multipart_t wire_msg;
        wire_msg.recv(m_stdin);
        try
        {
            xmessage msg = p_server->deserialize(wire_msg);
            p_server->notify_stdin_listener(std::move(msg));
        }
        catch (std::exception& e)
        {
            std::cerr << e.what() << std::endl;
        }
    }

    void xshell_uv::publish(zmq::multipart_t& message)
    {
        message.send(m_publisher_pub);
    }

    void xshell_uv::abort_queue(const listener& l, long polling_interval)
    {
        while (true)
        {
            zmq::multipart_t wire_msg;
            bool received = wire_msg.recv(m_shell, ZMQ_NOBLOCK);
            if (!received)
            {
                return;
            }

            try
            {
                xmessage msg = p_server->deserialize(wire_msg);
                l(std::move(msg));
            }
            catch (std::exception& e)
            {
                std::cerr << e.what() << std::endl;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(polling_interval));
        }
    }

    void xshell_uv::reply_to_controller(zmq::multipart_t& message)
    {
        message.send(m_controller);
    }
}

