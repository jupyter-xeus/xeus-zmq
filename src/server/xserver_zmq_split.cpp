/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "xeus-zmq/xserver_zmq_split.hpp"
#include "xserver_zmq_split_impl.hpp"

namespace xeus
{
    xserver_zmq_split::xserver_zmq_split(xcontext& context,
                                         const xconfiguration& config,
                                         nl::json::error_handler_t eh,
                                         control_runner_ptr control,
                                         shell_runner_ptr shell)
        : p_impl(new xserver_zmq_split_impl(context.get_wrapped_context<zmq::context_t>(), config, eh))
        , p_control_runner(std::move(control))
        , p_shell_runner(std::move(shell))
        , m_error_handler(eh)
    {
        p_control_runner->register_server(*this);
        p_shell_runner->register_server(*this);
    }
    
    xserver_zmq_split::~xserver_zmq_split() = default;

    fd_t xserver_zmq_split::get_control_fd() const
    {
        return p_impl->get_control_fd();
    }

    std::optional<xmessage> xserver_zmq_split::read_control(int flags)
    {
        return p_impl->read_control(flags);
    }

    void xserver_zmq_split::send_control_message(xmessage msg)
    {
        p_impl->send_control(std::move(msg));
    }

    void xserver_zmq_split::stop_channels()
    {
        p_impl->stop_channels();
    }

    std::string xserver_zmq_split::notify_internal_listener(std::string message)
    {
        nl::json msg = nl::json::parse(std::move(message));
        nl::json reply = xserver::notify_internal_listener(std::move(msg));
        return reply.dump(-1, ' ', false, m_error_handler);
    }

    fd_t xserver_zmq_split::get_shell_fd() const
    {
        return p_impl->get_shell_fd();
    }

    fd_t xserver_zmq_split::get_shell_controller_fd() const
    {
        return p_impl->get_shell_controller_fd();
    }

    std::optional<channel> xserver_zmq_split::poll_shell_channels(long timeout)
    {
        return p_impl->poll_shell_channels(timeout);
    }

    std::optional<xmessage> xserver_zmq_split::read_shell(int flags)
    {
        return p_impl->read_shell(flags);
    }

    void xserver_zmq_split::send_shell_message(xmessage msg)
    {
        p_impl->send_shell(std::move(msg));
    }

    std::optional<std::string> xserver_zmq_split::read_shell_controller(int flags)
    {
        return p_impl->read_shell_controller(flags);
    }

    void xserver_zmq_split::send_shell_controller(std::string message)
    {
        p_impl->send_shell_controller(std::move(message));
    }

    void xserver_zmq_split::start_publisher_thread()
    {
        p_impl->start_publisher_thread();
    }

    void xserver_zmq_split::start_heartbeat_thread()
    {
        p_impl->start_heartbeat_thread();
    }
    
    void xserver_zmq_split::start_control_thread()
    {
        m_control_thread = xthread(&xcontrol_runner::run, p_control_runner.get());
    }

    void xserver_zmq_split::run_control()
    {
        p_control_runner->run();
    }

    void xserver_zmq_split::start_shell_thread()
    {
        m_shell_thread = xthread(&xshell_runner::run, p_shell_runner.get());
    }

    void xserver_zmq_split::run_shell()
    {
        p_shell_runner->run();
    }

    xcontrol_messenger& xserver_zmq_split::get_control_messenger_impl()
    {
        return p_impl->get_control_messenger();
    }

    void xserver_zmq_split::send_shell_impl(xmessage msg)
    {
        send_shell_message(std::move(msg));
    }

    void xserver_zmq_split::send_control_impl(xmessage msg)
    {
        send_control_message(std::move(msg));
    }

    void xserver_zmq_split::send_stdin_impl(xmessage msg)
    {
        auto rep = p_impl->send_stdin(std::move(msg));
        if (rep)
        {
            notify_stdin_listener(std::move(rep.value()));
        }
    }

    void xserver_zmq_split::publish_impl(xpub_message msg, channel c)
    {
        p_impl->publish(std::move(msg), c);
    }

    void xserver_zmq_split::stop_impl()
    {
        p_control_runner->stop();
    }

    void xserver_zmq_split::abort_queue_impl(const listener& l, long polling_interval)
    {
        p_impl->abort_queue(l, polling_interval);
    }

    void xserver_zmq_split::update_config_impl(xconfiguration& config) const
    {
        p_impl->update_config(config);
    }
}

