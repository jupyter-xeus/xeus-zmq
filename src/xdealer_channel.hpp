/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XEUS_DEALER_CHANNEL_HPP
#define XEUS_DEALER_CHANNEL_HPP

#include "zmq.hpp"
#include "zmq_addon.hpp"

#include "nlohmann/json.hpp"
#include "xeus/xkernel_configuration.hpp"

namespace xeus
{

    class xdealer_channel
    {
    public:

        xdealer_channel(zmq::context_t& context,
                        const xeus::xconfiguration& config);

        ~xdealer_channel();

        void send_message(zmq::multipart_t& message);
        std::optional<zmq::multipart_t> receive_message(long timeout);

        zmq::socket_t& get_socket();

    private:

        zmq::socket_t m_socket;
        std::string m_end_point;
    };
}

#endif