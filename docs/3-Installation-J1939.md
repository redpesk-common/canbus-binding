# Installation j1939 for AGL

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

## Enable support at build time

To enable J1939 support, the binding must be built with -DWITH_FEATURE_J1939=ON.
If using the autobuild script, add CONFIGURE_ARGS="-DWITH_FEATURE_J1939=ON" to the command used.
