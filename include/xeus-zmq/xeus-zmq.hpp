/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XEUS_ZMQ_EXPORT_HPP
#define XEUS_ZMQ_EXPORT_HPP

#ifdef _WIN32
    #ifdef XEUS_ZMQ_STATIC_LIB
        #define XEUS_ZMQ_API
    #else
        #ifdef XEUS_ZMQ_EXPORTS
            #define XEUS_ZMQ_API __declspec(dllexport)
        #else
            #define XEUS_ZMQ_API __declspec(dllimport)
        #endif
    #endif
#else
    #define XEUS_ZMQ_API __attribute__((visibility("default")))
#endif

// Project version
#define XEUS_ZMQ_VERSION_MAJOR 3
#define XEUS_ZMQ_VERSION_MINOR 1
#define XEUS_ZMQ_VERSION_PATCH 1

// Binary version
// See the following URL for explanations about the binary versionning
// https://www.gnu.org/software/libtool/manual/html_node/Updating-version-info.html#Updating-version-info
#define XEUS_ZMQ_BINARY_CURRENT 7
#define XEUS_ZMQ_BINARY_REVISION 1
#define XEUS_ZMQ_BINARY_AGE 1

#endif

