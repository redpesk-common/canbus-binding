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
add_library(bitfield-c STATIC
	src/bitfield/8byte.c
	src/bitfield/bitarray.c 
	src/bitfield/bitfield.c
	src/canutil/read.c
	src/canutil/write.c)

target_compile_options(bitfield-c PUBLIC -fPIC)

# Binder exposes a unique public entry point
SET_TARGET_PROPERTIES(bitfield-c PROPERTIES
	OUTPUT_NAME bitfield-c
)

# Define target includes
TARGET_INCLUDE_DIRECTORIES(bitfield-c
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src
)
