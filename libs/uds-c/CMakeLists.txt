###########################################################################
# Copyright (C) 2015-2025 IoT.bzh Company
#
# Author: Fulup Ar Foll <fulup@iot.bzh>
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

# Define project Target
add_library(uds-c STATIC 
	src/uds/extras.c
	src/uds/uds.c)

target_compile_options(uds-c PUBLIC -fPIC)

# Binder exposes a unique public entry point
SET_TARGET_PROPERTIES(uds-c PROPERTIES
	OUTPUT_NAME uds-c
)

# Define target includes
TARGET_INCLUDE_DIRECTORIES(uds-c
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Library dependencies (include updates automatically)
TARGET_LINK_LIBRARIES(uds-c
	isotp-c)
