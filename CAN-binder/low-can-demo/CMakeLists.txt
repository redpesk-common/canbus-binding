###########################################################################
# Copyright 2015, 2016, 2017 IoT.bzh
#
# author: Fulup Ar Foll <fulup@iot.bzh>
# contrib: Romain Forlot <romain.forlot@iot.bzh>w
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

# Add subdir targets
search_targets()

# Add target to project dependency list
PROJECT_TARGET_ADD(low-can-demo)

	# Define project Targets
	add_custom_command(OUTPUT dist.prod
	DEPENDS ${TARGET_NAME}
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	COMMAND npm install
	COMMAND gulp build-app-prod
	COMMAND cp -r dist.prod ${CMAKE_CURRENT_BINARY_DIR})

	add_custom_target(${TARGET_NAME} ALL DEPENDS dist.prod)
 
	# Binder exposes a unique public entry point
	SET_TARGET_PROPERTIES(${TARGET_NAME} PROPERTIES
		PREFIX ""
		LABELS "HTDOCS"
		OUTPUT_NAME dist.prod)

	# installation directory
	INSTALL(DIRECTORY dist.prod/ DESTINATION ${BINDINGS_INSTALL_DIR})

	populate_widget()