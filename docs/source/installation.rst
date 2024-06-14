.. Copyright (c) 2022, Johan Mabille and Sylvain Corlay

   Distributed under the terms of the BSD 3-Clause License.

   The full license is in the file LICENSE, distributed with this software.

Installation
============

With mamba or conda
-------------------

``xeus-zmq`` has been packaged for the mamba (or conda) package manager.

To ensure that the installation works, it is preferable to install ``xeus-zmq`` in a fresh environment.
It is also needed to use a `miniforge`_  or `miniconda`_ installation because with the full `anaconda`_
you may have a conflict with the ``ZeroMQ`` library already installed in the distribution.

The safest usage is to create an environment named ``xeus-env``

.. code:: bash

    mamba create -n xeus-env
    mamba activate xeus-env

Then you can install ``xeus-zmq`` and its dependencies in this freshly created environment:

.. code:: bash

    mamba install xeus-zmq -c conda-forge

From source
-----------

``xeus-zmq`` depends on the following libraries:
 - libzmq_
 - cppzmq_
 - OpenSSL_
 - nlohmann_json_
 - xeus_

We have packaged all these dependencies on conda-forge. The simplest way to install them is to run:

.. code:: bash

    mamba env create -f environment-dev.yml -n xeus-env
    mamba activate xeus-env

You can then build and install ``xeus-zmq``:

.. code:: bash

    cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$CONDA_PREFIX -DCMAKE_PREFIX_PATH=$CONDA_PREFIX
    make
    make install

.. _miniforge: https://github.com/conda-forge/miniforge#mambaforge
.. _miniconda: https://docs.anaconda.com/free/miniconda
.. _anaconda: https://www.anaconda.com

.. _libzmq: https://github.com/zeromq/libzmq
.. _cppzmq: https://github.com/zeromq/cppzmq
.. _OpenSSL: https://github.com/OpenSSL/OpenSSL
.. _nlohmann_json: https://github.com/nlohmann/json
.. _xeus: https://github.com/jupyter-xeus/xeus
