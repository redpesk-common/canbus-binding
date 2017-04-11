###########################################################################
# Copyright 2015, 2016, 2017 IoT.bzh
#
# author: Fulup Ar Foll <fulup@iot.bzh>
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
PROJECT_TARGET_ADD(low-can-binding)

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

	add_library(${TARGET_NAME} MODULE ${TARGET_NAME}.cpp configuration.cpp configuration-generated.cpp
		can/can-bus.cpp can/can-bus-dev.cpp can/can-message-set.cpp can/can-message-definition.cpp can/can-message.cpp can/can-signals.cpp can/can-decoder.cpp
		diagnostic/diagnostic-message.cpp diagnostic/diagnostic-manager.cpp diagnostic/active-diagnostic-request.cpp
		utils/signals.cpp utils/openxc-utils.cpp utils/timer.cpp utils/socket.cpp)

	# Binder exposes a unique public entry point
	SET_TARGET_PROPERTIES(${TARGET_NAME} PROPERTIES
	PREFIX ""
	LINK_FLAGS ${BINDINGS_LINK_FLAG}
		OUTPUT_NAME ${TARGET_NAME}
	)

	# Library dependencies (include updates automatically)
	TARGET_LINK_LIBRARIES(${TARGET_NAME}
		${EXTRA_LINK_LIBRARIES}
		${link_libraries}
	)

	# installation directory
	INSTALL(TARGETS ${TARGET_NAME}
		LIBRARY DESTINATION ${BINDINGS_INSTALL_DIR})

	# WGT packaging
	message(STATUS "Creation of ${TARGET_NAME}.wgt package for AppFW")

	configure_file(${PROJECT_WGT_DIR}/config.xml config.xml)

	if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_WGT_DIR}/etc")
		file(GLOB conf_files "${PROJECT_WGT_DIR}/etc/*")
		foreach(filename ${conf_files})
			STRING(REGEX REPLACE "^.*\/(.*)$" "\\1" shortname ${filename})
			message(STATUS "Copying config file ${shortname}")
			configure_file(${filename} ${shortname})
		endforeach()
	endif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_WGT_DIR}/etc")

	add_custom_command(
		OUTPUT ${TARGET_NAME}.wgt
		DEPENDS ${TARGET_NAME} ${EXTRA_LINK_LIBRARIES} ${conf_files} ${PROJECT_WGT_DIR}/config.xml
		COMMAND rm -rf package
		COMMAND mkdir -p package/lib package/htdocs package/etc
		COMMAND cp config.xml package/
		COMMAND cp ${conf_files} package/etc
		COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/packaging/wgt/${PROJECT_ICON} package/icon.png
		COMMAND cp ${TARGET_NAME}.so package/lib
		COMMAND wgtpkg-pack -f -o ${TARGET_NAME}.wgt package
	)
	add_custom_target(widget ALL DEPENDS ${TARGET_NAME}.wgt)
