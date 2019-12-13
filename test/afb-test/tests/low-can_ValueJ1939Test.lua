
--[[
    Copyright (C) 2019 "IoT.bzh"
    Author Arthur Guyader <arthur.guyader@iot.bzh>

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
    local can = io.open("/sys/class/net/can2")
    if can == nil then
        print("# You do not have 'can2' device set. Please run the following command:\n### sudo modprobe vcan; sudo ip link add can2 type vcan; sudo ip link set can2 up ")
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
##################################################################
####################### VALUES J1939 TESTS #######################
##################################################################
]]

print("\n########################################################################")
print("####################### BEGIN VALUES J1939 TESTS #######################")
print("########################################################################\n")

--[[
++++++++++++++++
Tests n2k values
++++++++++++++++
]]

local api = "low-can"
local evt1 = "messages.Wind.Data.Reference"
local evt2 = "messages.Wind.Data.Sid"
local evt3 = "messages.Wind.Data.Wind.Angle"
local evt4 = "messages.Wind.Data.Wind.Speed"


_AFT.describe("Value_Test_N2K_1", function()
    _AFT.addEventToMonitor(api .. "/" ..evt1, function(eventname, data)
        _AFT.assertEquals(eventname, api.."/"..evt1)
        _AFT.assertIsTrue(data.value == "Apparent")
    end)

    _AFT.assertVerbStatusSuccess(api ,"subscribe", { event = evt1 })
    local ret = os.execute("bash ".._AFT.bindingRootDir.."/var/replay_launcher.sh ".._AFT.bindingRootDir.."/var/testValueJ1939_1.canreplay");
    _AFT.assertIsTrue(ret)

    _AFT.assertEvtReceived(api .. "/" ..evt1, 1000000)
end,
nil,
function()
    _AFT.callVerb(api, "unsubscribe", { event = evt1 })
end)

_AFT.describe("Value_Test_N2K_2", function()
    _AFT.addEventToMonitor(api .. "/" ..evt2, function(eventname, data)
        _AFT.assertEquals(eventname, api.."/"..evt2)
        _AFT.assertIsTrue(data.value == 236)
    end)

    _AFT.assertVerbStatusSuccess(api ,"subscribe", { event = evt2 })
    local ret = os.execute("bash ".._AFT.bindingRootDir.."/var/replay_launcher.sh ".._AFT.bindingRootDir.."/var/testValueJ1939_1.canreplay");
    _AFT.assertIsTrue(ret)

    _AFT.assertEvtReceived(api .. "/" ..evt2, 1000000)
end,
nil,
function()
    _AFT.callVerb(api, "unsubscribe", { event = evt2 })
end)

_AFT.describe("Value_Test_N2K_3", function()
    _AFT.addEventToMonitor(api .. "/" ..evt3, function(eventname, data)
        _AFT.assertEquals(eventname, api.."/"..evt3)
        _AFT.assertIsTrue(math.abs(data.value - 4.9798998832703) < 0.00000001)
    end)

    _AFT.assertVerbStatusSuccess(api ,"subscribe", { event = evt3 })
    local ret = os.execute("bash ".._AFT.bindingRootDir.."/var/replay_launcher.sh ".._AFT.bindingRootDir.."/var/testValueJ1939_1.canreplay");
    _AFT.assertIsTrue(ret)

    _AFT.assertEvtReceived(api .. "/" ..evt3, 1000000)
end,
nil,
function()
    _AFT.callVerb(api, "unsubscribe", { event = evt3 })
end)

_AFT.describe("Value_Test_N2K_4", function()
    _AFT.addEventToMonitor(api .. "/" ..evt4, function(eventname, data)
        _AFT.assertEquals(eventname, api.."/"..evt4)
        _AFT.assertIsTrue(math.abs(data.value - 0.93000000715256) < 0.00000001)
    end)

    _AFT.assertVerbStatusSuccess(api ,"subscribe", { event = evt4 })
    local ret = os.execute("bash ".._AFT.bindingRootDir.."/var/replay_launcher.sh ".._AFT.bindingRootDir.."/var/testValueJ1939_1.canreplay");
    _AFT.assertIsTrue(ret)

    _AFT.assertEvtReceived(api .. "/" ..evt4, 1000000)
end,
nil,
function()
    _AFT.callVerb(api, "unsubscribe", { event = evt4 })
end)