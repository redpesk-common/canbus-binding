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


#--------------------------------------------------------------------------
#  WARNING: 
#     Do not change this cmake template
#     Customise your preferences in "./etc/config.cmake"
#--------------------------------------------------------------------------


# Generic useful macro
# -----------------------
macro(PROJECT_TARGET_ADD TARGET_NAME)
	set(PROJECT_TARGETS ${PROJECT_TARGETS} ${TARGET_NAME} CACHE INTERNAL PROJECT_TARGETS)
	set(TARGET_NAME ${TARGET_NAME})
endmacro(PROJECT_TARGET_ADD)

macro(defstr name value)
	add_definitions(-D${name}=${value})
endmacro(defstr)

macro(setc name value)
	if(NOT DEFINED ${name})
	set(${name} ${value})
	endif(NOT DEFINED ${name})
endmacro(setc)

# Dumb macro to add each directory under a path. Make sure we grab all header files!
macro(fill_include_dir path)
	file(GLOB_RECURSE dirlist LIST_DIRECTORIES true "${path}/*")
	foreach(filename ${dirlist})
		if(IS_DIRECTORY ${filename})
			include_directories(${filename})
		endif(IS_DIRECTORY ${filename})
	endforeach()
endmacro(fill_include_dir)

# Helper function to retrieve source files from a library repo.
function(find_source_files path)
	file(GLOB_RECURSE cfiles "${path}/*.[c]")
	file(GLOB_RECURSE cppfiles "${path}/*.cpp")
	foreach(filename ${cfiles})
		if(NOT ${filename} MATCHES ".*([Tt]est|[Ee]xample|[Ss]ample).*")
			string(APPEND sources "${filename};")
		endif(NOT ${filename} MATCHES ".*([Tt]est|[Ee]xample|[Ss]ample).*")
	endforeach()
	foreach(filename ${cppfiles})
		if(NOT ${filename} MATCHES ".*([Tt]est|[Ee]xample|[Ss]ample).*")
			string(APPEND sources "${filename};")
		endif(NOT ${filename} MATCHES ".*([Tt]est|[Ee]xample|[Ss]ample).*")
	endforeach()
	set(sources_files ${sources} PARENT_SCOPE)
endfunction(find_source_files)

# WGT packaging
macro(populate_widget)
	# Declaration of a custom command that will populate widget tree with the target
	set(POPULE_WIDGET_TARGET "populate_${TARGET_NAME}")

	get_target_property(T ${TARGET_NAME} LABELS)
	if(${T} STREQUAL "BINDING")
		add_custom_command(OUTPUT ${WIDGET_LIBDIR}/${TARGET_NAME}.so
			DEPENDS ${TARGET_NAME}
			COMMAND mkdir -p ${WIDGET_LIBDIR}
			COMMAND cp ${TARGET_NAME}.so ${WIDGET_LIBDIR}
		)
		add_custom_target(${POPULE_WIDGET_TARGET} ALL DEPENDS ${WIDGET_LIBDIR}/${TARGET_NAME}.so)
	elseif(${T} STREQUAL "EXECUTABLE")
		add_custom_command(OUTPUT ${WIDGET_BINDIR}/${TARGET_NAME}
			DEPENDS ${TARGET_NAME}
			COMMAND mkdir -p ${WIDGET_BINDIR}
			COMMAND cp ${TARGET_NAME} ${WIDGET_BINDIR}
		)
		add_custom_target(${POPULE_WIDGET_TARGET} ALL DEPENDS ${WIDGET_BINDIR}/${TARGET_NAME})
	elseif(${T} STREQUAL "HTDOCS")
		get_target_property(OUT ${TARGET_NAME} OUTPUT_NAME)
		add_custom_command(OUTPUT ${WIDGET_HTTPDIR}
			DEPENDS ${TARGET_NAME}
			COMMAND cp -r ${OUT} ${WIDGET_HTTPDIR}
			)
			add_custom_target(${POPULE_WIDGET_TARGET} ALL DEPENDS ${WIDGET_HTTPDIR})
	elseif(${T} STREQUAL "DATA")
		get_target_property(OUT ${TARGET_NAME} OUTPUT_NAME)
		add_custom_command(OUTPUT ${WIDGET_DATADIR}
			DEPENDS ${TARGET_NAME}
			COMMAND cp -r ${OUT} ${WIDGET_DATADIR}
			)
			add_custom_target(${POPULE_WIDGET_TARGET} ALL DEPENDS ${WIDGET_HTTPDIR})
	endif(${T} STREQUAL "BINDING")
	PROJECT_TARGET_ADD(${POPULE_WIDGET_TARGET})
endmacro(populate_widget)

macro(build_widget)
	if("${PROJECT_TARGETS}" MATCHES "populate_")
		if(NOT EXISTS ${WIDGET_DIR}/config.xml.in OR NOT EXISTS ${WIDGET_DIR}/${PROJECT_ICON}.in)
			configure_file(${PROJECT_WGT_DIR}/config.xml.in ${WIDGET_DIR}/config.xml)
			file(COPY ${PROJECT_WGT_DIR}/${PROJECT_ICON}.in DESTINATION ${WIDGET_DIR}/${PROJECT_ICON})
		endif(NOT EXISTS ${WIDGET_DIR}/config.xml.in OR NOT EXISTS ${WIDGET_DIR}/${PROJECT_ICON}.in)

		file(GLOB PROJECT_CONF_FILES "${PROJECT_WGT_DIR}/etc/*")
		if(${PROJECT_CONF_FILES})
			file(COPY "${PROJECT_WGT_DIR}/etc/*" DESTINATION ${WIDGET_ETCDIR}/)
		endif(${PROJECT_CONF_FILES})

		add_custom_command(OUTPUT ${PROJECT_NAME}.wgt
		DEPENDS ${PROJECT_TARGETS}
		COMMAND wgtpkg-pack -f -o ${PROJECT_NAME}.wgt ${WIDGET_DIR}
		)
		add_custom_target(widget DEPENDS ${PROJECT_NAME}.wgt)
	else()
		MESSAGE(FATAL_ERROR "Widget tree empty, please populate it by calling  populate_widget() macro with target you want to include into it.")
	endif("${PROJECT_TARGETS}" MATCHES "populate_")
endmacro(build_widget)

macro(search_targets)
	file(GLOB filelist "*")
	foreach(filename ${filelist})
	if(EXISTS "${filename}/CMakeLists.txt")
		add_subdirectory(${filename})
	endif(EXISTS "${filename}/CMakeLists.txt")
	endforeach()
endmacro()

setc(CMAKE_BUILD_TYPE Debug)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMP0048 1)

# Include project configuration
# ------------------------------
project(${NAME} VERSION ${VERSION})
setc(PROJECT_WGT_DIR "packaging/wgt")
setc(PROJECT_LIBDIR "libs")
setc(PROJECT_RESOURCES "data")

INCLUDE(FindPkgConfig)
INCLUDE(CheckIncludeFiles)
INCLUDE(CheckLibraryExists)
INCLUDE(GNUInstallDirs)

# Default compilation options
############################################################################
link_libraries(-Wl,--as-needed -Wl,--gc-sections)
add_compile_options(-Wall -Wextra -Wconversion)
add_compile_options(-Wno-unused-parameter) # frankly not using a parameter does it care?
add_compile_options(-Wno-sign-compare -Wno-sign-conversion)
add_compile_options(-Werror=maybe-uninitialized)
add_compile_options(-Werror=implicit-function-declaration)
add_compile_options(-ffunction-sections -fdata-sections)
add_compile_options(-fPIC)
add_compile_options(-g)

setc(CMAKE_C_FLAGS_PROFILING   "-g -O2 -pg -Wp,-U_FORTIFY_SOURCE")
setc(CMAKE_C_FLAGS_DEBUG       "-g -O2 -ggdb -Wp,-U_FORTIFY_SOURCE")
setc(CMAKE_C_FLAGS_RELEASE     "-O2")
setc(CMAKE_C_FLAGS_CCOV        "-g -O2 --coverage")

set(CMAKE_CXX_FLAGS_PROFILING    "-g -O0 -pg -Wp,-U_FORTIFY_SOURCE")
set(CMAKE_CXX_FLAGS_DEBUG        "-g -O0 -ggdb -Wp,-U_FORTIFY_SOURCE")
set(CMAKE_CXX_FLAGS_RELEASE      "-g -O2")
set(CMAKE_CXX_FLAGS_CCOV "-g -O2 --coverage")

setc(CMAKE_INSTALL_PREFIX      "${CMAKE_SOURCE_DIR}/Install")

# (BUG!!!) as PKG_CONFIG_PATH does not work [should be en env variable]
setc(PKG_CONFIG_USE_CMAKE_PREFIX_PATH 1)

# Loop on required package and add options
foreach (PKG_CONFIG ${PKG_REQUIRED_LIST})
	PKG_CHECK_MODULES(${PKG_CONFIG} REQUIRED ${PKG_CONFIG})

	INCLUDE_DIRECTORIES(${${PKG_CONFIG}_INCLUDE_DIRS})
	list (APPEND link_libraries ${${PKG_CONFIG}_LIBRARIES})
	add_compile_options (${${PKG_CONFIG}_CFLAGS})
endforeach(PKG_CONFIG)

# Optional LibEfence Malloc debug library
IF(CMAKE_BUILD_TYPE MATCHES DEBUG)
CHECK_LIBRARY_EXISTS(efence malloc "" HAVE_LIBEFENCE)
IF(HAVE_LIBEFENCE)
	MESSAGE(STATUS "Linking with ElectricFence for debugging purposes...")
	SET(libefence_LIBRARIES "-lefence")
	list (APPEND link_libraries libefence_LIBRARIES})
ENDIF(HAVE_LIBEFENCE)
ENDIF(CMAKE_BUILD_TYPE MATCHES DEBUG)

# set default include directories
INCLUDE_DIRECTORIES(${EXTRA_INCLUDE_DIRS})

# If no install dir try to guess some smart default
if(BINDINGS_INSTALL_PREFIX)
	set(BINDINGS_INSTALL_DIR ${BINDINGS_INSTALL_PREFIX}/${PROJECT_NAME})
else()
	set(BINDINGS_INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME})
endif()

# Define a default package directory
if(WIDGET_PREFIX)
	set(WIDGET_DIR ${WIDGET_PREFIX}/package)
else()
	set(WIDGET_DIR ${CMAKE_CURRENT_BINARY_DIR}/package)
endif()

# and their subsequent subdir
set(WIDGET_BINDIR ${WIDGET_DIR}/bin)
set(WIDGET_ETCDIR ${WIDGET_DIR}/etc)
set(WIDGET_LIBDIR ${WIDGET_DIR}/lib)
set(WIDGET_HTTPDIR ${WIDGET_DIR}/htdocs)
set(WIDGET_DATADIR ${WIDGET_DIR}/data)

# Default Linkflag
if(NOT BINDINGS_LINK_FLAG)
	set(BINDINGS_LINK_FLAG "-Wl,--version-script=${CMAKE_SOURCE_DIR}/etc/export.map")
endif()

# Add a dummy target to enable global dependency order
# -----------------------------------------------------
if(EXTRA_DEPENDENCIES_ORDER)
	set(DEPENDENCIES_TARGET ${PROJECT_NAME}_extra_dependencies)
	add_custom_target(${DEPENDENCIES_TARGET} ALL
		DEPENDS ${EXTRA_DEPENDENCY_ORDER} 
	)
endif()

# Cmake does not maintain targets list before 3.7
# -------------------------------------------------
if(${CMAKE_VERSION} VERSION_LESS 3.7)
	set(GLOBAL_TARGET_LIST ${PROJECT_TARGETS})
else()
	get_property(GLOBAL_TARGET_LIST GLOBAL PROPERTY GlobalTargetList)
endif()

# Print developer helper message when everything is done
# -------------------------------------------------------
if(CLOSING_MESSAGE AND GLOBAL_TARGET_LIST)
	add_custom_target(${PROJECT_NAME}_done ALL
		DEPENDS ${DEPENDENCIES_TARGET} ${GLOBAL_TARGET_LIST}
		COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --cyan "++ ${CLOSING_MESSAGE}"
	)
endif()

set(ADDITIONAL_MAKE_CLEAN_FILES, "low-can-binding/low-can-binding.wgt")
