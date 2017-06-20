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
PROJECT_TARGET_ADD(isotp-c)

	# Define project Target
	add_library(${TARGET_NAME} STATIC 
		src/isotp/isotp.c
		src/isotp/receive.c
		src/isotp/send.c)

	# Binder exposes a unique public entry point
	SET_TARGET_PROPERTIES(${TARGET_NAME} PROPERTIES
		OUTPUT_NAME ${TARGET_NAME}
	)

	# Define target includes
	TARGET_INCLUDE_DIRECTORIES(${TARGET_NAME}
		PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src
	)

	# Library dependencies (include updates automatically)
	TARGET_LINK_LIBRARIES(${TARGET_NAME}
		bitfield-c
		${link_libraries})
