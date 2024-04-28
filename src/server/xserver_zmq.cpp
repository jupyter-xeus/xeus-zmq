/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "xeus-zmq/xserver_zmq.hpp"
#include "xserver_zmq_impl.hpp"

namespace xeus
{
    xserver_zmq::xserver_zmq(xcontext& context,
                             const xconfiguration& config,
                             nl::json::error_handler_t eh)
        : p_impl(std::make_unique<xserver_zmq_impl>(
                    context.get_wrapped_context<zmq::context_t>(),
                    config,
                    eh,
                    std::bind(&xserver_zmq::notify_internal_listener, this, std::placeholders::_1)))
    {
    }

    // Has to be in the cpp because incomplete
    // types are used in unique_ptr in the header
    xserver_zmq::~xserver_zmq() = default;

    ////////////////////////////////
    // API for inheriting classes //
    ////////////////////////////////

    void xserver_zmq::start_publisher_thread()
    {
        p_impl->start_publisher_thread();
    }

    void xserver_zmq::start_heartbeat_thread()
    {
        p_impl->start_heartbeat_thread();
    }

    void xserver_zmq::stop_channels()
    {
        p_impl->stop_channels();
    }

    void xserver_zmq::set_request_stop(bool stop)
    {
        p_impl->set_request_stop(stop);
    }

    bool xserver_zmq::is_stopped() const
    {
        return p_impl->is_stopped();
    }
    
    auto xserver_zmq::poll_channels(long timeout) -> std::optional<message_channel>
    {
        return p_impl->poll_channels(timeout);
    }

    void xserver_zmq::send_shell_message(xmessage msg)
    {
        p_impl->send_shell(std::move(msg));
    }

    void xserver_zmq::send_control_message(xmessage msg)
    {
        p_impl->send_control(std::move(msg));
    }

    ///////////////////////////////////////////////
    // Implementation of xserver virtual methods //
    ///////////////////////////////////////////////

    xcontrol_messenger& xserver_zmq::get_control_messenger_impl()
    {
        return p_impl->get_control_messenger();
    }

    void xserver_zmq::send_shell_impl(xmessage msg)
    {
        send_shell_message(std::move(msg));
    }

    void xserver_zmq::send_control_impl(xmessage msg)
    {
        send_control_message(std::move(msg));
    }

    void xserver_zmq::send_stdin_impl(xmessage msg)
    {
        auto reply = p_impl->send_stdin(std::move(msg));
        if (reply)
        {
            xserver::notify_stdin_listener(std::move(reply.value()));
        }
    }

    void xserver_zmq::publish_impl(xpub_message msg, channel c)
    {
        p_impl->publish(std::move(msg), c);
    }

    void xserver_zmq::abort_queue_impl(const listener& l, long polling_interval)
    {
        p_impl->abort_queue(l, polling_interval);
    }

    void xserver_zmq::update_config_impl(xconfiguration& config) const
    {
        p_impl->update_config(config);
    }
}

