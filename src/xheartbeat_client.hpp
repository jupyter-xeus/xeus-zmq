/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XEUS_HEARTBEAT_CLIENT_HPP
#define XEUS_HEARTBEAT_CLIENT_HPP

#include "zmq.hpp"

#include "xeus/xkernel_configuration.hpp"

namespace xeus
{
    class xclient_zmq_impl;

    class xheartbeat_client
    {
    public:

        xheartbeat_client(zmq::context_t& context,
                        const xeus::xconfiguration& config);

        ~xheartbeat_client();

        void run(long timeout);

    private:
        void send_heartbeat_message();
        bool wait_for_answer(long timeout);

        zmq::socket_t m_heartbeat;
        zmq::socket_t m_controller;

        xclient_zmq_impl* p_client_impl;
    };
}

#endif