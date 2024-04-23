/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XEUS_SERVER_ZMQ_SPLIT_IMPL_HPP
#define XEUS_SERVER_ZMQ_SPLIT_IMPL_HPP

#include <memory>

#include "zmq.hpp"
#include "zmq_addon.hpp"

#include "xeus/xkernel_configuration.hpp"

#include "xeus-zmq/xmiddleware.hpp"
#include "xeus-zmq/xthread.hpp"

#include "../common/xauthentication.hpp"
#include "xcontrol.hpp"
#include "xheartbeat.hpp"
#include "xpublisher.hpp"
#include "xshell.hpp"

namespace xeus
{

    class xserver_zmq_split_impl
    {
    public:

        using listener = std::function<void(xmessage)>;
        
        xserver_zmq_split_impl(zmq::context_t& context,
                               const xconfiguration& config,
                               nl::json::error_handler_t eh);

        void start_heartbeat_thread();
        void start_publisher_thread();
        void stop_channels();

        fd_t get_shell_fd() const;
        fd_t get_shell_controller_fd() const;
        fd_t get_control_fd() const;

        std::optional<channel> poll_shell_channels(long timeout);
        std::optional<xmessage> read_shell(int flags);
        std::optional<std::string> read_shell_controller(int flags);
        std::optional<xmessage> read_control(int flags);

        xcontrol_messenger& get_control_messenger();

        void send_shell(xmessage message);
        void send_shell_controller(std::string message);
        void send_control(xmessage message);
        std::optional<xmessage> send_stdin(xmessage message);
        void publish(xpub_message message, channel c);

        void abort_queue(const listener& l, long polling_interval);
        void update_config(xconfiguration& config) const;

        xmessage deserialize(zmq::multipart_t& wire_msg) const;
        zmq::multipart_t serialize_iopub(xpub_message&& msg);
    
    private:

        using authentication_ptr = std::unique_ptr<xauthentication>;
        authentication_ptr p_auth;

        xcontrol m_control;
        xheartbeat m_heartbeat;
        xpublisher m_publisher;
        xshell m_shell;

        xthread m_hb_thread;
        xthread m_iopub_thread;

        nl::json::error_handler_t m_error_handler;
    };
}

#endif

