/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "xeus-zmq/xcontrol_runner.hpp"
#include "xeus-zmq/xserver_zmq_split.hpp"

namespace xeus
{

    void xcontrol_runner::register_server(xserver_zmq_split& server)
    {
        p_server = &server;
    }

    void xcontrol_runner::run()
    {
        run_impl();
    }

    void xcontrol_runner::stop()
    {
        stop_impl();
    }

    fd_t xcontrol_runner::get_control_fd() const
    {
        return p_server->get_control_fd();
    }

    std::optional<xmessage> xcontrol_runner::read_control(int flags)
    {
        return p_server->read_control(flags);
    }

    void xcontrol_runner::stop_channels()
    {
        p_server->stop_channels();
    }


    void xcontrol_runner::notify_control_listener(xmessage message)
    {
        p_server->notify_control_listener(std::move(message));
    }
}

