#!/usr/bin/make -f
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
PACKAGING_DIR := conf.d/app-templates/wgt

VPATH = etc:$(PACKAGING_DIR):$(PACKAGING_DIR)/etc:$(BUILD_DIR)

.PHONY: all clean mrproper package

all: build

clean:
	@([ -d ${BUILD_DIR} ] && make -C ${BUILD_DIR} clean) || echo Nothing to clean

mrproper:
	@rm -rf ${BUILD_DIR} package packaging

build: conf.d/app-templates/cmake config.xml.in icon-default.png ${BUILD_DIR}/Makefile
	@cmake --build ${BUILD_DIR} --target all

package: build | $(PKG_FILELIST)
	@mkdir -p ${BUILD_DIR}/$@/bin
	@mkdir -p ${BUILD_DIR}/$@/etc
	@mkdir -p ${BUILD_DIR}/$@/lib
	@mkdir -p ${BUILD_DIR}/$@/htdocs
	@mkdir -p ${BUILD_DIR}/$@/data
	@mkdir -p package
	@cmake --build ${BUILD_DIR} --target widget
	@cp ${BUILD_DIR}/*wgt package

${BUILD_DIR}/Makefile:
	@[ -d ${BUILD_DIR} ] || mkdir -p ${BUILD_DIR}
	@[ -f ${BUILD_DIR}/Makefile ] || (cd ${BUILD_DIR} && cmake ${CMAKE_OPTS} ..)

submodule:
	@[ -d conf.d/app-templates/cmake ] || git submodule init
	@[ -d conf.d/app-templates/cmake ] || git submodule update

conf.d/app-templates/cmake: submodule
config.xml.in: submodule
icon-default.png: submodule