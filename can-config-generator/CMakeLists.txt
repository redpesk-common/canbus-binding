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
	fill_include_dir(${PROJECT_LIBDIR})
	file(GLOB filelist "${PROJECT_LIBDIR}/*")
	foreach(filename ${filelist})
		if(IS_DIRECTORY ${filename})
			include_directories(${filename})
			STRING(REGEX REPLACE "^.*\/(.*)$" "\\1" shortname ${filename})
			find_source_files(${PROJECT_LIBDIR})
			add_library(${shortname} STATIC ${sources_files})
		endif(IS_DIRECTORY ${filename})
	endforeach()

	add_executable(can-config-generator
		src/main.cpp
		src/openxc/message_set.cpp
		src/openxc/can_bus.cpp
		src/openxc/can_message.cpp
		src/openxc/command.cpp
		src/openxc/diagnostic_message.cpp
		src/openxc/mapping.cpp
		src/openxc/signal.cpp)

	# Library dependencies (include updates automatically)
	TARGET_LINK_LIBRARIES(can-config-generator
		${EXTRA_LINK_LIBRARIES}
		${link_libraries}
	)

	# installation directory
	INSTALL(TARGETS can-config-generator
		RUNTIME DESTINATION ${BINDINGS_INSTALL_DIR})
