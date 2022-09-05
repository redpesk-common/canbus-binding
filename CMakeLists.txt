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

CMAKE_MINIMUM_REQUIRED(VERSION 3.3)

project(canbus-binding)

include(CTest)
enable_testing()
include(${CMAKE_CURRENT_SOURCE_DIR}/conf.d/cmake/config.cmake)

include(GNUInstallDirs)
configure_file(${PROJECT_NAME}.pc.in ${PROJECT_NAME}.pc @ONLY)

INSTALL(FILES
	${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc
	DESTINATION
	${CMAKE_INSTALL_LIBDIR}/pkgconfig
)

configure_file(${PROJECT_NAME}.conf.in ${PROJECT_NAME}.conf @ONLY)

INSTALL(FILES
	${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.conf
	DESTINATION
	${CMAKE_INSTALL_SYSCONFDIR}/ld.so.conf.d
)
INSTALL(CODE "execute_process(COMMAND ldconfig)")
