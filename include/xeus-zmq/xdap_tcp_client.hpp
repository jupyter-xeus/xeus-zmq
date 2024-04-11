/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XEUS_DAP_TCP_CLIENT_HPP
#define XEUS_DAP_TCP_CLIENT_HPP

#include <functional>
#include <memory>
#include <string>

#include "nlohmann/json.hpp"
#include "xeus/xkernel_configuration.hpp"
#include "xeus/xeus_context.hpp"

#include "xeus-zmq.hpp"

namespace nl = nlohmann;

namespace xeus
{
    enum class dap_tcp_type
    {
        client,
        server
    };

    enum class dap_init_type
    {
        sequential,
        parallel
    };

    struct XEUS_ZMQ_API xdap_tcp_configuration
    {
        dap_tcp_type m_dap_tcp_type;
        dap_init_type m_dap_init_type;
        std::string m_user_name;
        std::string m_session_id;

        xdap_tcp_configuration(dap_tcp_type adap_tcp_type,
                               dap_init_type adap_init_type,
                               const std::string& user_name,
                               const std::string& session_id);
    };

    class xdap_tcp_client_impl;

    class XEUS_ZMQ_API xdap_tcp_client
    {
    public:

        static constexpr const char* HEADER = "Content-Length: ";
        static constexpr size_t HEADER_LENGTH = 16;
        static constexpr const char* SEPARATOR = "\r\n\r\n";
        static constexpr size_t SEPARATOR_LENGTH = 4;

        using event_callback = std::function<void(const nl::json&)>;

        virtual ~xdap_tcp_client();

        xdap_tcp_client(const xdap_tcp_client&) = delete;
        xdap_tcp_client& operator=(const xdap_tcp_client&) = delete;

        xdap_tcp_client(xdap_tcp_client&&) = delete;
        xdap_tcp_client& operator=(xdap_tcp_client&&) = delete;

        void start_debugger(std::string tcp_end_point,
                            std::string publisher_end_point,
                            std::string controller_end_point,
                            std::string controller_header_end_point);

    protected:

        xdap_tcp_client(xcontext& context,
                        const xeus::xconfiguration& config,
                        int socket_linger,
                        const xdap_tcp_configuration& dap_config,
                        const event_callback& cb);

        void forward_event(nl::json message);
        void send_dap_request(nl::json message);

        using message_condition = std::function<bool(const nl::json&)>;
        nl::json wait_for_message(const message_condition& condition);

    private:

        virtual void handle_event(nl::json message) = 0;

        std::unique_ptr<xdap_tcp_client_impl> p_impl;
    };
}

#endif

