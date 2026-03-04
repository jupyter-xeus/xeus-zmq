/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XEUS_HANSHAKING_HPP
#define XEUS_HANSHAKING_HPP

#include "zmq_addon.hpp"

#include "xeus/xkernel_configuration.hpp"
#include "xeus/xserver.hpp"

#include "../common/xauthentication.hpp"

namespace xeus
{

    xkernel_configuration get_kernel_configuration(const xconfiguration& config);

    void send_connection_info(
        zmq::context_t& context,
        const xregistration_configuration& regis_config,
        const xkernel_configuration& kernel_config,
        const xauthentication& auth,
        nl::json::error_handler_t error_handler);
}

#endif
