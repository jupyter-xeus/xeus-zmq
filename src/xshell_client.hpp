/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XEUS_SHELL_CLIENT_HPP
#define XEUS_SHELL_CLIENT_HPP

#include "zmq.hpp"
#include "nlohmann/json.hpp"
#include "xeus/xkernel_configuration.hpp"
#include "xeus-zmq/xauthentication.hpp"

namespace xeus
{

    class xshell_client
    {
    public:

        xshell_client(zmq::context_t& context,
                        const std::string& user_name,
                        const xeus::xconfiguration& config);

        ~xshell_client();

        void send_message(xmessage msg);

        nl::json receive_message();

        nl::json check_received_message(long timeout);

        zmq::socket& get_socket();

        void start();

    private:
        void send_message_impl(xmessage msg,
                            zmq::socket_t& socket,
                            const xeus::xauthentication& auth);

        nl::json receive_message_impl(zmq::socket_t& socket,
                                const xeus::xauthentication& auth);

        using authentication_ptr = std::unique_ptr<xeus::xauthentication>;
        authentication_ptr p_authentication;

        zmq::socket_t m_socket;

        std::string m_end_point;
        std::string m_user_name;
        std::string m_session_id;
    };
}

#endif