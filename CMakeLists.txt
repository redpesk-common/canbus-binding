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

macro(fill_include_dir path)
	file(GLOB_RECURSE dirlist LIST_DIRECTORIES true "${path}/*")
	foreach(filename ${dirlist})
		if(IS_DIRECTORY ${filename})
			include_directories(${filename})
		endif(IS_DIRECTORY ${filename})
	endforeach()
endmacro(fill_include_dir)

function(find_source_files path)
	file(GLOB_RECURSE filelist "${path}/*.[cpx]")
	foreach(filename ${filelist})
		if(NOT ${filename} MATCHES ".*(test|example|sample).*")
			string(APPEND sources "${filename};")
		endif(NOT ${filename} MATCHES ".*(test|example|sample).*")
	endforeach()
	set(sources_files ${sources} PARENT_SCOPE)
endfunction(find_source_files)

CMAKE_MINIMUM_REQUIRED(VERSION 3.3)
setc(CMAKE_BUILD_TYPE Debug)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMP0048 1)

# Compiler selection should append before project definition
# -----------------------------------------------------------
foreach (GCC_VERSION "6" "5")
	execute_process(COMMAND gcc-${GCC_VERSION} -dumpversion RESULT_VARIABLE GCCRC OUTPUT_QUIET ERROR_QUIET)
	if(GCCRC EQUAL 0)
		set(CMAKE_C_COMPILER "gcc-${GCC_VERSION}")
		set(CMAKE_CXX_COMPILER "g++-${GCC_VERSION}")
		set(CMAKE_C_VERSION ${GCC_VERSION})
		break()
	endif(GCCRC EQUAL 0)
endforeach(GCC_VERSION)

# Include project configuration
# ------------------------------
include(${CMAKE_CURRENT_SOURCE_DIR}/etc/config.cmake)
project(${NAME} VERSION ${VERSION})
setc(PROJECT_PRETTY_NAME "${PRETTY_NAME}")
setc(PROJECT_DESCRIPTION "${DESCRIPTION}")
setc(PROJECT_WGT_DIR "packaging/wgt")
setc(PROJECT_LIBDIR "libs")
setc(PROJECT_RESOURCES "data")

message(STATUS "")
message(STATUS "Project=${PROJECT_NAME}/${VERSION}[${PRETTY_NAME} ${DESCRIPTION}]")
message(STATUS "")
message(STATUS "gcc/g++ version-${CMAKE_C_VERSION}.x selected")

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

# If no install dir try to guest some smart default
if(BINDINGS_INSTALL_PREFIX)
	set(BINDINGS_INSTALL_DIR ${BINDINGS_INSTALL_PREFIX}/${PROJECT_NAME})
else()
	set(BINDINGS_INSTALL_DIR ${CMAKE_INSTALL_FULL_LIBDIR}/${PROJECT_NAME})
endif()

# Default Linkflag
if(NOT BINDINGS_LINK_FLAG)
	set(BINDINGS_LINK_FLAG "-Wl,--version-script=${CMAKE_SOURCE_DIR}/etc/export.map")
endif()

# Bindings to compile
# --------------------
file(GLOB filelist "[A-z]*")
foreach(filename ${filelist})
if(EXISTS "${filename}/CMakeLists.txt")
	add_subdirectory(${filename})
endif(EXISTS "${filename}/CMakeLists.txt")
endforeach()

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
