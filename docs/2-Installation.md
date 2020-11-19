# Installation Prerequisites

This part is only useful if you plan to install the package from source.

If you aren't planing to build it from source, add the redpesk@ repository
to your package manager.
Here is the url for redpesk and fedora:
`download.redpesk.bzh`

## From source

We advise you to use the [local builder](http://redpesk-doc.lorient.iot/docs/en/master/getting_started/local-builder/local-builder.html) for from source installation. The
local builder comes with everything setup to build redpesk@ projects.
Some of the dependencies below will still be needed, like **lua** for example.

Here is the [local builder documentation](http://redpesk-doc.lorient.iot/docs/en/master/getting_started/local-builder/local-builder.html).

Else install the building tools

* git
* cmake
* make
* G++, Clang++ or any C++11 compliant compiler

Then the following dependencies:

* json-c
* libsystemd >= 222
* afb-daemon
* afb-helpers
* appcontroller
* lua >= 5.3

# Getting started

## Easy installation

If you are on redpesk@ or if you have installed the required repositories
simply install "rp-can-low-level" with you package manager.

Example for redpesk@ distro:

```bash
dnf install rp-can-low-level
```

## Compile and install the binding

```bash
git clone https://github.com/redpesk-common/rp-can-low-level.git
cd rp-can-low-level
mkdir build
cd build
cmake ..
make -j
sudo make install
```

# J1939 installation

## Minimum kernel version : 4.19

### Compilation of kernel j1939

Clone linux-can-next repository on kernel.org

```bash
git clone https://git.kernel.org/pub/scm/linux/kernel/git/mkl/linux-can-next.git/
```

Checkout on j1939 branch

```bash
git checkout j1939
```

Add the compilation of the j1939

```bash
make menuconfig
	- Networking Support
		- Can bus subsystem support
			- <M> SAE J1939
			- [*] 	debug SAE J1939
```

Compile

```bash
make
```

Install

```bash
make modules_install
make install
```

### Update grub

#### CentOS/RHEL/Oracle/Scientific and Fedora Linux

```bash
grub2-mkconfig -o /boot/grub2/grub.cfg
grubby --set-default /boot/vmlinuz-...
reboot
```

#### Debian/Ubuntu Linux

```bash
update-grub
reboot
```

### Check if the installation is correct

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

Clone repository Linux Kernel Module for ISO 15765-2:2016 CAN transport protocol

```bash
git clone https://github.com/hartkopp/can-isotp.git
```

Move into the new repository

```bash
cd can-isotp
```

Install packages to build

```bash
sudo apt-get install build-essential linux-headers-$(uname -r)
```

Compile

```bash
make
```

Install

```bash
sudo make modules_install
```

Load module

```bash
modprobe can
modprobe vcan
sudo insmod ./net/can/can-isotp.ko
```

Include headers  files

```bash
sudo cp include/uapi/linux/can/isotp.h /usr/include/linux/can/
```
