
/////////////////////////////////////////////////////////////////////////////////////
//							THIS IS AN EXAMPLE
/////////////////////////////////////////////////////////////////////////////////////

#include "configuration.hpp"
#include "can/can-decoder.hpp"

// Pre-defined OBD-II PIDs to query for if supported by the vehicle.
 std::vector<obd2_signal_t> OBD2_PIDS = {
	obd2_signal_t(0x04, "engine.load", 0, 100, POURCENT, 5, false),
	obd2_signal_t(0x05, "engine.coolant.temperature", -40, 215, DEGREES_CELSIUS, 1, false),
	obd2_signal_t(0x0a, "fuel.pressure", 0, 765, KPA, 1, false),
	obd2_signal_t(0x0b, "intake.manifold.pressure", 0, 255, KPA, 1, false),
	obd2_signal_t(0x0c, "engine.speed", 0, 16383, RPM, 5, false),
	obd2_signal_t(0x0d, "vehicle.speed", 0, 255, KM_H, 5, false),
	obd2_signal_t(0x0f, "intake.air.temperature", -40, 215, DEGREES_CELSIUS, 1, false),
	obd2_signal_t(0x10, "mass.airflow", 0, 655, GRAMS_SEC, 5, false),
	obd2_signal_t(0x11, "throttle.position", 0, 100, POURCENT, 5, false),
	obd2_signal_t(0x1f, "running.time", 0, 65535, SECONDS, 1, false),
	obd2_signal_t(0x2d, "EGR.error", -100, 99, POURCENT, 0, false),
	obd2_signal_t(0x2f, "fuel.level", 0, 100, POURCENT, 1, false),
	obd2_signal_t(0x33, "barometric.pressure", 0, 255, KPA, 1, false),
	obd2_signal_t(0x4c, "commanded.throttle.position", 0, 100, POURCENT, 1, false),
	obd2_signal_t(0x52, "ethanol.fuel.percentage", 0, 100, POURCENT, 1, false),
	obd2_signal_t(0x5a, "accelerator.pedal.position", 0, 100, POURCENT, 5, false),
	obd2_signal_t(0x5b, "hybrid.battery-pack.remaining.life", 0, 100, POURCENT, 5, false),
	obd2_signal_t(0x5c, "engine.oil.temperature",-40, 210, DEGREES_CELSIUS, 1, false),
	obd2_signal_t(0x63, "engine.torque", 0, 65535, NM, 1, false)
};

std::vector<obd2_signal_t> get_predefined_obd2()
{
	return OBD2_PIDS;
}

std::vector<can_message_set_t> CAN_MESSAGE_SET =
{
	can_message_set_t(0, "example_messages", 1, 2, 2, 0, 19),
};

std::vector<std::vector<can_message_definition_t>> CAN_MESSAGES_DEFINITION = {
	{
		can_message_definition_t(0, 0x620, can_message_format_t::STANDARD, frequency_clock_t(), false),
		can_message_definition_t(0, 0x442, can_message_format_t::STANDARD, frequency_clock_t(), false),
	},
};

/**
 * @brief Dumb SIGNALS array. It is composed by can_message_set_t
 * SIGNALS[MESSAGE_SET_ID][can_signal_t]
 */
std::vector<std::vector<can_signal_t>> SIGNALS = {
	{
		can_signal_t((CAN_MESSAGES_DEFINITION[MESSAGE_SET_ID][0]), "driver_door.open", 2, 4, 1.000000, 0.000000, 0.000000, 0.000000, frequency_clock_t(), false, true, std::vector<std::string>(), false, decoder_t::booleanDecoder, nullptr, false),
		can_signal_t((CAN_MESSAGES_DEFINITION[MESSAGE_SET_ID][1]), "driver_door.close", 0, 4, 1.000000, 0.000000, 0.000000, 0.000000, frequency_clock_t(), false, true, std::vector<std::string>(),false, decoder_t::booleanDecoder, nullptr, false)
	},
};

configuration_t::configuration_t()
	: can_message_set_{CAN_MESSAGE_SET}, can_message_definition_{CAN_MESSAGES_DEFINITION}, can_signals_{SIGNALS}, obd2_signals_{OBD2_PIDS}
{}