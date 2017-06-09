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

* Make sure you already set up the AGL SDK using the following [SDK Quick Setup Guide](http://docs.iot.bzh/docs/getting_started/en/dev/reference/setup-sdk-environment.html). Alternatively, please refer to official guides available on [AGL Developer Site](http://docs.automotivelinux.org/docs/devguides/en/dev/#guides).

> **CAUTION** Don't source the environment file to get this project compile. We just use CMake from SDK environment, so make sure to not have CMake on the system or remove it if needed with ```sudo apt-get remove cmake```. 

## Compile

```bash
$ export PATH=$PATH:/xdt/sdk/sysroots/x86_64-aglsdk-linux/usr/bin
$ export WD=$(pwd)
$ git clone --recursive https://gerrit.automotivelinux.org/gerrit/apps/low-level-can-service
$ cd can-config-generator
$ mkdir -p build
$ cd build
$ cmake -G "Unix Makefiles" ..
$ make
```

### Naming convention

We chose a doted naming convention because it's a well know schema.

It separates and organize names into hierarchy. From the left to right, you describe your names using the more common ancestor at the left then more you go to the right the more it will be accurate.

Let's take an example, here is an example about standard PID name following this convention:

```
engine.load
engine.coolant.temperature
fuel.pressure
intake.manifold.pressure
engine.speed
vehicle.speed
intake.air.temperature
mass.airflow
throttle.position
running.time
EGR.error
fuel.level
barometric.pressure
commanded.throttle.position
ethanol.fuel.percentage
accelerator.pedal.position
hybrid.battery-pack.remaining.life
engine.oil.temperature
engine.torque
```

> **NOTE** It's recommended that you follow this naming convention to name your CAN signals.
>
> There is only character `*` that is forbidden in names because it's used as wildcard for subscription and unsubscrition.
>
> This described in the below chapter.

### Available decoder

You can use some basic decoder provided by default by the binding which are:

* decoder_t::noopDecoder : Default decoder if not specified, return raw value from signal's bitfield.
* decoder_t::booleanDecoder : Coerces a numerical value to a boolean.
* decoder_t::stateDecoder : Find and return the corresponding string state for a CAN signal's raw integer value.

## Generate your config file

To generate your config file you just have to run the generator using the `-m` option to specify your JSON file.

```bash
./can-config-generator -m ../tests/basic.json -o configuration-generated.cpp
```

If you omit the `-o` option, then code is generated on the stdout.
You also can specify a header and a footer file.
These files must be valid C++ fragment as long as they will be inserted as is.
Use the `-h` option to display help.

## Supported OpenXC items

As long as the low level CAN signaling binding doesn't fully support OpenXC yet, this generator is also limited.
For instance, `initializers`, `loopers`, `commands` and `handlers` nodes are ignored for now.
This generator will follow OpenXC support status of the low level CAN signaling binding.

The `buses` item will not be supported by this generator because the binding use another way to declare and configure buses. Please refer to the binding's documentation.

## Known issues

* Each `diagnostic_message` must define the same `bus` as the binding will use only one bus.
* Only 1 messages_set is used

## Dependencies

* [json](https://github.com/nlohmann/json) by nlohmann

## License

Copyright 2017 - iotbzh

Licensed under the [Apache License 2.0](https://github.com/iotbzh/can-config-generator/blob/master/LICENSE)
