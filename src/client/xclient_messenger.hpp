/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XEUS_CLIENT_MESSENGER_HPP
#define XEUS_CLIENT_MESSENGER_HPP

#include <zmq.hpp>

namespace xeus
{
    class xclient_messenger
    {
    public:
        explicit xclient_messenger(zmq::context_t& context);
        virtual ~xclient_messenger();

        void connect();
        void stop_channels();

    private:
        zmq::socket_t m_iopub_controller;
        zmq::socket_t m_heartbeat_controller;
    };
}

#endif