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
PROJECT_TARGET_ADD(can-config-generator)

	# Define project Targets
	add_executable(can-config-generator
		main.cpp
		openxc/message_set.cpp
		openxc/can_bus.cpp
		openxc/can_message.cpp
		openxc/command.cpp
		openxc/diagnostic_message.cpp
		openxc/mapping.cpp
		openxc/signal.cpp)

	# Library dependencies (include updates automatically)
	TARGET_LINK_LIBRARIES(can-config-generator
		${link_libraries}
	)

	# installation directory
	INSTALL(TARGETS can-config-generator
RUNTIME DESTINATION ${BINDINGS_INSTALL_DIR})
