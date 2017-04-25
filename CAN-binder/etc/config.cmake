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

# Project Info
# ------------------
set(NAME low-can-project)
set(VERSION "1.0")
set(PRETTY_NAME "Low level CAN binding")
set(DESCRIPTION "Expose CAN Low Level APIs through AGL Framework")
set(URL "https://github.com/iotbzh/CAN_signaling")
set(PROJECT_ICON "icon.png")

# Compilation Mode (DEBUG, RELEASE)
# ----------------------------------
setc(CMAKE_BUILD_TYPE "DEBUG")

# PKG_CONFIG required packages
# -----------------------------
set (PKG_REQUIRED_LIST 
	json-c
	libsystemd
	afb-daemon
)

# Static constante definition
# -----------------------------
add_compile_options(-D_REENTRANT)
add_compile_options(-DPB_FIELD_16BIT)

# LANG Specific compile flags set for all build types
set(CMAKE_C_FLAGS "")
set(CMAKE_CXX_FLAGS "-std=c++11")

# Print a helper message when every thing is finished
setc(CLOSING_MESSAGE "Test with: afb-daemon --rootdir=\$\$(pwd)/low-can-binding/package --ldpaths=\$\$(pwd)/low-can-binding/package/lib --port=1234 --roothttp=\$\$(pwd)/low-can-binding/package/htdocs --tracereq=common --token=\"\" --verbose")
# ----------------------------------------------------

# (BUG!!!) as PKG_CONFIG_PATH does not work [should be an env variable]
# ---------------------------------------------------------------------
setc(CMAKE_INSTALL_PREFIX ${HOME}/opt)
setc(CMAKE_PREFIX_PATH ${CMAKE_INSTALL_PREFIX}/lib64/pkgconfig ${CMAKE_INSTALL_PREFIX}/lib/pkgconfig)
setc(LD_LIBRARY_PATH ${CMAKE_INSTALL_PREFIX}/lib64 ${CMAKE_INSTALL_PREFIX}/lib)

# Optional dependencies order
# ---------------------------
set(EXTRA_DEPENDENCIES_ORDER can-config-generator low-can-binding)

# Optional Extra global include path
# -----------------------------------
#set(EXTRA_INCLUDE_DIRS can-config-generator/3rdparty/json libs/openxc-message-format/gen/cpp libs/nanopb libs/uds-c/src libs/isotp-c/src libs/bitfield-c/src)

# Optional extra libraries
# -------------------------
#set(EXTRA_LINK_LIBRARIES)

# Optional force binding installation
# ------------------------------------
# set(BINDINGS_INSTALL_PREFIX DestinationPath )

# Optional force binding Linking flag
# ------------------------------------
# set(BINDINGS_LINK_FLAG LinkOptions )


