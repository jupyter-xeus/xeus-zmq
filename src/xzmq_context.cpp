#include "zmq.hpp"

#include "xeus-zmq/xzmq_context.hpp"

namespace xeus
{
    std::unique_ptr<xcontext> make_zmq_context()
    {
        return std::unique_ptr<xcontext>(new xcontext_impl<zmq::context_t>());
    }
}

