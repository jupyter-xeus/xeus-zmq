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
#include "xeus/xmessage.hpp"
#include "xeus-zmq/xdap_tcp_client.hpp"
#include "xdap_tcp_client_impl.hpp"
#include "../common/xzmq_serializer.hpp"

namespace xeus
{
    /*********************************
     * xdap_tcp_client implemenation *
     *********************************/

    xdap_tcp_configuration::xdap_tcp_configuration(dap_tcp_type adap_tcp_type,
                                                   dap_init_type adap_init_type,
                                                   const std::string& user_name,
                                                   const std::string& session_id)
        : m_dap_tcp_type(adap_tcp_type)
        , m_dap_init_type(adap_init_type)
        , m_user_name(user_name)
        , m_session_id(session_id)
    {
    }

    xdap_tcp_client::xdap_tcp_client(xcontext& context,
                                     const xeus::xconfiguration& config,
                                     int socket_linger,
                                     const xdap_tcp_configuration& dap_config,
                                     const event_callback& cb)
        : p_impl(new xdap_tcp_client_impl(
            context,
            config,
            socket_linger,
            dap_config,
            cb,
            [this](nl::json message) { handle_event(std::move(message)); }
          ))
    {       
    }
    
    xdap_tcp_client::~xdap_tcp_client() = default;

    void xdap_tcp_client::start_debugger(std::string tcp_end_point,
                                         std::string publisher_end_point,
                                         std::string controller_end_point,
                                         std::string controller_header_end_point)
    {
        p_impl->start_debugger(
            tcp_end_point,
            publisher_end_point,
            controller_end_point,
            controller_header_end_point
        );
    }

    void xdap_tcp_client::forward_event(nl::json message)
    {
        p_impl->forward_event(std::move(message));
    }

    void xdap_tcp_client::send_dap_request(nl::json message)
    {
        p_impl->send_dap_request(std::move(message));
    }

    nl::json xdap_tcp_client::wait_for_message(const message_condition& condition)
    {
        return p_impl->wait_for_message(condition);
    }
}

