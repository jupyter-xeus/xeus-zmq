/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XEUS_MIDDLEWARE_HPP
#define XEUS_MIDDLEWARE_HPP

#include <string>

#include "xeus-zmq.hpp"

namespace xeus
{

// Duplicate definition for cppzmq
#if defined _WIN64
    using fd_t = __int64;
#elif defined _WIN32
    using fd_t = unsinged int;
#else
    using fd_t = int;
#endif

    XEUS_ZMQ_API
    std::string get_controller_end_point(const std::string& channel);
    
    XEUS_ZMQ_API
    std::string get_publisher_end_point();

    XEUS_ZMQ_API
    std::string get_end_point(const std::string& transport,
                              const std::string& ip,
                              const std::string& port);

    XEUS_ZMQ_API
    int get_socket_linger();

    XEUS_ZMQ_API
    std::string find_free_port(std::size_t max_tries = 100, int start = 49152, int stop = 65536);
}

#endif
