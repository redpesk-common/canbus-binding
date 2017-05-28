#!/bin/bash

cd CAN-config-generator && make build && cd -
cd CAN-binder && ./build.mk mrproper && ./build.mk build && cd -
