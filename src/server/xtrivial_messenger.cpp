/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "xtrivial_messenger.hpp"

namespace xeus
{
    xtrivial_messenger::xtrivial_messenger(listener l)
        : m_listener(std::move(l))
    {
    }

    nl::json xtrivial_messenger::send_to_shell_impl(const nl::json& message)
    {
        return m_listener(message);
    }
}

