/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XHOOK_BASE_HPP
#define XHOOK_BASE_HPP

#include "xeus-zmq.hpp"

#include <uvw.hpp>

namespace xeus
{
    class XEUS_ZMQ_API xhook_base
    {
    public:

        virtual ~xhook_base() = default;

        xhook_base(const xhook_base&) = delete;
        xhook_base& operator=(const xhook_base&) = delete;

        xhook_base(xhook_base&&) = delete;
        xhook_base& operator=(xhook_base&&) = delete;

        void pre_hook();
        void post_hook();
        void run(std::shared_ptr<uvw::loop> loop);

    protected:

        xhook_base() = default;

    private:

        virtual void pre_hook_impl() = 0;
        virtual void post_hook_impl() = 0;
        virtual void run_impl(std::shared_ptr<uvw::loop> loop);

    };
}

#endif
