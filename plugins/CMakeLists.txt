###########################################################################
# Copyright 2015, 2016, 2017 IoT.bzh
#
# author: Romain Forlot <romain.forlot@iot.bzh>
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#	http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
###########################################################################

PROJECT_TARGET_ADD(default-signals)

	# Define targets
	ADD_LIBRARY(${TARGET_NAME} MODULE ${TARGET_NAME}.cpp)

	# Alsa Plugin properties
	SET_TARGET_PROPERTIES(${TARGET_NAME} PROPERTIES
		LABELS "PLUGIN"
		PREFIX ""
		SUFFIX ".ctlso"
		OUTPUT_NAME ${TARGET_NAME}
	)

	TARGET_INCLUDE_DIRECTORIES(${TARGET_NAME}
		PUBLIC "/opt/AGL/include"
	)

	# Library dependencies (include updates automatically)
	TARGET_LINK_LIBRARIES(${TARGET_NAME}
		low-can
		appcontroller
		lua
		afb-helpers
	)
