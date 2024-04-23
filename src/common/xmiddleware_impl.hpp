/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XEUS_MIDDLEWARE_IMPL_HPP
#define XEUS_MIDDLEWARE_IMPL_HPP

#include <string>
#include "zmq.hpp"

#include "xeus-zmq/xmiddleware.hpp"

namespace xeus
{
    void init_socket(zmq::socket_t& socket,
                     const std::string& transport,
                     const std::string& ip,
                     const std::string& port);

    void init_socket(zmq::socket_t& socket, const std::string& end_point);

    std::string get_socket_port(const zmq::socket_t& socket);
}

#endif
