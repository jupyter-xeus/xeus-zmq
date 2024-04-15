/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XEUS_SHELL_DEFAULT_HPP
#define XEUS_SHELL_DEFAULT_HPP

#include "xshell_base.hpp"

namespace xeus
{

    class xshell_default : public xshell_base
    {
    public:

        xshell_default(zmq::context_t& context,
                       const std::string& transport,
                       const std::string& ip,
                       const std::string& shell_port,
                       const std::string& stdin_port,
                       xserver_zmq_split* server);

        virtual ~xshell_default() = default;

    private:

        void run_impl() override;
    };

}

#endif
