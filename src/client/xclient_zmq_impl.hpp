/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XEUS_CLIENT_ZMQ_IMPL_HPP
#define XEUS_CLIENT_ZMQ_IMPL_HPP

#include <nlohmann/json.hpp>
#include "zmq.hpp"

#include "xeus/xeus_context.hpp"
#include "xeus/xkernel_configuration.hpp"
#include "xeus/xmessage.hpp"

#include "xeus-zmq/xthread.hpp"

#include "xdealer_channel.hpp"
#include "xiopub_client.hpp"
#include "xheartbeat_client.hpp"
#include "xclient_messenger.hpp"

namespace xeus
{
    class xauthentication;

    class xclient_zmq_impl
    {
    public:
        using listener = std::function<void(xmessage)>;
        using iopub_listener = std::function<void(xpub_message)>;
        using kernel_status_listener = std::function<void(bool)>;

        xclient_zmq_impl(zmq::context_t& context,
                    const xconfiguration& config,
                    nl::json::error_handler_t eh);

        ~xclient_zmq_impl();

        xclient_zmq_impl(const xclient_zmq_impl&) = delete;
        xclient_zmq_impl& operator=(const xclient_zmq_impl&) = delete;

        xclient_zmq_impl(xclient_zmq_impl&&) = delete;
        xclient_zmq_impl& operator=(xclient_zmq_impl&&) = delete;

        // shell channel
        void send_on_shell(xmessage msg);
        std::optional<xmessage> receive_on_shell(bool blocking);
        void register_shell_listener(const listener& l);

        // control channel
        void send_on_control(xmessage msg);
        std::optional<xmessage> receive_on_control(bool blocking);
        void register_control_listener(const listener& l);

        // iopub channel
        std::size_t iopub_queue_size() const;
        std::optional<xpub_message> pop_iopub_message();
        void register_iopub_listener(const iopub_listener& l);

        // heartbeat channel
        void register_kernel_status_listener(const kernel_status_listener& l);

        // client messenger
        void connect();
        void stop_channels();

        void wait_for_message();
        void start();

        xmessage deserialize(zmq::multipart_t& wire_msg) const;
        xpub_message deserialize_iopub(zmq::multipart_t& wire_msg) const;

    private:

        void start_iopub_thread();
        void start_heartbeat_thread();
        void poll(long timeout);

        void notify_shell_listener(xmessage msg);
        void notify_control_listener(xmessage msg);
        void notify_iopub_listener(xpub_message msg);
        void notify_kernel_dead(bool status);

        using authentication_ptr = std::unique_ptr<xauthentication>;
        authentication_ptr p_auth;

        xdealer_channel m_shell_client;
        xdealer_channel m_control_client;
        xiopub_client m_iopub_client;
        xheartbeat_client m_heartbeat_client;

        xclient_messenger p_messenger;

        nl::json::error_handler_t m_error_handler;

        listener m_shell_listener;
        listener m_control_listener;
        iopub_listener m_iopub_listener;

        xthread m_iopub_thread;
        xthread m_heartbeat_thread;
    };
}

#endif
