 --[[
    Copyright (C) 2018 "IoT.bzh"
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

_AFT.setAfterEach( function()
    os.execute("pkill canplayer")
    os.execute("pkill linuxcan-canpla")
end)

_AFT.describe("Filter_Test_01/Step_1", function()
    local api = "low-can"
    local evt = "engine.speed"
    local min = 30;
    local max = 100;

    _AFT.addEventToMonitor(api .. "/" ..evt, function(eventname,data)
        _AFT.assertEquals(data.name,evt)
    end)
    _AFT.assertVerbStatusSuccess(api ,"subscribe", { event = evt, filter =  { min = 30, max = 100}})
    os.execute("./var/replay_launcher.sh ./var/testFilter01filteredOut.canreplay");
    _AFT.assertEvtNotReceived(api .. "/" ..evt, 1000000)
end)

_AFT.describe("Filter_Test_01/Step_2", function()
    local api = "low-can"
    local evt = "messages.engine.speed"
    local min = 30;
    local max = 100;

    _AFT.addEventToMonitor(api .. "/" ..evt, function(eventname,data)
        print(data.name .."-vs-" .. evt)
        _AFT.assertEquals(data.name,evt)
        _AFT.assertIsTrue(data.value > min and data.value < max )
    end)
    _AFT.assertVerbStatusSuccess(api ,"subscribe", { event = evt, filter =  { min = 30, max = 100}})
    os.execute("./var/replay_launcher.sh ./var/testFilter01pass.canreplay");

    _AFT.assertEvtReceived(api .. "/" ..evt, 1000000)
    _AFT.assertVerbStatusSuccess(api,"unsubscribe", { event = evt })
end)