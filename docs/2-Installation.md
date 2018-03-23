# Prerequisites

* An AGL system installed with latest Daring Dab version with latest Application
framework version >= 0.6.

* Make sure you built the AGL generator else you will not be able to generate custom low-level CAN binding.

It will produce a _application-generated.cpp_ file to paste in the source, _CAN-binder/low-can-binding/binding/_, directory.

* Make sure you already set up the AGL SDK using the following [SDK Quick Setup Guide](http://docs.iot.bzh/docs/getting_started/en/dev/reference/setup-sdk-environment.html). Alternatively, please refer to official guides available on [AGL Developer Site](http://docs.automotivelinux.org/docs/devguides/en/dev/#guides).

If you need to have the graphic stack inside your SDK, you have to prepare your environment with the **iotbzh**, or **Daring Dab** flavor using _prepare_meta_ tool. To do so, run the following command in your docker image in the step 4 in place of `... [ prepare build environment ] ...`:

> **NOTE** These commands assume that proprietary graphic drivers for Renesas Porter board are located in _/home/devel/share/proprietary-renesas-rcar_ directory.

```bash
prepare_meta -f iotbzh -o /xdt -l /home/devel/mirror -p /home/devel/share/proprietary-renesas-rcar/ -t m3ulcb -e wipeconfig -e rm_work -e cleartemp
/xdt/build/m3ulcb/agl-init-build-env
```

* (Optionnal) An [USB CAN adapter](http://shop.8devices.com/usb2can) connected to connector through the [right cable](http://www.mouser.fr/ProductDetail/EasySync/OBD-M-DB9-F-ES/)) if you want to connect to a real car through the OBD2 connector.

<!-- pagebreak -->

# Getting started

## CAN config generator usage

### Build requirements

* CMake version 3.3 or later
* G++, Clang++ or any C++11 compliant compiler.

### Compile

```bash
source /xdt/sdk/environment-setup-aarch64-agl-linux
export PATH=$PATH:/xdt/sdk/sysroots/x86_64-aglsdk-linux/usr/bin
export WD=$(pwd)
git clone --recursive https://gerrit.automotivelinux.org/gerrit/apps/agl-service-can-low-level -b Renesas_delivery_Q2
git clone --recursive https://gerrit.automotivelinux.org/gerrit/apps/low-level-can-generator
cd ${WD}/low-level-can-generator
mkdir -p build
cd build
cmake -G "Unix Makefiles" ..
make
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
> There is only character `*` that is forbidden in names because it's used as wildcard for subscription and unsubscription.
>
> This described in the below chapter.

### Available decoder

You can use some basic decoder provided by default by the binding which are:

* ***decoder_t::decode_noop*** : Default decoder if not specified, return raw value from signal's bitfield.
* ***decoder_t::decode_boolean*** : Coerces a numerical value to a boolean.
* ***decoder_t::decode_state*** : Find and return the corresponding string state for a CAN signal's raw integer value.

### Generating JSON from Vector CANoe Database

> **CAUTION** This chapter has not been tested since it haven't necessary automotive tools for that.

If you use CANoe to store your `gold standard` CAN signal definitions, you may be able to use the OpenXC `xml_to_json.py` script to make your JSON for you. First, export the Canoe .dbc file as XML - you can do this with Vector CANdb++. Next, create a JSON file according to the format defined above, but only define:

* CAN messages.
* Name of CAN signals within messages and their generic_name.
* Optionnaly name of diagnostic messages and their name.

To install the OpenXC utilities and runs `xml_to_json.py` script:

```bash
sudo pip install openxc
cd /usr/local/lib/python2.7/dist-packages/openxc/generator
```

Assuming the data exported from Vector is in `signals.xml` and your minimal mapping file is `mapping.json`, run the script:

```bash
python -m openxc.utils ./xml_to_json.py signals.xml mapping.json signals.json
```

The script scans `mapping.json` to identify the CAN messages and signals that you want to use from the XML file. It pulls the neccessary details of the messages (bit position, bit size, offset, etc) and outputs the resulting subset as JSON into the output file, `signals.json`.

The resulting file together with `mapping.json` will work as input to the code generation script.

### Generate your config file

To generate your config file you just have to run the generator using the `-m` option to specify your JSON file.

```bash
./can-config-generator -m ../tests/basic.json -o application-generated.cpp
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

### Build requirements

* Kernel >= 4.8
* CMake version 3.3 or later
* G++, Clang++ or any C++11 compliant compiler.

### Compile

Clone the binding repository, copy the generated file and updated the git submodules.

Execute the following commands from this repository:

```bash
cd ${WD}/agl-service-can-low-level
cp ${WD}/low-level-can-generator/build/application-generated.cpp ../low-can-binding/binding
```

### Installation

```bash
cd ${WD}/agl-service-can-low-level
mkdir build
cd build
cmake ..
make
make widget
```

To install it manually, you need to copy the _low-can-service.wgt_ file on your target, then from it execute the following commands :

On your host, to copy over the network :

```bash
scp low-can-service.wgt root@<target_IP>:~
```

On the target, assuming _**wgt**_ file is in the root home directory:

```bash
afm-util install low-can-service.wgt
{ "added": "low-can-service@4.0" }
```
