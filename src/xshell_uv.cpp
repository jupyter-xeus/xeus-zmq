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
#include "xserver_uv_shell_main.hpp"
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
        std::shared_ptr<uvw::loop> loop = uvw::loop::get_default();

        // Get the file descriptor for the shell and controller sockets
        zmq::fd_t shell_fd = m_shell.get(zmq::sockopt::fd);
        zmq::fd_t controller_fd = m_controller.get(zmq::sockopt::fd);

        // Create (libuv) poll handles and bind them to the loop
        auto shell_poll = loop->resource<uvw::poll_handle>(shell_fd);
        auto controller_poll = loop->resource<uvw::poll_handle>(controller_fd);

        // Register callbacks
        shell_poll->on<uvw::poll_event>(
            [this](uvw::poll_event&, uvw::poll_handle&)
            {
                std::cout << "[SHELL] inside shell poll\n"; // REMOVE
                zmq::multipart_t wire_msg;
                if (wire_msg.recv(m_shell, ZMQ_DONTWAIT)) // non-blocking
                {
                    try
                    {
                        std::cout << "\t[SHELL] msgsg\n";
                        xmessage msg = p_server->deserialize(wire_msg);
                        std::cout << "\tMessage content " << msg.content() << '\n';
                        p_server->notify_shell_listener(std::move(msg));
                        std::cout << "\t[SHELL] msgsg end\n";
                    }
                    catch(std::exception& e)
                    {
                        std::cout << "\t[SHELL ERROR] " << e.what() << std::endl;
                    }
                    catch(...)
                    {
                        std::cout << "\t[SHELL ERROR] Unknown exception\n";
                    }
                }
                std::cout << "[SHELL] poll end\n"; // REMOVE
            }
        );

        controller_poll->on<uvw::poll_event>(
            [this, &loop](uvw::poll_event&, uvw::poll_handle&)
            {
                std::cout << "[CONTROL] inside controller poll\n"; // REMOVE
                zmq::multipart_t wire_msg;
                if (wire_msg.recv(m_controller, ZMQ_DONTWAIT))
                {
                    std::cout << "[CONTROL] inside controller if\n"; // REMOVE
                    std::string msg = wire_msg.peekstr(0);
                    if(msg == "stop")
                    {
                        std::cout << "[CONTROL] stopping\n"; // REMOVE
                        wire_msg.send(m_controller);
                        loop->stop();
                    }
                    else
                    {
                        std::cout << "[CONTROL] not stopping\n"; // REMOVE
                        zmq::multipart_t wire_reply = p_server->notify_internal_listener(wire_msg);
                        wire_reply.send(m_controller);
                    }
                }
            }
        );

        // Resources are event emitters to which listeners are attached
        shell_poll->on<uvw::error_event>(
            [](const uvw::error_event&, uvw::poll_handle&)
            {
                // TODO: handle errors
                std::cout << "Something wrong with the shell.\n";
            }
        );

        controller_poll->on<uvw::error_event>(
            [](const uvw::error_event&, uvw::poll_handle&)
            {
                std::cerr << "Something wrong with the controller.\n";
            }
        );

        // Start the polls
        shell_poll->start(uvw::poll_handle::poll_event_flags::READABLE);
        controller_poll->start(uvw::poll_handle::poll_event_flags::READABLE);

        std::cout << "After starting polls\n"; // REMOVE

        loop->run();

        // TODO: close resources ??
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

