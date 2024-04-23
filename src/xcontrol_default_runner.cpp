/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "xeus-zmq/xcontrol_default_runner.hpp"

namespace xeus
{
    void xcontrol_default_runner::run_impl() 
    {
        m_request_stop = false;

        while (!m_request_stop)
        {
            auto msg = read_control();
            if (msg.has_value())
            {
                notify_control_listener(std::move(msg.value()));
            }
        }

        stop_channels();
    }

    void xcontrol_default_runner::stop_impl()
    {
        m_request_stop = true;
    }
}

