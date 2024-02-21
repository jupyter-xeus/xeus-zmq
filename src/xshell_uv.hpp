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

#include <string>
#include <memory> // std::shared_ptr

#ifndef UVW_AS_LIB
#define UVW_AS_LIB
#include <uvw.hpp>
#endif

#include "zmq.hpp"
#include "zmq_addon.hpp"

#include "xeus/xmessage.hpp"

namespace xeus
{
    class xserver_uv_shell_main;

    class xshell_uv
    {
    public:

        using listener = std::function<void(xmessage)>;

        xshell_uv(std::shared_ptr<uvw::loop> loop_ptr,
            zmq::context_t& context,
            const std::string& transport,
            const std::string& ip,
            const std::string& shell_port,
            const std::string& sdtin_port,
            xserver_uv_shell_main* server);

        ~xshell_uv();

        std::string get_shell_port() const;
        std::string get_stdin_port() const;

        void run();

        void send_shell(zmq::multipart_t& message);
        void send_stdin(zmq::multipart_t& message);
        void publish(zmq::multipart_t& message);
        void abort_queue(const listener& l, long polling_interval);

        void reply_to_controller(zmq::multipart_t& message);

    private:

        zmq::socket_t m_shell;
        zmq::socket_t m_stdin;
        zmq::socket_t m_publisher_pub;
        zmq::socket_t m_controller;
        xserver_uv_shell_main* p_server;
        std::shared_ptr<uvw::loop> p_loop;
    };
}

#endif

