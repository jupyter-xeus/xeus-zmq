/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "xdealer_channel.hpp"

namespace xeus
{

    xdealer_channel::xdealer_channel(zmq::context_t& context,
                                 const xeus::xconfiguration& config)
        : m_socket(context, zmq::socket_type::dealer)
        , m_end_point("")
    {
        m_end_point = xeus::get_end_point(config.m_transport, config.m_ip, config.m_shell_port);

        m_socket.connect(m_end_point);
    }

    xdealer_channel::~xdealer_channel()
    {
        m_socket.disconnect(m_end_point);
    }

    void xdealer_channel::send_message(zmq::multipart_t& message)
    {
        message.send(m_socket);
    }

    std::optional<zmq::multipart_t> xdealer_channel::receive_message(long timeout)
    {
        zmq::multipart_t wire_msg;
        m_socket.setsockopt(ZMQ_RCVTIMEO, timeout);
        if (wire_msg.recv(m_socket, zmq::recv_flags::none))
        {
            return wire_msg;
        } else {
            return std::nullopt;
        }
    }

    zmq::socket& xdealer_channel::get_socket()
    {
        return m_socket;
    }
}