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

include(GNUInstallDirs)

set(TARGET "root@192.168.1.206")
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

pkg_check_modules(EXTRAS REQUIRED json-c libsystemd afb-daemon)
add_compile_options(${EXTRAS_CFLAGS})
add_compile_options(${EXTRAS_CXXFLAGS})

# Needed to compile openxc-message-format library.
#
# The reason you need to do this is that some of your messages contain tag
# numbers or field sizes that are larger than what can fit in the default
# 8 bit descriptors.
add_definitions(-DPB_FIELD_16BIT)

# Needed extra directories to hit the required headers files.
include_directories(${EXTRAS_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR} ${PROJECT_LIBDIR}/openxc-message-format/gen/cpp ${PROJECT_LIBDIR}/nanopb/  ${PROJECT_LIBDIR}/uds-c/src  ${PROJECT_LIBDIR}/bitfield-c/src ${PROJECT_LIBDIR}/isotp-c/src)
link_libraries(${EXTRAS_LIBRARIES})
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
add_library(${PROJECT_NAME} MODULE ${PROJECT_NAME}.cpp configuration.cpp configuration-generated.cpp
	can/can-bus.cpp can/can-bus-dev.cpp can/can-message-set.cpp can/can-message-definition.cpp can/can-message.cpp can/can-signals.cpp can/can-decoder.cpp
	diagnostic/diagnostic-message.cpp diagnostic/diagnostic-manager.cpp diagnostic/active-diagnostic-request.cpp
	utils/signals.cpp utils/openxc-utils.cpp utils/timer.cpp utils/socket.cpp)
target_link_libraries(${PROJECT_NAME} pthread bitfield uds isotp openxc)

set_target_properties(${PROJECT_NAME} PROPERTIES
	PREFIX ""
	LINK_FLAGS "-Wl,--version-script=${CMAKE_CURRENT_SOURCE_DIR}/export.map"
)

###########################################################################
# WGT packaging

message(STATUS "Creation of ${PROJECT_NAME}.wgt package for AppFW")
###########################################################################
configure_file(config.xml.in config.xml)
configure_file(can_buses.json.in can_buses.json)

add_custom_command(
	OUTPUT ${PROJECT_NAME}.wgt
	DEPENDS ${PROJECT_NAME} bitfield isotp uds openxc config.xml.in can_buses.json.in
	COMMAND rm -rf package
	COMMAND mkdir -p package/${PROJECT_LIBDIR} package/htdocs package/etc
	COMMAND cp config.xml package/
	COMMAND cp can_buses.json package/etc
	COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_ICON} package/icon.png
	COMMAND cp ${PROJECT_NAME}.so package/libs
	COMMAND wgtpkg-pack -f -o ${PROJECT_NAME}.wgt package
)
add_custom_target(widget ALL DEPENDS ${PROJECT_NAME}.wgt)

###########################################################################
# WGT install

#message(STATUS "Creation of ${PROJECT_NAME}.wgt package for AppFW")
###########################################################################
install(CODE "execute_process(
	COMMAND scp -r src/${PROJECT_NAME}.wgt ${CMAKE_SOURCE_DIR} ${TARGET}:/tmp)"
	CODE "execute_process(
	COMMAND ssh ${TARGET} \"/usr/bin/afm-util remove ${PROJECT_NAME}@${PROJECT_VERSION} && /usr/bin/afm-util install /tmp/${PROJECT_NAME}.wgt\")"
	)
