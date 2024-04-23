/***************************************************************************
* Copyright (c) 2018, Martin Renou, Johan Mabille, Sylvain Corlay, and     *
* Wolf Vollprecht                                                          *
* Copyright (c) 2018, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XEUS_DEBUGGER_MIDDLEWARE_HPP
#define XEUS_DEBUGGER_MIDDLEWARE_HPP

#include <string>

#include "zmq.hpp"
#include "xeus/xeus_context.hpp"

namespace xeus
{
    class xdebugger_middleware
    {
    public:

        explicit xdebugger_middleware(xcontext& context);

        void bind_sockets(const std::string& header_end_point,
                          const std::string& request_end_point);

        void unbind_sockets(const std::string& header_end_point,
                            const std::string& request_end_point);

        std::string send_recv_header(const std::string& header);
        std::string send_recv_request(const std::string& request);

    private:

        std::string send_recv(const std::string& msg, zmq::socket_t& socket);

        zmq::socket_t m_header_socket;
        zmq::socket_t m_request_socket;
    };
}

#endif

