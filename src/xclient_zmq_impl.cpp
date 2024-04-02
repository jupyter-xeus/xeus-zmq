/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include <iostream>

#include "xeus-zmq/xauthentication.hpp"
#include "xclient_zmq_impl.hpp"
#include "xeus-zmq/xzmq_serializer.hpp"

namespace xeus
{

    xclient_zmq_impl::xclient_zmq_impl(zmq::context_t& context,
                                    const xeus::xconfiguration& config,
                                    nl::json::error_handler_t eh)
        : p_auth(make_xauthentication(config.m_signature_scheme, config.m_key))
        , m_shell_client(context, config.m_transport, config.m_ip, config.m_shell_port)
        , m_control_client(context, config.m_transport, config.m_ip, config.m_control_port)
        , m_iopub_client(context, config)
        , m_heartbeat_client(context, config)
        , p_messenger(context)
        , m_error_handler(eh)
    {
    }

    // Has to be in the cpp because incomplete
    // types are used in unique_ptr in the header
    xclient_zmq_impl::~xclient_zmq_impl() = default;

    void xclient_zmq_impl::send_on_shell(xmessage msg)
    {
        zmq::multipart_t wire_msg = xzmq_serializer::serialize(std::move(msg), *p_auth, m_error_handler);
        m_shell_client.send_message(wire_msg);
    }

    void xclient_zmq_impl::send_on_control(xmessage msg)
    {
        zmq::multipart_t wire_msg = xzmq_serializer::serialize(std::move(msg), *p_auth, m_error_handler);
        m_control_client.send_message(wire_msg);
    }

    std::optional<xmessage> xclient_zmq_impl::receive_on_shell(long timeout)
    {
        std::optional<zmq::multipart_t> wire_msg = m_shell_client.receive_message(timeout);

        if (wire_msg.has_value())
        {
            return deserialize(wire_msg.value());
        } else {
            return std::nullopt;
        }
    }

    std::optional<xmessage> xclient_zmq_impl::receive_on_control(long timeout)
    {
        std::optional<zmq::multipart_t> wire_msg = m_control_client.receive_message(timeout);

        if (wire_msg.has_value())
        {
            return deserialize(wire_msg.value());
        } else {
            return std::nullopt;
        }
    }

    void xclient_zmq_impl::register_shell_listener(const listener& l)
    {
        m_shell_listener = l;
    }

    void xclient_zmq_impl::register_control_listener(const listener& l)
    {
        m_control_listener = l;
    }

    std::size_t xclient_zmq_impl::iopub_queue_size() const
    {
        return m_iopub_client.iopub_queue_size();
    }

    std::optional<xmessage> xclient_zmq_impl::pop_iopub_message()
    {
        return m_iopub_client.pop_iopub_message();
    }

    void xclient_zmq_impl::register_iopub_listener(const listener& l)
    {
        m_iopub_listener = l;
    }

    void xclient_zmq_impl::register_kernel_status_listener(const kernel_status_listener& l)
    {
        m_kernel_status_listener = l;
    }

    void xclient_zmq_impl::connect()
    {
        p_messenger.connect();
    }

    void xclient_zmq_impl::stop_channels()
    {
        p_messenger.stop_channels();
    }

    void xclient_zmq_impl::notify_shell_listener(xmessage msg)
    {
        m_shell_listener(std::move(msg));
    }

    void xclient_zmq_impl::notify_control_listener(xmessage msg)
    {
        m_control_listener(std::move(msg));
    }

    void xclient_zmq_impl::notify_iopub_listener(xmessage msg)
    {
        m_iopub_listener(std::move(msg));
    }

    void xclient_zmq_impl::notify_kernel_dead(bool status)
    {
        m_kernel_status_listener(status);
    }

    void xclient_zmq_impl::poll(long timeout)
    {
        zmq::multipart_t wire_msg;
        zmq::pollitem_t items[]
            = { { m_shell_client.get_socket(), 0, ZMQ_POLLIN, 0 }, { m_control_client.get_socket(), 0, ZMQ_POLLIN, 0 } };

        while (true)
        {
            zmq::poll(&items[0], 2, std::chrono::milliseconds(timeout));
            try
            {
                if (items[0].revents & ZMQ_POLLIN)
                {
                    wire_msg.recv(m_shell_client.get_socket());
                    xmessage msg = deserialize(wire_msg);
                    notify_shell_listener(std::move(msg));
                    return;
                }
                if (items[1].revents & ZMQ_POLLIN)
                {
                    wire_msg.recv(m_control_client.get_socket());
                    xmessage msg = deserialize(wire_msg);
                    notify_control_listener(std::move(msg));
                    return;
                }
            }
            catch (std::exception& e)
            {
                std::cerr << e.what() << std::endl;
            }
        }
    }

    void xclient_zmq_impl::wait_for_message()
    {
        std::optional<xmessage> pending_message = pop_iopub_message();

        if (pending_message.has_value())
        {
            notify_iopub_listener(std::move(*pending_message));
        } else {
            poll(-1);
        }
    }

    void xclient_zmq_impl::start()
    {
        start_iopub_thread();
        // pass in desired timeout value below
        start_heartbeat_thread();
    }

    void xclient_zmq_impl::start_iopub_thread()
    {
        m_iopub_thread = std::move(xthread(&xiopub_client::run, p_iopub_client.get()));
    }

    void xclient_zmq_impl::start_heartbeat_thread(long timeout)
    {
        m_heartbeat_thread = std::move(xthread(&xheartbeat_client::run, p_heartbeat_client.get(), timeout));
    }

    xmessage xclient_zmq_impl::deserialize(zmq::multipart_t& wire_msg) const
    {
        return xzmq_serializer::deserialize(wire_msg, *p_auth);
    }

}