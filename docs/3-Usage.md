# Configure the AGL system

## Virtual CAN device

Connected to the target, here is how to load the virtual CAN device driver and
set up a new vcan device :

```bash
modprobe vcan
ip link add vcan0 type vcan
ip link set vcan0 up
```

You also can named your linux CAN device like you want and if you need name it
`can0` :

```bash
modprobe vcan
ip link add can0 type vcan
ip link set can0 up
```

## CAN device using the USB CAN adapter

Using real connection to CAN bus of your car using the USB CAN adapter
connected to the OBD2 connector.

Once connected, launch `dmesg` command and search which device to use:

```bash
dmesg
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

Here device is named `can0`.

This instruction assuming a speed of 500000kbps for your CAN bus, you can try
others supported bitrate like 125000, 250000 if 500000 doesn't work:

```bash
ip link set can0 type can bitrate 500000
ip link set can0 up
ip link show can0
  can0: <NOARP,UP,LOWER_UP,ECHO> mtu 16 qdisc pfifo_fast state UNKNOWN qlen 10
    link/can
    can state ERROR-ACTIVE (berr-counter tx 0 rx 0) restart-ms 0
    bitrate 500000 sample-point 0.875
    tq 125 prop-seg 6 phase-seg1 7 phase-seg2 2 sjw 1
    sja1000: tseg1 1..16 tseg2 1..8 sjw 1..4 brp 1..64 brp-inc 1
    clock 16000000
```

On a Rcar Gen3 board, you'll have your CAN device as `can1` because `can0`
already exists as an embedded device.

The instructions will be the same:

```bash
ip link set can1 type can bitrate 500000
ip link set can1 up
ip link show can1
  can0: <NOARP,UP,LOWER_UP,ECHO> mtu 16 qdisc pfifo_fast state UNKNOWN qlen 10
    link/can
    can state ERROR-ACTIVE (berr-counter tx 0 rx 0) restart-ms 0
    bitrate 500000 sample-point 0.875
    tq 125 prop-seg 6 phase-seg1 7 phase-seg2 2 sjw 1
    sja1000: tseg1 1..16 tseg2 1..8 sjw 1..4 brp 1..64 brp-inc 1
    clock 16000000
```

## Rename an existing CAN device

You can rename an existing CAN device using following command and doing so move
an existing `can0` device to anything else and then use another device as `can0`
. For a Rcar Gen3 board do the following by example:

```bash
sudo ip link set can0 down
sudo ip link set can0 name bsp-can0
sudo ip link set bsp-can0 up
```

Then connect your USB CAN device that will be named `can0` by default.

# Configure the binding

The binding reads system configuration file _/etc/dev-mapping.conf_ at start to
map logical name from signals described in JSON file to linux devices name
initialized by the system.

Edit file _/etc/dev-mapping.conf_ and add mapping in section `CANbus-mapping`.

Default binding configuration use a CAN bus named `hs` so you need to map it to
the real one, here are some examples:

* Using virtual CAN device as described in the previous chapter:

```ini
[CANbus-mapping]
hs="vcan0"
ls="vcan1"
```

* Using real CAN device, this example assume CAN bus traffic will be on can0.

```ini
[CANbus-mapping]
hs="can0"
ls="can1"
```

* On a Rcar Gen3 board there is an embedded CAN device so `can0` already exists. So you might want to use your USB CAN adapter plugged to the OBD2 connector, in this case use `can1`:

```ini
[CANbus-mapping]
hs="can1"
```

> **CAUTION VERY IMPORTANT:** Make sure the CAN bus\(es\) you specify in your
> configuration file match those specified in your generated source file with
> the `CAN-config-generator`.

# Run it, test it, use it.

You can run the binding using **afm-util** tool, here is the classic way to go :

```bash
afm-util run low-can-service@4.0
1
```

You can find instructions to use afm-util tool
[here](http://docs.iot.bzh/docs/apis_services/en/dev/reference/af-main/afm-daemons.html#using-afm-util),
 as well as documentation about Application Framework.

But you can't control nor interact with it because you don't know security
token that **Application Framework** gaves it at launch.

So, to test it, it is better to launch the binding manually. In the following
example, it will use port **1234** and left empty security token for testing
purpose:

```bash
afb-daemon --binding=/var/lib/afm/applications/low-can-service/4.0/lib/afb-low-can.so --rootdir=/var/lib/afm/applications/low-can-service/4.0/ --port=1234 --token=1
NOTICE: binding [/usr/lib/afb/afb-dbus-binding.so] calling registering function afbBindingV1Register
NOTICE: binding /usr/lib/afb/afb-dbus-binding.so loaded with API prefix dbus
NOTICE: binding [/usr/lib/afb/authLogin.so] calling registering function afbBindingV1Register
NOTICE: binding /usr/lib/afb/authLogin.so loaded with API prefix auth
NOTICE: binding [/var/lib/afm/applications/low-can-service/4.0/libs//low-can-binding.so] calling registering function afbBindingV1Register
NOTICE: binding /var/lib/afm/applications/low-can-service/4.0/libs//low-can-binding.so loaded with API prefix low-can
NOTICE: Waiting port=1234 rootdir=/var/lib/afm/applications/low-can-service/4.0/
NOTICE: Browser URL= http:/*localhost:1234
```

On another terminal, connect to the binding using previously installed
**AFB Websocket CLI** tool:

```bash
afb-client-demo ws://localhost:1234/api?token=1
```

You will be on an interactive session where you can communicate directly with
the binding API.

The binding provides at this moment 2 verbs, _subscribe_ and _unsubscribe_,
which can take argument by a JSON **event** object.

The argument value is the CAN message **generic\_name** as described in the
JSON file used to generate cpp file for the binding.

To use the _**AFB Websocket CLI**_ tool, a command line will be like the
following:

```
<api> <verb> <arguments>
```

Where:

* API : _**low-can**_.
* Verb : _**subscribe**_ or _**unsubscribe**_
* Arguments : _**{ "event": "driver.doors.open" }**_

## Subscription and unsubscription

You can ask to subscribe to chosen CAN event with a call to _subscribe_ API
verb with the CAN messages name as JSON argument.

> **NOTE:** If no argument is provided, then you'll subscribe to all signals
> at once.

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

Then you will receive an event each time a CAN message is decoded for the event
named _doors.driver.open_ with its received timestamp if available:

```json
ON-EVENT low-can/messages.doors.driver.open({"event":"low-can\/messages.doors.driver.open","data":{"name":"messages.doors.driver.open","value":true, "timestamp": 1505812906020023},"jtype":"afb-event"})
```

Notice that event shows you that the CAN event is named
_messages.doors.driver.open_ but you ask for event about
_doors.driver.open_.

This is because all CAN messages or diagnostic messages are prefixed by the
JSON parent node name, **messages** for CAN messages and
**diagnostic\_messages** for diagnostic messages like OBD2.

This will let you subscribe or unsubcribe to all signals at once, not
recommended, and better make filter on subscribe operation based upon their type. Examples:

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

### Filtering capabilities

It is possible to limits received event notifications into minimum and maximum
boundaries as well as doing frequency thinning. This is possible using the
argument filter with one or more of the filters available :

* frequency: specify in Hertz the frequency which will be used to getting
 notified of new CAN events for the designated signal. If, during the blocked
 time, further changed CAN messages are received, the last valid one will be
 transferred after the lockout with a RX_CHANGED.
* min: Minimum value that the decoded value needs to be above to get pushed to
 the subscribed client(s).
* max: Maximum value that the decoded value needs to be below to get pushed to
 the subscribed client(s)

Order doesn't matter neither the number of filters chosen, you can use one, two
or all of them at once.

Usage examples :

```json
low-can subscribe {"event": "messages.engine.speed", "filter": { "frequency": 3, "min": 1250, "max": 3500}}
low-can subscribe {"event": "messages.engine.load", "filter": { "min": 30, "max": 100}}
low-can subscribe {"event": "messages.vehicle.speed", "filter": { "frequency": 2}}
```

## Get last signal value and list of configured signals

You can also ask for a particular signal value on one shot using **get** verb, like
this:

```json
low-can get {"event": "messages.engine.speed"}
ON-REPLY 1:low-can/get: {"response":[{"event":"messages.engine.speed","value":0}],"jtype":"afb-reply","request":{"status":"success"}}
```

> **CAUTION** Only one event could be requested.

Also, if you want to know the supported CAN signals loaded by **low-can**, use
verb **list**

```json
low-can list
ON-REPLY 2:low-can/list: {"response":["messages.hvac.fan.speed","messages.hvac.temperature.left","messages.hvac.temperature.right","messages.hvac.temperature.average","messages.engine.speed","messages.fuel.level.low","messages.fuel.level","messages.vehicle.average.speed","messages.engine.oil.temp","messages.engine.oil.temp.high","messages.doors.boot.open","messages.doors.front_left.open","messages.doors.front_right.open","messages.doors.rear_left.open","messages.doors.rear_right.open","messages.windows.front_left.open","messages.windows.front_right.open","messages.windows.rear_left.open","messages.windows.rear_right.open","diagnostic_messages.engine.load","diagnostic_messages.engine.coolant.temperature","diagnostic_messages.fuel.pressure","diagnostic_messages.intake.manifold.pressure","diagnostic_messages.engine.speed","diagnostic_messages.vehicle.speed","diagnostic_messages.intake.air.temperature","diagnostic_messages.mass.airflow","diagnostic_messages.throttle.position","diagnostic_messages.running.time","diagnostic_messages.EGR.error","diagnostic_messages.fuel.level","diagnostic_messages.barometric.pressure","diagnostic_messages.ambient.air.temperature","diagnostic_messages.commanded.throttle.position","diagnostic_messages.ethanol.fuel.percentage","diagnostic_messages.accelerator.pedal.position","diagnostic_messages.hybrid.battery-pack.remaining.life","diagnostic_messages.engine.oil.temperature","diagnostic_messages.engine.fuel.rate","diagnostic_messages.engine.torque"],"jtype":"afb-reply","request":{"status":"success","uuid":"32df712a-c7fa-4d58-b70b-06a87f03566b"}}
```

## Write on CAN buses

A new capability as been introcuded to be able to write on handled CAN buses.
Two modes could be used for that which is either specifying the CAN bus and a
*RAW* CAN message either by specifying a defined signal and its value.

Examples:

```json
# Write a raw can frame to the CAN id 0x620
low-can write { "bus_name": "hs", "frame": { "can_id": 1568, "can_dlc":
8, "can_data": [ 255,255,255,255,255,255,255,255]} }
# Write a signal's value.
low-can write { "signal_name": "engine.speed", "value": 1256}
```

To be able to use write capability, you need to add the permission
 ```urn:AGL:permission::platform:can:write``` to your package configuration
 file that need to write on CAN bus through **low-can** api.

Then in order to write on bus, your app needs to call verb **auth**
before calling **write**, to raise its **LOA**, Level Of Assurance,
which controls usage of verb **write**.

## Using CAN utils to monitor CAN activity

You can watch CAN traffic and send custom CAN messages using can-utils
preinstalled on AGL target.

To watch watch going on a CAN bus use:

```bash
candump can0
```

Or for an USB CAN adapter connected to porter board:

```bash
candump can1
```

Send a custom message:

```bash
cansend can0 ID#DDDDAAAATTTTAAAA
```

You can also replay a previously dumped CAN logfiles. These logfiles can be
found in _can_samples_ directory under Git repository. Following examples use
a real trip from an Auris Toyota car.

Trace has been recorded from a CAN device `can0` so you have to map it to the
correct one you use for your tests.

Replay on a virtual CAN device `vcan0`:

```bash
canplayer -I trip_test_with_obd2_vehicle_speed_requests vcan0=can0
```

Replay on a CAN device `can0`:

```bash
canplayer -I trip_test_with_obd2_vehicle_speed_requests can0
```

Replay on a CAN device `can1` (porter by example):

```bash
canplayer -I trip_test_with_obd2_vehicle_speed_requests can1=can0
```
