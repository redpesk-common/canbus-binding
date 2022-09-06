###########################################################################
# Copyright 2015, 2016, 2017 IoT.bzh
#
# author: Fulup Ar Foll <fulup@iot.bzh>
# contrib: Romain Forlot <romain.forlot@iot.bzh>
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

# Add target to project dependency list

PROJECT_TARGET_ADD(canbus)

	if(WITH_FEATURE_ISOTP)
	set(SOURCES_ISOTP
		utils/socketcan-isotp.cpp
	)
	endif()


	if(WITH_FEATURE_J1939)
	set(SOURCES_J1939
		can/message/j1939-message.cpp
		utils/socketcan-j1939/socketcan-j1939.cpp
		utils/socketcan-j1939/socketcan-j1939-data.cpp
		utils/socketcan-j1939/socketcan-j1939-addressclaiming.cpp
	)
	endif()

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
	)

	add_library(${TARGET_NAME} SHARED ${SOURCES_LIB} ${SOURCES_J1939} ${SOURCES_ISOTP})

	# Binder exposes a unique public entry point
	SET_TARGET_PROPERTIES(${TARGET_NAME} PROPERTIES
	LABELS "LIBRARY"
	IMPORTED_LOCATION ${CMAKE_CURRENT_BINARY_DIR}/lib${TARGET_NAME}.so
	)

	target_include_directories(${TARGET_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/include)

	TARGET_LINK_LIBRARIES(${TARGET_NAME}
		openxc-message-format
		uds-c
		isotp-c
		bitfield-c
		${link_libraries})

PROJECT_TARGET_ADD(canbus-binding)

	# Define project Targets
	set (SOURCES
		binding/low-can-cb.cpp
		)

	add_library(${TARGET_NAME} MODULE ${SOURCES})

	set(OPENAPI_DEF "binding/low-can-apidef" CACHE STRING "name and path to the JSON API definition without extension")
	# Binder exposes a unique public entry point
	SET_TARGET_PROPERTIES(${TARGET_NAME} PROPERTIES
		PREFIX "afb-"
		LABELS "BINDINGV3"
		LINK_FLAGS ${BINDINGS_LINK_FLAG}
		OUTPUT_NAME ${TARGET_NAME}
	)

	target_include_directories(${TARGET_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/include)

	# Library dependencies (include updates automatically)
	TARGET_LINK_LIBRARIES(${TARGET_NAME}
		canbus
		openxc-message-format
		uds-c
		isotp-c
		bitfield-c
		${link_libraries})
