/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XEUS_CLIENT_ZMQ_HPP
#define XEUS_CLIENT_ZMQ_HPP

#include <nlohmann/json.hpp>

#include "xeus-zmq.hpp"

namespace xeus
{
    class xclient_zmq_impl;

    class XEUS_ZMQ_API xclient_zmq
    {
    public:
        xclient_zmq::xclient_zmq(xcontext& context,
                                const std::string& user_name,
                                const xeus::xconfiguration& config);
        ~xclient_zmq();

        void send_shell(xmessage msg);

        nl::json check_shell_answer();

    private:
        std::unique_ptr<xclient_zmq_impl> p_client_impl;
    };
}

#endif