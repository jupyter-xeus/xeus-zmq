/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "xserver_zmq_split_impl.hpp"
#include "../common/xzmq_serializer.hpp"

namespace xeus
{
    xserver_zmq_split_impl::xserver_zmq_split_impl(zmq::context_t& context,
                                                   const xconfiguration& config,
                                                   nl::json::error_handler_t eh)     
        : p_auth(make_xauthentication(config.m_signature_scheme, config.m_key))
        , m_control(context, config.m_transport, config.m_ip ,config.m_control_port, this)
        , m_heartbeat(context, config.m_transport, config.m_ip, config.m_hb_port)
        , m_publisher(context,
                      std::bind(&xserver_zmq_split_impl::serialize_iopub, this, std::placeholders::_1),
                      config.m_transport, config.m_ip, config.m_iopub_port)
        , m_shell(context, config.m_transport, config.m_ip ,config.m_shell_port, config.m_stdin_port, this)
        , m_hb_thread()
        , m_iopub_thread()
        , m_error_handler(eh)
    {
        m_control.connect_messenger();
    }

    void xserver_zmq_split_impl::start_heartbeat_thread()
    {
        m_hb_thread = xthread(&xheartbeat::run, &m_heartbeat);
    }

    void xserver_zmq_split_impl::start_publisher_thread()
    {
        m_iopub_thread = xthread(&xpublisher::run, &m_publisher);
    }

    void xserver_zmq_split_impl::stop_channels()
    {
        m_control.stop_channels();
    }

    fd_t xserver_zmq_split_impl::get_shell_fd() const
    {
        return m_shell.get_shell_fd();
    }

    fd_t xserver_zmq_split_impl::get_shell_controller_fd() const
    {
        return m_shell.get_controller_fd();
    }
    
    fd_t xserver_zmq_split_impl::get_control_fd() const
    {
        return m_control.get_fd();
    }

    std::optional<channel> xserver_zmq_split_impl::poll_shell_channels(long timeout)
    {
        return m_shell.poll_channels(timeout);
    }

    std::optional<xmessage> xserver_zmq_split_impl::read_shell(int flags)
    {
        return m_shell.read_shell(flags);
    }

    std::optional<std::string> xserver_zmq_split_impl::read_shell_controller(int flags)
    {
        return m_shell.read_controller(flags);
    }

    std::optional<xmessage> xserver_zmq_split_impl::read_control(int flags)
    {
        return m_control.read_control(flags);
    }

    xcontrol_messenger& xserver_zmq_split_impl::get_control_messenger()
    {
        return m_control.get_messenger();
    }

    void xserver_zmq_split_impl::send_shell(xmessage message)
    {
        zmq::multipart_t wire_msg = xzmq_serializer::serialize(std::move(message), *p_auth, m_error_handler);
        m_shell.send_shell(wire_msg);
    }

    void xserver_zmq_split_impl::send_shell_controller(std::string message)
    {
        m_shell.send_controller(std::move(message));
    }

    void xserver_zmq_split_impl::send_control(xmessage message)
    {
        zmq::multipart_t wire_msg = xzmq_serializer::serialize(std::move(message), *p_auth, m_error_handler);
        m_control.send_control(wire_msg);
    }

    std::optional<xmessage> xserver_zmq_split_impl::send_stdin(xmessage message)
    {
        zmq::multipart_t wire_msg = xzmq_serializer::serialize(std::move(message), *p_auth, m_error_handler);
        return m_shell.send_stdin(wire_msg);
    }

    void xserver_zmq_split_impl::publish(xpub_message message, channel c)
    {
        zmq::multipart_t wire_msg = xzmq_serializer::serialize_iopub(std::move(message), *p_auth, m_error_handler);
        if (c == channel::SHELL)
        {
            m_shell.publish(wire_msg);
        }
        else
        {
            m_control.publish(wire_msg);
        }
    }

    void xserver_zmq_split_impl::abort_queue(const listener& l, long polling_interval)
    {
        m_shell.abort_queue(l, polling_interval);
    }

    void xserver_zmq_split_impl::update_config(xconfiguration& config) const
    {
        config.m_control_port = m_control.get_port();
        config.m_shell_port = m_shell.get_shell_port();
        config.m_stdin_port = m_shell.get_stdin_port();
        config.m_iopub_port = m_publisher.get_port();
        config.m_hb_port = m_heartbeat.get_port();
    }

    xmessage xserver_zmq_split_impl::deserialize(zmq::multipart_t& wire_msg) const
    {
        return xzmq_serializer::deserialize(wire_msg, *p_auth);
    }
    
    zmq::multipart_t xserver_zmq_split_impl::serialize_iopub(xpub_message&& msg)
    {
        return xzmq_serializer::serialize_iopub(std::move(msg), *p_auth, m_error_handler);
    }

}

