/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "xeus-zmq/xshell_runner.hpp"
#include "xeus-zmq/xserver_zmq_split.hpp"

namespace xeus
{
    void xshell_runner::register_server(xserver_zmq_split& server)
    {
        p_server = &server;
    }

    void xshell_runner::run()
    {
        run_impl();
    }

    fd_t xshell_runner::get_shell_fd() const
    {
        return p_server->get_shell_fd();
    }

    fd_t xshell_runner::get_shell_controller_fd() const
    {
        return p_server->get_shell_controller_fd();
    }

    auto xshell_runner::poll_channels(long timeout) -> optional_channel
    {
        return p_server->poll_shell_channels(timeout);
    }

    std::optional<xmessage> xshell_runner::read_shell(int flags)
    {
        return p_server->read_shell(flags);
    }

    std::optional<xmessage> xshell_runner::read_shell(optional_channel chan, int flags)
    {
        if (chan.has_value() && chan.value() == channel::SHELL)
        {
            return read_shell(flags);
        }
        return std::nullopt;
    }

    std::optional<std::string> xshell_runner::read_controller(int flags)
    {
        return p_server->read_shell_controller(flags);
    }

    std::optional<std::string> xshell_runner::read_controller(optional_channel chan, int flags)
    {
        if (chan.has_value() && chan.value() == channel::CONTROL)
        {
            return read_controller(flags);
        }
        return std::nullopt;
    }

    void xshell_runner::send_controller(std::string message)
    {
        p_server->send_shell_controller(std::move(message));
    }

    void xshell_runner::notify_shell_listener(xmessage message)
    {
        p_server->notify_shell_listener(std::move(message));
    }

    std::string xshell_runner::notify_internal_listener(std::string message)
    {
        return p_server->notify_internal_listener(std::move(message));
    }
}

