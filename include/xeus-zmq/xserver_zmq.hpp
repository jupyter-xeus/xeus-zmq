/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XEUS_SERVER_ZMQ_HPP
#define XEUS_SERVER_ZMQ_HPP

#ifndef UVW_AS_LIB
#define UVW_AS_LIB
#include <uvw.hpp>
#endif

#include <memory> // std::unique_ptr

#include "xeus/xeus_context.hpp"
#include "xeus/xkernel_configuration.hpp"
#include "xeus/xserver.hpp"

#include "xeus-zmq.hpp"
#include "xhook_base.hpp"

namespace xeus
{
    class xserver_zmq_impl;
    class XEUS_ZMQ_API xserver_zmq final: public xserver
    {
    public:

        explicit xserver_zmq(std::unique_ptr<xserver_zmq_impl> impl);
        ~xserver_zmq() override;

        void call_notify_shell_listener(xmessage msg);
        void call_notify_control_listener(xmessage msg);
        void call_notify_stdin_listener(xmessage msg);
        nl::json call_notify_internal_listener(nl::json msg);

    private:

        // Implementation of xserver virtual methods
        xcontrol_messenger& get_control_messenger_impl() override;

        void send_shell_impl(xmessage msg) override;
        void send_control_impl(xmessage msg) override;
        void send_stdin_impl(xmessage msg) override;
        void publish_impl(xpub_message msg, channel c) override;

        void start_impl(xpub_message msg) override;
        void abort_queue_impl(const listener& l, long polling_interval) override;
        void stop_impl() override;
        void update_config_impl(xconfiguration& config) const override;

        std::unique_ptr<xserver_zmq_impl> p_impl;
    };

    XEUS_ZMQ_API
    std::unique_ptr<xserver> make_xserver_default(xcontext& context,
                                              const xconfiguration& config,
                                              nl::json::error_handler_t eh = nl::json::error_handler_t::strict);

    XEUS_ZMQ_API
    std::unique_ptr<xserver> make_xserver_control_main(xcontext& context,
                                                       const xconfiguration& config,
                                                       nl::json::error_handler_t eh = nl::json::error_handler_t::strict);

    XEUS_ZMQ_API
    std::unique_ptr<xserver> make_xserver_shell_main(xcontext& context,
                                                     const xconfiguration& config,
                                                     nl::json::error_handler_t eh = nl::json::error_handler_t::strict);

    XEUS_ZMQ_API
    std::unique_ptr<xserver> make_xserver_uv_shell_main(
        xcontext& context,
        const xconfiguration& config,
        nl::json::error_handler_t eh = nl::json::error_handler_t::strict,
        std::shared_ptr<uvw::loop> loop_ptr = nullptr,
        std::unique_ptr<xhook_base> hook = nullptr);

}

#endif
