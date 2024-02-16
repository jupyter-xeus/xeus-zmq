/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "xeus-zmq/xclient_zmq.hpp"
#include "xclient_zmq_impl.hpp"

namespace xeus
{

    xclient_zmq::xclient_zmq(xcontext& context,
                            const std::string& user_name,
                            const xeus::xconfiguration& config)
        : p_client_impl(std::make_unique<xclient_zmq_impl>(context.get_wrapped_context<zmq::context_t>(), user_name, config))
    {
    }

    // Has to be in the cpp because incomplete
    // types are used in unique_ptr in the header
    xclient_zmq::~xclient_zmq() = default;


    void xclient_zmq::send_shell(xmessage msg)
    {
        p_client_impl->send_shell(std::move(msg));
    }

    nl::json xclient_zmq::check_shell_answer()
    {
        // TODO
        return nl::json::object();
    }

}
