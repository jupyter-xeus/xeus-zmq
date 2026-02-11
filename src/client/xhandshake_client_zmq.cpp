/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "zmq.hpp"
#include "zmq_addon.hpp"

#include <nlohmann/json.hpp>

#include "xeus-zmq/xhandshake_client_zmq.hpp"

#include "../common/xauthentication.hpp"
#include "../common/xmiddleware_impl.hpp"
#include "../common/xzmq_serializer.hpp"

namespace nl = nlohmann;

namespace xeus
{
    /******************************
     * xhandshake_client_zmq_impl *
     ******************************/

    class xhandshake_client_zmq_impl
    {
    public:

        xhandshake_client_zmq_impl(zmq::context_t& context, const xregistration_configuration& config);

        std::string get_registration_port() const;

        xkernel_configuration wait_for_configuration();

    private:

        zmq::context_t* p_context;
        std::string m_key;
        zmq::socket_t m_handshake;
        using authentication_ptr = std::unique_ptr<xauthentication>;
        authentication_ptr p_auth;
    };


    xhandshake_client_zmq_impl::xhandshake_client_zmq_impl
    (
        zmq::context_t& context,
        const xregistration_configuration& config
    )
        : p_context(&context)
        , m_key(config.m_key)
        , m_handshake(context, zmq::socket_type::router)
        , p_auth(make_xauthentication(config.m_signature_scheme, config.m_key))
    {
        init_socket(m_handshake, config.m_transport, config.m_registration_ip, config.m_registration_port);
    }

    std::string xhandshake_client_zmq_impl::get_registration_port() const
    {
        return get_socket_port(m_handshake);
    }

    xkernel_configuration wait_for_configuration();

    xkernel_configuration xhandshake_client_zmq_impl::wait_for_configuration()
    {
        zmq::multipart_t wire_msg;
        if (!wire_msg.recv(m_handshake))
        {
            throw std::runtime_error("Did not receive kernel configuration");
        }
        auto routing_ids = xzmq_serializer::deserialize_zmq_id(wire_msg);
        // TODO: check signature
        wire_msg.pop(); // signature
        zmq::message_t content = wire_msg.pop();
        const char* buf = content.data<const char>();
        nl::json j = nl::json::parse(buf, buf + content.size());

        xeus::xkernel_configuration config;
        config.m_key = m_key;
        // TODO: should we read and return kernel_id ?
        config.m_control_port = j["control_port"].get<std::string>();
        config.m_shell_port = j["shell_port"].get<std::string>();
        config.m_stdin_port = j["stdin_port"].get<std::string>();
        config.m_iopub_port = j["iopub_port"].get<std::string>();
        config.m_hb_port = j["hb_port"].get<std::string>();

        zmq::multipart_t wire_rep;
        std::string rep_buffer = "ACK";
        zmq::message_t rep_content(rep_buffer.c_str(), rep_buffer.size());
        auto auth = xeus::make_xauthentication("hmac-sha256", m_key);
        std::string sig = auth->sign(xeus::xzmq_serializer::make_raw_buffer(rep_content));
        zmq::message_t signature(sig.begin(), sig.end());
        xzmq_serializer::serialize_zmq_id(routing_ids, wire_rep);
        wire_rep.add(std::move(signature));
        wire_rep.add(std::move(rep_content));
        wire_rep.send(m_handshake);
        return config;
    }

    /*************************
     * xhandshake_client_zmq *
     *************************/

    xhandshake_client_zmq::xhandshake_client_zmq
    (
        xcontext& context,
        const xregistration_configuration& config
    )
        : p_client_impl(new xhandshake_client_zmq_impl(context.get_wrapped_context<zmq::context_t>(), config))
    {
    }

    xhandshake_client_zmq::~xhandshake_client_zmq() = default;

    std::string xhandshake_client_zmq::get_registration_port() const
    {
        return p_client_impl->get_registration_port();
    }

    xkernel_configuration xhandshake_client_zmq::wait_for_configuration()
    {
        return p_client_impl->wait_for_configuration();
    }
}
