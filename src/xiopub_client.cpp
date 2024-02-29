/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "xiopub_client.hpp"

#include "xeus-zmq/xzmq_serializer.hpp"

namespace xeus
{

    xiopub_client::xiopub_client(zmq::context_t& context,
                                 const xeus::xconfiguration& config)
        : m_iopub(context, zmq::socket_type::sub)
        , m_iopub_end_point("")
    {
        m_iopub_end_point = xeus::get_end_point(config.m_transport, config.m_ip, config.m_iopub_port);

        m_iopub.connect(m_iopub_end_point);
    }

    xiopub_client::~xiopub_client()
    {
        m_iopub.disconnect(m_iopub_end_point);
    }

    std::size_t xiopub_client::iopub_queue_size() const
    {
        std::lock_guard<std::mutex> guard(m_queue_mutex);
        return m_message_queue.size();
    }

    std::optional<xmessage> xiopub_client::pop_iopub_message()
    {
        std::lock_guard<std::mutex> guard(m_queue_mutex);
        if (!m_message_queue.empty())
        {
            xmessage msg = m_message_queue.back();
            m_message_queue.pop();
            return msg;
        } else {
            return std::nullopt;
        }
    }

    void xiopub_client::run()
    {
        zmq::pollitem_t items[] = {
            { m_iopub, 0, ZMQ_POLLIN, 0 }
        };

        while (true)
        {
            zmq::poll(&items[0], 1, std::chrono::milliseconds(-1));

            if (items[0].revents & ZMQ_POLLIN)
            {
                zmq::multipart_t wire_msg;
                wire_msg.recv(m_iopub);
                try
                {
                    xmessage msg = p_client_impl->deserialize(wire_msg);
                    {
                        std::lock_guard<std::mutex> guard(m_queue_mutex);
                        m_message_queue.push(msg);
                    }
                    p_client_impl->notify_shell_listener(std::move(msg));
                }
                catch(std::exception& e)
                {
                    std::cerr << e.what() << std::endl;
                }
            }
        }
    }
}