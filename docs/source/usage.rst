.. Copyright (c) 2022, Johan Mabille and Sylvain Corlay

   Distributed under the terms of the BSD 3-Clause License.

   The full license is in the file LICENSE, distributed with this software.

Usage
=====

Instantiating a server
----------------------

`xeus-zmq` provides server building functions that can be passed to the kernel constructor:

.. code::

    #include <iostream>
    #include <memory>

    #include "xeus/xkernel.hpp"
    #include "xeus/xkernel_configuration.hpp"
    #include "xeus-zmq/xzmq_context.hpp"
    #include "xeus-zmq/xserver_zmq.hpp"
    #include "xmock_interpreter.hpp"

    int main(int argc, char* argv[])
    {
        std::string file_name = (argc == 1) ? "connection.json" : argv[2];
        xeus::xconfiguration config = xeus::load_configuration(file_name);

        auto context = xeus::make_zmq_context();

        using interpreter_ptr = std::unique_ptr<my_custom_interpreter>;
        interpreter_ptr interpreter = interpreter_ptr(new my_custom_interpreter());
        xeus::xkernel kernel(config,
                             xeus::get_user_name(),
                             std::move(context),
                             std::move(interpreter),
                             xeus::make_xserver_default);
        std::cout << "starting kernel" << std::endl;
        kernel.start();

        return 0;
    }

`xeus-zmq` provides three different implementations for the server:

- ``xserver_zmq_default`` is the default server implementation; it runs three threads, one for publishing,
  one for the heartbeat messages, and the main thread for handling the shell, control and stdin sockets. To
  instantiate this implementation, include ``xserver_zmq.hpp``  and call the ``make_xserver_default``
  function.
- ``xserver_control_main`` runs an additional thread for handling the shell and the stdin sockets. Therefore,
  the main thread only listens to the control socket. This allows us to easily implement interruption of code
  execution. This server is required if you want to plug a debugger in the kernel. To instantiate this
  implementation, include ``xserver_zmq_split.hpp`` and call the ``make_xserver_control_main`` function.
- ``xserver_shell_main`` is similar to ``xserver_control_main`` except that the main thread handles the shell
  and the stdin sockets while the additional thread listens to the control socket. This server is required if
  you want to plug a debugger that does not support native threads and requires the code to be run by the main
  thread. To instantiate this implementation, include ``xserver_zmq_split.hpp``  and call the
  ``make_xserver_shell_main`` function.

Instantiating a client
----------------------

`xeus-zmq` provides client building functions that can be used by custom clients and end users. For starters on usage,
have a look at our `ipc client class`_ and the `ipc client implementation file`_.

`xeus-zmq` currently provides a single implementation for the client:

- ``xclient_zmq`` is the primary client implementation, it runs two threads, one for sending a "ping" message to the
  heartbeat every 100ms, and one for polling the iopub socket and pushing the received message into a queue. The main
  thread waits for messages by either popping messages from the queue or polling the shell and the control sockets for
  received messages. To instantiate this implementation, include ``xclient_zmq.hpp`` and call the
  ``make_xclient_zmq`` function.

.. _ipc client class: https://github.com/jupyter-xeus/xeus-zmq/blob/main/test/xipc_client.hpp
.. _ipc client implementation file: https://github.com/jupyter-xeus/xeus-zmq/blob/main/test/client_ipc.cpp

