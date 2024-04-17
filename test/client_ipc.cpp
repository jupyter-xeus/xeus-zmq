/***************************************************************************
* Copyright (c) 2016, Johan Mabille and Sylvain Corlay                     *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

// TODO: this client should be reimplemented with
// the new client framework. Notice that it was not
// tested in the CI but was here for trying to reproduce
// some issue with IPC.

#include <iostream>

#include "zmq_addon.hpp"
#include "nlohmann/json.hpp"
#include "xeus/xguid.hpp"
#include "xeus/xmessage.hpp"
#include "xeus-zmq/xauthentication.hpp"
#include "xeus-zmq/xmiddleware.hpp"
#include "xeus-zmq/xzmq_serializer.hpp"

namespace nl = nlohmann;

int main(int, char**)
{
    zmq::context_t context;
    zmq::socket_t cli(context, zmq::socket_type::dealer);
    xeus::xguid socket_id = xeus::new_xguid();
    //cli.set(ZMQ_IDENTITY, socket_id.c_str(), socket_id.size());
    cli.set(zmq::sockopt::linger, xeus::get_socket_linger());
    cli.connect(xeus::get_end_point("ipc", "localhost", "shell"));

    auto auth = xeus::make_xauthentication("none", "");

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
    auto wire_msg = xeus::xzmq_serializer::serialize(std::move(msg), *auth);
    wire_msg.send(cli);

    zmq::multipart_t resp_msg;
    resp_msg.recv(cli);

    auto resp = xeus::xzmq_serializer::deserialize(resp_msg, *auth);
    std::cout << resp.content().dump(4) << std::endl;

    return 0;
}

