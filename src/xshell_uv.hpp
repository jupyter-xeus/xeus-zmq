/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XEUS_SHELL_UV_HPP
#define XEUS_SHELL_UV_HPP

#include <memory>

#ifndef UVW_AS_LIB
#define UVW_AS_LIB
#include <uvw.hpp>
#endif

#include "xshell_base.hpp"
#include "xeus-zmq/xhook_base.hpp"

namespace xeus
{


    class xshell_uv : public xshell_base
    {
    public:

        xshell_uv(zmq::context_t& context,
                       const std::string& transport,
                       const std::string& ip,
                       const std::string& shell_port,
                       const std::string& stdin_port,
                       xserver_zmq_split* server,
                       std::shared_ptr<uvw::loop> loop_ptr,
                       std::unique_ptr<xhook_base> hook);

        ~xshell_uv() override = default;

    private:

        void run_impl() override;
        void create_polls();

        std::shared_ptr<uvw::loop> p_loop{ nullptr };
        std::shared_ptr<uvw::poll_handle> p_shell_poll{ nullptr };
        std::shared_ptr<uvw::poll_handle> p_controller_poll{ nullptr };
        std::unique_ptr<xhook_base> p_hook{ nullptr };
    };

}

#endif
