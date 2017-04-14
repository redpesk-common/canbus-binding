# Copyright (C) 2015, 2016 "IoT.bzh"
# Author "Romain Forlot" <romain.forlot@iot.bzh>
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#	 http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

BUILD_DIR   := build

.PHONY: all clean mrproper ${BUILD_DIR}/Makefile

all: build package

clean:
	@([ -d ${BUILD_DIR} ] && make -C ${BUILD_DIR} clean) || echo Nothing to clean

mrproper:
	rm -rf ${BUILD_DIR}

build:  ${BUILD_DIR}/Makefile
	cmake --build ${BUILD_DIR} --clean-first


package:
	cmake --build ${BUILD_DIR} --target widget

${BUILD_DIR}/Makefile:
	@[ -d ${BUILD_DIR} ] || mkdir -p ${BUILD_DIR}
	@[ -f ${BUILD_DIR}/Makefile ] || (cd ${BUILD_DIR} && cmake ${CMAKE_OPTS} ..)
