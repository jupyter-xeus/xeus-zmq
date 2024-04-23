/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XEUS_TRIVIAL_MESSENGER_HPP
#define XEUS_TRIVIAL_MESSENGER_HPP

#include <functional>

#include "nlohmann/json.hpp"

#include "xeus/xcontrol_messenger.hpp"

namespace nl = nlohmann;

namespace xeus
{
    class xserver_zmq_default;

    class xtrivial_messenger : public xcontrol_messenger
    {
    public:
        
        using listener = std::function<nl::json(nl::json)>;

        explicit xtrivial_messenger(listener l);
        virtual ~xtrivial_messenger() = default;

    private:

        nl::json send_to_shell_impl(const nl::json& message) override;

        listener m_listener;
    };
}

#endif
