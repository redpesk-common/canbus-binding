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
PROJECT_TARGET_ADD(low-can)

	# Define project Targets
	add_library(${TARGET_NAME} MODULE
		#binding/${TARGET_NAME}-hat.cpp
		binding/${TARGET_NAME}-cb.cpp
		binding/${TARGET_NAME}-socket.cpp
		binding/${TARGET_NAME}-subscription.cpp
		binding/application.cpp
		binding/application-generated.cpp
		can/can-bus.cpp
		can/can-message-set.cpp
		can/can-message-definition.cpp
		can/can-message.cpp
		can/can-signals.cpp
		can/can-decoder.cpp
		can/can-encoder.cpp
		diagnostic/diagnostic-message.cpp
		diagnostic/diagnostic-manager.cpp
		diagnostic/active-diagnostic-request.cpp
		utils/signals.cpp
		utils/openxc-utils.cpp
		utils/timer.cpp
		utils/socketcan.cpp
		#utils/socketcan-raw.cpp
		utils/socketcan-bcm.cpp
		utils/config-parser.cpp)

	set(OPENAPI_DEF "binding/low-can-apidef" CACHE STRING "name and path to the JSON API definition without extension")
	# Binder exposes a unique public entry point
	SET_TARGET_PROPERTIES(${TARGET_NAME} PROPERTIES
		PREFIX "afb-"
		LABELS "BINDINGV2"
		LINK_FLAGS ${BINDINGS_LINK_FLAG}
		OUTPUT_NAME ${TARGET_NAME}
	)

	# Library dependencies (include updates automatically)
	TARGET_LINK_LIBRARIES(${TARGET_NAME}
		ini-config
		openxc-message-format
		uds-c
		isotp-c
		bitfield-c
		${link_libraries})

