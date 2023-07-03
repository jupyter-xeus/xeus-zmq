/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include <iostream>
#include <string>

#include "xeus-zmq/xmiddleware.hpp"
#include "xpublisher.hpp"

namespace xeus
{
    xpublisher::xpublisher(zmq::context_t& context,
                           const std::string& transport,
                           const std::string& ip,
                           const std::string& port)
        : m_publisher(context, zmq::socket_type::xpub)
        , m_listener(context, zmq::socket_type::sub)
        , m_controller(context, zmq::socket_type::rep)
    {
        init_socket(m_publisher, transport, ip, port);
        // Set xpub_verbose option to 1 to pass all subscription messages (not only unique ones).
        m_publisher.set(zmq::sockopt::xpub_verbose, 1);
        m_listener.set(zmq::sockopt::subscribe, "");
        m_listener.bind(get_publisher_end_point());
        m_controller.set(zmq::sockopt::linger, get_socket_linger());
        m_controller.bind(get_controller_end_point("publisher"));
    }

    xpublisher::~xpublisher()
    {
    }

    std::string xpublisher::get_port() const
    {
        return get_socket_port(m_publisher);
    }

    void xpublisher::run()
    {
        zmq::pollitem_t items[] = {
            { m_listener, 0, ZMQ_POLLIN, 0 },
            { m_controller, 0, ZMQ_POLLIN, 0 },
            { m_publisher, 0, ZMQ_POLLIN, 0 }
        };

        while (true)
        {
            zmq::poll(&items[0], 3, std::chrono::milliseconds(-1));

            if (items[0].revents & ZMQ_POLLIN)
            {
                zmq::multipart_t wire_msg;
                wire_msg.recv(m_listener);
                wire_msg.send(m_publisher);
            }

            if (items[1].revents & ZMQ_POLLIN)
            {
                // stop message
                zmq::multipart_t wire_msg;
                wire_msg.recv(m_controller);
                wire_msg.send(m_controller);
                break;
            }

            if (items[2].revents & ZMQ_POLLIN)
            {
                // Received event: Single frame
                // Either `1{subscription-topic}` for subscription
                // or `0{subscription-topic}` for unsubscription
                zmq::multipart_t wire_msg;
                wire_msg.recv(m_publisher);

                // Received event should be a single frame
                if (wire_msg.size() != 1)
                {
                    throw std::runtime_error("ERROR: Received message on XPUB is not a single frame");
                }

                zmq::message_t frame = wire_msg.pop();
                if (frame.size() == 0)
                {
                    break;
                }

                //  Event is one byte 0 = unsub or 1 = sub, followed by topic
                uint8_t *event = (uint8_t *)frame.data();
                // If subscription (unsubscription is ignored)
                if (event[0] == 1)
                {
                    std::string topic((char *)(event + 1), frame.size() - 1);
                    if (m_serialize_iopub_msg_cb)
                    {
                        // Construct the `iopub_welcome` message
                        zmq::multipart_t iopub_welcome_wire_msg = m_serialize_iopub_msg_cb(topic);
                        // Send the `iopub_welcome` message
                        iopub_welcome_wire_msg.send(m_publisher);
                    }
                    else
                    {
                        throw std::runtime_error("ERROR: IOPUB serialization callback not set");
                    }
                }
            }
        }
    }
}
