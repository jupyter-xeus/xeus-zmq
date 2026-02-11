/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XHANDSHAKE_CLIENT_ZMQ_HPP
#define XHANDSHAKE_CLIENT_ZMQ_HPP

#include <string>

#include "xeus/xeus_context.hpp"
#include "xeus/xkernel_configuration.hpp"

#include "xeus-zmq.hpp"

namespace xeus
{

    class xhandshake_client_zmq_impl;

    class XEUS_ZMQ_API xhandshake_client_zmq
    {
    public:

        xhandshake_client_zmq(xcontext& context, const xregistration_configuration& config);
        ~xhandshake_client_zmq();

        std::string get_registration_port() const;

        xkernel_configuration wait_for_configuration();

    private:

        std::unique_ptr<xhandshake_client_zmq_impl> p_client_impl;
    };
}

#endif

