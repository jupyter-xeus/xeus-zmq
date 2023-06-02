#ifndef XZMQ_CONTEXT_HPP
#define XZMQ_CONTEXT_HPP

#include <memory>

#include "xeus/xeus_context.hpp"

#include "xeus-zmq.hpp"

namespace xeus
{
    XEUS_ZMQ_API
    std::unique_ptr<xcontext> make_zmq_context();
}

#endif

