#ifndef HOOK_BASE_HPP
#define HOOK_BASE_HPP

#ifndef UVW_AS_LIB
#define UVW_AS_LIB
#include <uvw.hpp>
#endif

namespace xeus
{
    class hook_base
    {
    public:

        virtual ~hook_base() = default;

        virtual void pre_hook() = 0;
        virtual void post_hook() = 0;

        virtual void run(std::shared_ptr<uvw::loop> loop)
        {
            loop->run();
        };

    };
}

#endif
