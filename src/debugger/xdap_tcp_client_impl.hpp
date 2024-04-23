/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XEUS_DAP_TCP_CLIENT_IMPL_HPP
#define XEUS_DAP_TCP_CLIENT_IMPL_HPP

#include <deque>
#include <string>

#include "zmq.hpp"
#include "nlohmann/json.hpp"
#include "xeus/xkernel_configuration.hpp"
#include "xeus/xeus_context.hpp"

#include "xeus-zmq/xdap_tcp_client.hpp"
#include "../common/xauthentication.hpp"

namespace nl = nlohmann;

namespace xeus
{
    class xdap_tcp_client_impl
    {
    public:

        using event_callback = std::function<void(const nl::json&)>;

        xdap_tcp_client_impl(xcontext& context,
                             const xeus::xconfiguration& config,
                             int socket_linger,
                             const xdap_tcp_configuration& dap_config,
                             const event_callback& cb,
                             const event_callback& handler);

        void start_debugger(std::string tcp_end_point,
                            std::string publisher_end_point,
                            std::string controller_end_point,
                            std::string controller_header_end_point);

        void forward_event(nl::json message);
        void send_dap_request(nl::json message);

        using message_condition = std::function<bool(const nl::json&)>;
        nl::json wait_for_message(const message_condition& condition);

    private:

        using queue_type = std::deque<std::string>;

        zmq::message_t get_tcp_id() const;
        void init_tcp_socket(const std::string& tcp_end_point);
        void finalize_tcp_socket(const std::string& tcp_end_point);
        void handle_header_socket();
        void handle_control_socket();
        void handle_tcp_socket(queue_type& message_queue);
        void append_tcp_message(std::string& buffer);
        void process_message_queue();
        void handle_init_sequence();

        zmq::socket_t m_tcp_socket;
        zmq::message_t m_socket_id;

        zmq::socket_t m_publisher;
        zmq::socket_t m_controller;
        zmq::socket_t m_controller_header;

        dap_tcp_type m_dap_tcp_type;
        dap_init_type m_dap_init_type;

        std::string m_user_name;
        std::string m_session_id;

        event_callback m_event_callback;
        event_callback m_event_handler;

        using authentication_ptr = std::unique_ptr<xeus::xauthentication>;
        authentication_ptr p_auth;

        // Saves parent header for future debug events
        std::string m_parent_header;

        bool m_request_stop;
        bool m_wait_attach;

        queue_type m_message_queue;
        queue_type m_stopped_queue;
    };
}

#endif

