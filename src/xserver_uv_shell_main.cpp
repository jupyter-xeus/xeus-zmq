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

#include "zmq_addon.hpp"
#include "xeus/xguid.hpp"
#include "xeus/xeus_context.hpp"
#include "xeus-zmq/xauthentication.hpp"
#include "xeus-zmq/xserver_uv_shell_main.hpp"
#include "xeus-zmq/xmiddleware.hpp"
#include "xeus-zmq/xzmq_serializer.hpp"
#include "xshell_uv.hpp"
#include "xcontrol_uv.hpp"
#include "xheartbeat.hpp"
#include "xpublisher.hpp"
#include "xzmq_messenger.hpp"

namespace xeus
{
    xserver_uv_shell_main::xserver_uv_shell_main(zmq::context_t& context,
                                         const xconfiguration& config,
                                         nl::json::error_handler_t eh)
        : p_auth(make_xauthentication(config.m_signature_scheme, config.m_key))
        , p_controller(new xcontrol_uv(context, config.m_transport, config.m_ip ,config.m_control_port, this))
        , p_heartbeat(new xheartbeat(context, config.m_transport, config.m_ip, config.m_hb_port))
        , p_publisher(new xpublisher(context,
                                     std::bind(&xserver_uv_shell_main::serialize_iopub, this, std::placeholders::_1),
                                     config.m_transport, config.m_ip, config.m_iopub_port))
        , p_shell(new xshell_uv(context, config.m_transport, config.m_ip ,config.m_shell_port, config.m_stdin_port, this))
        , m_control_thread()
        , m_hb_thread()
        , m_iopub_thread()
        , m_shell_thread()
        , m_error_handler(eh)
        , m_control_stopped(false)
    {
        p_controller->connect_messenger();
    }

    // Has to be in the cpp because incomplete
    // types are used in unique_ptr in the header
    xserver_uv_shell_main::~xserver_uv_shell_main() = default;

    void xserver_uv_shell_main::start_server(zmq::multipart_t& wire_msg)
    {
        start_publisher_thread();
        start_heartbeat_thread();
        start_control_thread();

        get_shell().publish(wire_msg);
        get_shell().run();
    }

    zmq::multipart_t xserver_uv_shell_main::notify_internal_listener(zmq::multipart_t& wire_msg)
    {
        nl::json msg = nl::json::parse(wire_msg.popstr());
        nl::json reply = xserver::notify_internal_listener(msg);
        return zmq::multipart_t(reply.dump(-1, ' ', false, m_error_handler));
    }

    void xserver_uv_shell_main::notify_control_stopped()
    {
        m_control_stopped = true;
    }

    xmessage xserver_uv_shell_main::deserialize(zmq::multipart_t& wire_msg) const
    {
        return xzmq_serializer::deserialize(wire_msg, *p_auth);
    }

    xcontrol_messenger& xserver_uv_shell_main::get_control_messenger_impl()
    {
        return p_controller->get_messenger();
    }

    void xserver_uv_shell_main::send_shell_impl(xmessage msg)
    {
        zmq::multipart_t wire_msg = xzmq_serializer::serialize(std::move(msg), *p_auth, m_error_handler);
        p_shell->send_shell(wire_msg);
    }

    void xserver_uv_shell_main::send_control_impl(xmessage msg)
    {
        zmq::multipart_t wire_msg = xzmq_serializer::serialize(std::move(msg), *p_auth, m_error_handler);
        p_controller->send_control(wire_msg);
    }

    void xserver_uv_shell_main::send_stdin_impl(xmessage msg)
    {
        zmq::multipart_t wire_msg = xzmq_serializer::serialize(std::move(msg), *p_auth, m_error_handler);
        p_shell->send_stdin(wire_msg);
    }

    void xserver_uv_shell_main::publish_impl(xpub_message msg, channel c)
    {
        zmq::multipart_t wire_msg = xzmq_serializer::serialize_iopub(std::move(msg), *p_auth, m_error_handler);
        if(c == channel::SHELL)
        {
            p_shell->publish(wire_msg);
        }
        else
        {
            p_controller->publish(wire_msg);
        }
    }

    void xserver_uv_shell_main::start_impl(xpub_message msg)
    {
        zmq::multipart_t wire_msg = xzmq_serializer::serialize_iopub(std::move(msg), *p_auth, m_error_handler);
        start_server(wire_msg);
    }

    void xserver_uv_shell_main::abort_queue_impl(const listener& l, long polling_interval)
    {
        p_shell->abort_queue(l, polling_interval);
    }

    void xserver_uv_shell_main::stop_impl()
    {
        p_controller->stop();
    }

    void xserver_uv_shell_main::update_config_impl(xconfiguration& config) const
    {
        config.m_control_port = p_controller->get_port();
        config.m_shell_port = p_shell->get_shell_port();
        config.m_stdin_port = p_shell->get_stdin_port();
        config.m_iopub_port = p_publisher->get_port();
        config.m_hb_port = p_heartbeat->get_port();
    }

    void xserver_uv_shell_main::start_control_thread()
    {
        m_control_thread = std::move(xthread(&xcontrol_uv::run, p_controller.get()));
    }

    void xserver_uv_shell_main::start_heartbeat_thread()
    {
        m_hb_thread = std::move(xthread(&xheartbeat::run, p_heartbeat.get()));
    }

    void xserver_uv_shell_main::start_publisher_thread()
    {
        m_iopub_thread = std::move(xthread(&xpublisher::run, p_publisher.get()));
    }

    void xserver_uv_shell_main::start_shell_thread()
    {
        m_shell_thread = std::move(xthread(&xshell_uv::run, p_shell.get()));
    }

    xcontrol_uv& xserver_uv_shell_main::get_controller()
    {
        return *p_controller;
    }

    xshell_uv& xserver_uv_shell_main::get_shell()
    {
        return *p_shell;
    }

    bool xserver_uv_shell_main::is_control_stopped() const
    {
        return m_control_stopped;
    }

    zmq::multipart_t xserver_uv_shell_main::serialize_iopub(xpub_message&& msg)
    {
        return xzmq_serializer::serialize_iopub(std::move(msg), *p_auth, m_error_handler);
    }

    std::unique_ptr<xserver> make_xserver_uv_shell_main(
        xcontext& context,
        const xconfiguration& config,
        nl::json::error_handler_t eh)
    {
        return std::make_unique<xserver_uv_shell_main>(
            context.get_wrapped_context<zmq::context_t>(), config, eh);
    }
}

