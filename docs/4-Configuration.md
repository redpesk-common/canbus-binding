# Configure the system

## Virtual CAN device

Connected to the target, here is how to load the virtual CAN device driver and
set up a new vcan device :

```bash
modprobe vcan
ip link add vcan0 type vcan
ip link set vcan0 up
```

You can also call your linux CAN device as you like, for example if you need to name it `can0` :

```bash
modprobe vcan
ip link add can0 type vcan
ip link set can0 up
```

## CAN device using the USB CAN adapter

Use the real connection to CAN bus of your device using an USB CAN adapter.

Once connected, launch `dmesg` command and search which device to use:

```bash
$ dmesg
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

Here the device is named `can0`.

For this instruction, we are assuming a speed of 500000kbps for your CAN bus,
you can try others supported bitrate like 125000,
250000 if 500000 doesn’t work:

```bash
$ ip link set can0 type can bitrate 500000
$ ip link set can0 up
$ ip link show can0
  can0: <NOARP, UP, LOWER_UP, ECHO> mtu 16 qdisc pfifo_fast state UNKNOWN qlen 10
    link/can
    can state ERROR-ACTIVE (berr-counter tx 0 rx 0) restart-ms 0
    bitrate 500000 sample-point 0.875
    tq 125 prop-seg 6 phase-seg1 7 phase-seg2 2 sjw 1
    sja1000: tseg1 1..16 tseg2 1..8 sjw 1..4 brp 1..64 brp-inc 1
    clock 16000000
```

On a Rcar Gen3 board for example, you’ll have your CAN device as can1 because `can0` already
exists as an embedded device.

The instructions will be the same:

```bash
$ ip link set can1 type can bitrate 500000
$ ip link set can1 up
$ ip link show can1
  can0: <NOARP, UP, LOWER_UP, ECHO> mtu 16 qdisc pfifo_fast state UNKNOWN qlen 10
    link/can
    can state ERROR-ACTIVE (berr-counter tx 0 rx 0) restart-ms 0
    bitrate 500000 sample-point 0.875
    tq 125 prop-seg 6 phase-seg1 7 phase-seg2 2 sjw 1
    sja1000: tseg1 1..16 tseg2 1..8 sjw 1..4 brp 1..64 brp-inc 1
    clock 1600000
```

## Rename an existing CAN device

You can rename an existing CAN device using following command and thus move
an existing `can0` device to anything else. You will then be able to use
another device as `can0`. For example, using a Rcar Gen3 board,
do the following :

```bash
sudo ip link set can0 down
sudo ip link set can0 name bsp-can0
sudo ip link set bsp-can0 up
```

Then connect your USB CAN device which should be named `can0` by default.

# Configure the binding

## Choose your active message set to use

Using several plugins you have the possibility to add several message set.
A message set is a coherent set of message representing a usage of a CAN
network. By example a car could run with a certain message set and could
need to access sensitive data through another so you can separate those using
this key **active_message_set**

```json
	"active_message_set": 0
```

## Choose the diagnostic bus

The diagnostic bus use on existing bus devices to communicate to OBD2 ECUs.
Use the following key to specify it:

```json
	"diagnostic_bus": "hs",
```

## Choose a default j1939 ECU name

On a j1939 capable CAN network you can claim an address on it and then be
able to retrieve the message addressed to this addr as the j1939 protocol is
a point-to-point protocol. Use the following key to claim a name:

```json
	"defaut_j1939_ecu": "0xC0509600227CC7AA"
```

## The dev-mapping section

The binding reads system configuration file
_/usr/local/canbus-binding/etc/control-canbus-binding.json_ at start to
map logical name from signals described in JSON file to linux devices name
initialized by the system.

Edit file _control-canbus-binding.json_ and add mapping in section `config`.

Default binding configuration use a CAN bus named `hs` so you need to map it to
the real one, here are some examples:

* Using virtual CAN device as described in the previous chapter:

```json
"config": {
  "dev-mapping": {
			"hs": "vcan0",
			"ls": "vcan1"
    }
}
```

* Using real CAN device, this example assume CAN bus traffic will be on can0.

```json
"config": {
  "dev-mapping": {
			"hs": "can0",
			"ls": "can1"
    }
}
```

* On a Rcar Gen3 board there is an embedded CAN device so `can0` already exists. So you might want to use your USB CAN adapter plugged to the OBD2 connector, in this case use `can1`:

```json
"config": {
  "dev-mapping": {
			"hs": "can1"
    }
}
```

## The preinit/postinit sections

These sections define actions from plugins to be executed before and after the
binding initialization. If you need set and executed some arbitrary code this is
a good place to go.

The operations executed between the those functions include the following:

* starting 2 threads, one for decoding incoming CAN messages and another to send
them as *afb event* to the user that has subscribed to them through the binding
API.
* Add a recurring dignostic message request to get engine speed at all times.
This was made to make sure that the engine is **On** and output a **warning** if
not.
* Claim the default j1939 ECU name to the CAN network.

Here is an example:

```json
"preinit": {
	"uid": "preinit",
	"action": "plugin://generated-generated#preinit"
},
"postinit": {
	"uid": "postinit",
	"action": "plugin://generated-generated#postinit"
},
```

## Example of a typical binding configuration file

The _control-canbus-binding.json_ file should have this structure:

```json
{
	"$schema": "",
	"metadata": {
		"uid": "CAN bus",
		"version": "2.0",
		"api": "canbus",
		"info": "CAN bus Configuration"
	},
	"config": {
		"active_message_set": 0,
		"dev-mapping": {
			"hs": "can0",
			"ls": "can0"
		},
		"preinit": {
			"uid": "preinit",
			"action": "plugin://generated-generated#preinit"
		},
		"postinit": {
			"uid": "postinit",
			"action": "plugin://generated-generated#postinit"
		},
		"diagnostic_bus": "hs",
		"defaut_j1939_ecu": "0xC0509600227CC7AA"
	},
	"plugins": [
		{
			"uid": "generated-generated",
			"info": "custom generated plugin",
			"libs": "generated-plugin.ctlso"
		}
	]
}
```

> **CAUTION VERY IMPORTANT:** Make sure the CAN bus\(es\) you specify in your
> configuration file match those specified in your generated source file with
> the `can-config-generator`.
