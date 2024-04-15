/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "xeus-zmq/xhook_base.hpp"

#include <uvw.hpp>

namespace xeus
{
    void xhook_base::pre_hook()
    {
        pre_hook_impl();
    }

    void xhook_base::post_hook()
    {
        post_hook_impl();
    }

    void xhook_base::run(std::shared_ptr<uvw::loop> loop)
    {
        run_impl(loop);
    }

    void xhook_base::run_impl(std::shared_ptr<uvw::loop> loop)
    {
        loop->run();
    }
}
