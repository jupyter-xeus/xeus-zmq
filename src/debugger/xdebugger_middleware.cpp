/***************************************************************************
* Copyright (c) 2018, Martin Renou, Johan Mabille, Sylvain Corlay, and     *
* Wolf Vollprecht                                                          *
* Copyright (c) 2018, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "xeus-zmq/xmiddleware.hpp"

#include "xdebugger_middleware.hpp"

namespace xeus
{
    xdebugger_middleware::xdebugger_middleware(xcontext& context)
        : m_header_socket(context.get_wrapped_context<zmq::context_t>(), zmq::socket_type::req)
        , m_request_socket(context.get_wrapped_context<zmq::context_t>(), zmq::socket_type::req)
    {
        m_header_socket.set(zmq::sockopt::linger, xeus::get_socket_linger());
        m_request_socket.set(zmq::sockopt::linger, xeus::get_socket_linger());
    }

    void xdebugger_middleware::bind_sockets(const std::string& header_end_point,
                                            const std::string& request_end_point)
    {
        m_header_socket.bind(header_end_point);
        m_request_socket.bind(request_end_point);
    }

    void xdebugger_middleware::unbind_sockets(const std::string& header_end_point,
                                              const std::string& request_end_point)
    {
        m_header_socket.unbind(header_end_point);
        m_request_socket.unbind(request_end_point);
    }

    std::string xdebugger_middleware::send_recv_header(const std::string& header)
    {
        return send_recv(header, m_header_socket);
    }

    std::string xdebugger_middleware::send_recv_request(const std::string& request)
    {
        return send_recv(request, m_request_socket);
    }

    std::string xdebugger_middleware::send_recv(const std::string& msg, zmq::socket_t& socket)
    {
        zmq::message_t raw_msg(msg.c_str(), msg.length());
        socket.send(raw_msg, zmq::send_flags::none);
        zmq::message_t raw_reply;
        (void)socket.recv(raw_reply);
        return std::string(raw_reply.data<const char>(), raw_reply.size());
    }
}

