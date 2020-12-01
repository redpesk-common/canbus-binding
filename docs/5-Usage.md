# Run it, test it, use it

You can run the binding using **afm-util** tool, here is the classic way to go :

```bash
afm-util run canbus-service@4.0
1
```

You can find instructions to use afm-util tool
[here](../../reference/af-main/1-afm-daemons.html#using-afm-util),
 as well as documentation about Application Framework.

But you can't control nor interact with it because you don't know security
token that **Application Framework** gave it at launch.

So, to test it, it is better to launch the binding manually. In the following
example, it will use port **1234** and left empty security token for testing
purpose:

```bash
afb-daemon --name=afbd-canbus-binding --workdir=/usr/local/canbus-binding --binding=lib/afb-canbus-binding.so --port=1234 --token=1 --tracereq=common -vvv
```

On another terminal, connect to the binding using previously installed
**AFB Websocket CLI** tool:

```bash
afb-client-demo -H ws://localhost:1234/api?token=1
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

### Using general purpose verbs

You can ask to subscribe to chosen CAN event with a call to _subscribe_ API
verb with the CAN messages name as JSON argument.

> **NOTE:** If no argument is provided, then you'll subscribe to all signals
> at once.

For example from a websocket session:

```json
canbus subscribe { "event": "doors.driver.open" }
ON-REPLY 1:canbus/subscribe: {"jtype":"afb-reply","request":{"status":"success","uuid":"a18fd375-b6fa-4c0e-a1d4-9d3955975ae8"}}
```

Subscription and unsubscription can take wildcard in their _event_ value and are
**case-insensitive**.

To receive all doors events :

```json
canbus subscribe { "event" : "doors*" }
ON-REPLY 1:canbus/subscribe: {"jtype":"afb-reply","request":{"status":"success","uuid":"511c872e-d7f3-4f3b-89c2-aa9a3e9fbbdb"}}
```

Then you will receive an event each time a CAN message is decoded for the event
named _doors.driver.open_ with its received timestamp if available:

```json
ON-EVENT canbus/messages.doors.driver.open({"event":"canbus\/messages.doors.driver.open","data":{"name":"messages.doors.driver.open","value":true, "timestamp": 1505812906020023},"jtype":"afb-event"})
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
canbus subscribe { "event" : "*speed*" } --> will subscribe to all messages with speed in their name. Search will be make without prefix for it.
canbus subscribe { "event" : "speed*" } --> will subscribe to all messages begin by speed in their name. Search will be make without prefix for it.
canbus subscribe { "event" : "messages*speed*" } --> will subscribe to all CAN messages with speed in their name. Search will be on prefixed messages here.
canbus subscribe { "event" : "messages*speed" } --> will subscribe to all CAN messages ending with speed in their name. Search will be on prefixed messages here.
canbus subscribe { "event" : "diagnostic*speed*" } --> will subscribe to all diagnostic messages with speed in their name. Search will be on prefixed messages here.
canbus subscribe { "event" : "diagnostic*speed" } --> will subscribe to all diagnostic messages ending with speed in their name. Search will be on prefixed messages here.
```

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
canbus unsubscribe { "event": "doors.driver.open" }
ON-REPLY 2:canbus/unsubscribe: {"jtype":"afb-reply","request":{"status":"success"}}
canbus unsubscribe { "event" : "doors*" }
ON-REPLY 3:canbus/unsubscribe: {"jtype":"afb-reply","request":{"status":"success"}}
```

### Using signal dedicated verbs

If you only need to subscribe a signal you hav the possibility to use its
name to interact with it. For example, to subscribe to signal
**messages.doors.driver.open** you can simply call:

```
canbus sub_messages_doors_driver_open
ON-EVENT-CREATE: [1:canbus/messages.doors.driver.open*]
ON-EVENT-SUBSCRIBE 1:sub_messages_doors_driver_open: [1]
ON-REPLY 1:sub_messages_doors_driver_open: success Signal messages.messages.doors.driver.open* subscribed.
null
```

> **CAUTION** Note the **point** characters translated to **underscore**
> characters.

And to unsubscribe:

```
canbus unsub_messages_doors_driver_open
ON-REPLY 1:unsub_messages_doors_driver_open: success Signal messages.messages.doors.driver.open* unsubscribed.
null
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
canbus subscribe {"event": "messages.engine.speed", "filter": { "frequency": 3, "min": 1250, "max": 3500}}
canbus subscribe {"event": "messages.engine.load", "filter": { "min": 30, "max": 100}}
canbus subscribe {"event": "messages.vehicle.speed", "filter": { "frequency": 2}}
# ISOTP
canbus subscribe {"id": 273, "filter": {"tx_id" : 562}}
```

## Get last signal value and list of configured signals

### Using general purpose verbs

You can also ask for a particular signal value on one shot using **get** verb, like
this:

```json
canbus get {"event": "messages.engine.speed"}
ON-REPLY 1:canbus/get: {"response":[{"event":"messages.engine.speed","value":0}],"jtype":"afb-reply","request":{"status":"success"}}
```

> **CAUTION** Only one event could be requested.

Also, if you want to know the supported CAN signals loaded by **canbus**, use
verb **list**

```json
canbus list
ON-REPLY 2:canbus/list: {"response":["messages.hvac.fan.speed","messages.hvac.temperature.left","messages.hvac.temperature.right","messages.hvac.temperature.average","messages.engine.speed","messages.fuel.level.low","messages.fuel.level","messages.vehicle.average.speed","messages.engine.oil.temp","messages.engine.oil.temp.high","messages.doors.boot.open","messages.doors.front_left.open","messages.doors.front_right.open","messages.doors.rear_left.open","messages.doors.rear_right.open","messages.windows.front_left.open","messages.windows.front_right.open","messages.windows.rear_left.open","messages.windows.rear_right.open","diagnostic_messages.engine.load","diagnostic_messages.engine.coolant.temperature","diagnostic_messages.fuel.pressure","diagnostic_messages.intake.manifold.pressure","diagnostic_messages.engine.speed","diagnostic_messages.vehicle.speed","diagnostic_messages.intake.air.temperature","diagnostic_messages.mass.airflow","diagnostic_messages.throttle.position","diagnostic_messages.running.time","diagnostic_messages.EGR.error","diagnostic_messages.fuel.level","diagnostic_messages.barometric.pressure","diagnostic_messages.ambient.air.temperature","diagnostic_messages.commanded.throttle.position","diagnostic_messages.ethanol.fuel.percentage","diagnostic_messages.accelerator.pedal.position","diagnostic_messages.hybrid.battery-pack.remaining.life","diagnostic_messages.engine.oil.temperature","diagnostic_messages.engine.fuel.rate","diagnostic_messages.engine.torque"],"jtype":"afb-reply","request":{"status":"success","uuid":"32df712a-c7fa-4d58-b70b-06a87f03566b"}}
```

### Using signal dedicated verbs

The same as when you subscribe or unsubscribe for a signals you can use a
dedicated verb to get the last value of it:

```
canbus r_messages_engine_speed
ON-REPLY 1:canbus/get: {"response":[{"event":"messages.engine.speed","value":0}],"jtype":"afb-reply","request":{"status":"success"}}
null
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
canbus w_messages_engine_speed { "signal_name": "engine.speed", "signal_value": 1256}
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
