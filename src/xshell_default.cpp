/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include <iostream>

#include "xshell_default.hpp"
#include "xserver_zmq_split.hpp"

namespace xeus
{
    xshell_default::xshell_default(zmq::context_t& context,
                                   const std::string& transport,
                                   const std::string& ip,
                                   const std::string& shell_port,
                                   const std::string& stdin_port,
                                   xserver_zmq_split* server)
        : xshell_base(context, transport, ip, shell_port, stdin_port, server)
    {
    }

    void xshell_default::run_impl()
    {
        zmq::pollitem_t items[] = {
            { m_shell, 0, ZMQ_POLLIN, 0 },
            { m_controller, 0, ZMQ_POLLIN, 0 }
        };

        while (true)
        {
            zmq::poll(&items[0], 2, std::chrono::milliseconds(-1));

            if (items[0].revents & ZMQ_POLLIN)
            {
                zmq::multipart_t wire_msg;
                wire_msg.recv(m_shell);
                try
                {
                    xmessage msg = p_server->deserialize(wire_msg);
                    p_server->notify_shell_listener(std::move(msg));
                }
                catch(std::exception& e)
                {
                    std::cerr << e.what() << std::endl;
                }
            }

            if (items[1].revents & ZMQ_POLLIN)
            {
                // stop message
                zmq::multipart_t wire_msg;
                wire_msg.recv(m_controller);
                std::string msg = wire_msg.peekstr(0);
                if(msg == "stop")
                {
                    wire_msg.send(m_controller);
                    break;
                }
                else
                {
                    zmq::multipart_t wire_reply = p_server->notify_internal_listener(wire_msg);
                    wire_reply.send(m_controller);
                }
            }
        }
    }

}
