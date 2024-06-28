/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XEUS_IOPUB_CLIENT_HPP
#define XEUS_IOPUB_CLIENT_HPP

#include <queue>
#include <mutex>

#include "zmq.hpp"

#include "xeus/xmessage.hpp"
#include "xeus/xkernel_configuration.hpp"

namespace xeus
{
    class xclient_zmq_impl;

    class xiopub_client
    {
    public:

        xiopub_client(zmq::context_t& context,
                      const xeus::xconfiguration& config,
                      xclient_zmq_impl* client);

        ~xiopub_client();

        std::size_t iopub_queue_size() const;
        std::optional<xpub_message> pop_iopub_message();

        void run();

    private:
        zmq::socket_t m_iopub;
        zmq::socket_t m_controller;

        std::string m_iopub_end_point;

        std::queue<xpub_message> m_message_queue;
        mutable std::mutex m_queue_mutex;

        xclient_zmq_impl* p_client_impl;
    };
}

#endif
