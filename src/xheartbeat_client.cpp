/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "xheartbeat_client.hpp"
#include "xclient_zmq_impl.hpp"

#include "xeus-zmq/xmiddleware.hpp"

namespace xeus
{

    xheartbeat_client::xheartbeat_client(zmq::context_t& context,
                                 const xeus::xconfiguration& config)
        : m_heartbeat(context, zmq::socket_type::req)
        , m_controller(context, zmq::socket_type::rep)
    {
        m_heartbeat.connect(get_end_point(config.m_transport, config.m_ip, config.m_hb_port));
        init_socket(m_controller, get_controller_end_point("heartbeat"));
    }

    xheartbeat_client::~xheartbeat_client()
    {
    }

    void xheartbeat_client::send_heartbeat_message()
    {
        zmq::message_t ping_msg("ping", 4);
        m_heartbeat.send(ping_msg, zmq::send_flags::none);
    }

    void xheartbeat_client::run()
    {
        bool stop = false;
        while(!stop)
        {
            send_heartbeat_message();
            // TODO
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
}