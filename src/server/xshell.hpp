/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XEUS_SHELL_HPP
#define XEUS_SHELL_HPP

#include <optional>
#include <string>

#include "zmq.hpp"
#include "zmq_addon.hpp"

#include "xeus/xmessage.hpp"
#include "xeus/xserver.hpp"

#include "xeus-zmq/xmiddleware.hpp"

namespace xeus
{
    class xzmq_server_split_impl;

    class xshell
    {
    public:

        using listener = std::function<void(xmessage)>;

        xshell(zmq::context_t& context,
               const std::string& transport,
               const std::string& ip,
               const std::string& shell_port,
               const std::string& stdin_port,
               xserver_zmq_split_impl* server);
 
        std::string get_shell_port() const;
        std::string get_stdin_port() const;

        fd_t get_shell_fd() const;
        fd_t get_controller_fd() const;

        std::optional<channel> poll_channels(long timeout);
        std::optional<xmessage> read_shell(int flags);
        std::optional<std::string> read_controller(int flags);

        void send_shell(zmq::multipart_t& message);
        std::optional<xmessage> send_stdin(zmq::multipart_t& message);
        void send_controller(std::string message);

        void publish(zmq::multipart_t& message);
        void abort_queue(const listener& l, long polling_interval);

    private:

        zmq::socket_t m_shell;
        zmq::socket_t m_stdin;
        zmq::socket_t m_publisher_pub;
        zmq::socket_t m_controller;
        xserver_zmq_split_impl* p_server;
    };
}

#endif

