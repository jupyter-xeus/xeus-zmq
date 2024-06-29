/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "nlohmann/json.hpp"
#include "xeus-zmq/xmiddleware.hpp"
#include "xclient_messenger.hpp"

namespace nl = nlohmann;

namespace xeus
{
    xclient_messenger::xclient_messenger(zmq::context_t& context)
        : m_iopub_controller(context, zmq::socket_type::req)
        , m_heartbeat_controller(context, zmq::socket_type::req)
    {
    }

    xclient_messenger::~xclient_messenger()
    {
    }

    void xclient_messenger::connect()
    {
        m_iopub_controller.set(zmq::sockopt::linger, get_socket_linger());
        m_iopub_controller.connect(get_controller_end_point("iopub"));

        m_heartbeat_controller.set(zmq::sockopt::linger, get_socket_linger());
        m_heartbeat_controller.connect(get_controller_end_point("heartbeat"));
    }

    void xclient_messenger::stop_channels()
    {
        zmq::message_t stop_msg("stop", 4);
        zmq::message_t response;

        // Wait for iopub answer
        m_iopub_controller.send(stop_msg, zmq::send_flags::none);
        (void)m_iopub_controller.recv(response);

        // Wait for heartbeat answer
        m_heartbeat_controller.send(stop_msg, zmq::send_flags::none);
        (void)m_heartbeat_controller.recv(response);
    }
}
