/***************************************************************************
* Copyright (c) 2016, Johan Mabille and Sylvain Corlay                     *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include <chrono>
#include <iostream>
#include <thread>

#include "xeus/xkernel.hpp"
#include "xeus/xkernel_configuration.hpp"
#include "xeus-zmq/xzmq_context.hpp"
#include "xeus-zmq/xserver_zmq.hpp"
#include "xmock_interpreter.hpp"

int main(int /*argc*/, char* /*argv*/[])
{

    using namespace std::chrono_literals;

    xeus::xconfiguration config;
    config.m_transport = "ipc";
    config.m_ip = "localhost";
    config.m_control_port = "control";
    config.m_shell_port = "shell";
    config.m_stdin_port = "stdin";
    config.m_iopub_port = "iopub";
    config.m_hb_port = "heartbeat";
    config.m_signature_scheme = "none";
    config.m_key = "";

    auto context = xeus::make_zmq_context();

    using interpreter_ptr = std::unique_ptr<xeus::xmock_interpreter>;
    interpreter_ptr interpreter = interpreter_ptr(new xeus::xmock_interpreter());
    xeus::xkernel kernel(config,
                         xeus::get_user_name(),
                         std::move(context),
                         std::move(interpreter),
                         xeus::make_xserver_default);
    std::cout << "starting kernel" << std::endl;
    kernel.start();

    return 0;
}

