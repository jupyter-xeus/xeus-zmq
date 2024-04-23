/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XEUS_CONTROL_DEFAULT_RUNNER_HPP
#define XEUS_CONTROL_DEFAULT_RUNNER_HPP

#include "xeus-zmq.hpp"
#include "xcontrol_runner.hpp"

namespace xeus
{
    class XEUS_ZMQ_API xcontrol_default_runner final : public xcontrol_runner
    {
    public:

        xcontrol_default_runner() = default;
        ~xcontrol_default_runner() override = default;

    private:

        void run_impl() override;
        void stop_impl() override;
        
        bool m_request_stop;
    };
}

#endif

