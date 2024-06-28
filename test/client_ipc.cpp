/***************************************************************************
* Copyright (c) 2016, Johan Mabille and Sylvain Corlay                     *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include <iostream>

#include "zmq_addon.hpp"
#include "nlohmann/json.hpp"
#include "xeus/xguid.hpp"
#include "xeus/xmessage.hpp"
#include "xeus/xkernel_configuration.hpp"
#include "xeus-zmq/xzmq_context.hpp"
#include "xipc_client.hpp"

namespace nl = nlohmann;

int main(int, char**)
{
    auto context_ptr = xeus::make_zmq_context();
    xeus::xconfiguration config;
    config.m_transport = "ipc";
    config.m_ip = "localhost";
    config.m_control_port = "control";
    config.m_shell_port = "shell";
    config.m_stdin_port = "stdin";
    config.m_iopub_port = "iopub";
    config.m_hb_port = "heartbeat";
    config.m_signature_scheme = "none";
    config.m_key = "";

    xeus::xipc_client ipc_client(*context_ptr, config);

    xeus::xguid socket_id = xeus::new_xguid();
    nl::json header = xeus::make_header("execute_request", "tester", "DAEDZFAEDE12");
    std::string code = "std::cout << \"this is a test\" << std::endl;";
    nl::json req = {
        { "code", code }
    };

    xeus::xmessage msg(
        { socket_id},
        header,
        code,
        nl::json::object(),
        nl::json::object(),
        xeus::buffer_sequence()
    );

    ipc_client.send_on_shell(std::move(msg));

    auto response = ipc_client.receive_on_shell(false);
    if (response.has_value())
    {
        std::cout << response->content().dump(4) << std::endl;
    }
    else
    {
        std::cout << "No response received" << std::endl;
    }

    return 0;
}

