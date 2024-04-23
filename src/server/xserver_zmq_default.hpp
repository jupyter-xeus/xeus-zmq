/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XSERVER_ZMQ_DEFAULT_HPP
#define XSERVER_ZMQ_DEFAULT_HPP

#include "xeus-zmq/xserver_zmq.hpp"

namespace xeus
{
    class xserver_zmq_default final : public xserver_zmq
    {
    public:

        xserver_zmq_default(xcontext& context,
                            const xconfiguration& config,
                            nl::json::error_handler_t eh);

        ~xserver_zmq_default() override = default;

    private:

        void start_impl(xpub_message msg) override;
        void stop_impl() override;
    };
}

#endif

