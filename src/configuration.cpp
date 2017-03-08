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

#include <configuration.hpp>

#define OBD2_FUNCTIONAL_BROADCAST_ID 0x7df

/*
* Pre-defined OBD-II PIDs to query for if supported by the vehicle.
*/
 std::vector<obd2_signals_t> OBD2_PIDS = {
	{ 0x04, "obd2.engine.load", 0, 100, POURCENT, 5, false},
	{ 0x05, "obd2.engine.coolant.temperature", -40, 215, DEGREES_CELSIUS, 1, false},
	{ 0x0a, "obd2.fuel.pressure", 0, 765, KPA, 1, false},
	{ 0x0b, "obd2.intake.manifold.pressure", 0, 255, KPA, 1, false},
	{ 0x0c, "obd2.engine.speed", 0, 16383, RPM, 5, false},
	{ 0x0d, "obd2.vehicle.speed", 0, 255, KM_H, 5, false},
	{ 0x0f, "obd2.intake.air.temperature", -40, 215, DEGREES_CELSIUS, 1, false},
	{ 0x10, "obd2.mass.airflow", 0, 655, GRAMS_SEC, 5, false},
	{ 0x11, "obd2.throttle.position", 0, 100, POURCENT, 5, false},
	{ 0x1f, "obd2.running.time", 0, 65535, SECONDS, 1, false},
	{ 0x2d, "obd2.EGR.error", -100, 99, POURCENT, 0, false},
	{ 0x2f, "obd2.fuel.level", 0, 100, POURCENT, 1, false},
	{ 0x33, "obd2.barometric.pressure", 0, 255, KPA, 1, false},
	{ 0x4c, "obd2.commanded.throttle.position", 0, 100, POURCENT, 1, false},
	{ 0x52, "obd2.ethanol.fuel.percentage", 0, 100, POURCENT, 1, false},
	{ 0x5a, "obd2.accelerator.pedal.position", 0, 100, POURCENT, 5, false},
	{ 0x5b, "obd2.hybrid.battery-pack.remaining.life", 0, 100, POURCENT, 5, false},
	{ 0x5c, "obd2.engine.oil.temperature",-40, 210, DEGREES_CELSIUS, 1, false},
	{ 0x63, "obd2.engine.torque", 0, 65535, NM, 1, false}
};

std::vector<can_message_set_t> CAN_MESSAGE_SET =
{
	{0, "example_messages", 1, 2, 2, 0, 19},
};

std::vector<std::vector<can_message_definition_t>> CAN_MESSAGES_DEFINTION = {
	{
		{0x620, CanMessageFormat::STANDARD, {10, 0, nullptr}, false, (uint8_t)NULL},
		{0x442, CanMessageFormat::STANDARD, {10, 0, nullptr}, false, (uint8_t)NULL},
	},
};

/**
 * @brief Dumb SIGNALS array. It is composed by CanMessageSet
 * SIGNALS[MESSAGE_SET_ID][CanSignal]
 */
std::vector<std::vector<can_signal_t>> SIGNALS = {
	{
		{&(CAN_MESSAGES_DEFINTION[MESSAGE_SET_ID][0]), "can.driver_door.open", 2, 4, 1.000000, 0.000000, 0.000000, 0.000000, {10, 0, nullptr}, false, true, nullptr, 0, false, decoder_t::booleanDecoder, nullptr, false, 0.0},
		{&(CAN_MESSAGES_DEFINTION[MESSAGE_SET_ID][1]), "can.driver_door.close", 0, 4, 1.000000, 0.000000, 0.000000, 0.000000, {10, 0, nullptr}, false, true, nullptr, 0, false, decoder_t::booleanDecoder, nullptr, false, 0.0}
	},
};

configuration_t::configuration_t()
	: obd2_signals_{OBD2_PIDS}, can_message_set_{CAN_MESSAGE_SET}, can_signals_{SIGNALS}, can_message_definition_{CAN_MESSAGES_DEFINTION}