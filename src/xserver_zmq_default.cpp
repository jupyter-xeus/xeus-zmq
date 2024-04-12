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

#include "xeus/xguid.hpp"
#include "xauthentication.hpp"
#include "xzmq_serializer.hpp"
#include "xserver_zmq_default.hpp"
#include "xmiddleware_impl.hpp"
#include "xpublisher.hpp"
#include "xheartbeat.hpp"
#include "xtrivial_messenger.hpp"

namespace xeus
{

    xserver_zmq_default::xserver_zmq_default(zmq::context_t& context,
                             const xconfiguration& config,
                             nl::json::error_handler_t eh)
        : m_shell(context, zmq::socket_type::router)
        , m_controller(context, zmq::socket_type::router)
        , m_stdin(context, zmq::socket_type::router)
        , m_publisher_pub(context, zmq::socket_type::pub)
        , m_publisher_controller(context, zmq::socket_type::req)
        , m_heartbeat_controller(context, zmq::socket_type::req)
        , p_auth(make_xauthentication(config.m_signature_scheme, config.m_key))
        , p_publisher(new xpublisher(context,
                                     std::bind(&xserver_zmq_default::serialize_iopub, this, std::placeholders::_1),
                                     config.m_transport, config.m_ip, config.m_iopub_port))
        , p_heartbeat(new xheartbeat(context, config.m_transport, config.m_ip, config.m_hb_port))
        , m_iopub_thread()
        , m_hb_thread()
        , p_messenger(new xtrivial_messenger(this))
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

    // Has to be in the cpp because incomplete
    // types are used in unique_ptr in the header
    xserver_zmq_default::~xserver_zmq_default() = default;

    xcontrol_messenger& xserver_zmq_default::get_control_messenger_impl()
    {
        return *p_messenger;
    }

    void xserver_zmq_default::send_shell_impl(xmessage msg)
    {
        zmq::multipart_t wire_msg = xzmq_serializer::serialize(std::move(msg), *p_auth, m_error_handler);
        wire_msg.send(m_shell);
    }

    void xserver_zmq_default::send_control_impl(xmessage msg)
    {
        zmq::multipart_t wire_msg = xzmq_serializer::serialize(std::move(msg), *p_auth, m_error_handler);
        wire_msg.send(m_controller);
    }

    void xserver_zmq_default::send_stdin_impl(xmessage msg)
    {
        zmq::multipart_t wire_msg = xzmq_serializer::serialize(std::move(msg), *p_auth, m_error_handler);
        wire_msg.send(m_stdin);
        zmq::multipart_t wire_reply;
	// Block until a response to the input request is received.
        wire_reply.recv(m_stdin);
        try
        {
            xmessage reply = xzmq_serializer::deserialize(wire_reply, *p_auth);
            xserver_zmq_impl::notify_stdin_listener(std::move(reply));
        }
        catch (std::exception& e)
        {
            std::cerr << e.what() << std::endl;
        }
    }

    void xserver_zmq_default::publish_impl(xpub_message msg, channel)
    {
        zmq::multipart_t wire_msg = xzmq_serializer::serialize_iopub(std::move(msg), *p_auth, m_error_handler);
        wire_msg.send(m_publisher_pub);
    }

    void xserver_zmq_default::start_impl(xpub_message message)
    {
        start_publisher_thread();
        start_heartbeat_thread();

        m_request_stop = false;

        publish(std::move(message), channel::SHELL);

        while (!m_request_stop)
        {
            poll(-1);
        }

        stop_channels();
    }

    void xserver_zmq_default::start_publisher_thread()
    {
        m_iopub_thread = std::move(xthread(&xpublisher::run, p_publisher.get()));
    }

    void xserver_zmq_default::start_heartbeat_thread()
    {
        m_hb_thread = std::move(xthread(&xheartbeat::run, p_heartbeat.get()));
    }

    void xserver_zmq_default::poll(long timeout)
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
                xserver_zmq_impl::notify_control_listener(std::move(msg));
            }

            if (!m_request_stop && (items[1].revents & ZMQ_POLLIN))
            {
                zmq::multipart_t wire_msg;
                wire_msg.recv(m_shell);
                xmessage msg = xzmq_serializer::deserialize(wire_msg, *p_auth);
                xserver_zmq_impl::notify_shell_listener(std::move(msg));
            }
        }
        catch (std::exception& e)
        {
            std::cerr << e.what() << std::endl;
        }
    }

    void xserver_zmq_default::abort_queue_impl(const listener& l, long polling_interval)
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

    void xserver_zmq_default::stop_impl()
    {
        m_request_stop = true;
    }

    void xserver_zmq_default::update_config_impl(xconfiguration& config) const
    {
        config.m_control_port = get_socket_port(m_controller);
        config.m_shell_port = get_socket_port(m_shell);
        config.m_stdin_port = get_socket_port(m_stdin);
        config.m_iopub_port = p_publisher->get_port();
        config.m_hb_port = p_heartbeat->get_port();
    }

    void xserver_zmq_default::stop_channels()
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

    zmq::multipart_t xserver_zmq_default::serialize_iopub(xpub_message&& msg)
    {
        return xzmq_serializer::serialize_iopub(std::move(msg), *p_auth, m_error_handler);
    }
}
