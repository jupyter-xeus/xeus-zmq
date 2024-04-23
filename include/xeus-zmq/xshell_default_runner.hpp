/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XEUS_SHELL_DEFAULT_RUNNER_HPP
#define XEUS_SHELL_DEFAULT_RUNNER_HPP

#include "xeus-zmq.hpp"
#include "xshell_runner.hpp"

namespace xeus
{
    class XEUS_ZMQ_API xshell_default_runner final : public xshell_runner
    {
    public:

        xshell_default_runner() = default;
        ~xshell_default_runner() override = default;

    private:

        void run_impl() override;
    };
}

#endif

