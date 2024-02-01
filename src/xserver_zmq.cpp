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
#include "xserver_zmq_default.hpp"
#include "xserver_control_main.hpp"
#include "xserver_shell_main.hpp"

namespace xeus
{

    xserver_zmq::xserver_zmq(xserver_zmq_impl* impl)
        : p_impl(impl)
    {
        // TODO if reqd
    }

    xserver_zmq::~xserver_zmq()
    {
        delete p_impl;
    }

    xcontrol_messenger& xserver_zmq::get_control_messenger_impl()
    {
        return p_impl->get_control_messenger();
    }

    void xserver_zmq::send_shell_impl(xmessage msg)
    {
        p_impl->send_shell(std::move(msg));
    }

    void xserver_zmq::send_control_impl(xmessage msg)
    {
        p_impl->send_control(std::move(msg));
    }

    void xserver_zmq::send_stdin_impl(xmessage msg)
    {
        p_impl->send_stdin(std::move(msg));
    }

    void xserver_zmq::publish_impl(xpub_message msg, channel c)
    {
        p_impl->publish(std::move(msg), c);
    }

    void xserver_zmq::start_impl(xpub_message msg)
    {
        p_impl->start(std::move(msg));
    }

    void xserver_zmq::abort_queue_impl(const listener& l, long polling_interval)
    {
        p_impl->abort_queue(l, polling_interval);
    }

    void xserver_zmq::stop_impl()
    {
        p_impl->stop();
    }

    void xserver_zmq::update_config_impl(xconfiguration& config) const
    {
        p_impl->update_config(config);
    }

    XEUS_ZMQ_API
    std::unique_ptr<xserver> make_xserver_zmq_default(xcontext& context,
                                              const xconfiguration& config,
                                              nl::json::error_handler_t eh)
    {
        auto* impl = new xserver_zmq_default(context.get_wrapped_context<zmq::context_t>(), config, eh);
        return std::make_unique<xserver_zmq>(impl);
    }

    XEUS_ZMQ_API
    std::unique_ptr<xserver> make_xserver_control_main(xcontext& context,
                                                       const xconfiguration& config,
                                                       nl::json::error_handler_t eh)
    {
        auto* impl = new xserver_control_main(context.get_wrapped_context<zmq::context_t>(), config, eh);
        return std::make_unique<xserver_zmq>(impl);
    }

    XEUS_ZMQ_API
    std::unique_ptr<xserver> make_xserver_shell_main(xcontext& context,
                                                     const xconfiguration& config,
                                                     nl::json::error_handler_t eh)
    {
        auto* impl = new xserver_shell_main(context.get_wrapped_context<zmq::context_t>(), config, eh);
        return std::make_unique<xserver_zmq>(impl);
    }
}
