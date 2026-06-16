# Communication protocol

## J1939 installation

### Minimum kernel version : 4.19

### Compilation of kernel j1939

#### Clone linux-can-next repository on kernel.org

```bash
git clone https://git.kernel.org/pub/scm/linux/kernel/git/mkl/linux-can-next.git/
```

#### Checkout on j1939 branch

```bash
git checkout j1939
```

#### Add the compilation of the j1939

```bash
make menuconfig
    - Networking Support
        - Can bus subsystem support
            - <M> SAE J1939
            - [*]   debug SAE J1939
```

#### Compile

```bash
make
```

#### Install

```bash
make modules_install
make install
```

#### Update grub

##### CentOS/RHEL/Oracle/Scientific and Fedora Linux

```bash
grub2-mkconfig -o /boot/grub2/grub.cfg
grubby --set-default /boot/vmlinuz-...
reboot
```

##### Debian/Ubuntu Linux

```bash
update-grub
reboot
```

##### Check if the installation is correct

```bash
modprobe can-j1939
```

If no errors are generated you have successfully installed a kernel with j1939 module.

You can have a problem with header file, to check that go in the file /usr/include/linux/can.h

```bash
vi /usr/include/linux/can.h
```

If in the struct sockaddr_can you don't see j1939, the headers are not upgraded.

So you need to do this manually, go to your linux-can-next repository and do the following command:

```bash
cp include/uapi/linux/can.h /usr/include/linux/can.h
cp include/uapi/linux/can/j1939.h /usr/include/linux/can/
```

## ISOTP installation

### Compilation and installation of module kernel isotp

#### Clone repository Linux Kernel Module for ISO 15765-2:2016 CAN transport protocol

```bash
git clone https://github.com/hartkopp/can-isotp.git
```

#### Move into the new repository

```bash
cd can-isotp
```

#### Install packages to build

```bash
sudo apt-get install build-essential linux-headers-$(uname -r)
```

#### Compile

```bash
make
```

#### Install

```bash
sudo make modules_install
```

#### Load module

```bash
modprobe can
modprobe vcan
sudo insmod ./net/can/can-isotp.ko
```

### Include headers  files

```bash
sudo cp include/uapi/linux/can/isotp.h /usr/include/linux/can/
```
