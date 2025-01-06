
--[[
    Copyright (C) 2019-2025 IoT.bzh Company
    Author: Arthur Guyader <arthur.guyader@iot.bzh>

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
        print("# You do not have 'can0' device set. Please run the following command:\n### sudo modprobe vcan; sudo ip link add can2 type vcan; sudo ip link set can2 up ")
        return -1
    end
    return 0
end)

_AFT.setBeforeEach(function() print("~~~~~ Begin Test ~~~~~") end)

_AFT.setAfterEach(function()
    os.execute("pkill canplayer")
    os.execute("pkill linuxcan-canpla")
    print("~~~~~ End Test ~~~~~")
end)

_AFT.setAfterAll( function()
    os.execute("pkill canplayer")
    os.execute("pkill linuxcan-canpla")
    return 0
end)


--[[
################################################################
####################### VALUES CAN TESTS #######################
################################################################
]]

print("\n######################################################################")
print("####################### BEGIN VALUES CAN TESTS #######################")
print("######################################################################\n")

--[[
++++++++++++++++++++++
Tests basic can values
++++++++++++++++++++++
]]

local api = "low-can"
local evt = "messages.vehicle.average.speed"
local evt2 = "diagnostic_messages.engine.speed"
local evt3 = "diagnostic_messages.engine.load"

_AFT.describe("Value_Test_Can", function()
    _AFT.addEventToMonitor(api .. "/" ..evt, function(eventname, data)
        _AFT.assertEquals(eventname, api.."/"..evt)
        _AFT.assertIsTrue(data.value == 8)
    end)

    _AFT.assertVerbStatusSuccess(api ,"subscribe", { event = evt })
    local ret = os.execute("bash ".._AFT.bindingRootDir.."/var/replay_launcher.sh ".._AFT.bindingRootDir.."/var/testValueCAN_1.canreplay");
    _AFT.assertIsTrue(ret)

    _AFT.assertEvtReceived(api .. "/" ..evt, 1000000)
end,
nil,
function()
    _AFT.callVerb(api, "unsubscribe", { event = evt })
end)


_AFT.describe("Value_Test_Can2", function()
    _AFT.addEventToMonitor(api .. "/" ..evt2, function(eventname, data)
        _AFT.assertEquals(eventname, api.."/"..evt2)
        _AFT.assertIsTrue(math.abs(data.value - 2045.25) < 0.00000001)
    end)

    _AFT.assertVerbStatusSuccess(api ,"subscribe", { event = evt2 })
    local ret = os.execute("bash ".._AFT.bindingRootDir.."/var/replay_launcher.sh ".._AFT.bindingRootDir.."/var/testValueCAN_2.canreplay");
    _AFT.assertIsTrue(ret)

    _AFT.assertEvtReceived(api .. "/" ..evt2, 1000000)
end,
nil,
function()
    _AFT.callVerb(api, "unsubscribe", { event = evt2 })
end)


_AFT.describe("Value_Test_Can3", function()
    _AFT.addEventToMonitor(api .. "/" ..evt3, function(eventname, data)
        _AFT.assertEquals(eventname, api.."/"..evt3)
        _AFT.assertIsTrue(math.abs(data.value - 54.901962280273438) < 0.00000001)
    end)

    _AFT.assertVerbStatusSuccess(api ,"subscribe", { event = evt3 })
    local ret = os.execute("bash ".._AFT.bindingRootDir.."/var/replay_launcher.sh ".._AFT.bindingRootDir.."/var/testValueCAN_3.canreplay");
    _AFT.assertIsTrue(ret)

    _AFT.assertEvtReceived(api .. "/" ..evt3, 1000000)
end,
nil,
function()
    _AFT.callVerb(api, "unsubscribe", { event = evt3 })
end)
