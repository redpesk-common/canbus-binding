###########################################################################
# Copyright (C) 2015-2025 IoT.bzh Company
#
# Author: IoT.bzh team <team@iot.bzh>
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

include_directories(BEFORE SYSTEM ${CMAKE_SOURCE_DIR}/include)

add_definitions(-DAFB_BINDING_VERSION=4)

pkg_check_modules(LIBDEPS REQUIRED
	json-c
	afb-binding
	afb-helpers4
	librp-utils
)

###########################################################################
# The library libcanbus
###########################################################################

# Activate J1939
# Need module in kernel
# --------------

if(WITH_FEATURE_J1939)
	find_file(J1939 j1939.h $ENV{PKG_CONFIG_SYSROOT_DIR}/usr/include/linux/can/)
	if(J1939)
		message("Feature J1939 enabled")
		add_definitions(-DUSE_FEATURE_J1939)
		set(SOURCES_J1939
			can/message/j1939-message.cpp
			utils/socketcan-j1939/socketcan-j1939.cpp
			utils/socketcan-j1939/socketcan-j1939-data.cpp
			utils/socketcan-j1939/socketcan-j1939-addressclaiming.cpp
		)
	else(J1939)
		message(FATAL_ERROR "ERROR: No $ENV{PKG_CONFIG_SYSROOT_DIR}/usr/include/linux/can/j1939.h headers found")
	endif(J1939)
else(WITH_FEATURE_J1939)
	message("Feature J1939 disabled")
endif(WITH_FEATURE_J1939)

# Activate ISO TP
# Need module in kernel
# --------------

if(WITH_FEATURE_ISOTP)
	find_file(ISOTP isotp.h $ENV{PKG_CONFIG_SYSROOT_DIR}/usr/include/linux/can)
	if(ISOTP)
		message("Feature ISO TP enabled")
		add_definitions(-DUSE_FEATURE_ISOTP)
		set(SOURCES_ISOTP
			utils/socketcan-isotp.cpp
		)
	else(ISOTP)
		message(FATAL_ERROR "ERROR: No $ENV{PKG_CONFIG_SYSROOT_DIR}/usr/include/linux/can/isotp.h headers found")
	endif(ISOTP)
else(WITH_FEATURE_ISOTP)
	message("Feature ISO TP disabled")
endif(WITH_FEATURE_ISOTP)

# Add target to project dependency list

set (SOURCES_LIB
	binding/low-can-hat.cpp
	binding/low-can-subscription.cpp
	binding/application.cpp
	can/can-bus.cpp
	can/can-decoder.cpp
	can/can-encoder.cpp
	can/message-set.cpp
	can/message-definition.cpp
	can/message/message.cpp
	can/message/can-message.cpp
	can/signals.cpp
	diagnostic/diagnostic-message.cpp
	diagnostic/diagnostic-manager.cpp
	diagnostic/active-diagnostic-request.cpp
	utils/signals.cpp
	utils/openxc-utils.cpp
	utils/timer.cpp
	utils/socketcan.cpp
	#utils/socketcan-raw.cpp
	utils/socketcan-bcm.cpp
	utils/converter.cpp
	utils/frame-codec.cpp
)

add_library(canbus SHARED ${SOURCES_LIB} ${SOURCES_J1939} ${SOURCES_ISOTP})

target_compile_options(canbus PRIVATE ${LIBDEPS_CFLAGS} -pthread)

#target_include_directories(canbus PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(canbus
	openxc-message-format
	uds-c
	isotp-c
	bitfield-c
	pthread
	${LIBDEPS_LINK_LIBRARIES})

set_target_properties(
	canbus PROPERTIES
		VERSION 2
		SOVERSION 2.0
)

install(TARGETS canbus)


###########################################################################
# The binding afb-canbus-binding
###########################################################################

find_program(GENSKEL afb-genskel)
if(NOT GENSKEL)
	message(FATAL_ERROR "can't find afb-genskel")
endif(NOT GENSKEL)

add_custom_command(
	OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/binding/low-can-apidef.h
	COMMAND ${GENSKEL} ${CMAKE_CURRENT_SOURCE_DIR}/binding/low-can-apidef.json > ${CMAKE_CURRENT_SOURCE_DIR}/binding/low-can-apidef.h
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/binding/low-can-apidef.json
)
add_custom_command(
	OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/.version
	COMMAND echo ${PROJECT_VERSION} > ${CMAKE_CURRENT_BINARY_DIR}/.version
)


# Define project Targets
set (SOURCES
	binding/low-can-cb.cpp
	binding/low-can-apidef.h
	${CMAKE_CURRENT_BINARY_DIR}/.version
	)

add_library(canbus-binding SHARED ${SOURCES})

# Binder exposes a unique public entry point
SET_TARGET_PROPERTIES(canbus-binding PROPERTIES
	PREFIX "afb-"
	LABELS "BINDING"
	INSTALL_RPATH $ORIGIN
	OUTPUT_NAME canbus-binding
)

target_include_directories(canbus-binding PRIVATE ${LIBDEPS_INCLUDE_DIRS})

# Library dependencies (include updates automatically)
TARGET_LINK_LIBRARIES(canbus-binding
	canbus
	${LIBDEPS_LINK_LIBRARIES})

configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/manifest.yml.in
	${CMAKE_CURRENT_BINARY_DIR}/manifest.yml
	@ONLY)

set(AFM_APP_DIR ${CMAKE_INSTALL_PREFIX}/redpesk CACHE PATH "Application directory")
set(appdir ${AFM_APP_DIR}/canbus-binding)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/manifest.yml DESTINATION ${appdir}/.rpconfig)
install(FILES canbus-binding-config.json DESTINATION ${appdir}/etc)
install(TARGETS canbus-binding DESTINATION ${appdir}/lib)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/.version DESTINATION ${appdir}/plugins)
