/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include <memory>
#include <iostream>

#ifndef UVW_AS_LIB
#define UVW_AS_LIB
#endif
#include <uvw.hpp>

#include "xshell_uv.hpp"
#include "xserver_zmq_split.hpp"

namespace xeus
{
    xshell_uv::xshell_uv(zmq::context_t& context,
                         const std::string& transport,
                         const std::string& ip,
                         const std::string& shell_port,
                         const std::string& stdin_port,
                         xserver_zmq_split* server,
                         std::shared_ptr<uvw::loop> loop_ptr,
                         std::unique_ptr<xhook_base> hook)
        : xshell_base(context, transport, ip, shell_port, stdin_port, server)
        , p_loop{loop_ptr}
        , p_hook{std::move(hook)}
    {
        if (!p_loop)
        {
            std::cerr << "No loop provided, using default loop." << std::endl;
            p_loop = uvw::loop::get_default();
        }
        create_polls();
    }

    void xshell_uv::create_polls()
    {
        // Get the file descriptor for the shell and controller sockets
        zmq::fd_t shell_fd = m_shell.get(zmq::sockopt::fd);
        zmq::fd_t controller_fd = m_controller.get(zmq::sockopt::fd);

        // Create (libuv) poll handles and bind them to the loop
        p_shell_poll = p_loop->resource<uvw::poll_handle>(shell_fd);
        p_controller_poll = p_loop->resource<uvw::poll_handle>(controller_fd);

        p_shell_poll->on<uvw::poll_event>(
            [this](uvw::poll_event&, uvw::poll_handle&)
            {
                if (this->p_hook)
                {
                    this->p_hook->pre_hook();
                }
                zmq::multipart_t wire_msg;
                if (wire_msg.recv(m_shell, ZMQ_DONTWAIT)) // non-blocking
                {
                    xmessage msg = p_server->deserialize(wire_msg);
                    p_server->notify_shell_listener(std::move(msg));
                }
                if (this->p_hook)
                {
                    this->p_hook->post_hook();
                }
            }
        );

        p_controller_poll->on<uvw::poll_event>(
            [this](uvw::poll_event&, uvw::poll_handle&)
            {
                if (this->p_hook)
                {
                    this->p_hook->pre_hook();
                }

                zmq::multipart_t wire_msg;
                if (wire_msg.recv(m_controller, ZMQ_DONTWAIT))
                {
                    zmq::multipart_t wire_reply = p_server->notify_internal_listener(wire_msg);
                    wire_reply.send(m_controller);
                }

                if (this->p_hook)
                {
                    this->p_hook->post_hook();
                }
            }
        );

        p_shell_poll->on<uvw::error_event>(
            [](const uvw::error_event& e, uvw::poll_handle&)
            {
                std::cerr << e.what() << std::endl;
            });

        p_controller_poll->on<uvw::error_event>(
            [](const uvw::error_event& e, uvw::poll_handle&)
            {
                std::cerr << e.what() << std::endl;
            });
    }

    void xshell_uv::run_impl()
    {
        p_shell_poll->start(uvw::poll_handle::poll_event_flags::READABLE);
        p_controller_poll->start(uvw::poll_handle::poll_event_flags::READABLE);

        if (p_hook)
        {
            p_hook->run(p_loop);
        }
        else
        {
            p_loop->run();
        }
    }

}
