
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

print("\n##########################################")
print("#########    BEGIN BASIC TESTS   #########")
print("##########################################\n")

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

_AFT.testVerbStatusError("low-can_write_wo_auth", "low-can", "write", { signal_name = "engine.speed", signal_value = 1234})
_AFT.testVerbStatusSuccess("low-can_auth", "low-can", "auth", {})
_AFT.testVerbStatusSuccess("low-can_write", "low-can", "write", { signal_name = "hvac.temperature.left", signal_value = 21})
_AFT.testVerbStatusSuccess("low-can_write_raw", "low-can", "write", { bus_name= "hs", frame= { can_id= 1568, can_dlc=8, can_data= {255, 255, 255, 255, 255, 255, 255, 255}}})

_AFT.testVerbStatusError("low-can_write_unwritable_signal", "low-can", "write", { signal_name = "vehicle.average.speed", signal_value = 1234})
_AFT.testVerbStatusError("low-can_write_wrong_name_key", "low-can", "write", { name = "hvac.temperature.left", signal_value = 21})
_AFT.testVerbStatusError("low-can_write_wrong_value_key", "low-can", "write", { signal_name = "hvac.temperature.left", value = 21})
_AFT.testVerbStatusError("low-can_write_raw_invalid_bus_key", "low-can", "write", { bus= "hs", frame= { can_id= 1568, can_dlc=8, can_data= {255, 255, 255, 255, 255, 255, 255, 255}}})
_AFT.testVerbStatusError("low-can_write_raw_invalid_frame_key", "low-can", "write", { bus_name= "hs", fram= { can_id= 1568, can_dlc=8, can_data= {255, 255, 255, 255, 255, 255, 255, 255}}})
_AFT.testVerbStatusError("low-can_write_raw_invalid_can_id_key", "low-can", "write", { bus_name= "hs", frame= { id= 1568, can_dlc=8, can_data= {255, 255, 255, 255, 255, 255, 255, 255}}})
_AFT.testVerbStatusError("low-can_write_raw_invalid_can_id_args", "low-can", "write", { bus_name= "hs", frame= { can_id= "1568", can_dlc=8, can_data= {255, 255, 255, 255, 255, 255, 255, 255}}})
_AFT.testVerbStatusError("low-can_write_raw_invalid_can_dlc_key", "low-can", "write", { bus_name= "hs", frame= { can_id= 1568, dlc=8, can_data= {255, 255, 255, 255, 255, 255, 255, 255}}})
_AFT.testVerbStatusError("low-can_write_raw_invalid_can_dlc_args", "low-can", "write", { bus_name= "hs", frame= { can_id= 1568, dlc="8", can_data= {255, 255, 255, 255, 255, 255, 255, 255}}})
_AFT.testVerbStatusError("low-can_write_raw_invalid_can_data_key", "low-can", "write", { bus= "hs", frame= { can_id= 1568, can_dlc=8, data= {255, 255, 255, 255, 255, 255, 255, 255}}})
_AFT.testVerbStatusError("low-can_write_raw_invalid_can_data_key", "low-can", "write", { bus= "hs", frame= { can_id= 1568, can_dlc=8, can_data= {"255", 255, 255, 255, 255, 255, 255, 255}}})

_AFT.testVerbStatusSuccess("low-can_list", "low-can", "list", {})
_AFT.testVerbStatusSuccess("low-can_get", "low-can", "get", { event = "engine.speed"})

_AFT.describe("Diagnostic_engine_speed_simulation", function()

    local api = "low-can"
    local evt = "diagnostic_messages.engine.speed"
    _AFT.assertVerbStatusSuccess(api,"subscribe", { event = evt })

    _AFT.addEventToMonitor(api .."/diagnostic_messages", function(eventName, data)
        _AFT.assertIsTrue(data.name == evt)
    end)

    local ret = os.execute("bash ".._AFT.bindingRootDir.."/var/replay_launcher.sh ".._AFT.bindingRootDir.."/var/test1.canreplay")
    _AFT.assertIsTrue(ret)

    _AFT.assertEvtReceived(api .."/diagnostic_messages", 1000000)

    _AFT.assertVerbStatusSuccess("low-can","unsubscribe", { event = evt })
end)

_AFT.describe("Subscribe_all", function()
    local api = "low-can"
    local evt = "messages.vehicle.average.speed"

    _AFT.addEventToMonitor(api .."/".. evt,  function(eventName, data)
        _AFT.assertEquals(eventName, api .."/".. evt)
    end)

    _AFT.assertVerbStatusSuccess(api,"subscribe", { event = "*" })

    local ret = os.execute("bash ".._AFT.bindingRootDir.."/var/replay_launcher.sh ".._AFT.bindingRootDir.."/var/test2-3.canreplay")
    _AFT.assertIsTrue(ret)

    _AFT.assertEvtReceived(api .."/".. evt, 5000000);

    _AFT.assertVerbStatusSuccess(api,"unsubscribe", { event = "*" })

end)

_AFT.exitAtEnd()
