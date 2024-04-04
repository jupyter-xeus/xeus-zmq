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

#include <functional>

#include "zmq.hpp"

#include "xeus/xkernel_configuration.hpp"

namespace xeus
{
    class xheartbeat_client
    {
    public:

        using kernel_status_listener = std::function<void(bool)>;

        xheartbeat_client(zmq::context_t& context,
                        const xeus::xconfiguration& config);

        ~xheartbeat_client();

        void run(long timeout);

        void register_kernel_status_listener(const kernel_status_listener& l);
        void notify_kernel_dead(bool status);

    private:
        void send_heartbeat_message();
        bool wait_for_answer(long timeout);

        zmq::socket_t m_heartbeat;
        zmq::socket_t m_controller;

        kernel_status_listener m_kernel_status_listener;
    };
}

#endif