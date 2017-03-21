# Prerequisites

* An AGL system installed with latest Chinook version \(&gt;3.0.2\).

* Make sure you built the AGL generator else you will not be able to generate custom low-level CAN binding. Generator can be found [here](http://github.com/iotbzh/can-config-generator) with the attached instruction to install and run it.

It will produce a _configuration-generated.cpp_ file to paste in the source, _src/_, directory.

* Make sure you already set up the AGL SDK using the following [guide](http://docs.iot.bzh/docs/getting_started/en/dev/reference/setup-sdk-environment.html).

To get the correct SDK version installed, you **must** prepare your environment with the **chinook-next** version. To do so, run the following command in your docker image in the step 4 in place of `... [ prepare build environment ] ...`:

> **NOTE** These commands assume that proprietary graphic drivers for Renesas Porter board are located in `/home/devel/share/proprietary-renesas-rcar` directory.

```bash
$ prepare_meta -f chinook-next -o /xdt -l /home/devel/mirror -p /home/devel/share/proprietary-renesas-rcar/ -t porter -e wipeconfig -e rm_work
$ /xdt/build/agl-init-build-env
```

* An [USB CAN adapter](http://shop.8devices.com/usb2can) connected to connector through the [right cable](http://www.mouser.fr/ProductDetail/EasySync/OBD-M-DB9-F-ES/)).

# Getting started

## Use of CAN config generator

### Build requirements

* CMake version 3.0 or later
* G++, Clang++ or any C++11 complient compiler.
* Boost
  * filesystem
  * program\_options
  * system

You can install any of these using your package manager. For instance, inside the iotbzh's docker image, you must enter this command :

```bash
$ sudo apt-get update
$ sudo apt-get install libboost-system-dev libboost-filesystem-dev libboost-program-options-dev
```

You may want to install `libboost-all-dev` to get all boost components even if it's not required.

### Compile

> **CAUTION** It is **very important** that you do not source the SDK environment file to compile this project because some build requirements aren't installed in the AGL SDK for now.

```bash
$ export PATH=$PATH:/xdt/sdk/sysroots/x86_64-aglsdk-linux/usr/bin
$ export WD=$(pwd)
$ git clone https://github.com/iotbzh/can-config-generator.git
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

> **NOTE** It's recommended that you follow this naming convention to named your CAN signals.
>
> There is only character `*` that is forbidden in names because it's used as wildcard for subscription and unsubscrition.
>
> This described in the below chapter.

### Generating JSON from Vector CANoe Database

> **CAUTION** This chapter has not been tested since we haven't necessary automotive tools for that. 

If you use CANoe to store your `gold standard` CAN signal definitions, you may be able to use the OpenXC `xml_to_json.py` script to make your JSON for you. First, export the Canoe .dbc file as XML - you can do this with Vector CANdb++. Next, create a JSON file according to the format defined above, but only define:

- CAN messages.
- Name of CAN signals within messages and their generic_name.
- Optionnaly name of diagnostic messages and their name.

To install the OpenXC utilities and runs `xml_to_json.py` script:

```bash
$ sudo pip install openxc
$ cd /usr/local/lib/python2.7/dist-packages/openxc/generator
```

Assuming the data exported from Vector is in `signals.xml` and your minimal mapping file is `mapping.json`, run the script:

```bash
$ python -m openxc.utils ./xml_to_json.py signals.xml mapping.json signals.json
```

The script scans `mapping.json` to identify the CAN messages and signals that you want to use from the XML file. It pulls the neccessary details of the messages (bit position, bit size, offset, etc) and outputs the resulting subset as JSON into the output file, `signals.json`.

The resulting file together with `mapping.json` will work as input to the code generation script.

### Generate your config file

To generate your config file you just have to run the generator using the `-m` option to specify your JSON file.

```bash
$ ./can-config-generator -m ../tests/basic.json -o configuration-generated.cpp
```

If you omit the `-o` option, then code is generated on the stdout.  
You also can specify a header and a footer file.  
These files must be valid C++ fragment as long as they will be inserted as is.  
Use the `-h` option to display help.

> **CAUTION:** Each `diagnostic_message` must define the same `bus` as the binding will use only one bus.

### Supported OpenXC items

About now, compliance with OpenXC reference is in progress, can-config-generator and CAN\_signaling will implement them soon.  
`initializers`, `loopers`, `commands` and `handlers` nodes are ignored for now.

This generator will follow OpenXC support status of the low level CAN signaling binding.

> **NOTE**: The `buses` item will not be supported by this generator because the binding use another way to declare and configure buses. Please refer to the binding's documentation.

## Compile and install the binding
Clone the binding repository, copy the generated file and updated the git submodules.

Execute the following commands from this repository:

```bash
$ cd $WD
$ git clone https://github.com/iotbzh/CAN_signaling
$ cd CAN_signaling
$ git submodule init
$ git submodule update
$ cp $WD/can-config-generator/build/configuration-generated.cpp src/
```

### Installation using *make install*

With an AGL SDK environment correctly configured and **sourced**, I suggest you to set the TARGET variable in the CMakeLists.txt file located under _src_ directory if you have an AGL target already running in your network.

Then you can directly build and install the binding and source directory on your target system.

Execute these commands to get your binding compile:

```bash
$ mkdir -p build
$ cd build
$ cmake ..
$ make
```

And if you have set TARGET variable, you can install it on your AGL system:

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

If it is the first time that you make the installation then you'll have this message in place of _**true**_.

### Installation manually

To install it manually, you need to copy the _low-can-binding.wgt_ file on your target, then from it execute the following commands :

On your host, to copy over the network :

```bash
$ scp low-can-binding.wgt root@<target_IP>:~
```

On the target, assuming _**wgt**_ file is in the root home directory:

```bash
# afm-util install low-can-binding.wgt
{ "added": "low-can-binding@0.1" }
```
