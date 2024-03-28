###########################################################################
# Copyright 2015 - 2018 IoT.bzh
#
# author: Romain Forlot <romain.forlot@iot.bzh>
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

CMAKE_MINIMUM_REQUIRED(VERSION 3.12)

project(canbus-binding
        VERSION 2.0.3
        DESCRIPTION "Expose CAN bus APIs through redpesk Framework"
        HOMEPAGE_URL "https://github.com/redpesk-common/canbus-binding"
        LANGUAGES C CXX)

set(PROJECT_AUTHOR "IoT.bzh")
set(PROJECT_LICENSE "APL2.0")

include(FindPkgConfig)
include(GNUInstallDirs)
include(CTest)
enable_testing()

option(WITH_FEATURE_J1939 "Activate support of J1939" ON)
option(WITH_FEATURE_ISOTP "Activate support of ISOTP" ON)

include(detect-can.cmake)


# sub projects

add_subdirectory(include)
add_subdirectory(libs)
add_subdirectory(low-can-binding)

# install the pkg-config manifest
configure_file(${PROJECT_NAME}.pc.in ${PROJECT_NAME}.pc @ONLY)
INSTALL(FILES
	${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc
	DESTINATION
	${CMAKE_INSTALL_LIBDIR}/pkgconfig
)

# install documentation and templates
INSTALL(DIRECTORY docs DESTINATION ${CMAKE_INSTALL_DATADIR}/canbus-binding)
INSTALL(DIRECTORY _plugin_template/ DESTINATION ${CMAKE_INSTALL_DATADIR}/canbus-binding/plugin_template)
