/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XEUS_PUBLISHER_HPP
#define XEUS_PUBLISHER_HPP

#include <functional>
#include <string>

#include "zmq.hpp"
#include "zmq_addon.hpp"

namespace xeus
{
    class xpublisher
    {
    public:

        xpublisher(zmq::context_t& context,
                   const std::string& transport,
                   const std::string& ip,
                   const std::string& port);

        ~xpublisher();

        std::string get_port() const;

        template <class C>
        inline void set_iopub_cb(zmq::multipart_t(C::*cb)(const std::string&), C* data)
        {
            m_serialize_iopub_msg_cb = std::bind(cb, data, std::placeholders::_1);
        }

        void run();

    private:

        zmq::socket_t m_publisher;
        zmq::socket_t m_listener;
        zmq::socket_t m_controller;

        std::function<zmq::multipart_t(const std::string&)> m_serialize_iopub_msg_cb;
    };
}

#endif
