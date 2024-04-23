/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "xeus-zmq/xshell_default_runner.hpp"

namespace xeus
{
    void xshell_default_runner::run_impl()
    {
        while (true)
        {
            auto chan = poll_channels();
            if (auto msg = read_shell(chan))
            {
                notify_shell_listener(std::move(msg.value()));
            }
            else if (auto msg = read_controller(chan))
            {
                std::string val = std::move(msg.value());
                if (val == "stop")
                {
                    send_controller(std::move(val));
                    break;
                }
                else
                {
                    std::string rep = notify_internal_listener(std::move(val));
                    send_controller(std::move(rep));
                }
            }
        }
    }
}

