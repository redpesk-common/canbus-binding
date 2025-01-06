 --[[
    Copyright (C) 2018-2025 IoT.bzh Company
    Author Clément Malléjac <clementmallejac@gmail.com>

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

print("\n##########################################")
print("#########    BEGIN FILTER TESTS   #########")
print("##########################################\n")

local api = "low-can"
local evt = "messages.engine.speed"

_AFT.setBeforeAll(function()
    local can = io.open("/sys/class/net/can0")
    if can == nil then
        print("# You do not have 'can0' device set. Please run the following command:\n### sudo modprobe vcan; sudo ip link add can0 type vcan; sudo ip link set can0 up ")
        return -1
    end
    return 0
end)

_AFT.setAfterEach(function()
    os.execute("pkill canplayer")
    os.execute("pkill linuxcan-canpla")
end)

_AFT.setAfterAll( function()
    os.execute("pkill canplayer")
    os.execute("pkill linuxcan-canpla")
    return 0
end)

_AFT.describe("Filter_Test_01/Step_1", function()
    _AFT.addEventToMonitor(api .. "/" ..evt, function(eventname, data)
        _AFT.assertEquals(data.name, evt)
    end)

    _AFT.assertVerbStatusSuccess(api ,"subscribe", { event = evt, filter =  { min = 30, max = 100}})
    local ret = os.execute("bash ".._AFT.bindingRootDir.."/var/replay_launcher.sh ".._AFT.bindingRootDir.."/var/testFilter01filteredOut.canreplay");
    _AFT.assertIsTrue(ret)

    _AFT.assertEvtNotReceived(api .. "/" ..evt, 1000000)
end,
nil,
function()
    _AFT.callVerb(api, "unsubscribe", { event = evt, filter =  {min = 30, max = 100} })
end)

_AFT.describe("Filter_Test_01/Step_2", function()
    _AFT.addEventToMonitor(api .. "/" ..evt, function(eventname, data)
        _AFT.assertEquals(eventname, api.."/"..evt)
        _AFT.assertIsTrue(data.value > 30 and data.value < 100)
    end)

    _AFT.assertVerbStatusSuccess(api ,"subscribe", { event = evt, filter =  {min = 30, max = 100}})
    local ret = os.execute("bash ".._AFT.bindingRootDir.."/var/replay_launcher.sh ".._AFT.bindingRootDir.."/var/testFilter01pass.canreplay");
    _AFT.assertIsTrue(ret)

    _AFT.assertEvtReceived(api .. "/" ..evt, 1000000)
end,
nil,
function()
    _AFT.callVerb(api, "unsubscribe", { event = evt, filter =  {min = 30, max = 100} })
end)

_AFT.describe("Filter_Test_01/Step_3", function()
    _AFT.enableEventHistory() -- You will need this to check for time intervals between two events

    _AFT.addEventToMonitor(api .. "/" ..evt, function(eventname, data, dataH)
        _AFT.assertEquals(dataH[1].name, evt)
        _AFT.assertIsTrue(dataH[1].value > 30 and dataH[1].value < 100 )
        _AFT.assertEquals(dataH[2].name, evt)
        _AFT.assertIsTrue(dataH[2].value > 30 and dataH[2].value < 100 )
        _AFT.assertIsTrue(dataH[2].timestamp - dataH[1].timestamp >= 1000000)
    end)

    _AFT.assertVerbStatusSuccess(api ,"subscribe", { event = evt, filter =  { frequency = 1 , min = 30, max = 100}})
    local ret = os.execute("bash ".._AFT.bindingRootDir.."/var/replay_launcher.sh ".._AFT.bindingRootDir.."/var/testFilter01pass.canreplay");
    _AFT.assertIsTrue(ret)

    -- NEED TO BE CHECK (FAILED if [api .."/"..evt]= 2)
    _AFT.assertEvtGrpReceived({[api .."/"..evt]= 1}, 3000000)
end,
nil,
function()
    _AFT.callVerb(api, "unsubscribe", { event = evt, filter =  { frequency = 1 , min = 30, max = 100}})
end)
