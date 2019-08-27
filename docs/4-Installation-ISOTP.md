# Installation isotp for AGL

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