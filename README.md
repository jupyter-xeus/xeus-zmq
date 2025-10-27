# xeus-zmq

[![GithubActions](https://github.com/jupyter-xeus/xeus-zmq/actions/workflows/main.yml/badge.svg)](https://github.com/jupyter-xeus/xeus-zmq/actions/workflows/main.yml)
[![Documentation Status](https://readthedocs.org/projects/xeus-zmq/badge/?version=latest)](https://xeus-zmq.readthedocs.io/en/latest/?badge=latest)
[![Zulip](https://img.shields.io/badge/social_chat-zulip-blue.svg)](https://jupyter.zulipchat.com/#narrow/channel/539737-Jupyter-Kernels)

ZeroMQ-based middleware for xeus

## Introduction

`xeus-zmq` provides various implementations of the [xserver](https://github.com/jupyter-xeus/xeus/blob/main/include/xeus/xserver.hpp)
API from `xeus`, based on the [ZeroMQ library](https://zeromq.org/). These implementations all conform to the
[Jupyter Kernel Protocol specification](https://jupyter-client.readthedocs.io/en/stable/messaging.html).

## Installation

`xeus-zmq` has been packaged on all platforms for the mamba (or conda) package manager.

```
mamba install xeus-zmq -c conda-forge
```

## Documentation

The documentation can be found with that of xeus at http://xeus-zmq.readthedocs.io/

## Usage

`xeus-zmq` provides server building functions that can be passed to the kernel constructor:


```cpp
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
```

See the [documentation](http://xeus.readthedocs.io/) for an exhaustive list of the available functions.

### Building from sources

`xeus-zmq` depends on the following libraries: [xeus](https://github.com/jupyter-xeus/xeus), [`ZeroMQ`](https://github.com/zeromq/libzmq),
[`cppzmq`](https://github.com/zeromq/cppzmq), [`OpenSSL`](https://github.com/openssl/openssl), and [`nlohmann_json`](https://github.com/nlohmann/json).

| xeus-zmq |  xeus   | ZeroMQ  | cppzmq  | nlohmann json | OpenSSL |
|----------|---------|---------|---------|---------------|---------|
|  main    | ^5.2.4  | ^4.2.5  | ^4.8.1  |     ^3.12.0   | ^3.0    |
|  3.1.1   | ^5.2.4  | ^4.2.5  | ^4.8.1  |     ^3.12.0   | ^3.0    |
|  3.1.0   | ^5.0.0  | ^4.2.5  | ^4.8.1  |     ^3.11.3   | ^3.0    |
|  3.0.0   | ^5.0.0  | ^4.2.5  | ^4.8.1  |     ^3.11.3   | ^3.0    |

Versions prior to version 3 depend have an additional dependency on [xtl](https://github.com/xtensor-stack/xtl).

| xeus-zmq |  xeus   | ZeroMQ  | cppzmq  | nlohmann json | OpenSSL |      xtl       |
|----------|---------|---------|---------|---------------|---------|----------------|
|  main    | ^4.0.0  | ^4.2.5  | ^4.8.1  |     ^3.11.3   | ^3.0    | >=0.7.0,<0.8.0 |
|  2.0.0   | ^4.0.0  | ^4.2.5  | ^4.8.1  |     ^3.11.3   | ^3.0    | >=0.7.0,<0.8.0 |
|  1.x     | ^3.0.0  | ^4.2.5  | ^4.8.1  |     ^3.2.0    | ^3.0    | >=0.7.0,<0.8.0 |


We have packaged all these dependencies on conda-forge. The simplest way to install them is to run:

```bash
mamba install cmake pkg-config zeromq cppzmq OpenSSL nlohmann_json=3.11.2 xeus -c conda-forge
```

Once you have installed the dependencies, you can build and install `xeus-zmq`:

```bash
cmake -D CMAKE_BUILD_TYPE=Release
make
make install
```

## Installing the Dependencies from Source

The dependencies can also be installed from source. Simply clone the directories and run the following cmake (cmake >= 3.8)  and make instructions.

### xeus

[xeus](https://github.com/jupyter-xeus/xeus) is the core implementation of the Jupyter kernel protocol.
```bash
cmake -D CMAKE_BUILD_TYPE=Rlease
make
make install
```

### ZeroMQ

[ZeroMQ](https://github.com/zeromq/libzmq) is the messaging library underlying the Jupyter kernel protocol.

```bash
cmake -D WITH_PERF_TOOL=OFF -D ZMQ_BUILD_TESTS=OFF -D ENABLE_CPACK=OFF
-D CMAKE_BUILD_TYPE=Release
make
make install
```

## OpenSSL

[OpenSSL](https://www.openssl.org/) is packaged for most package managers (apt-get, rpm, mamba).
We recommend making use of an off-the-shelf build of OpenSSL for your system.

For more information on building OpenSSL, check out the official [OpenSSL wiki](https://wiki.openssl.org/index.php/Compilation_and_Installation).

### cppzmq

[cppzmq](https://github.com/zeromq/cppzmq) is a header only library:

```bash
cmake -D CMAKE_BUILD_TYPE=Release
make install
```

### json for modern cpp

[nlohmann_json](https://github.com/nlohmann/json) is a header only library

```bash
cmake
make install
```

## Contributing

See [CONTRIBUTING.md](./CONTRIBUTING.md) to know how to contribute and set up a
development environment.

## License

We use a shared copyright model that enables all contributors to maintain the
copyright on their contributions.

This software is licensed under the BSD-3-Clause license. See the [LICENSE](LICENSE) file for details.
