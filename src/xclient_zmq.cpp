/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "xeus-zmq/xclient_zmq.hpp"
#include "xclient_zmq_impl.hpp"

namespace xeus
{

    xclient_zmq::xclient_zmq(std::unique_ptr<xclient_zmq_impl> impl)
        : p_client_impl(std::move(impl))
    {
    }

    // Has to be in the cpp because incomplete
    // types are used in unique_ptr in the header
    xclient_zmq::~xclient_zmq() = default;


    void xclient_zmq::send_on_shell(xmessage msg)
    {
        p_client_impl->send_on_shell(std::move(msg));
    }

    void xclient_zmq::send_on_control(xmessage msg)
    {
        p_client_impl->send_on_control(std::move(msg));
    }

    std::optional<xmessage> xclient_zmq::check_shell_answer()
    {
        return p_client_impl->receive_on_shell(-1);
    }

    std::optional<xmessage> xclient_zmq::check_control_answer()
    {
        return p_client_impl->receive_on_control(-1);
    }

    void xclient_zmq::register_shell_listener(const listener& l)
    {
        p_client_impl->register_shell_listener(l);
    }

    void xclient_zmq::register_control_listener(const listener& l)
    {
        p_client_impl->register_control_listener(l);
    }

    void xclient_zmq::register_iopub_listener(const listener& l)
    {
        p_client_impl->register_iopub_listener(l);
    }

    void xclient_zmq::register_kernel_status_listener(const kernel_status_listener& l)
    {
        p_client_impl->register_kernel_status_listener(l);
    }

    void xclient_zmq::notify_shell_listener(xmessage msg)
    {
        p_client_impl->notify_shell_listener(std::move(msg));
    }

    void xclient_zmq::notify_control_listener(xmessage msg)
    {
        p_client_impl->notify_control_listener(std::move(msg));
    }

    void xclient_zmq::notify_iopub_listener(xmessage msg)
    {
        p_client_impl->notify_iopub_listener(std::move(msg));
    }

    void xclient_zmq::notify_kernel_dead(bool status)
    {
        p_client_impl->notify_kernel_dead(status);
    }

    std::size_t xclient_zmq::iopub_queue_size() const
    {
        return p_client_impl->iopub_queue_size();
    }

    std::optional<xmessage> xclient_zmq::pop_iopub_message()
    {
        return p_client_impl->pop_iopub_message();
    }

    void xclient_zmq::connect()
    {
        p_client_impl->connect();
    }

    void xclient_zmq::stop_channels()
    {
        p_client_impl->stop_channels();
    }

    void xclient_zmq::start()
    {
        p_client_impl->start();
    }

    void xclient_zmq::wait_for_message()
    {
        p_client_impl->wait_for_message();
    }

    std::unique_ptr<xclient_zmq> make_xclient_zmq(xcontext& context,
                                                const xconfiguration& config,
                                                nl::json::error_handler_t eh)
    {
        auto impl = std::make_unique<xclient_zmq_impl>(context.get_wrapped_context<zmq::context_t>(), config, eh);
        return std::make_unique<xclient_zmq>(std::move(impl));
    }
}
