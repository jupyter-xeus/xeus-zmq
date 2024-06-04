.. Copyright (c) 2022, Johan Mabille and Sylvain Corlay

   Distributed under the terms of the BSD 3-Clause License.

   The full license is in the file LICENSE, distributed with this software.

========
XEUS-ZMQ
========

ZeroMQ-based middleware for xeus

Introduction
------------

``xeus-zmq`` provides various implementations of the `xserver`_  API from ``xeus``, based on the `ZeroMQ library`_.
These implementations all conform to the `Jupyter Kernel Protocol specification`_.

It also provides a client framework that can be used to build Jupyter clients, and basic blocks to build a
debugger for Jupyter kernels.

Licensing
---------

We use a shared copyright model that enables all contributors to maintain the
copyright on their contributions.

This software is licensed under the BSD-3-Clause license. See the LICENSE file
for details.

.. toctree::
   :caption: INSTALLATION
   :maxdepth: 2

   installation

.. toctree::
   :caption: USAGE
   :maxdepth: 2

   usage
   server

.. _xserver: https://github.com/jupyter-xeus/xeus/blob/main/include/xeus/xserver.hpp
.. _ZeroMQ library: https://zeromq.org/
.. _Jupyter Kernel Protocol specification: https://jupyter-client.readthedocs.io/en/stable/messaging.html
