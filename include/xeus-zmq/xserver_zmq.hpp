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

#include "xeus/xeus_context.hpp"
#include "xeus/xkernel_configuration.hpp"
#include "xeus/xserver.hpp"

#include "xeus-zmq.hpp"

namespace xeus
{
    class xserver_zmq_impl;
    class XEUS_ZMQ_API xserver_zmq : public xserver
    {
    public:

        explicit xserver_zmq(xserver_zmq_impl* impl);
        ~xserver_zmq();

    protected:

        xcontrol_messenger& get_control_messenger_impl() override;

        void send_shell_impl(xmessage msg) override;
        void send_control_impl(xmessage msg) override;
        void send_stdin_impl(xmessage msg) override;
        void publish_impl(xpub_message msg, channel c) override;

        void start_impl(xpub_message msg) override;
        void abort_queue_impl(const listener& l, long polling_interval) override;
        void stop_impl() override;
        void update_config_impl(xconfiguration& config) const override;

    private:

        xserver_zmq_impl* p_impl;
    };

    XEUS_ZMQ_API
    std::unique_ptr<xserver> make_xserver_zmq_default(xcontext& context,
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
}

#endif
