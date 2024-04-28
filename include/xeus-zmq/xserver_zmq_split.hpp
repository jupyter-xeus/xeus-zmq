/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XEUS_SERVER_ZMQ_SPLIT_HPP
#define XEUS_SERVER_ZMQ_SPLIT_HPP

#include <memory>

#include "xeus/xeus_context.hpp"
#include "xeus/xkernel_configuration.hpp"
#include "xeus/xserver.hpp"

#include "xeus-zmq.hpp"
#include "xcontrol_runner.hpp"
#include "xmiddleware.hpp"
#include "xshell_runner.hpp"
#include "xthread.hpp"

namespace xeus
{
    class xserver_zmq_split_impl;

    class XEUS_ZMQ_API xserver_zmq_split : public xserver
    {
    public:

        ~xserver_zmq_split() override;

        // API for xcontrol_runner
        using xserver::notify_control_listener;

        fd_t get_control_fd() const;
        std::optional<xmessage> read_control(int flags);
        void send_control_message(xmessage msg);
        void stop_channels();

        // API for xshell_runner
        using xserver::notify_shell_listener;
        std::string notify_internal_listener(std::string message);

        fd_t get_shell_fd() const;
        fd_t get_shell_controller_fd() const;

        std::optional<channel> poll_shell_channels(long timeout);
        std::optional<xmessage> read_shell(int flags);
        void send_shell_message(xmessage msg);
        std::optional<std::string> read_shell_controller(int flags);
        void send_shell_controller(std::string message);
   
    protected:

        using control_runner_ptr = std::unique_ptr<xcontrol_runner>;
        using shell_runner_ptr = std::unique_ptr<xshell_runner>;

        xserver_zmq_split(xcontext& context,
                          const xconfiguration& config,
                          nl::json::error_handler_t eh,
                          control_runner_ptr control,
                          shell_runner_ptr shell);

        // API for inheriting classes
        void start_publisher_thread();
        void start_heartbeat_thread();
        
        void start_control_thread();
        void run_control();

        void start_shell_thread();
        void run_shell();
        
    private:

        // Implementation of xserver virtual methods
        xcontrol_messenger& get_control_messenger_impl() override;

        void send_shell_impl(xmessage msg) override;
        void send_control_impl(xmessage msg) override;
        void send_stdin_impl(xmessage msg) override;
        void publish_impl(xpub_message msg, channel c) override;

        void stop_impl() override;
        void abort_queue_impl(const listener& l, long polling_interval) override;
        void update_config_impl(xconfiguration& config) const override;

        std::unique_ptr<xserver_zmq_split_impl> p_impl;
        control_runner_ptr p_control_runner;
        shell_runner_ptr p_shell_runner;

        xthread m_control_thread;
        xthread m_shell_thread;

        nl::json::error_handler_t m_error_handler;
    };

    XEUS_ZMQ_API
    std::unique_ptr<xserver>
    make_xserver_control_main(xcontext& context,
                              const xconfiguration& config,
                              nl::json::error_handler_t eh = nl::json::error_handler_t::strict);
    
    XEUS_ZMQ_API
    std::unique_ptr<xserver>
    make_xserver_control(xcontext& context,
                         const xconfiguration& config,
                         nl::json::error_handler_t eh,
                         std::unique_ptr<xcontrol_runner> control,
                         std::unique_ptr<xshell_runner> shell);


    XEUS_ZMQ_API
    std::unique_ptr<xserver>
    make_xserver_shell_main(xcontext& context,
                            const xconfiguration& config,
                            nl::json::error_handler_t eh = nl::json::error_handler_t::strict);

    XEUS_ZMQ_API
    std::unique_ptr<xserver>
    make_xserver_shell(xcontext& context,
                       const xconfiguration& config,
                       nl::json::error_handler_t eh,
                       std::unique_ptr<xcontrol_runner> control,
                       std::unique_ptr<xshell_runner> shell);
}

#endif

