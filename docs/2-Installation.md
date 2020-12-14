# Installation Prerequisites

This part is only useful if you plan to install the package from source.

If you aren't planing to build it from source, add the redpesk@ repository
to your package manager.
To set up your build host please follow [this section](../../developer-guides/host-configuration/docs/1-Setup-your-build-host.html).

## From source

We advise you to use the [local builder](../../getting_started/local_builder/docs/0_introduction.html) for from source installation. The
local builder comes with everything setup to build redpesk@ projects.
Some of the dependencies below will still be needed, like **lua** for example.

Here is the [local builder documentation](../../getting_started/local_builder/docs/3_build.html).

Else install the building tools
* git
* cmake
* make
* G++, Clang++ or any C++11 compliant compiler

Then the following dependencies:

* json-c
* libsystemd >= 222
* afb-binding
* afb-libhelpers
* afb-libcontroller
* lua >= 5.3

Fedora/OpenSuse:
```
dnf install git cmake make gcc-c++ afb-cmake-modules afb-binding-devel json-c-devel lua-devel afb-libhelpers-devel afb-libcontroller-devel
```

Ubuntu:
```
apt install git cmake make g++ afb-cmake-modules libsystemd-dev afb-binding-dev libjson-c-dev liblua5.3-dev afb-libhelpers-dev afb-libcontroller-dev
```

# Getting started

## Easy installation

If you are on redpesk@ or if you have installed the required repositories
simply install "canbus-binding" with you package manager.

Example for redpesk@ distro, Fedora or Opensuse:
```bash
dnf install canbus-binding
```

For Ubuntu:
```bash
sudo apt install canbus-binding-bin
```


## Compile and install the binding

```bash
git clone https://github.com/redpesk-common/canbus-binding.git
cd canbus-binding
mkdir build
cd build
cmake ..
make -j
sudo make install
```

# J1939 installation

#### Minimum kernel version : 4.19

## Compilation of kernel j1939

##### Clone linux-can-next repository on kernel.org

```bash
git clone https://git.kernel.org/pub/scm/linux/kernel/git/mkl/linux-can-next.git/
```

##### Checkout on j1939 branch

```bash
git checkout j1939
```

##### Add the compilation of the j1939

```bash
make menuconfig
	- Networking Support
		- Can bus subsystem support
			- <M> SAE J1939
			- [*] 	debug SAE J1939
```

##### Compile

```bash
make
```

##### Install

```bash
make modules_install
make install
```

##### Update grub

###### CentOS/RHEL/Oracle/Scientific and Fedora Linux

```bash
grub2-mkconfig -o /boot/grub2/grub.cfg
grubby --set-default /boot/vmlinuz-...
reboot
```

###### Debian/Ubuntu Linux

```bash
update-grub
reboot
```

##### Check if the installation is correct

```bash
modprobe can-j1939
```

If no errors are generated you have successfully install a kernel with j1939 module.

You can have a problem with header file, to check that go in the file /usr/include/linux/can.h

```bash
vi /usr/include/linux/can.h
```

If in the struct sockaddr_can you don't see j1939, the header are not upgrade.

So you need to do this manually, go to you're linux-can-next repository and do the following command:

```bash
cp include/uapi/linux/can.h /usr/include/linux/can.h
cp include/uapi/linux/can/j1939.h /usr/include/linux/can/
```

# ISOTP installation

## Compilation and installation of module kernel isotp

##### Clone repository Linux Kernel Module for ISO 15765-2:2016 CAN transport protocol

```bash
git clone https://github.com/hartkopp/can-isotp.git
```

##### Move into the new repository

```bash
cd can-isotp
```

##### Install packages to build

```bash
sudo apt-get install build-essential linux-headers-$(uname -r)
```

##### Compile

```bash
make
```

##### Install

```bash
sudo make modules_install
```

##### Load module


```bash
modprobe can
modprobe vcan
sudo insmod ./net/can/can-isotp.ko
```


## Include headers  files


```bash
sudo cp include/uapi/linux/can/isotp.h /usr/include/linux/can/
```
