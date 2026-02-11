
#include "zmq.hpp"
#include "zmq_addon.hpp"

#include "nlohmann/json.hpp"

#include <cstdlib>
#include <fstream>
#include <future>

#include "xeus/xguid.hpp"
#include "xeus/xmessage.hpp"

#include "xeus-zmq/xclient_zmq.hpp"
#include "xeus-zmq/xhandshake_client_zmq.hpp"
#include "xeus-zmq/xmiddleware.hpp"
#include "xeus-zmq/xthread.hpp"
#include "xeus-zmq/xzmq_context.hpp"

#include "../src/common/xauthentication.hpp"
#include "../src/common/xzmq_serializer.hpp"

namespace nl = nlohmann;

nl::json send_kernel_info_request(xeus::xclient_zmq& client)
{
    nl::json header = xeus::make_header("kernel_info_request", "tester", "DAEDZFAEDE12");
    xeus::xmessage req(
        { xeus::new_xguid() },
        header,
        nl::json::object(),
        nl::json::object(),
        nl::json::object(),
        xeus::buffer_sequence()
    );

    client.send_on_shell(std::move(req));
    auto rep = client.receive_on_shell();
    const nl::json& rep_content = rep.value().content(); 
    return rep_content;
}

void send_shutdown_request(xeus::xclient_zmq& client)
{
    nl::json header = xeus::make_header("shutdown_request", "tester", "DAEDZFAEDE12");
    nl::json content = { {"restart", false} };
    xeus::xmessage req(
        { xeus::new_xguid() },
        header,
        nl::json::object(),
        nl::json::object(),
        content,
        xeus::buffer_sequence()
    );

    client.send_on_control(std::move(req));
    client.receive_on_control();
}

int main(int /*argc*/, char** /*argv*/)
{
    xeus::xregistration_configuration config;
    config.m_kernel_id = "testing_id";
    config.m_registration_ip = "127.0.0.1";
    config.m_key = xeus::new_xguid();

    std::unique_ptr<xeus::xcontext> context = xeus::make_zmq_context();
    xeus::xhandshake_client_zmq handshake_client(*context, config);
    config.m_registration_port = handshake_client.get_registration_port();

    std::promise<bool> status_promise;
    std::future<bool> status_future = status_promise.get_future();

    auto handshake_thread = xeus::xthread([&context, &handshake_client](std::promise<bool> status)
    {
        auto kernel_config = handshake_client.wait_for_configuration();
        auto client = xeus::make_xclient_zmq(*context, kernel_config);
        auto rep_content = send_kernel_info_request(*client);
        status.set_value(rep_content["status"].get<std::string>() == "ok");
        send_shutdown_request(*client);
    }, std::move(status_promise));

    nl::json handshake_json =
    {
        { "transport", config.m_transport },
        { "ip", config.m_ip },
        { "signature_scheme", config.m_signature_scheme },
        { "key", config.m_key },
        { "kernel_id", config.m_kernel_id },
        { "registration_ip", config.m_registration_ip },
        { "registration_port", config.m_registration_port }
    };
    std::ofstream out("connection_file");
    out << handshake_json << std::endl;

    std::string cmd = "./test_kernel_shell -f connection_file";
    std::system(cmd.c_str());

    bool status = status_future.get();

    return !status;
}
