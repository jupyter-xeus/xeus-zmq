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
    xcontrol_messenger& xserver_zmq_impl::get_control_messenger()
    {
        return get_control_messenger_impl();
    }

    void xserver_zmq_impl::send_shell(xmessage message)
    {
        send_shell_impl(std::move(message));
    }

    void xserver_zmq_impl::send_control(xmessage message)
    {
        send_control_impl(std::move(message));
    }

    void xserver_zmq_impl::send_stdin(xmessage message)
    {
        send_stdin_impl(std::move(message));
    }

    void xserver_zmq_impl::publish(xpub_message message, channel c)
    {
        publish_impl(std::move(message), c);
    }

    void xserver_zmq_impl::start(xpub_message message)
    {
        start_impl(std::move(message));
    }

    void xserver_zmq_impl::abort_queue(const listener& l, long polling_interval)
    {
        abort_queue_impl(l, polling_interval);
    }

    void xserver_zmq_impl::stop()
    {
        stop_impl();
    }

    void xserver_zmq_impl::update_config(xconfiguration& config) const
    {
        update_config_impl(config);
    }

    void xserver_zmq_impl::register_kernel_notifier(xserver_zmq& notifier)
    {
        p_kernel_notifier = &notifier;
    }

    void xserver_zmq_impl::notify_shell_listener(xmessage msg)
    {
        p_kernel_notifier->call_notify_shell_listener(std::move(msg));
    }

    void xserver_zmq_impl::notify_control_listener(xmessage msg)
    {
        p_kernel_notifier->call_notify_control_listener(std::move(msg));
    }

    void xserver_zmq_impl::notify_stdin_listener(xmessage msg)
    {
        p_kernel_notifier->call_notify_stdin_listener(std::move(msg));
    }

    nl::json xserver_zmq_impl::notify_internal_listener(nl::json msg)
    {
        return p_kernel_notifier->call_notify_internal_listener(std::move(msg));
    }
}
