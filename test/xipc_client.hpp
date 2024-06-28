/***************************************************************************
* Copyright (c) 2016, Johan Mabille and Sylvain Corlay                     *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XEUS_IPC_CLIENT_HPP
#define XEUS_IPC_CLIENT_HPP

#include "xeus-zmq/xclient_zmq.hpp"

namespace xeus
{
    class xipc_client
    {
    public:
        using client_ptr = std::unique_ptr<xclient_zmq>;

        xipc_client(xcontext& context, const xconfiguration& config);

        void send_on_shell(xmessage msg);
        std::optional<xmessage> receive_on_shell(bool blocking = true);

    private:
        client_ptr p_client;
    };
}

#endif
