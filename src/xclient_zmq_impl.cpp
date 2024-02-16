/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "xclient_zmq_impl.hpp"

namespace xeus
{

    xclient_zmq_impl::xclient_zmq_impl(zmq::context_t& context,
                                    const std::string& user_name,
                                    const xeus::xconfiguration& config)
        : m_shell_client(context, user_name, config)
        , p_messenger(context)
    {
    }

    // Has to be in the cpp because incomplete
    // types are used in unique_ptr in the header
    xclient_zmq_impl::~xclient_zmq_impl() = default;

    void xclient_zmq_impl::connect_client_messenger()
    {
        p_messenger.connect();
    }

    xclient_messenger& xclient_zmq_impl::get_client_messenger()
    {
        return p_messenger;
    }

    void xclient_zmq_impl::send_shell(xmessage msg) 
    {
        m_shell_client.send_message(std::move(msg));
    }

    void xclient_zmq_impl::start_channels()
    {
        m_shell_client.start();
    }

}