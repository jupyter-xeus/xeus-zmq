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
#include <memory> // std::shared_ptr

#ifndef UVW_AS_LIB
#define UVW_AS_LIB
#include <uvw.hpp>
#endif

#include "xeus-zmq/xmiddleware.hpp"
#include "xserver_uv_shell_main.hpp"
#include "xshell_uv.hpp"

// namespace py = pybind11;

namespace xeus
{
    xshell_uv::xshell_uv(std::shared_ptr<uvw::loop> loop_ptr,
                        zmq::context_t& context,
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
        , p_loop(loop_ptr)
    {
        init_socket(m_shell, transport, ip, shell_port);
        init_socket(m_stdin, transport, ip, stdin_port);
        m_publisher_pub.set(zmq::sockopt::linger, get_socket_linger());
        m_publisher_pub.connect(get_publisher_end_point());

        m_controller.set(zmq::sockopt::linger, get_socket_linger());
        m_controller.bind(get_controller_end_point("shell"));
        create_polls();
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

    void xshell_uv::create_polls()
    {
        if (!p_loop)
            throw std::runtime_error("No loop provided");

        // Get the file descriptor for the shell and controller sockets
        zmq::fd_t shell_fd = m_shell.get(zmq::sockopt::fd);
        zmq::fd_t controller_fd = m_controller.get(zmq::sockopt::fd);

        // Create (libuv) poll handles and bind them to the loop
        p_shell_poll = p_loop->resource<uvw::poll_handle>(shell_fd);
        p_controller_poll = p_loop->resource<uvw::poll_handle>(controller_fd);

        // Register callbacks
        p_shell_poll->on<uvw::poll_event>(
            [this](uvw::poll_event&, uvw::poll_handle&)
            {
                std::cout << "Shell poll event\t"; // REMOVE
                zmq::multipart_t wire_msg;
                if (wire_msg.recv(m_shell, ZMQ_DONTWAIT)) // non-blocking
                {
                    xmessage msg = p_server->deserialize(wire_msg);
                    p_server->notify_shell_listener(std::move(msg));
                }
                std::cout << "Done\n"; // REMOVE
            }
        );

        p_controller_poll->on<uvw::poll_event>(
            [this](uvw::poll_event&, uvw::poll_handle&)
            {
                std::cout << "Controller poll event\t"; // REMOVE
                zmq::multipart_t wire_msg;
                if (wire_msg.recv(m_controller, ZMQ_DONTWAIT))
                {
                    zmq::multipart_t wire_reply = p_server->notify_internal_listener(wire_msg);
                    wire_reply.send(m_controller);
                }
                std::cout << "Done\n"; // REMOVE
            }
        );

        p_shell_poll->on<uvw::error_event>(
            [](const uvw::error_event&, uvw::poll_handle&)
            {   // TODO: handle errors
                std::cout << "Something wrong with the shell.\n";
            });

        p_controller_poll->on<uvw::error_event>(
            [](const uvw::error_event&, uvw::poll_handle&)
            {   // TODO: handle errors
                std::cout << "Something wrong with the controller.\n";
            });

        std::cout << "Polls created\n"; // REMOVE
    }

    void xshell_uv::run()
    {
        std::cout << "Starting polls\n";
        // Start the polls
        p_shell_poll->start(uvw::poll_handle::poll_event_flags::READABLE);
        p_controller_poll->start(uvw::poll_handle::poll_event_flags::READABLE);

        std::cout << "After starting polls\n"; // REMOVE

        p_loop->run();

        std::cout << "Loop done running\n"; // REMOVE

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

