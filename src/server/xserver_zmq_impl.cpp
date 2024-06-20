/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include <iostream>

#include "xserver_zmq_impl.hpp"
#include "../common/xmiddleware_impl.hpp"
#include "../common/xzmq_serializer.hpp"

namespace xeus
{
    xserver_zmq_impl::xserver_zmq_impl(zmq::context_t& context,
                                       const xconfiguration& config,
                                       nl::json::error_handler_t eh,
                                       internal_listener listener)
        : m_shell(context, zmq::socket_type::router)
        , m_controller(context, zmq::socket_type::router)
        , m_stdin(context, zmq::socket_type::router)
        , m_publisher_pub(context, zmq::socket_type::pub)
        , m_publisher_controller(context, zmq::socket_type::req)
        , m_heartbeat_controller(context, zmq::socket_type::req)
        , p_auth(make_xauthentication(config.m_signature_scheme, config.m_key))
        , m_publisher(context,
                      std::bind(&xserver_zmq_impl::serialize_iopub, this, std::placeholders::_1),
                      config.m_transport, config.m_ip, config.m_iopub_port)
        , m_heartbeat(context, config.m_transport, config.m_ip, config.m_hb_port)
        , m_iopub_thread()
        , m_hb_thread()
        , m_messenger(std::move(listener))
        , m_error_handler(eh)
        , m_request_stop(false)
    {
        init_socket(m_shell, config.m_transport, config.m_ip, config.m_shell_port);
        init_socket(m_controller, config.m_transport, config.m_ip, config.m_control_port);
        init_socket(m_stdin, config.m_transport, config.m_ip, config.m_stdin_port);
        m_publisher_pub.set(zmq::sockopt::linger, get_socket_linger());
        m_publisher_pub.connect(get_publisher_end_point());

        m_publisher_controller.set(zmq::sockopt::linger, get_socket_linger());
        m_publisher_controller.connect(get_controller_end_point("publisher"));
        m_heartbeat_controller.set(zmq::sockopt::linger, get_socket_linger());
        m_heartbeat_controller.connect(get_controller_end_point("heartbeat"));
    }

    void xserver_zmq_impl::start_publisher_thread()
    {
        m_iopub_thread = xthread(&xpublisher::run, &m_publisher);
    }

    void xserver_zmq_impl::start_heartbeat_thread()
    {
        m_hb_thread = xthread(&xheartbeat::run, &m_heartbeat);
    }

    void xserver_zmq_impl::stop_channels()
    {
        zmq::message_t stop_msg("stop", 4);
        zmq::message_t response;

        // Wait for publisher answer
        m_publisher_controller.send(stop_msg, zmq::send_flags::none);
        (void)m_publisher_controller.recv(response);

        // Wait for heartbeat answer
        m_heartbeat_controller.send(stop_msg, zmq::send_flags::none);
        (void)m_heartbeat_controller.recv(response);
    }

    void xserver_zmq_impl::set_request_stop(bool stop)
    {
        m_request_stop = stop;
    }
    
    bool xserver_zmq_impl::is_stopped() const
    {
        return m_request_stop;
    }
    
    auto xserver_zmq_impl::poll_channels(long timeout) -> std::optional<message_channel>
    {
        zmq::pollitem_t items[]
            = { { m_controller, 0, ZMQ_POLLIN, 0 }, { m_shell, 0, ZMQ_POLLIN, 0 } };

        zmq::poll(&items[0], 2, std::chrono::milliseconds(timeout));

        try
        {
            if (items[0].revents & ZMQ_POLLIN)
            {
                zmq::multipart_t wire_msg;
                wire_msg.recv(m_controller);
                xmessage msg = xzmq_serializer::deserialize(wire_msg, *p_auth);
                return { std::make_pair(std::move(msg), channel::CONTROL) };
            }

            if (!m_request_stop && (items[1].revents & ZMQ_POLLIN))
            {
                zmq::multipart_t wire_msg;
                wire_msg.recv(m_shell);
                xmessage msg = xzmq_serializer::deserialize(wire_msg, *p_auth);
                return { std::make_pair(std::move(msg), channel::SHELL) };
            }
        }
        catch (std::exception& e)
        {
            std::cerr << e.what() << std::endl;
        }

        return std::nullopt;
    }

    xcontrol_messenger& xserver_zmq_impl::get_control_messenger()
    {
        return m_messenger;
    }

    void xserver_zmq_impl::send_shell(xmessage message)
    {
        zmq::multipart_t wire_msg = xzmq_serializer::serialize(std::move(message), *p_auth, m_error_handler);
        wire_msg.send(m_shell);
    }

    void xserver_zmq_impl::send_control(xmessage message)
    {
        zmq::multipart_t wire_msg = xzmq_serializer::serialize(std::move(message), *p_auth, m_error_handler);
        wire_msg.send(m_controller);
    }

    std::optional<xmessage> xserver_zmq_impl::send_stdin(xmessage message)
    {
        zmq::multipart_t wire_msg = xzmq_serializer::serialize(std::move(message), *p_auth, m_error_handler);
        wire_msg.send(m_stdin);
        zmq::multipart_t wire_reply;
	// Block until a response to the input request is received.
        wire_reply.recv(m_stdin);
        try
        {
            return xzmq_serializer::deserialize(wire_reply, *p_auth);
        }
        catch (std::exception& e)
        {
            std::cerr << e.what() << std::endl;
        }
        return std::nullopt;
    }

    void xserver_zmq_impl::publish(xpub_message message, channel)
    {
        zmq::multipart_t wire_msg = xzmq_serializer::serialize_iopub(std::move(message), *p_auth, m_error_handler);
        wire_msg.send(m_publisher_pub);
    }

    void xserver_zmq_impl::abort_queue(const listener& l, long polling_interval)
    {
        while (true)
        {
            zmq::multipart_t wire_msg;
            bool msg = wire_msg.recv(m_shell, ZMQ_NOBLOCK);
            if (!msg)
            {
                return;
            }

            try
            {
                xmessage msg = xzmq_serializer::deserialize(wire_msg, *p_auth);
                l(std::move(msg));
            }
            catch (std::exception& e)
            {
                std::cerr << e.what() << std::endl;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(polling_interval));
        }
    }

    void xserver_zmq_impl::update_config(xconfiguration& config) const
    {
        config.m_control_port = get_socket_port(m_controller);
        config.m_shell_port = get_socket_port(m_shell);
        config.m_stdin_port = get_socket_port(m_stdin);
        config.m_iopub_port = m_publisher.get_port();
        config.m_hb_port = m_heartbeat.get_port();
    }

    zmq::multipart_t xserver_zmq_impl::serialize_iopub(xpub_message&& msg)
    {
        return xzmq_serializer::serialize_iopub(std::move(msg), *p_auth, m_error_handler);
    }
}

