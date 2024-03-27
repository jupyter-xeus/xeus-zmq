/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "xheartbeat_client.hpp"

#include "xeus-zmq/xmiddleware.hpp"

namespace xeus
{

    xheartbeat_client::xheartbeat_client(zmq::context_t& context,
                                 const xeus::xconfiguration& config)
        : m_heartbeat(context, zmq::socket_type::sub)
        , m_controller(context, zmq::socket_type::rep)
    {
        m_heartbeat.connect(get_end_point(config.m_transport, config.m_ip, config.m_hb_port));
        init_socket(m_controller, get_controller_end_point("heartbeat"));
    }

    xheartbeat_client::~xheartbeat_client()
    {
    }
}