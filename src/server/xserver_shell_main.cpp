/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "xeus-zmq/xcontrol_default_runner.hpp"
#include "xeus-zmq/xshell_default_runner.hpp"
#include "xserver_shell_main.hpp"

namespace xeus
{
    xserver_shell_main::xserver_shell_main(xcontext& context,
                                           const xconfiguration& config,
                                           nl::json::error_handler_t eh,
                                           control_runner_ptr control,
                                           shell_runner_ptr shell)
        : xserver_zmq_split(
            context,
            config,
            eh,
            std::move(control),
            std::move(shell)
        )
    {
    }
    
    void xserver_shell_main::start_impl(xpub_message message)
    {
        xserver_zmq_split::start_publisher_thread();
        xserver_zmq_split::start_heartbeat_thread();
        xserver_zmq_split::start_control_thread();

        xserver_zmq_split::publish(std::move(message), channel::SHELL);
        xserver_zmq_split::run_shell();
    }

    std::unique_ptr<xserver>
    make_xserver_shell_main(xcontext& context,
                            const xconfiguration& config,
                            nl::json::error_handler_t eh)
    {
        return make_xserver_shell
        (
            context,
            config,
            eh,
            std::make_unique<xcontrol_default_runner>(),
            std::make_unique<xshell_default_runner>()
        );
    }

    std::unique_ptr<xserver>
    make_xserver_shell(xcontext& context,
                       const xconfiguration& config,
                       nl::json::error_handler_t eh,
                       std::unique_ptr<xcontrol_runner> control,
                       std::unique_ptr<xshell_runner> shell)
    {
        return std::make_unique<xserver_shell_main>
        (
            context,
            config,
            eh,
            std::move(control),
            std::move(shell)
        );
    }
}

