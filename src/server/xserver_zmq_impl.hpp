/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XEUS_SERVER_ZMQ_IMPL_HPP
#define XEUS_SERVER_ZMQ_IMPL_HPP

#include <memory>

#include "zmq.hpp"
#include "zmq_addon.hpp"

#include "xeus/xeus_context.hpp"
#include "xeus/xkernel_configuration.hpp"
#include "xeus/xserver.hpp"

#include "xeus-zmq/xeus-zmq.hpp"
#include "xeus-zmq/xthread.hpp"

#include "../common/xauthentication.hpp"
#include "xpublisher.hpp"
#include "xheartbeat.hpp"
#include "xtrivial_messenger.hpp"

namespace xeus
{
    class xserver_zmq_impl
    {
    public:

        using listener = std::function<void(xmessage)>;
        using internal_listener = xtrivial_messenger::listener;

        xserver_zmq_impl(zmq::context_t& context,
                         const xconfiguration& config,
                         nl::json::error_handler_t eh,
                         internal_listener listener);

        void start_publisher_thread();
        void start_heartbeat_thread();
        void stop_channels();

        void set_request_stop(bool stop);
        bool is_stopped() const;

        using message_channel = std::pair<xmessage, channel>;
        std::optional<message_channel> poll_channels(long timeout);

        xcontrol_messenger& get_control_messenger();

        void send_shell(xmessage message);
        void send_control(xmessage message);
        std::optional<xmessage> send_stdin(xmessage message);
        void publish(xpub_message message, channel c);

        void abort_queue(const listener& l, long polling_interval);
        void update_config(xconfiguration& config) const;

        zmq::multipart_t serialize_iopub(xpub_message&& msg);

    private:

        zmq::socket_t m_shell;
        zmq::socket_t m_controller;
        zmq::socket_t m_stdin;
        zmq::socket_t m_publisher_pub;
        zmq::socket_t m_publisher_controller;
        zmq::socket_t m_heartbeat_controller;

        using authentication_ptr = std::unique_ptr<xauthentication>;
        authentication_ptr p_auth;

        xpublisher m_publisher;
        xheartbeat m_heartbeat;

        xthread m_iopub_thread;
        xthread m_hb_thread;

        xtrivial_messenger m_messenger;

        nl::json::error_handler_t m_error_handler;

        bool m_request_stop;
    };
}

#endif

