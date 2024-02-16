/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "xshell_client.hpp"

namespace xeus
{

    xshell_client::xshell_client(zmq::context_t& context,
                                 const std::string& user_name,
                                 const xeus::xconfiguration& config)
        : p_authentication(xeus::make_xauthentication(config.m_signature_scheme, config.m_key))
        , m_socket(context, zmq::socket_type::dealer)
        , m_end_point("")
        , m_user_name(user_name)
        , m_session_id(xeus::new_xguid())
    {
        // TODO
        m_end_point = xeus::get_end_point(config.m_transport, config.m_ip, config.m_shell_port);

        m_socket.connect(m_end_point);
    }

    xshell_client::~xshell_client()
    {
        m_socket.disconnect(m_end_point);
    }

    void xshell_client::send_message(xmessage msg)
    {
        send_message_impl(std::move(msg), m_socket, *p_authentication);
    }

    nl::json xshell_client::receive_message()
    {
        return receive_message_impl(m_socket, *p_authentication);
    }

    nl::json xshell_client::check_received_message(long timeout)
    {
        // TODO
        return nl::json::object();
    }

    zmq::socket& xshell_client::get_socket()
    {
        return m_socket;
    }

    void start()
    {
        // TODO
    }

    void send_message_impl(xmessage msg,
                        zmq::socket_t& socket,
                        const xeus::xauthentication& auth)
    {
        // TODO: Implement message sending
    }

    nl::json xshell_client::receive_message_impl(zmq::socket_t& socket,
                                                 const xeus::xauthentication& auth)
    {
        // TODO: Implement message receiving
        return nl::json::object();
    }

}