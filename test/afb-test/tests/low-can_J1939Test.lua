
--[[
    Copyright (C) 2018 "IoT.bzh"
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
###########################################################
####################### J1939 TESTS #######################
###########################################################
]] 

print("\n##########################################")
print("#########    BEGIN J1939 TESTS   #########")
print("##########################################\n")

--[[
+++++++++++++++++++++++++++
Tests subscribe/unsubscribe
+++++++++++++++++++++++++++
]]


--print("\n++++++++++++++++++++++++++++++++++++++++++")
--print("++++  TESTS SUBSCRIBE / UNSUBSCRIBE   ++++")
--print("++++++++++++++++++++++++++++++++++++++++++\n")

_AFT.describe("Test subscribe/unsubscribe", function()
_AFT.testVerbStatusSuccess("low-can_subscribe_j1939_event", "low-can", "subscribe", {event= "Eng.Momentary.Overspeed.Enable"})
_AFT.testVerbStatusSuccess("low-can_subscribe_j1939_events", "low-can", "subscribe", {event= "Eng.*"})
_AFT.testVerbStatusSuccess("low-can_subscribe_j1939_pgn", "low-can", "subscribe", {pgn= 61442})
_AFT.testVerbStatusSuccess("low-can_subscribe_j1939_all_pgn", "low-can", "subscribe", {pgn= "*"})
_AFT.testVerbStatusSuccess("low-can_subscribe_j1939_id", "low-can", "subscribe", {id= 61442})
_AFT.testVerbStatusSuccess("low-can_subscribe_j1939_all_id", "low-can", "subscribe", {id= "*"})
_AFT.testVerbStatusSuccess("low-can_subscribe_j1939_event", "low-can", "unsubscribe", {event= "Eng.Momentary.Overspeed.Enable"})
_AFT.testVerbStatusSuccess("low-can_subscribe_j1939_event", "low-can", "unsubscribe", {event= "*"})
_AFT.testVerbStatusSuccess("low-can_subscribe_j1939_id", "low-can", "subscribe", {id= 61442})
_AFT.testVerbStatusError("low-can_subscribe_j1939_no_event", "low-can", "subscribe", {event= ""})
_AFT.testVerbStatusError("low-can_subscribe_j1939_no_event", "low-can", "subscribe", {id= ""})
_AFT.testVerbStatusError("low-can_subscribe_j1939_no_event", "low-can", "subscribe", {pgn= ""})
end)


--[[
+++++++++++
Tests write
+++++++++++
]] 

--print("\n++++++++++++++++++++++++++")
--print("++++   TESTS WRITE    ++++")
--print("++++++++++++++++++++++++++\n")

_AFT.describe("Test write", function()
_AFT.testVerbStatusError("low-can_write_wo_auth", "low-can", "write", { signal_name = "Eng.Momentary.Overspeed.Enable", signal_value = 1})
_AFT.testVerbStatusSuccess("low-can_auth", "low-can", "auth", {})
_AFT.testVerbStatusSuccess("low-can_write_signal", "low-can", "write", { signal_name = "Eng.Momentary.Overspeed.Enable", signal_value = 1})
_AFT.testVerbStatusSuccess("low-can_write_frame", "low-can", "write", { bus_name= "j1939", frame= { pgn= 62420, length=8, data= {1,2,3,4,5,6,7,8}}})
_AFT.testVerbStatusSuccess("low-can_write_frame_other_pgn", "low-can", "write", { bus_name= "j1939", frame= { pgn= 126208, length=8, data= {9,10,11,12,13,14,15,16}}})
_AFT.testVerbStatusError("low-can_write_frame_invalid_pgn", "low-can", "write", { bus_name= "j1939", frame= { pgn= 1234, length=8, data= {9,10,11,12,13,14,15,16}}})
_AFT.testVerbStatusSuccess("low-can_write_multi_frame", "low-can", "write", { bus_name= "j1939", frame= { pgn= 126208, length=9, data= {9,10,11,12,13,14,15,16,17}}})
end)
--[[
++++++++++
Tests read
++++++++++
]] 

local api = "low-can"
local evt = "messages.Eng.Momentary.Overspeed.Enable"
local evt2 = "messages.Actl.Eng.Prcnt.Trque.High.Resolution"

_AFT.describe("Test subscribe read frame", function()
    _AFT.addEventToMonitor(api .. "/" ..evt, function(eventname,data)
        _AFT.assertEquals(eventname, api.."/"..evt)
    end)

    _AFT.assertVerbStatusSuccess(api ,"subscribe", { event = evt})
    local ret = os.execute("bash ".._AFT.bindingRootDir.."/var/replay_launcher.sh ".._AFT.bindingRootDir.."/var/testj1939.canreplay");
    _AFT.assertIsTrue(ret)

    _AFT.assertEvtReceived(api .. "/" ..evt, 1000000)
end,
nil,
function()
    _AFT.callVerb(api, "unsubscribe", { event = evt})
end)

------------------------------------------------------------------------------

_AFT.describe("Test subscribe not read all frame", function()
    _AFT.addEventToMonitor(api .. "/" ..evt2, function(eventname,data)
        _AFT.assertEquals(eventname, api.."/"..evt2)
    end)

    _AFT.assertVerbStatusSuccess(api ,"subscribe", { event = evt2})
    local ret = os.execute("bash ".._AFT.bindingRootDir.."/var/replay_launcher.sh ".._AFT.bindingRootDir.."/var/testj1939.canreplay");
    _AFT.assertIsTrue(ret)

    _AFT.assertEvtNotReceived(api .. "/" ..evt2, 1000000)
end,
nil,
function()
    _AFT.callVerb(api, "unsubscribe", { event = evt2})
end)


---------------

_AFT.describe("Test subscribe read frame low time", function()
    _AFT.addEventToMonitor(api .. "/" ..evt, function(eventname,data)
        _AFT.assertEquals(eventname, api.."/"..evt)
    end)

    _AFT.assertVerbStatusSuccess(api ,"subscribe", { event = evt})
    local ret = os.execute("bash ".._AFT.bindingRootDir.."/var/replay_launcher.sh ".._AFT.bindingRootDir.."/var/testj1939.canreplay");
    _AFT.assertIsTrue(ret)
end,
nil,
function()
    _AFT.callVerb(api, "unsubscribe", { event = evt})
end)

_AFT.exitAtEnd()
