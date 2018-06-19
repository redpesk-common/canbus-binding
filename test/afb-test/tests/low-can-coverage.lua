
--[[
    Copyright (C) 2018 "IoT.bzh"
    Author Romain Forlot <romain.forlot@iot.bzh>

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.


    NOTE: strict mode: every global variables should be prefixed by '_'
--]]

_AFT.setBeforeAll(function()
    local can = io.open("/sys/class/net/can0")
    if can == nil then
        print("# You do not have 'can0' device set. Please run the following command:\n### sudo modprobe vcan; sudo ip link add can0 type vcan; sudo ip link set can0 up ")
        return -1
    end
    return 0
end)

_AFT.testVerbStatusSuccess("low-can_list", "low-can", "list", {})
_AFT.testVerbStatusSuccess("low-can_get", "low-can", "get", { event = "engine.speed"})

_AFT.describe("Diagnostic_engine_speed_simulation", function()
    _AFT.assertVerbStatusSuccess("low-can","subscribe", { event = "diagnostic_messages.engine.speed" })

    local evt = "low-can/diagnostic_messages"
    _AFT.addEventToMonitor(evt)

    local ret = os.execute("./var/replay_launcher.sh ./var/test1.canreplay")
    _AFT.assertIsTrue(ret)

    _AFT.assertEvtReceived(evt, function(eventName, data)
        _AFT.assertIsTrue(data.name == "diagnostic_messages.engine.speed")
    end)

    _AFT.assertVerbStatusSuccess("low-can","unsubscribe", { event = "diagnostic_messages.engine.speed" })

    local ret = os.execute("pkill canplayer")
    _AFT.assertIsTrue(ret)
end)

_AFT.describe("Subscribe_all", function()
    _AFT.assertVerbStatusSuccess("low-can","subscribe", { event = "*" })

    local evt = "low-can/messages.vehicle.average.speed"
    _AFT.addEventToMonitor(evt)

    local ret = os.execute("./var/replay_launcher.sh ./var/test2-3.canreplay")
    _AFT.assertIsTrue(ret)

    _AFT.assertEvtReceived(evt, function(eventName, data)
        _AFT.assertIsTrue(data.name == "messages.vehicle.average.speed")
    end)

    _AFT.assertVerbStatusSuccess("low-can","unsubscribe", { event = "*" })

    local ret = os.execute("pkill canplayer")
    _AFT.assertIsTrue(ret)
end)

_AFT.exitAtEnd()
