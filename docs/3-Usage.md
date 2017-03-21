# Install AFB Websocket CLI client to test the binding.

You can test it using afb-client-demo CLI tool provided by the RPM package _libafbwsc-dev_.

You can find this package in your build environment, using docker SDK recommended setup the file is `/xdt/build/tmp/deploy/rpm/<your-target-arch>/`.

After a successful bitbake build and using Renesas RCar Gen2, Porter, you have to copy the file if your board is connected to your network and you know its IP address:

```bash
$ scp /xdt/build/tmp/deploy/rpm/cortex15hf_neon/libafbwsc-dev-1.0-r0.cortexa15hf_neon.rpm root@<target_IP>:~
```

Else, you have to copy into the SDcard with the AGL image installed on it.

From the docker image copy RPM to the shared directory between docker image and your host:

```bash
$ cp /xdt/build/tmp/deploy/rpm/cortex15hf_neon/libafbwsc-dev-1.0-r0.cortexa15hf_neon.rpm ~/share
```

Then plugin you SDcard in your Linux host \(Windows can't read ext4 filesystem AGL runs on\) and copy RPM file on it.

From your host, identify SDcard block device node here it is **sdc** with the correct capacity automounted by the desktop manager:

```bash
$ lsblk
loop1                                                                                     7:1    0     2G  0 loop
└─docker-253:0-3146365-pool                                                             253:3    0   100G  0 dm
  └─docker-253:0-3146365-e9f80849a2681e18549d3a4238cbf031e44052e36cd88a0abf041804b799b61c
    253:4    0    10G  0 dm   /var/lib/docker/devicemapper/mnt/e9f80849a2681e18549d3a4238cbf031e44052e36cd88a0abf041804b799b61c
sdb                                                                                       8:16   0 238.5G  0 disk
├─sdb2                                                                                    8:18   0   238G  0 part
│ └─Shamash-agl                                                                         253:1    0   238G  0 lvm  /home/claneys/Workspace/agl-docker
└─sdb1                                                                                    8:17   0   500M  0 part /boot
sr0                                                                                      11:0    1  1024M  0 rom
loop0                                                                                     7:0    0   100G  0 loop
└─docker-253:0-3146365-pool                                                             253:3    0   100G  0 dm
  └─docker-253:0-3146365-e9f80849a2681e18549d3a4238cbf031e44052e36cd88a0abf041804b799b61c
    253:4    0    10G  0 dm   /var/lib/docker/devicemapper/mnt/e9f80849a2681e18549d3a4238cbf031e44052e36cd88a0abf041804b799b61c
sdc                                                                                       8:32   1  14.9G  0 disk
└─sdc1                                                                                    8:33   1     2G  0 part /run/media/claneys/97f418a5-612f-44e9-b968-a19505695151
sda                                                                                       8:0    0 931.5G  0 disk
├─sda2                                                                                    8:2    0   500G  0 part
│ ├─Shamash-home                                                                        253:2    0   150G  0 lvm  /home
│ └─Shamash-root                                                                        253:0    0    50G  0 lvm  /
└─sda1                                                                                    8:1    0    16G  0 part [SWAP]
```

Copy, still from your host:

> **CAUTION:** Make sure to sync IO with sync command before unplug your SDcard. It could be corrupted if removed before all pending IO aren't done.

```bash
$ sudo umount /dev/sdc1
$ export SDCARD=/mnt
$ sudo mount /dev/sdc1 $SDCARD
$ sudo cp ~/devel/docker/share/libafbwsc-dev-1.0-r0.cortexa15hf_neon.rpm $SDCARD/home/root
$ sync
$ sudo umount $SDCARD
```

Insert the modified SDcard in your Porter board and boot from it. You are ready to go.

## Configure the AGL system

### Virtual CAN device

Connected to the target, here is how to load the virtual CAN device driver and set up a new vcan device :

```bash
# modprobe vcan
# ip link add vcan0 type vcan
# ip link set vcan0 up
```

### CAN device using the USB CAN adapter

Using real connection to CAN bus of your car using the USB CAN adapter connected to the OBD2 connector.

Once connected, launch `dmesg` command and search which device to use :

```bash
# dmesg
[...]
[  131.871441] usb 1-3: new full-speed USB device number 4 using ohci-pci
[  161.860504] can: controller area network core (rev 20120528 abi 9)
[  161.860522] NET: Registered protocol family 29
[  177.561620] usb 1-3: USB disconnect, device number 4
[  191.061423] usb 1-2: USB disconnect, device number 3
[  196.095325] usb 1-2: new full-speed USB device number 5 using ohci-pci
[  327.568882] usb 1-2: USB disconnect, device number 5
[  428.594177] CAN device driver interface
[ 1872.551543] usb 1-2: new full-speed USB device number 6 using ohci-pci
[ 1872.809302] usb_8dev 1-2:1.0 can0: firmware: 1.7, hardware: 1.0
[ 1872.809356] usbcore: registered new interface driver usb_8dev
```

Here device is named **can0**.

This instruction assuming a speed of 500000kbps for your CAN bus, you can try others supported bitrate like 125000, 250000 if 500000 doesn't work:

```bash
# ip link set can0 type can bitrate 500000
# ip link set can0 up
# ip link show can0
  can0: <NOARP,UP,LOWER_UP,ECHO> mtu 16 qdisc pfifo_fast state UNKNOWN qlen 10
    link/can
    can state ERROR-ACTIVE (berr-counter tx 0 rx 0) restart-ms 0
    bitrate 500000 sample-point 0.875
    tq 125 prop-seg 6 phase-seg1 7 phase-seg2 2 sjw 1
    sja1000: tseg1 1..16 tseg2 1..8 sjw 1..4 brp 1..64 brp-inc 1
    clock 16000000
```

## Configure the binding

Configure the binding specifying in the JSON configuration file the CAN device\(s\) that it will to connect to. Edit file _/var/lib/afm/applications/low-can-binding/0.1/can\_buses.json_ and change the CAN device name to the one you have :

```json
{
    "canbus":  "can0"
}
```

If you have several specify CAN bus devices use an array:

```json
{
    "canbus": [ "vcan0", "can0" ]
}
```

> **WARNING:** Make sure the CAN bus\(es\) you specify in your configuration file match those specified in your generated source file with the [can-config-generator](http://github.com/iotbzh/can-config-generator).

## Run it, test it, use it !

You can run the binding using **afm-util** tool, here is the classic way to go :

```bash
# afm-util run low-can-binding@0.1
1
```

You can find instructions to use afm-util tool [here](http://docs.iot.bzh/docs/apis_services/en/dev/reference/af-main/afm-daemons.html#using-afm-util), as well as documentation about Application Framework.

But you can't control nor interact with it because you don't know security token that **Application Framework** gaves it at launch.

So, to test it, it is better to launch the binding manually. In the following example, we will use port **1234** and left empty security token for testing purpose:

```bash
# afb-daemon --ldpaths=/usr/lib/afb:/var/lib/afm/applications/low-can-binding/0.1/libs/ --rootdir=/var/lib/afm/applications/low-can-binding/0.1/ --port=1234 --token=
NOTICE: binding [/usr/lib/afb/afb-dbus-binding.so] calling registering function afbBindingV1Register
NOTICE: binding /usr/lib/afb/afb-dbus-binding.so loaded with API prefix dbus
NOTICE: binding [/usr/lib/afb/authLogin.so] calling registering function afbBindingV1Register
NOTICE: binding /usr/lib/afb/authLogin.so loaded with API prefix auth
NOTICE: binding [/var/lib/afm/applications/low-can-binding/0.1/libs//low-can-binding.so] calling registering function afbBindingV1Register
NOTICE: binding /var/lib/afm/applications/low-can-binding/0.1/libs//low-can-binding.so loaded with API prefix low-can
NOTICE: Waiting port=1234 rootdir=/var/lib/afm/applications/low-can-binding/0.1/
NOTICE: Browser URL= http:/*localhost:1234
NOTICE: vcan0 device opened and reading {binding low-can}
NOTICE: Initialized 1/1 can bus device(s) {binding low-can}
```

On another terminal, connect to the binding using previously installed _**AFB Websocket CLI**_ tool:

```bash
# afb-client-demo ws://localhost:1234/api?token=
```

You will be on an interactive session where you can communicate directly with the binding API.

The binding provides at this moment 2 verbs, _subscribe_ and _unsubscribe_, which can take argument by a JSON **event** object.

The argument value is the CAN message **generic\_name** as described in the JSON file used to generate cpp file for the binding.

To use the _**AFB Websocket CLI**_ tool, a command line will be like the following :

```
<api> <verb> <arguments>
```

Where:

* API : _**low-can**_.
* Verb : _**subscribe**_ or _**unsubscribe**_
* Arguments : _**{ "event": "driver.doors.open" }**_

### Subscription and unsubscription

You can ask to subscribe to chosen CAN event with a call to _subscribe_ API verb with the CAN messages name as JSON argument.

> **NOTE:** If no argument is provided, then you'll subscribe to all signals at once.

For example from a websocket session:

```json
low-can subscribe { "event": "doors.driver.open" }
ON-REPLY 1:low-can/subscribe: {"jtype":"afb-reply","request":{"status":"success","uuid":"a18fd375-b6fa-4c0e-a1d4-9d3955975ae8"}}
```

Subscription and unsubscription can take wildcard in their _event_ value.

To receive all doors events :

```json
low-can subscribe { "event" : "doors*" }
ON-REPLY 1:low-can/subscribe: {"jtype":"afb-reply","request":{"status":"success","uuid":"511c872e-d7f3-4f3b-89c2-aa9a3e9fbbdb"}}
```

Then you will receive an event each time a CAN message is decoded for the event named _doors.driver.open_

```json
ON-EVENT low-can/messages.doors.driver.open({"event":"low-can\/messages.doors.driver.open","data":{"name":"messages.doors.driver.open","value":true},"jtype":"afb-event"})
```

Notice that event shows you that the CAN event is named _messages.doors.driver.open_ but you ask for event about _doors.driver.open_.

This is because all CAN messages or diagnostic messages are prefixed by the JSON parent node name, **messages** for CAN messages and **diagnostic\_messages** for diagnostic messages like OBD2.

This will let you subscribe or unsubcribe to all signals at once, not recommended, and better make filter on subscribe operation based upon their type. Examples:

```json
low-can subscribe { "event" : "*speed*" } --> will subscribe to all messages with speed in their name. Search will be make without prefix for it.
low-can subscribe { "event" : "speed*" } --> will subscribe to all messages begin by speed in their name. Search will be make without prefix for it.
low-can subscribe { "event" : "messages*speed*" } --> will subscribe to all CAN messages with speed in their name. Search will be on prefixed messages here.
low-can subscribe { "event" : "messages*speed" } --> will subscribe to all CAN messages ending with speed in their name. Search will be on prefixed messages here.
low-can subscribe { "event" : "diagnostic*speed*" } --> will subscribe to all diagnostic messages with speed in their name. Search will be on prefixed messages here.
low-can subscribe { "event" : "diagnostic*speed" } --> will subscribe to all diagnostic messages ending with speed in their name. Search will be on prefixed messages here.
```

You can stop receiving event from it by unsubscribe the signal the same way you did for subscribe

```json
low-can unsubscribe { "event": "doors.driver.open" }
ON-REPLY 2:low-can/unsubscribe: {"jtype":"afb-reply","request":{"status":"success"}}
low-can unsubscribe { "event" : "doors*" }
ON-REPLY 3:low-can/unsubscribe: {"jtype":"afb-reply","request":{"status":"success"}}
```

### Using CAN utils to monitor CAN activity

You can watch CAN traffic and send custom CAN messages using can-utils preinstalled on AGL target.

To watch watch going on a CAN bus use:

```bash
# candump can0
```

Send a custom message:

```bash
# cansend can0 ID#DDDDAAAATTTTAAAA
```



