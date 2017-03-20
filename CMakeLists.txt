###########################################################################
# Copyright 2016 IoT.bzh
#
# author: José Bollo <jose.bollo@iot.bzh>
# author: Stéphane Desneux <stephane.desneux@iot.bzh>
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

cmake_minimum_required(VERSION 3.3)
project(low-can-binding)
add_subdirectory(src)

add_custom_command(
	OUTPUT ${PROJECT_NAME}.pdf
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	COMMAND gitbook install
	COMMAND gitbook pdf docs ${PROJECT_NAME}.pdf
)
add_custom_target(pdf
	DEPENDS ${PROJECT_NAME}.pdf)

add_custom_command(
	OUTPUT _book
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	COMMAND gitbook install
	COMMAND gitbook serve
)
add_custom_target(serve
	DEPENDS _book)

add_custom_command(
	OUTPUT docs_doxygen
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	COMMAND doxygen CAN_signaling.doxygen
)
add_custom_target(doxygen
	DEPENDS docs_doxygen)