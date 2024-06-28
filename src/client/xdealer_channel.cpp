/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "xeus-zmq/xmiddleware.hpp"

#include "xdealer_channel.hpp"

namespace xeus
{

    xdealer_channel::xdealer_channel(zmq::context_t& context,
                                    const std::string& transport,
                                    const std::string& ip,
                                    const std::string& port)
        : m_socket(context, zmq::socket_type::dealer)
        , m_dealer_end_point("")
    {
        m_dealer_end_point = get_end_point(transport, ip, port);
        m_socket.connect(m_dealer_end_point);
    }

    xdealer_channel::~xdealer_channel()
    {
        m_socket.disconnect(m_dealer_end_point);
    }

    void xdealer_channel::send_message(zmq::multipart_t& message)
    {
        message.send(m_socket);
    }

    std::optional<zmq::multipart_t> xdealer_channel::receive_message(bool blocking)
    {
        zmq::multipart_t wire_msg;
        zmq::recv_flags flags = zmq::recv_flags::none;

        if (!blocking)
        {
            flags = zmq::recv_flags::dontwait;
        }

        if (wire_msg.recv(m_socket, static_cast<int>(flags)))
        {
            return wire_msg;
        }
        else
        {
            return std::nullopt;
        }
    }

    zmq::socket_t& xdealer_channel::get_socket()
    {
        return m_socket;
    }
}
