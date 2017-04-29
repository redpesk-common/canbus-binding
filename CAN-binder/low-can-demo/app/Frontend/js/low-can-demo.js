// parse location to get security token
var urlParams={};
location.search.substr(1).split("&").forEach(function(item) {
	var k = item.split("=")[0];
	var v = decodeURIComponent(item.split("=")[1]); 
	if (k in urlParams) urlParams[k].push(v); else urlParams[k] = [v];
});

var afb = new AFB("api"/*root*/, urlParams.token[0]);
var ws;
var vspeed = 0, espeed = 0, torque = 0;
var R2D = 180.0 / Math.PI;
var D2R = Math.PI / 180.0;
var fuel;
var con,cons,consa = [ ];
var minspeed = 5;
var temp = 18;
var wdgTem, wdgVsp, wdgEsp, wdgTrq;
var wdgFue, wdgGpred, wdgGpblack;
var conscale = 40;
var condt = 60000;

/* gauges creation */
var gauges={};
function initGauges() {
	gauges.speed = new steelseries.Radial('speedGauge', {
		gaugeType: steelseries.GaugeType.TYPE4,
		frameDesign: steelseries.FrameDesign.BLACK_METAL,
		backgroundColor: steelseries.BackgroundColor.CARBON,
		size: 250,
		titleString: "Speed",
		unitString: "Km/h",
		lcdVisible: true,
		niceScale: true,
		maxValue: 200,
		maxMeasuredValue: 0,
		maxMeasuredValueVisible: true,
		thresholdVisible: false,
		ledVisible: false,
		pointerType: steelseries.PointerType.TYPE11,
		useOdometer: false,
		odometerParams: {
			digits: 6
		}
	});

	gauges.rpm = new steelseries.Radial('rpmGauge', {
		gaugeType: steelseries.GaugeType.TYPE4,
		frameDesign: steelseries.FrameDesign.BLACK_METAL,
		backgroundColor: steelseries.BackgroundColor.CARBON,
		size: 200,
		titleString: "RPM",
		unitString: "x1000",
		lcdVisible: false,
		niceScale: true,
		maxValue: 5,
		maxMeasuredValue: 0,
		maxMeasuredValueVisible: false,
		section: [
			steelseries.Section(4, 8, 'rgba(255, 0, 0, 0.7)')
		],
		area: [
			steelseries.Section(5, 8, 'rgba(255, 0, 0, 0.3)')
		],
		thresholdVisible: false,
		ledVisible: false,
		pointerType: steelseries.PointerType.TYPE11
	});

	gauges.maf = new steelseries.Radial('MAFGauge', {
		gaugeType: steelseries.GaugeType.TYPE4,
		frameDesign: steelseries.FrameDesign.BLACK_METAL,
		backgroundColor: steelseries.BackgroundColor.CARBON,
		size: 200,
		titleString: "Air flow Rate",
		unitString: "grams/sec",
		lcdVisible: true,
		lcdColor: steelseries.LcdColor.STANDARD,
		lcdDecimals: 1,
		niceScale: true,
		minValue: 0,
		maxValue: 655,
		minMeasuredValue: 0,
		maxMeasuredValue: conscale,
		maxMeasuredValueVisible: true,
		section: [
			steelseries.Section(0, 255, 'rgba(0, 255, 0, 0.5)'),
			steelseries.Section(256, 326, 'rgba(255, 255, 0, 0.5)'),
			steelseries.Section(327, 600, 'rgba(255, 128, 0, 0.5)'),
			steelseries.Section(601, 655, 'rgba(255, 0, 0, 0.5)')
		],
		useValueGradient: true,
		thresholdVisible: false,
		ledVisible: false,
		pointerType: steelseries.PointerType.TYPE11
	});

	gauges.iatemp = new steelseries.Radial('IATempGauge', {
		gaugeType: steelseries.GaugeType.TYPE4,
		frameDesign: steelseries.FrameDesign.BLACK_METAL,
		backgroundColor: steelseries.BackgroundColor.CARBON,
		size: 200,
		titleString: "Intake air temp",
		unitString: "°C",
		lcdVisible: true,
		lcdColor: steelseries.LcdColor.STANDARD,
		lcdDecimals: 1,
		niceScale: true,
		minValue: 0,
		maxValue: 100,
		minMeasuredValue: 0,
		maxMeasuredValue: 100,
		maxMeasuredValueVisible: true,
		section: [
			steelseries.Section(0, 30, 'rgba(0, 255, 0, 0.5)'),
			steelseries.Section(31, 50, 'rgba(255, 255, 0, 0.5)'),
			steelseries.Section(51, 70, 'rgba(255, 128, 0, 0.5)'),
			steelseries.Section(71, 100, 'rgba(255, 0, 0, 0.5)')
		],
		useValueGradient: true,
		thresholdVisible: false,
		ledVisible: false,
		pointerType: steelseries.PointerType.TYPE11
	});
	
	gauges.torque = new steelseries.Radial('torqueGauge', {
		gaugeType: steelseries.GaugeType.TYPE2,
		frameDesign: steelseries.FrameDesign.BLACK_METAL,
		backgroundColor: steelseries.BackgroundColor.CARBON,
		size: 200,
		titleString: "Load",
		unitString: "%",
		lcdVisible: false,
		niceScale: true,
		minValue: 0,
		maxValue: 100,
		maxMeasuredValue: 0,
		maxMeasuredValueVisible: false,
		section: [
			steelseries.Section(0, 0, 'rgba(0, 255, 0, 0.7)'),
			steelseries.Section(50, 1500, 'rgba(255, 128, 0, 0.7)')
		],
		area: [
			steelseries.Section(0, 0, 'rgba(0, 255, 0, 0.3)'),
			steelseries.Section(50, 1500, 'rgba(255, 128, 0, 0.3)')
		],
		threshold: 0,
		thresholdVisible: true,
		ledVisible: false,
		pointerType: steelseries.PointerType.TYPE4
	});

	/* adjust cluster background size upon resize */
	// TODO: could be doable through CSS, but a bit tricky
	function adjustCluster() {
		var qh=$("#quad1").outerHeight();
		var sh=$("#speedGauge").outerHeight();
		var pct=Math.ceil((1000*sh/qh))/10+1;
		$('#cluster').css("height",pct+"%");
	}
	$(window).resize(adjustCluster);
	adjustCluster();
}

function clearGauges() {
	for (var g in gauges) {
		switch(g) {
			case "clock":
				gauges[g].setValue("-");
				break;
			case "speed":
				gauges[g].setValue(0);
				break;
			default:
				gauges[g].setValue(0);
				break;
		}
	}
}

function gotVehicleSpeed(obj) {
	vspeed = Math.round(obj.data.value);
	wdgVsp.innerHTML = /* wdgVspeed.innerHTML = */ String(vspeed);
	//gauges.speed.setValueAnimated(vspeed);
	gauges.speed.setValue(vspeed);
}

function gotTorque(obj) {
	torque=Math.round(obj.data.value);
	wdgTrq.innerHTML=String(torque);
	gauges.torque.setValue(torque);
}

function gotEngineSpeed(obj) {
	espeed = Math.round(obj.data.value);
	wdgEsp.innerHTML = /* wdgEspeed.innerHTML = */ String(espeed);
	//gauges.rpm.setValueAnimated(espeed/1000);
	gauges.rpm.setValue(espeed/1000);
}

function gotFuelLevel(obj) {
	fuel = Math.round(obj.data.value);
	wdgFue.innerHTML = fuel;
	gauges.maf.setValue(fuel);
}

function gotTemp(obj) {
	temp = Math.round(obj.data.value);
	wdgTem.innerHTML = temp;
	gauges.iatemp.setValue(temp);
}

function gotStart(obj) {
	document.body.className = "started";
	vspeed = 0;
	espeed = 0;
	heading = 0;
	cons = undefined;
	consa = [ ];

	wdgVsp.innerHTML = /*wdgVspeed.innerHTML = */
	wdgEsp.innerHTML = /*wdgEspeed.innerHTML = */
	wdgTem.innerHTML = wdgFue.innerHTML = "?";
	for (var i = 0 ; i < 9 ; i++) {
		wdgConX[i].style.height = "0%";
		wdgConX[i].innerHTML = "";
	}
}

function gotStop(obj) {
	document.body.className = "connected";
}

var msgcnt=0;
var msgprv=0;
var msgprvts=0;

function gotAny(obj) { 
	if (obj.event != "low-can/STOP") {
		document.body.className = "started";
	}
	msgcnt++;

	wdgTem.innerHTML = temp;
	gauges.iatemp.setValue(temp);
//	updateClock(obj.data.timestamp);
}

function updateMsgRate() {
	var now=+new Date();
	if (msgprvts) {
		var dt=now-msgprvts;
		msgrate=Math.round((msgcnt-msgprv)*10000/dt)/10;
		wdgMsg.innerHTML=String(msgrate);
	}

	msgprv=msgcnt;
	msgprvts=now;
}

function gotStat(obj) {
	wdgStat.innerHTML = obj.data;
}

function onAbort() {
	document.body.className = "not-connected";
}

function onOpen() {
	ws.call("low-can/subscribe", {event:[
			"diagnostic_messages.vehicle.speed",
			"diagnostic_messages.mass.airflow",
			"diagnostic_messages.engine.speed",
			"diagnostic_messages.engine.load",
			"diagnostic_messages.intake.air.temperature"]},
			onSubscribed, onAbort);
	ws.call("stat/subscribe", true);
	ws.onevent("stat/stat", gotStat);
}

function onClose() {
	ws.call("low-can/unsubscribe", {event:[
			"diagnostic_messages.engine.speed",
			"diagnostic_messages.mass.airflow",
			"diagnostic_messages.vehicle.speed",
			"diagnostic_messages.engine.load",
			"diagnostic_messages.intake.air.temperature"]},
			onUnsubscribed, onAbort);
	ws.call("stat/unsubscribe", true);
	ws.onevent("stat/stat", gotStat);
}

function onSubscribed() {
	document.body.className = "connected";
	ws.onevent("low-can/diagnostic_messages.engine.speed", gotEngineSpeed);
	ws.onevent("low-can/diagnostic_messages.mass.airflow", gotFuelLevel);
	ws.onevent("low-can/diagnostic_messages.vehicle.speed", gotVehicleSpeed);
	ws.onevent("low-can/diagnostic_messages.engine.load", gotTorque);
	ws.onevent("low-can/diagnostic_messages.intake.air.temperature", gotTemp);
	ws.onevent("low-can",gotAny);
}

function onUnsubscribed() {
	document.body.className = "disconnected";
	ws.onevent("low-can/diagnostic_messages.engine.speed", gotEngineSpeed);
	ws.onevent("low-can/diagnostic_messages.mass.airflow", gotFuelLevel);
	ws.onevent("low-can/diagnostic_messages.vehicle.speed", gotVehicleSpeed);
	ws.onevent("low-can/diagnostic_messages.engine.load", gotTorque);
	ws.onevent("low-can/diagnostic_messages.intake.air.temperature", gotTemp);
	ws.onevent("low-can",gotAny);
}

function replyok(obj) {
	document.getElementById("output").innerHTML = "OK: "+JSON.stringify(obj);
}
function replyerr(obj) {
	document.getElementById("output").innerHTML = "ERROR: "+JSON.stringify(obj);
}
function send(message) {
	var api = document.getElementById("api").value;
	var verb = document.getElementById("verb").value;
	ws.call(api+"/"+verb, {data:message}, replyok, replyerr);
}

function doConnect() {
	document.body.className = "connecting";
	ws = new afb.ws(onOpen, onAbort);
}

function doDisconnect() {
	document.body.className = "connecting";
	ws = new afb.ws(onClose, onAbort);
}

$(function() {
	wdgVsp = document.getElementById("vsp");
	wdgEsp = document.getElementById("esp");
	wdgTrq = document.getElementById("trq");
	wdgFue = document.getElementById("fue");
	wdgTem = document.getElementById("tem");
	wdgStat = document.getElementById("stat");
	wdgMsg = document.getElementById("msg");

	initGauges();

	doConnect();

	// init interval to compute message rate
	setInterval(updateMsgRate,250);
});

