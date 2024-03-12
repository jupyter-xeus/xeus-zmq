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

#ifndef UVW_AS_LIB
#define UVW_AS_LIB
#include <uvw.hpp>
#endif

namespace xeus
{
    class xhook_base
    {
    public:

        virtual ~xhook_base() = default;

        virtual void pre_hook() = 0;
        virtual void post_hook() = 0;

        virtual void run(std::shared_ptr<uvw::loop> loop)
        {
            loop->run();
        };

    };
}

#endif
