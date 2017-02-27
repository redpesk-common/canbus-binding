###########################################################################
# Copyright 2016 IoT.bzh
#
# author: José Bollo <jose.bollo@iot.bzh>
# author: Stéphane Desneux <stephane.desneux@iot.bzh>
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
###########################################################################

project(low-can-binding)

cmake_minimum_required(VERSION 3.3)

include(GNUInstallDirs)

set(PROJECT_VERSION "0.1")
set(PROJECT_ICON "icon.png")
set(PROJECT_LIBDIR "libs")

set(CMAKE_BUILD_TYPE Debug)

###########################################################################

link_libraries(-Wl,--as-needed -Wl,--gc-sections)

add_compile_options(-Wall -Wextra -Wconversion)
add_compile_options(-Wno-unused-parameter) # frankly not using a parameter does it care?
add_compile_options(-Werror=maybe-uninitialized)
add_compile_options(-Werror=implicit-function-declaration)
add_compile_options(-ffunction-sections -fdata-sections)
add_compile_options(-Wl,--as-needed -Wl,--gc-sections)
add_compile_options(-fPIC)
add_compile_options(-D_REENTRANT)

set(CMAKE_C_FLAGS_PROFILING    "-g -O0 -pg -Wp,-U_FORTIFY_SOURCE")
set(CMAKE_C_FLAGS_DEBUG        "-g -O0 -ggdb -Wp,-U_FORTIFY_SOURCE")
set(CMAKE_C_FLAGS_RELEASE      "-g -O0 -Wall -Werror")
set(CMAKE_C_FLAGS_CCOV         "-g -O0 --coverage")

set(CMAKE_CXX_FLAGS_PROFILING    "-g -O0 -std=c++11 -pg -Wp,-U_FORTIFY_SOURCE")
set(CMAKE_CXX_FLAGS_DEBUG        "-g -O0 -std=c++11 -ggdb -Wp,-U_FORTIFY_SOURCE")
set(CMAKE_CXX_FLAGS_RELEASE      "-g -O2 -std=c++11")
set(CMAKE_CXX_FLAGS_CCOV         "-g -O2 -std=c++11 --coverage")

###########################################################################

include(FindPkgConfig)

pkg_check_modules(EXTRAS REQUIRED json-c afb-daemon)
#set(THREADS_PREFER_PTHREAD_FLAG ON)
#find_package(Threads REQUIRED)
add_compile_options(${EXTRAS_CFLAGS})
add_compile_options(${EXTRAS_CXXFLAGS})

# Needed to compile openxc-message-format library.
#
# The reason you need to do this is that some of your messages contain tag
# numbers or field sizes that are larger than what can fit in the default
# 8 bit descriptors.
add_definitions(-DPB_FIELD_16BIT)

# Needed extra directories to hit the required headers files.
include_directories(${EXTRAS_INCLUDE_DIRS} ${PROJECT_LIBDIR}/openxc-message-format/gen/cpp ${PROJECT_LIBDIR}/nanopb/  ${PROJECT_LIBDIR}/uds-c/src  ${PROJECT_LIBDIR}/bitfield-c/src ${PROJECT_LIBDIR}/isotp-c/src)

###########################################################################
# the library used by the binding : openxc, bitfield, uds, isotp

add_library(bitfield STATIC ${PROJECT_LIBDIR}/bitfield-c/src/bitfield/8byte.c ${PROJECT_LIBDIR}/bitfield-c/src/bitfield/bitarray.c ${PROJECT_LIBDIR}/bitfield-c/src/bitfield/bitfield.c ${PROJECT_LIBDIR}/bitfield-c/src/canutil/read.c ${PROJECT_LIBDIR}/bitfield-c/src/canutil/write.c)
add_library(isotp STATIC ${PROJECT_LIBDIR}/isotp-c/src/isotp/isotp.c ${PROJECT_LIBDIR}/isotp-c/src/isotp/receive.c ${PROJECT_LIBDIR}/isotp-c/src/isotp/send.c)
add_library(uds STATIC ${PROJECT_LIBDIR}/uds-c/src/uds/extras.c ${PROJECT_LIBDIR}/uds-c/src/uds/uds.c)
add_library(openxc STATIC ${PROJECT_LIBDIR}/openxc-message-format/gen/cpp/openxc.pb.c ${PROJECT_LIBDIR}/nanopb/pb_encode.c ${PROJECT_LIBDIR}/nanopb/pb_decode.c ${PROJECT_LIBDIR}/nanopb/pb_common.c)
###########################################################################
# the binding for afb

message(STATUS "Creation of ${PROJECT_NAME} binding for AFB-DAEMON")
###########################################################################
add_library(${PROJECT_NAME} MODULE ${PROJECT_NAME}.cpp can-signals.cpp can-utils.cpp can-decoder.cpp can_reader.cpp can_decode_message.cpp can_event_push.cpp openxc-utils.cpp timer.cpp)
target_link_libraries(${PROJECT_NAME} ${EXTRAS_LIBRARIES} bitfield isotp uds openxc pthread)

set_target_properties(${PROJECT_NAME} PROPERTIES
	PREFIX ""
	LINK_FLAGS "-Wl,--version-script=${CMAKE_CURRENT_SOURCE_DIR}/export.map"
)

###########################################################################
# WGT packaging

message(STATUS "Creation of ${PROJECT_NAME}.wgt package for AppFW")
###############################################################
configure_file(config.xml.in config.xml)
configure_file(can_buses.json.in can_buses.json)

add_custom_command(
	OUTPUT ${PROJECT_NAME}.wgt
	DEPENDS ${PROJECT_NAME} bitfield isotp uds openxc config.xml.in can_buses.json.in
	COMMAND rm -rf package
	COMMAND mkdir -p package/${PROJECT_LIBDIR} package/htdocs
	COMMAND cp config.xml package/
	COMMAND cp can_buses.json package/
	COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_ICON} package/icon.png
	COMMAND cp ${PROJECT_NAME}.so package/libs
	COMMAND wgtpkg-pack -f -o ${PROJECT_NAME}.wgt package
)
add_custom_target(widget ALL DEPENDS ${PROJECT_NAME}.wgt)
