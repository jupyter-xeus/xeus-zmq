.. Copyright (c) 2022, Johan Mabille and Sylvain Corlay

   Distributed under the terms of the BSD 3-Clause License.

   The full license is in the file LICENSE, distributed with this software.

Server
======

`xeus-zmq` provides two families of implementations for the `xeus server API`_:

- ``xserver_zmq``, where a single thread polls both the shell and the control channels.
  The default implementation is ``xserver_zmq_default`` and can be instantiated via the
  ``make_xserver_default`` function.
- ``xserver_zmq_split``, where the shell and the control channels have dedicated threads.
  `xeus-zmq` provides two implementations for this family, ``xserver_shell_main`` and
  ``xserver_control_main``, which can be instantiated via the ``make_xserver_shell_main``
  and the ``make_xserver_control_main`` functions respectively.

xserver_zmq_default internals
-----------------------------

The internals of the ``xserver_zmq_default`` class are illustrated in the following diagram:

.. image:: server.svg
   :alt: server

The default server is made of three threads communicating through internal `ZeroMQ` sockets. The main
thread is responsible for polling both ``shell`` and ``controller`` channels. When a message is received
on one of these channels, the corresponding callback is invoked. Any code executed in the interpreter
will be executed by the main thread. If the ``publish`` method is called, the main thread sends a message
to the publisher thread.

Having a dedicated thread for publishing messages makes this operation a non-blocking one. When the kernel
main thread needs to publish a message, it simply sends it to the publisher thread through an internal socket
and continues its execution. The publisher thread will poll its internal socket and forward the messages to
the ``publisher`` channel.

The last thread is the heartbeat. It is responsible for notifying the client that the kernel is still alive.
This is done by sending messages on the ``heartbeat`` channel at a regular rate.

The main thread is also connected to the publisher and the heartbeat threads through internal ``controller``
channels. These are used to send ``stop`` messages to the subthread and allow to stop the kernel in a clean
way.

Extending the default implementation
------------------------------------

The default implementation performs a blocking poll of the channels, which can be a limitation in some
use cases. For instance, you may way want to poll within an event loop, to allow asynchronous execution
of code. ``xeus-zmq`` makes it possible ot extend the default implementation by inheriting from the
`xserver_zmq class`_. It provides utility methods to poll, read and send messages, so that defining
a new server does not require a lot of code.

``TODO``: link to the xeus-qt example when it has been upgraded to xeus-zmq 2.0

xserver_control_main internals
------------------------------

The ``xserver_control_main`` class is an alternative implementation of the server API, its internals are
illustrated in the following diagram:

.. image:: server_control.svg
   :alt: server_control

This server runs four threads that communicate through internal `ZeroMQ` sockets. The main thread is
responsible for polling the ``control`` channel while a dedicated thread listens on the ``shell``
channel. Having separated threads for the ``control`` and ``shell`` channel makes it possible to send
messages on a channel while the kernel is already processing a message on the other channel. For instance
one can send on the ``control`` a request to interrupt a computation running on the ``shell``.

The control thread is also connected to the shell, the publisher and the heartbeat threads through internal
``controller`` channels. These are used to send ``stop`` messages to the subthread and allow to stop the
kernel in a clean way, similarly to the ``xserver_zmq``.

The rest of the implementation is similar to that of ``xserver_zmq``.

xserver_shell_main internals
----------------------------

The ``xserver_shell_main`` class is very similar to the ``xserver_control_main`` class, except that
the main thread listens on the ``shell`` channel as illustrated in the following diagram:

.. image:: server_main.svg
   :alt: server_main

Extending xserver_zmq_split
---------------------------

Like the default implementation, the ``xserver_control_main`` and ``xserver_shell_main`` servers
perform a blocking poll on each channel. It is possible to provide a different execution model for
both kind of servers. However, the methods to do it slightly differs from extending the default
implementation. Instead of inheriting from the `xserver_zmq_split class`_, one can provide independent
execution models for the control channel and the shell channel by inheriting from the `xcontrol_runner class`_
and the `xshell_runner class`_ respectively.

``TODO``: provide a code illustrating this

.. _xeus server API: https://xeus.readthedocs.io/en/latest/server.html#public-api
.. _xserver_zmq class: https://github.com/jupyter-xeus/xeus-zmq/blob/main/include/xeus-zmq/xserver_zmq.hpp
.. _xserver_zmq_split class: https://github.com/jupyter-xeus/xeus-zmq/blob/main/include/xeus-zmq/xserver_zmq_split.hpp
.. _xcontrol_runner class: https://github.com/jupyter-xeus/xeus-zmq/blob/main/include/xeus-zmq/xcontrol_runner.hpp
.. _xshell_runner class: https://github.com/jupyter-xeus/xeus-zmq/blob/main/include/xeus-zmq/xshell_runner.hpp
