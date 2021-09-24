/*******************************************************************************
 * basic test set
 ******************************************************************************/
exec = require("system");

ws = new AFBWSAPI("unix:/tmp/canbus");

wantedData = "";
expevents = 0;
call_success = ws.call_success.bind(ws)
call_error   = ws.call_error.bind(ws)

ws.onEventPush = function (id, obj) {
        if(obj["timestamp"])
                delete obj["timestamp"]
        if(JSON.stringify(obj) !== JSON.stringify(wantedData)) {
                print("Data received not expected. Wanted: "+ JSON.stringify(wantedData) +", received: "+ JSON.stringify(obj) +"\n");
                this.eventUnexpected_(id);
                return;
        }
        if (expevents)
                expevents--;
        if (this.unexpected) {
                print("unexpected event " + id + "\n");
                this.eventUnexpected_(id);
        }
};

function aCallTest(name, fun, verb, params) {
        diag.setTestName(name);
        fun(verb, params);
        wait_completion();
}

function waitAnEvent(cmd) {
        expevents++;
        exec.system(cmd);
        wait_completion();
}

//////////////////////// List/Get tests ////////////////////////////////////////

aCallTest("List available signals: ", call_success, "list", null)
aCallTest("Get never received signal value: ", call_error, "VirtualCar/hvac.temperature.average", "get");

ws.sessionCreate(1, "session");
ws.sessionId = 1;
ws.tokenCreate(1, "token");
ws.tokenId = 1;

aCallTest("Old Subscribe to signal: ", call_success, "VirtualCar/hvac.temperature.average", "subscribe");
wantedData={"VirtualCar/hvac.fan.speed":0,"VirtualCar/hvac.temperature.average":0,"VirtualCar/hvac.temperature.left":0,"VirtualCar/hvac.temperature.right":0,"id":48}
waitAnEvent("cansend can0 030#0000000000000000")

aCallTest("Get received signal value :", call_success, "VirtualCar/hvac.temperature.average", "get");
aCallTest("Old Unsubscribe previously subcribed signal: ", call_success, "VirtualCar/hvac.temperature.average", "unsubscribe");

///////////////// Make sure event isn't received after subscibe ////////////////
var ue = 0;
ws.onEventUnexpected = function() { ue++; }
exec.system("cansend can0 030#1111111111111111");
exec.system("sleep 1");
wait_completion();
diagnose(ue == 0, "Unexpected event not received");

///////////////////// Subscribe/Unsubscibe tests ///////////////////////////////

aCallTest("Subscribe to an inexistent signal: ", call_error, "WrongSet/WrongSignal", "subscribe");
aCallTest("Old Subscribe to an inexistent signal: ", call_error, "subscribe", {event: "inexistent signal"});
aCallTest("Wrong subscribe key given: ", call_error, "subscribe", {signal: "hvac.temperature.average"});

aCallTest("Subscribe to an CAN id: ", call_success, "subscribe", {id: 0x30});
wantedData = {"VirtualCar/":[{"hvac.fan.speed":799.9995727539063},{"hvac.fan.speed":799.9995727539063,"hvac.temperature.left":34},{"hvac.fan.speed":799.9995727539063,"hvac.temperature.left":34,"hvac.temperature.right":34},{"hvac.fan.speed":799.9995727539063,"hvac.temperature.average":34,"hvac.temperature.left":34,"hvac.temperature.right":34}],"VirtualCar/hvac.fan.speed":1199.9993896484375,"VirtualCar/hvac.temperature.average":0,"VirtualCar/hvac.temperature.left":51,"VirtualCar/hvac.temperature.right":51,"id":48}
waitAnEvent("cansend can0 030#2222222222222222")
aCallTest("Unsubscribe previously subcribed CAN id: ", call_success, "unsubscribe", {id: 0x30});

aCallTest("Subscribe all: ", call_success, "subscribe", null);
wantedData = {"VirtualCar/":[{"hvac.fan.speed":1199.9993896484375},{"hvac.fan.speed":1199.9993896484375,"hvac.temperature.left":51},{"hvac.fan.speed":1199.9993896484375,"hvac.temperature.left":51,"hvac.temperature.right":51},{"hvac.fan.speed":1199.9993896484375,"hvac.temperature.average":51,"hvac.temperature.left":51,"hvac.temperature.right":51}],"VirtualCar/hvac.fan.speed":1199.9993896484375,"VirtualCar/hvac.temperature.average":51,"VirtualCar/hvac.temperature.left":51,"VirtualCar/hvac.temperature.right":51,"id":48}
waitAnEvent("cansend can0 030#3333333333333333")
aCallTest("Unsubscribe all: ", call_success, "unsubscribe", null);

//////////////////////////// Write Tests ///////////////////////////////////////
aCallTest("Write nothing: ", call_error, "write", null);
aCallTest("Forbiden signal write: ", call_error, "write", {signal_name: "Eng.Momentary.Overspeed.Enable", signal_value: 1});
aCallTest("Authenticate: ", call_success, "auth", null);

aCallTest("Write a signal value: ", call_success, "write", {signal_name : "Eng.Momentary.Overspeed.Enable", signal_value : 1});
aCallTest("Write a PGN frame: ", call_success, "write", {bus_name: "can0", frame: { pgn: 62420, length:8, data: [1, 2, 3, 4, 5, 6, 7, 8]}});
aCallTest("Write an inexistent PGN frame:", call_error, "write", {bus_name: "can0", frame: { pgn: 1234, length:8, data: [9, 10, 11, 12, 13, 14, 15, 16]}});
aCallTest("Write J1939 multiframe: ", call_success, "write", {bus_name: "can0", frame: { pgn: 126208, length:9, data: [9, 10, 11, 12, 13, 14, 15, 16, 17]}});

ws.sessionRemove(1);
ws.tokenRemove(1);
delete ws.sessionId;
delete ws.tokenId;

terminate();

ws.disconnect();
