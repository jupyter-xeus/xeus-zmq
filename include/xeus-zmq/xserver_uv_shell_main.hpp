/***************************************************************************
* Copyright (c) 2024, Isabel Paredes                                       *
* Copyright (c) 2024, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XEUS_SERVER_UV_SHELL_MAIN_HPP
#define XEUS_SERVER_UV_SHELL_MAIN_HPP

#include "xeus/xeus_context.hpp"
#include "xeus/xkernel_configuration.hpp"

#include "xeus-zmq.hpp"
#include "xserver_zmq_split.hpp"

namespace xeus
{
    class XEUS_ZMQ_API xserver_uv_shell_main : public xserver_zmq_split
    {
    public:

        xserver_uv_shell_main(zmq::context_t& context,
                           const xconfiguration& config,
                           nl::json::error_handler_t he);
        virtual ~xserver_uv_shell_main();

    private:

        void start_server(zmq::multipart_t& wire_msg) override;
    };

    XEUS_ZMQ_API
    std::unique_ptr<xserver> make_xserver_uv_shell_main(
        xcontext& context,
        const xconfiguration& config,
        nl::json::error_handler_t eh = nl::json::error_handler_t::strict);
}

#endif // XEUS_SERVER_UV_SHELL_MAIN_HPP
