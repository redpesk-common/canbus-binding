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
# Low-CAN Lua Scripts
##################################################
PROJECT_TARGET_ADD(test-files)

	file(GLOB LUA_FILES "*.lua" "*.sh" "*.canreplay")
	add_input_files("${LUA_FILES}")

	SET_TARGET_PROPERTIES(${TARGET_NAME} PROPERTIES
	LABELS "TEST-DATA"
	OUTPUT_NAME ${TARGET_NAME}
	)
