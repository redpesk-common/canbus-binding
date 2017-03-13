# Low level CAN signaling binder

Low level CAN bus binder. Based upon OpenXC vi-firmware project. Purpose of this project is to offer a low level binding to an AGL platform, idea remains the same than the vi-firmware project. It's meant to generate from a JSON file describing CAN messages and diagnostic message (OBD2 for now) present in a car, a cpp file to integrate with the project and compile all together. Result will be a widget file to install on an AGL target system.

# Prerequirements

- Make sure you already have set up the AGL SDK before using the following [guide][SDK_instructions].

- This repo make use of git submodule, make sure to execute the following commands from the repository once cloned :

```bash
$ git submodule init
$ git submodule update
```

- Make sure you have installed the AGL generator else you aren't able to generate custom low-level CAN binding. Generator can be found [here][generator] with the attached instruction to install and run it. It will produce a *configuration-generated.cpp* file to paste in the source, *src/*, directory.

# Getting started

## Compile and install the binding

AGL SDK environment correctly set, if you have an AGL target already running in your network, I encourage you to set the TARGET variable in the root CMakeLists.txt file. Then you can directly install the binding and source directory on your target system.

Execute commands to get your binding compile :

```bash
$ mkdir build
$ cd build
$ cmake ..
$ make
```

And if you have set TARGET variable, you can install it on your AGL system :

```bash
$ make install
```

If not, you will have to install it manually copying the *low-can-binding.wgt* file on your target, then from it execute the following commands :

```bash
~# afm-util install low-can-binding.wgt
{ "added": "low-can-binding@0.1" } 
```

## Confiure system and binding

Configure the binding specifying in the JSON configuration file the CAN device that the binding will to connect to. Edit file */var/lib/afm/applications/low-can-binding/0.1/can_buses.json* and change the CAN device name to the one you have :

```json
{
	"canbus":  "vcan0"
}
```

If you have several CAN bus devices then use an array:

```json
{
	"canbus": [ "vcan0", "can0" ]
}
```

Connected to the target load the virtual CAN device driver and set up a new vcan device :

```bash
 ~# modprobe vcan
 ~# ip link add vcan0 type vcan
 ~# ip link set vcan0 up
 ```

Or a real CAN bus device if on is present on your board (this instruction assuming a speed of 500000kbps for your device):

```bash
~# modprobe can
~# ip link set can0 up type can bitrate 500000
```
## Run it, test it, use it !

You can run the binding using **afm-util** tool, it is the classic way to go :

```bash
~# afm-util run low-can-binding@0.1
1
```
You can find instructions to use afm-util tool [here][afm-util], as well as documentation about Application Framework.

[SDK_instructions]: http://docs.iot.bzh/docs/getting_started/en/dev/reference/setup-sdk-environment.html "Setup SDK environment"
[generator]: http://github.com/user/generator "AGL low level CAN binding Generator"
[afm-util]: http://docs.iot.bzh/docs/apis_services/en/dev/reference/af-main/afm-daemons.html#using-afm-util "afm-util usage"