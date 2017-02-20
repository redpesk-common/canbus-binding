/*
 * Copyright (C) 2015, 2016 "IoT.bzh"
 * Author "Romain Forlot" <romain.forlot@iot.bzh>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *	 http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <vector>
#include "uds/uds.h"
extern "C"
{
	#include <afb/afb-binding.h>
}

enum UNIT {
	POURCENT,
	DEGREES_CELSIUS,
	KPA,
	RPM,
	GRAMS_SEC,
	SECONDS,
	KM,
	KM_H,
	PA,
	NM
};

const char *UNIT_NAMES[10] = {
	"POURCENT",
	"DEGREES_CELSIUS",
	"KPA",
	"RPM",
	"GRAMS_SEC",
	"SECONDS",
	"KM",
	"KM_H",
	"PA",
	"NM"
};

/*
 *	A representation of an OBD-II PID.
 *
 * pid - The 1 byte PID.
 * name - A human readable name to use for this PID when published.
 * min - minimum value for this pid
 * max - maximum value for this pid
 * unit - unit used
 * frequency - The frequency to request this PID if supported by the vehicle
 *		when automatic, recurring OBD-II requests are enabled.
 *	supported - is it supported by the vehicle. Initialized after scan
 * event - application framework event handler.
 */
typedef struct _Obd2Pid {
	uint8_t pid;
	const char* name;
	const int min;
	const int max;
	enum UNIT unit;
	int frequency;
	bool supported;
	struct afb_event event;
} Obd2Pid;

/*
	* Pre-defined OBD-II PIDs to query for if supported by the vehicle.
	*/
const std::vector<Obd2Pid> OBD2_PIDS {
	{ pid: 0x04, name: "obd2.engine.load", min:0, max: 100, unit: POURCENT, frequency: 5, supported: false, event: {nullptr, nullptr} },
	{ pid: 0x05, name: "obd2.engine.coolant.temperature", min: -40, max: 215, unit: DEGREES_CELSIUS, frequency: 1, supported: false, event: {nullptr, nullptr} },
	{ pid: 0x0a, name: "obd2.fuel.pressure", min: 0, max: 765, unit: KPA, frequency: 1, supported: false, event: {nullptr, nullptr} },
	{ pid: 0x0b, name: "obd2.intake.manifold.pressure", min: 0, max: 255, unit: KPA, frequency: 1, supported: false, event: {nullptr, nullptr} },
	{ pid: 0x0c, name: "obd2.engine.speed", min: 0, max: 16383, unit: RPM, frequency: 5, supported: false, event: {nullptr, nullptr} },
	{ pid: 0x0d, name: "obd2.vehicle.speed", min: 0, max: 255, unit: KM_H, frequency: 5, supported: false, event: {nullptr, nullptr} },
	{ pid: 0x0f, name: "obd2.intake.air.temperature", min: -40, max:215, unit: DEGREES_CELSIUS, frequency: 1, supported: false, event: {nullptr, nullptr} },
	{ pid: 0x10, name: "obd2.mass.airflow", min: 0, max: 655, unit: GRAMS_SEC, frequency: 5, supported: false, event: {nullptr, nullptr} },
	{ pid: 0x11, name: "obd2.throttle.position", min: 0, max: 100, unit: POURCENT, frequency: 5, supported: false, event: {nullptr, nullptr} },
	{ pid: 0x1f, name: "obd2.running.time", min: 0, max: 65535, unit: SECONDS, frequency: 1, supported: false, event: {nullptr, nullptr} },
	{ pid: 0x2d, name: "obd2.EGR.error", min: -100, max: 99, unit: POURCENT, frequency: 0, supported: false, event: {nullptr, nullptr} },
	{ pid: 0x2f, name: "obd2.fuel.level", min: 0, max: 100, unit: POURCENT, frequency: 1, supported: false, event: {nullptr, nullptr} },
	{ pid: 0x33, name: "obd2.barometric.pressure", min: 0, max: 255, unit: KPA, frequency: 1, supported: false, event: {nullptr, nullptr} },
	{ pid: 0x4c, name: "obd2.commanded.throttle.position", min: 0, max: 100, unit: POURCENT, frequency: 1, supported: false, event: {nullptr, nullptr} },
	{ pid: 0x52, name: "obd2.ethanol.fuel.percentage", min: 0, max: 100, unit: POURCENT, frequency: 1, supported: false, event: {nullptr, nullptr} },
	{ pid: 0x5a, name: "obd2.accelerator.pedal.position", min: 0, max: 100, unit: POURCENT, frequency: 5, supported: false, event: {nullptr, nullptr} },
	{ pid: 0x5b, name: "obd2.hybrid.battery-pack.remaining.life", min: 0, max: 100, unit: POURCENT, frequency: 5, supported: false, event: {nullptr, nullptr} },
	{ pid: 0x5c, name: "obd2.engine.oil.temperature",min: -40, max: 210, unit: DEGREES_CELSIUS, frequency: 1, supported: false, event: {nullptr, nullptr} },
	{ pid: 0x63, name: "obd2.engine.torque", min: 0, max: 65535, unit: NM, frequency: 1, supported: false, event: {nullptr, nullptr} }
};

/* Public: Check if a request is an OBD-II PID request.
 *
 * Returns true if the request is a mode 1	request and it has a 1 byte PID.
 */
bool isObd2Request(DiagnosticRequest* request);

/* Public: Decode the payload of an OBD-II PID.
 *
 * This function matches the type signature for a DiagnosticResponseDecoder, so
 * it can be used as the decoder for a DiagnosticRequest. It returns the decoded
 * value of the PID, using the standard formulas (see
 * http://en.wikipedia.org/wiki/OBD-II_PIDs#Mode_01).
 */
float handleObd2Pid(const DiagnosticResponse* response, float parsedPayload);

/**
 * @brief - Object to handle obd2 session with pre-scan of supported pid
 * then request them regularly
 */
class obd2_handler_t {
	private:

	public:
		/**
		 * @brief:
		 *
		 * Returns
		 */
		void find_obd2_pid(const char *name, std::vector<Obd2Pid> *pids);

		/**
		 * @brief Check if a request is an OBD-II PID request.
		 *
		 * @return true if the request is a mode 1  request and it has a 1 byte PID.
		 */
		bool is_obd2_request(DiagnosticRequest *request);

		/**
		* @brief Check if requested signal name is an obd2 pid
		* 
		* @return true if name began with ob2.* else false.
		*/
		bool is_obd2_signal(const char *name);

		/*
		* @brief pass response to UDS-C library function 
		* diagnostic_decode_obd2_pid()
		*
		* @return float number representing the requested value.
		*/
		bool decode_obd2_response(DiagnosticResponse* responce);
};