# Run it, test it, use it

You can run the binding using **afm-util** tool, here is the classic way to go :

```bash
afm-util run canbus-service@4.0
1
```

You can find instructions to use afm-util tool
[here](../../developer-guides/1-afm-daemons.html#using-afm-util),
 as well as documentation about Application Framework.

But you can't control nor interact with it because you don't know security
token that **Application Framework** gave it at launch.

So, to test it, it is better to launch the binding manually. In the following
example, it will use port **1234** and left empty security token for testing
purpose:

```bash
afb-binder --name=afbd-canbus-binding --workdir=/usr/local/canbus-binding --binding=lib/afb-canbus-binding.so --port=1234 --token=1 --tracereq=common -vvv
```

On another terminal, connect to the binding using previously installed
**AFB Websocket CLI** tool:

```bash
afb-client -H ws://localhost:1234/api?token=1
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

* API : _**canbus**_.
* Verb : _**subscribe**_ or _**unsubscribe**_
* Arguments : _**{ "event": "driver.doors.open" }**_

## Subscription and unsubscription

### Signal (un)subscription

You can ask to subscribe to chosen CAN signal calling API and a verb with the
name of the signal as verb prefixed by the plugin name holding the signal.

For example from an opened session:

```json
canbus vcar/doors.driver.open "subscribe"
ON-REPLY 1:canbus/vcar/doors.driver.open: {"jtype":"afb-reply","request":{"status":"success","uuid":"a18fd375-b6fa-4c0e-a1d4-9d3955975ae8"}}
```

Then you will receive an event each time a CAN message is decoded for the event
named _doors.driver.open_ with its received timestamp if available:

```json
ON-EVENT canbus/vcar/vcar\/doors.driver.open({"event":"canbus\/vcar\/doors.driver.open","data":{"vcar\/doors.driver.open":true, "timestamp": 1505812906020023},"jtype":"afb-event"})
```

You can stop receiving event from it by unsubscribe the signal the same way you did for subscribe

```json
canbus vcar/doors.driver.open "unsubscribe"
ON-REPLY 2:canbus/vcar/doors.driver.open: {"jtype":"afb-reply","request":{"status":"success"}}
```

### Message (un)subscription

You can also subscribe to an event with the ID or the PGN of the message definition :

```json
canbus subscribe {"id" : 1568}
canbus subscribe {"pgn" : 61442}
```

And subscribe to all ID or PGN :

```json
canbus subscribe {"id" : "*"}
canbus subscribe {"pgn" : "*"}
```

You can stop receiving event from it by unsubscribe the signal the same way you did for subscribe

```json
canbus unsubscribe {"id" : "*"}
ON-REPLY 3:canbus/unsubscribe: {"jtype":"afb-reply","request":{"status":"success"}}
canbus unsubscribe {"pgn" : "*"}
ON-REPLY 4:canbus/unsubscribe: {"jtype":"afb-reply","request":{"status":"success"}}
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
* rx_id : For the ISO TP protocol, define the id of source to write a message
* tx_id : For the ISO TP protocol, define the id of emitter to receive message

Order doesn't matter neither the number of filters chosen, you can use one, two
or all of them at once.

Usage examples :

```json
canbus vcar/engine.speed { "subscribe": {"filter": { "frequency": 3, "min": 1250, "max": 3500}}}
canbus vcar/engine.load  {"subscribe": {"filter": { "min": 30, "max": 100}}}
canbus vcar/vehicle.speed  {"subscribe": {"filter": { "frequency": 2}}}
# ISOTP
canbus subscribe {"id": 273, "filter": {"tx_id" : 562}}
```

## Get last signal value

You can also ask for a particular signal value on one shot using **get** verb, like
this:

```json
canbus vcar/engine.speed "get"
ON-REPLY 1:canbus/vcar/engine.speed: {"response":[{"vcar\/engine.speed":0}],"jtype":"afb-reply","request":{"status":"success"}}
```

> **CAUTION** Only one event could be requested.

## List of current loaded signals

Also, if you want to know the supported CAN signals loaded by **canbus**, use
verb **list**

```json
canbus list
ON-REPLY 2:canbus/list: {"response":["vcar\/hvac.fan.speed","vcar\/hvac.temperature.left","vcar\/hvac.temperature.right","vcar\/hvac.temperature.average","vcar\/engine.speed","vcar\/fuel.level.low","vcar\/fuel.level","vcar\/vehicle.average.speed","vcar\/engine.oil.temp","vcar\/engine.oil.temp.high","vcar\/doors.boot.open","vcar\/doors.front_left.open","vcar\/doors.front_right.open","vcar\/doors.rear_left.open","vcar\/doors.rear_right.open","vcar\/windows.front_left.open","vcar\/windows.front_right.open","vcar\/windows.rear_left.open","vcar\/windows.rear_right.open","vcar\/engine.load","vcar\/engine.coolant.temperature","vcar\/fuel.pressure","vcar\/intake.manifold.pressure","vcar\/engine.speed","vcar\/vehicle.speed","vcar\/intake.air.temperature","vcar\/mass.airflow","vcar\/throttle.position","vcar\/running.time","vcar\/EGR.error","vcar\/fuel.level","vcar\/barometric.pressure","vcar\/ambient.air.temperature","vcar\/commanded.throttle.position","vcar\/ethanol.fuel.percentage","vcar\/accelerator.pedal.position","vcar\/hybrid.battery-pack.remaining.life","vcar\/engine.oil.temperature","vcar\/engine.fuel.rate","vcar\/engine.torque"],"jtype":"afb-reply","request":{"status":"success","uuid":"32df712a-c7fa-4d58-b70b-06a87f03566b"}}
```

## Write on CAN buses

Two modes could be used for that which is either specifying the CAN bus and a
*RAW* CAN message either by specifying a defined signal, **case-insensitively**,
and its value.

Examples:

```json
# Authentication
canbus auth
# Write a raw can frame to the CAN id 0x620
canbus write { "bus_name": "hs", "frame": { "can_id": 1568, "can_dlc": 8, "can_data": [ 255, 255, 255, 255, 255, 255, 255, 255]} }
# Write a signal's value.
canbus write { "signal_name": "engine.speed", "signal_value": 1256}
# Write J1939 'single frame'
canbus write { "bus_name": "j1939", "frame": { "pgn": 61442, "length":8, "data": [ 255, 255, 255, 255, 255, 255, 255, 255]} }
# Write J1939 'multi frame'
canbus write { "bus_name": "j1939", "frame": { "pgn": 61442, "length":9, "data": [ 255, 255, 255, 255, 255, 255, 255, 255, 254]} }
# Write ISOTP 'single frame'
canbus write {"bus_name": "hs", "filter": {"rx_id" : 562}, "frame": { "can_id": 273, "can_dlc": 8, "can_data": [ 255, 255, 255, 255, 255, 255, 255, 255]} }
# Write ISOTP 'multi frame'
canbus write {"bus_name": "hs", "filter": {"rx_id" : 562}, "frame": { "can_id": 273, "can_dlc": 9, "can_data": [ 255, 255, 255, 255, 255, 255, 255, 255, 25]} }
```

To be able to use write capability, you need to add the permission
 ```urn:redpesk:permission::platform:can:write``` to your package configuration
 file that need to write on CAN bus through **canbus** api.

Then in order to write on bus, your app needs to call verb **auth**
before calling **write**, to raise its **LOA**, Level Of Assurance,
which controls usage of verb **write**.

### Using signal dedicated verbs

The same as when you subscribe or unsubscribe for a signals you can use a
dedicated verb to write a new of it **if the signal has been set as writable**:

```
canbus auth
canbus vcar/engine_speed { "write": 1256 }
```

## Using CAN utils to monitor CAN activity

You can watch CAN traffic and send custom CAN messages using can-utils
preinstalled on redpesk@ targets.

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
