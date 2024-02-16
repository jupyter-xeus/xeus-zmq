/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XEUS_CLIENT_ZMQ_IMPL_HPP
#define XEUS_CLIENT_ZMQ_IMPL_HPP

#include "xshell_client.hpp"
#include "xclient_messenger.hpp"

#include <nlohmann/json.hpp>

namespace xeus
{
    class xclient_zmq_impl
    {
    public:

        virtual ~xclient_zmq_impl() = default;

        xclient_zmq_impl(const xclient_zmq_impl&) = delete;
        xclient_zmq_impl& operator=(const xclient_zmq_impl&) = delete;

        xclient_zmq_impl(xclient_zmq_impl&&) = delete;
        xclient_zmq_impl& operator=(xclient_zmq_impl&&) = delete;

        void connect_client_messenger();
        xclient_messenger& get_client_messenger();

        void start_channels();

        void send_shell(xmessage msg);

    protected:
        xclient_zmq_impl(zmq::context_t& context,
                        const std::string& user_name,
                        const xeus::xconfiguration& config);

    private:
        xclient_messenger p_messenger;

        xshell_client m_shell_client;
        // Other channel clients can be added here
    };
}

#endif