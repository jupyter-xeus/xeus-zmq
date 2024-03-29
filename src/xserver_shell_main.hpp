/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XEUS_SERVER_SHELL_MAIN_HPP
#define XEUS_SERVER_SHELL_MAIN_HPP

#include "xeus/xeus_context.hpp"
#include "xeus/xkernel_configuration.hpp"

#include "xeus-zmq/xeus-zmq.hpp"
#include "xserver_zmq_split.hpp"

namespace xeus
{
    class xserver_shell_main : public xserver_zmq_split
    {
    public:

        xserver_shell_main(zmq::context_t& context,
                           const xconfiguration& config,
                           nl::json::error_handler_t he);
        virtual ~xserver_shell_main();

    private:

        void start_server(zmq::multipart_t& wire_msg) override;
    };

}

#endif

