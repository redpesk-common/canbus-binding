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

#include "obd2-signals.hpp"

#include "signals.hpp"

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
* Pre-defined OBD-II PIDs to query for if supported by the vehicle.
*/
 std::vector<Obd2Pid> OBD2_PIDS = {
	{ 0x04, "obd2.engine.load", 0, 100, POURCENT, 5,  false},
	{ 0x05, "obd2.engine.coolant.temperature", -40, 215, DEGREES_CELSIUS, 1,  false},
	{ 0x0a, "obd2.fuel.pressure", 0, 765, KPA, 1,  false},
	{ 0x0b, "obd2.intake.manifold.pressure", 0, 255, KPA, 1,  false},
	{ 0x0c, "obd2.engine.speed", 0, 16383, RPM, 5,  false},
	{ 0x0d, "obd2.vehicle.speed", 0, 255, KM_H, 5,  false},
	{ 0x0f, "obd2.intake.air.temperature", -40, 215, DEGREES_CELSIUS, 1,  false},
	{ 0x10, "obd2.mass.airflow", 0, 655, GRAMS_SEC, 5,  false},
	{ 0x11, "obd2.throttle.position", 0, 100, POURCENT, 5,  false},
	{ 0x1f, "obd2.running.time", 0, 65535, SECONDS, 1,  false},
	{ 0x2d, "obd2.EGR.error", -100, 99, POURCENT, 0,  false},
	{ 0x2f, "obd2.fuel.level", 0, 100, POURCENT, 1,  false},
	{ 0x33, "obd2.barometric.pressure", 0, 255, KPA, 1,  false},
	{ 0x4c, "obd2.commanded.throttle.position", 0, 100, POURCENT, 1,  false},
	{ 0x52, "obd2.ethanol.fuel.percentage", 0, 100, POURCENT, 1,  false},
	{ 0x5a, "obd2.accelerator.pedal.position", 0, 100, POURCENT, 5,  false},
	{ 0x5b, "obd2.hybrid.battery-pack.remaining.life", 0, 100, POURCENT, 5,  false},
	{ 0x5c, "obd2.engine.oil.temperature",-40, 210, DEGREES_CELSIUS, 1,  false},
	{ 0x63, "obd2.engine.torque", 0, 65535, NM, 1,  false}
};

/**
 * @fn std::vector<std::string> find_signals(const openxc_DynamicField &key)
 * @brief return signals name found searching through CAN_signals and OBD2 pid
 * 
 * @param[in] const openxc_DynamicField : can contain numeric or string value in order to search against 
 *   can signals or obd2 signals name.
 *
 * @return std::vector<std::string> Vector of signals name found. 
 */
void find_signals(const openxc_DynamicField &key, std::vector<Obd2Pid*>& found_signals)
{
	switch(key.type)
	{
		case openxc_DynamicField_Type::openxc_DynamicField_Type_STRING:
				lookup_signals_by_name(key.string_value, get_obd2_signals(), found_signals);
			break;
		case openxc_DynamicField_Type::openxc_DynamicField_Type_NUM:
				lookup_signals_by_id(key.numeric_value, get_obd2_signals(), found_signals);
			break;
		default:
			ERROR(binder_interface, "find_signals: wrong openxc_DynamicField specified. Use openxc_DynamicField_Type_NUM or openxc_DynamicField_Type_STRING type only.");
			break;
	}
	DEBUG(binder_interface, "Found %d signal(s)", (int)found_signals.size());
}

std::vector<Obd2Pid>& get_obd2_signals()
{
	return OBD2_PIDS;
}

uint32_t get_signal_id(const Obd2Pid& sig)
{
	return (uint32_t)sig.pid;
}

void shims_logger(const char* m, const struct afb_binding_interface *interface)
{
	DEBUG(interface, "%s", m);
}

void shims_timer()
{
}

/*
 * Will scan for supported Obd2 pids
 *
obd2_handler_t::obd2_handler_t()
{
	DiagnosticShims shims_ = diagnostic_init_shims(shims_logger, can_bus.send_can_message, NULL);

	int n_pids_, i_;

	n_pids_ = size(Obd2Pid);
	for(i_=0; i_<=n_pids_; i_++)
	{
	}
}

void obd2_handler_t::add_request(int pid)
{
	DiagnosticRequest request = {
	arbitration_id: OBD2_FUNCTIONAL_BROADCAST_ID,
	mode: 0x1, has_true, pid};
}

bool obd2_handler_t::is_obd2_request(DiagnosticRequest* request)
{
	return request->mode == 0x1 && request->has_pid && request->pid < 0xff;
}

bool obd2_handler_t::is_obd2_signal(const char *name)
{
	if(fnmatch("obd2.*", name, FNM_CASEFOLD) == 0)
		return true;
	return false;
}

bool obd2_handler_t::decode_obd2_response(DiagnosticResponse* responce)
{
	return diagnostic_decode_obd2_pid(response);
}*/