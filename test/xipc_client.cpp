/***************************************************************************
* Copyright (c) 2016, Johan Mabille and Sylvain Corlay                     *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "xipc_client.hpp"

namespace xeus
{

    xipc_client::xipc_client(xcontext& context, const xconfiguration& config)
        : p_client(make_xclient_zmq(context, config))
    {
    }

    void xipc_client::send_on_shell(const xmessage& msg)
    {
        p_client->send_on_shell(msg);
    }

    std::optional<xmessage> xipc_client::check_shell_answer()
    {
        return p_client->check_shell_answer();
    }
}