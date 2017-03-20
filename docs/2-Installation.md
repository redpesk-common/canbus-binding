# Prerequisites

- An AGL system installed with a Chinook version.

- Make sure you built the AGL generator else you will not be able to generate custom low-level CAN binding. Generator can be found [here](http://github.com/iotbzh/can-config-generator) with the attached instruction to install and run it. It will produce a *configuration-generated.cpp* file to paste in the source, *src/*, directory.

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

- An [USB CAN adapter](http://shop.8devices.com/usb2can) connected to OBD2 connector through the [right cable](http://www.obd2cables.com/).

# Getting started

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
