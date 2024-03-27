/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XEUS_CLIENT_ZMQ_HPP
#define XEUS_CLIENT_ZMQ_HPP

#include <optional>

#include <nlohmann/json.hpp>

#include "xeus/xeus_context.hpp"
#include "xeus/xkernel_configuration.hpp"
#include "xeus/xmessage.hpp"

#include "xeus-zmq.hpp"

namespace xeus
{
    class xclient_zmq_impl;

    class XEUS_ZMQ_API xclient_zmq
    {
    public:

        using listener = std::function<void(xmessage)>;

        explicit xclient_zmq(std::unique_ptr<xclient_zmq_impl> impl);
        ~xclient_zmq();

        void send_on_shell(xmessage msg);
        void send_on_control(xmessage msg);

        std::optional<xmessage> check_shell_answer();
        std::optional<xmessage> check_control_answer();

        void register_shell_listener(const listener& l);
        void register_control_listener(const listener& l);
        void register_iopub_listener(const listener& l);
        void register_heartbeat_listener(const listener& l);

        void notify_shell_listener(xmessage msg);
        void notify_control_listener(xmessage msg);
        void notify_iopub_listener(xmessage msg);
        void notify_heartbeat_listener(xmessage msg);

        std::size_t iopub_queue_size() const;
        std::optional<xmessage> pop_iopub_message();
        void connect();
        void stop_channels();
        void start();
        void wait_for_message();

    private:
        std::unique_ptr<xclient_zmq_impl> p_client_impl;
    };

    XEUS_ZMQ_API
    std::unique_ptr<xclient_zmq> make_xclient_zmq(xcontext& context,
                                              const xconfiguration& config,
                                              nl::json::error_handler_t eh = nl::json::error_handler_t::strict);
}

#endif