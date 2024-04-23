/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XEUS_CONTROL_RUNNER_HPP
#define XEUS_CONTROL_RUNNER_HPP

#include <optional>

#include "xeus/xmessage.hpp"

#include "xeus-zmq.hpp"
#include "xmiddleware.hpp"

namespace xeus
{
    class xserver_zmq_split;

    class XEUS_ZMQ_API xcontrol_runner
    {
    public:

        virtual ~xcontrol_runner() = default;

        xcontrol_runner(const xcontrol_runner&) = delete;
        xcontrol_runner& operator=(const xcontrol_runner&) = delete;
        xcontrol_runner(xcontrol_runner&&) = delete;
        xcontrol_runner& operator=(xcontrol_runner&&) = delete;

        void register_server(xserver_zmq_split& server);
        void run();
        void stop();

    protected:

        xcontrol_runner() = default;

        fd_t get_control_fd() const;
        std::optional<xmessage> read_control(int flags = 0);
        void stop_channels();
        
        void notify_control_listener(xmessage message);

    private:

        virtual void run_impl() = 0;
        virtual void stop_impl() = 0;

        xserver_zmq_split* p_server = nullptr;
    };
}

#endif

