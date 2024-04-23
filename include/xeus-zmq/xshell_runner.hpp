/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XEUS_SHELL_RUNNER_HPP
#define XEUS_SHELL_RUNNER_HPP

#include <optional>
#include <string>

#include "xeus/xmessage.hpp"
#include "xeus/xserver.hpp"

#include "xeus-zmq.hpp"
#include "xmiddleware.hpp"

namespace xeus
{
    class xserver_zmq_split;

    class XEUS_ZMQ_API xshell_runner
    {
    public:

        virtual ~xshell_runner() = default;

        xshell_runner(const xshell_runner&) = delete;
        xshell_runner& operator=(const xshell_runner&) = delete;
        xshell_runner(xshell_runner&&) = delete;
        xshell_runner& operator=(xshell_runner&&) = delete;

        void register_server(xserver_zmq_split& server);
        void run();

    protected:

        xshell_runner() = default;

        fd_t get_shell_fd() const;
        fd_t get_shell_controller_fd() const;

        using optional_channel = std::optional<channel>;

        optional_channel poll_channels(long timeout = -1);
        std::optional<xmessage> read_shell(int flags = 0);
        std::optional<xmessage> read_shell(optional_channel chan, int flags = 0);
        std::optional<std::string> read_controller(int flags = 0);
        std::optional<std::string> read_controller(optional_channel chan, int flags = 0);

        void send_controller(std::string message);

        void notify_shell_listener(xmessage message);
        std::string notify_internal_listener(std::string message);

    private:

        virtual void run_impl() = 0;

        xserver_zmq_split* p_server = nullptr;
    };
}

#endif

