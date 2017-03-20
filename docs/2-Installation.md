# Prerequisites

- An AGL system installed with latest Chinook version (>3.0.2).

- Make sure you built the AGL generator else you will not be able to generate custom low-level CAN binding. Generator can be found [here](http://github.com/iotbzh/can-config-generator) with the attached instruction to install and run it.

It will produce a *configuration-generated.cpp* file to paste in the source, *src/*, directory.

- Make sure you already set up the AGL SDK using the following [guide](http://docs.iot.bzh/docs/getting_started/en/dev/reference/setup-sdk-environment.html).

To get the correct SDK version installed, you **must** prepare your environment with the **chinook-next** version. To do so, run the following command in your docker image:

```bash
$ prepare_meta -f chinook-next -o /xdt -l /home/devel/mirror -p /home/devel/share/proprietary-renesas-rcar/ -t porter -e wipeconfig -e rm_work
```

- Check that you updated git submodules, executing the following commands from this repository:

```bash
$ git submodule init
$ git submodule update
```

- An [USB CAN adapter](http://shop.8devices.com/usb2can) connected to OBD2 connector through the [right cable](http://www.mouser.fr/ProductDetail/EasySync/OBD-M-DB9-F-ES/)).

# Getting started

## Use of CAN config generator

### Build requirements

* CMake version 3.0 or later
* G++, Clang++ or any C++11 complient compiler.
* Boost
    * filesystem
    * program_options
    * system

You can install any of these using your package manager. For instance, inside the iotbzh's docker image, you must enter this command :

```bash
$ sudo apt-get install cmake libboost-system-dev libboost-filesystem-dev libboost-program-options-dev
```

You may want to install `libboost-all-dev` to get all boost components even if it's not required.

### Compile

```bash
$ git clone https://github.com/iotbzh/can-config-generator.git
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
```

### Generate your config file

To generate your config file you just have to run the generator using the `-m` option to specify your JSON file.

```bash
$ can-config-generator -m ../tests/basic.json -o configuration-generated.cpp
```

If you omit the `-o` option, then code is generated on the stdout.
You also can specify a header and a footer file.
These files must be valid C++ fragment as long as they will be inserted as is.
Use the `-h` option to display help.

> **CAUTION** Each `diagnostic_message` must define the same `bus` as the binding will use only one bus.

### Supported OpenXC items

About now, compliance with OpenXC reference is in progress, can-config-generator and CAN_signaling will implement them soon.
`initializers`, `loopers`, `commands` and `handlers` nodes are ignored for now.

This generator will follow OpenXC support status of the low level CAN signaling binding.

> **NOTE** The `buses` item will not be supported by this generator because the binding use another way to declare and configure buses. Please refer to the binding's documentation.

## Compile and install the binding

With an AGL SDK environment correctly configured, I suggest you to set the TARGET variable in the root CMakeLists.txt file, if you have an AGL target already running in your network.

Then you can directly build and install the binding and source directory on your target system.

Execute these commands to get your binding compile :

```bash
$ mkdir build
$ cd build
$ cmake ..
$ make
```

And if you have set TARGET variable, you can install it on your AGL system :

```bash
$ make install
[ 16%] Built target bitfield
[ 27%] Built target isotp
[ 40%] Built target openxc
[ 48%] Built target uds
[ 97%] Built target low-can-binding
[100%] Built target widget
Install the project...
-- Install configuration: ""
true
{ "added": "low-can-binding@0.1" }
```

It's possible that you'll see the following message :

```bash
Error org.freedesktop.DBus.Error.Failed: "system error"
```

It's because installation remove the binding before installing it.

If it is the first time that you make the installation then you'll have this message in place of ***true***.

To install it manually, you need to copy the *low-can-binding.wgt* file on your target, then from it execute the following commands :

On your host, to copy over the network :

```bash
$ scp low-can-binding.wgt root@<target_IP>:~
```

On the target, assuming ***wgt*** file is in the root home directory :

```bash
~# afm-util install low-can-binding.wgt
{ "added": "low-can-binding@0.1" }
```
