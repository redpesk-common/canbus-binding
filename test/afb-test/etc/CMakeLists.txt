###########################################################################
# Copyright (C) 2015-2025 - 2018 IoT.bzh Company
#
# Author: Romain Forlot <romain.forlot@iot.bzh>
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

##################################################
# Low-CAN test configuration files
##################################################
PROJECT_TARGET_ADD(afb-test-config)
	if(WITH_FEATURE_J1939)
		set(J1939_TEST_CONFFILE ",\"low-can_J1939Test.lua\",\"low-can_ValueJ1939Test.lua\"")
	endif()

	configure_file("aft-rp-low-can.json.in" "aft-rp-low-can.json")
	file(GLOB CONF_FILES "${CMAKE_CURRENT_BINARY_DIR}/*.json")

	add_input_files("${CONF_FILES}")

	SET_TARGET_PROPERTIES(${TARGET_NAME} PROPERTIES
	LABELS "TEST-CONFIG"
	OUTPUT_NAME ${TARGET_NAME}
	)
