/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "zmq_addon.hpp"
#include "nlohmann/json.hpp"

#include "xhandshaking.hpp"
#include "xeus-zmq/xmiddleware.hpp"

#include "../common/xzmq_serializer.hpp"

namespace nl = nlohmann;

namespace xeus
{
    xkernel_configuration get_kernel_configuration(const xconfiguration& config)
    {
        return std::visit([](const auto& conf)
        {
            if constexpr (std::is_same_v<std::decay_t<decltype(conf)>, xkernel_configuration>)
            {
                return conf;
            }
            else
            {
                xkernel_configuration res;
                res.m_transport = conf.m_transport;
                res.m_ip = conf.m_ip;
                res.m_signature_scheme = conf.m_signature_scheme;
                res.m_key = conf.m_key;
                return res;
            }
        }, config);
    }

    void send_connection_info(
        zmq::context_t& context,
        const xregistration_configuration& regis_config,
        const xkernel_configuration& kernel_config,
        const xauthentication& auth,
        nl::json::error_handler_t error_handler)
    {
        std::string end_point = get_end_point(
            regis_config.m_transport,
            regis_config.m_registration_ip,
            regis_config.m_registration_port);
        zmq::socket_t socket(context, zmq::socket_type::dealer);
        socket.set(zmq::sockopt::linger, get_socket_linger());
        socket.set(zmq::sockopt::rcvtimeo, 5000);
        socket.connect(end_point);

        zmq::multipart_t wire_msg;
        xzmq_serializer::serialize_zmq_id({}, wire_msg);

        nl::json msg = {
            { "kernel_id", regis_config.m_kernel_id },
            { "control_port", kernel_config.m_control_port },
            { "shell_port", kernel_config.m_shell_port },
            { "stdin_port", kernel_config.m_stdin_port },
            { "iopub_port", kernel_config.m_iopub_port },
            { "hb_port", kernel_config.m_hb_port }
        };
        std::string buffer = msg.dump(-1, ' ', false, error_handler);
        zmq::message_t content(buffer.c_str(), buffer.size());
        std::string sig = auth.sign(xzmq_serializer::make_raw_buffer(content));
        zmq::message_t signature(sig.begin(), sig.end());
        wire_msg.add(std::move(signature));
        wire_msg.add(std::move(content));

        wire_msg.send(socket);

        zmq::multipart_t rep;
        rep.recv(socket);

        xzmq_serializer::deserialize_zmq_id(rep);
        zmq::message_t rep_sig = rep.pop();
        zmq::message_t rep_content = rep.pop();
        if (!auth.verify(xzmq_serializer::make_raw_buffer(rep_sig),
                         xzmq_serializer::make_raw_buffer(rep_content)))
        {
            throw std::runtime_error("ERROR: Signatures don't match");
        }
    }
}
