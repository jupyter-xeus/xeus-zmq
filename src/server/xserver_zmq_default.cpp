/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "xserver_zmq_default.hpp"

namespace xeus
{
    xserver_zmq_default::xserver_zmq_default(xcontext& context,
                                             const xconfiguration& config,
                                             nl::json::error_handler_t eh)
        : xserver_zmq(context, config, eh)
    {
    }

    void xserver_zmq_default::start_impl(xpub_message msg)
    {
        start_publisher_thread();
        start_heartbeat_thread();

        publish(std::move(msg), channel::SHELL);

        while(!is_stopped())
        {
            auto msg = poll_channels(-1);
            if (msg)
            {
                if (msg.value().second == channel::SHELL)
                {
                    notify_shell_listener(std::move(msg.value().first));
                }
                else
                {
                    notify_control_listener(std::move(msg.value().first));
                }
            }
        }

        stop_channels();
    }

    void xserver_zmq_default::stop_impl()
    {
        set_request_stop(true);
    }

    std::unique_ptr<xserver> make_xserver_default(xcontext& context,
                                                  const xconfiguration& config,
                                                  nl::json::error_handler_t eh)
    {
        return std::make_unique<xserver_zmq_default>(context, config, eh);
    }
}

