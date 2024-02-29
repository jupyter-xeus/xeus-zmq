/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "nlohmann/json.hpp"
#include "xeus-zmq/xmiddleware.hpp"
#include "xclient_messenger.hpp"

namespace nl = nlohmann;

namespace xeus
{
    xclient_messenger::xclient_messenger(zmq::context_t& context)
    {
    }

    xclient_messenger::~xclient_messenger()
    {
    }

    void xclient_messenger::connect()
    {
        // TODO
    }

    void xclient_messenger::stop_channels()
    {
        // TODO
    }
}

