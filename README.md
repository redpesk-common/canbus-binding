# CAN Configuration Generator

## Abstract

This tool is used to generate a configuration source file for the [Low Level CAN bus binding](https://github.com/iotbzh/CAN_signaling).
This binding needs a configuration provided as a C++ source file.
As long this binding is based on [OpenXC vi-firmware](https://github.com/openxc/vi-firmware), this generator use the JSON format from the [python generator](https://github.com/openxc/openxc-python).
The reference documentation of the JSON file can be found [here](https://github.com/openxc/vi-firmware/blob/master/docs/config/reference.rst).

## Getting started
### Build requirements

* CMake version 3.0 or later
* G++, Clang++ or any C++11 complient compiler.
* Boost
    * filesystem
    * program_options
    * system

You can install any of these using your package manager. For instance, inside the iotbzh's docker image, you must enter this command :

	devel@bsp-devkit:~/$ sudo apt-get install cmake libboost-system-dev libboost-filesystem-dev libboost-program-options-dev

You may want to install `libboost-all-dev` to get all boost components even if it's not required.

## Compile

<pre>
<b>devel@bsp-devkit:~/projects/$</b> git clone https://github.com/iotbzh/can-config-generator.git
Cloning into 'can-config-generator'...
remote: Counting objects: 74, done.
remote: Compressing objects: 100% (42/42), done.
remote: Total 74 (delta 30), reused 67 (delta 27), pack-reused 0
Unpacking objects: 100% (74/74), done.
Checking connectivity... done.
	
<b>devel@bsp-devkit:~/projects/$</b> cd can-config-generator
<b>devel@bsp-devkit:~/projects/can-config-generator/(master)$</b> mkdir build
<b>devel@bsp-devkit:~/projects/can-config-generator/build(master)$</b> cmake -G "Unix Makefiles" ..
-- The C compiler identification is GNU 4.9.2
-- The CXX compiler identification is GNU 4.9.2
-- Check for working C compiler: /usr/bin/cc
-- Check for working C compiler: /usr/bin/cc -- works
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Check for working CXX compiler: /usr/bin/c++
-- Check for working CXX compiler: /usr/bin/c++ -- works
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Boost version: 1.55.0
-- Found the following Boost libraries:
--   program_options
--   filesystem
--   system
-- Configuring done
-- Generating done
-- Build files have been written to: /home/devel/projects/can-config-generator/build
<b>devel@bsp-devkit:~/projects/can-config-generator/build(master)$</b> make
Scanning dependencies of target can-config-generator
[ 12%] Building CXX object CMakeFiles/can-config-generator.dir/src/main.cpp.o
[ 25%] Building CXX object CMakeFiles/can-config-generator.dir/src/openxc/message_set.cpp.o
[ 37%] Building CXX object CMakeFiles/can-config-generator.dir/src/openxc/can_bus.cpp.o
[ 50%] Building CXX object CMakeFiles/can-config-generator.dir/src/openxc/can_message.cpp.o
[ 62%] Building CXX object CMakeFiles/can-config-generator.dir/src/openxc/command.cpp.o
[ 75%] Building CXX object CMakeFiles/can-config-generator.dir/src/openxc/diagnostic_message.cpp.o
[ 87%] Building CXX object CMakeFiles/can-config-generator.dir/src/openxc/mapping.cpp.o
[100%] Building CXX object CMakeFiles/can-config-generator.dir/src/openxc/signal.cpp.o
Linking CXX executable can-config-generator
[100%] Built target can-config-generator
</pre>

## Generate your config file

To generate your config file you just have to run the generator using the `-m` option to specify your JSON file.
<pre>
<b>devel@bsp-devkit:~/projects/can-config-generator/build(master)$</b> can-config-generator -m ../tests/basic.json -o configuration-generated.cpp
</pre>

If you omit the `-o` option, then code is generated on the stdout.
You also can specify a header and a footer file.
These files must be valid C++ fragment as long as they will be inserted as is.
Use the `-h` option to display help.

## Supported OpenXC items

As long as the low level CAN signaling binding doesn't fully support OpenXC yet, this generator is also limited.
For instance, `initializers`, `loopers`, `buses`, `commands` and `handlers` nodes are ignored for now.
This generator will follow OpenXC support status of the low level CAN signaling binding.

## Known issues

* Each `diagnostic_message` must define the same `bus` as the binding will use only one bus.

## Dependencies

* [json](https://github.com/nlohmann/json) by nlohmann

## License

Copyright 2017 - iotbzh

Licensed under the [Apache License 2.0](https://github.com/iotbzh/can-config-generator/blob/master/LICENSE)
