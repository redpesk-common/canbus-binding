#include "configuration.hpp"
#include "can/can-decoder.hpp"

configuration_t::configuration_t()
	: can_message_set_{{0, "example", 1, 2, 2, 1, 3}}
	, can_message_definition_
	{
		{
			can_message_definition_t(0, "vcan0", 0x620, can_message_format_t::STANDARD, frequency_clock_t(0.00000f), true),
			can_message_definition_t(0, "vcan0", 0x442, can_message_format_t::STANDARD, frequency_clock_t(0.00000f), true)
		}
	}
	, can_signals_
	{
		{
			{
				0,
				0,
				"doors.driver.open",
				0,
				4,
				0.00000f, 
				0, 
				0,
				0,
				frequency_clock_t(0.00000f),
				true,
				false,
				{
				},
				false,
				decoder_t::booleanDecoder,
				nullptr,
				false
			},
			{
				0,
				1,
				"doors.passenger.open",
				0,
				4,
				0.00000f, 
				0, 
				0,
				0,
				frequency_clock_t(0.00000f),
				true,
				false,
				{
				},
				false,
				decoder_t::booleanDecoder,
				nullptr,
				false
			}
		},
		{
		}
	}
	, diagnostic_messages_
	{
		std::vector<diagnostic_message_t>{
		diagnostic_message_t(0x04, "engine.load", 0, 100, POURCENT, 5, decoder_t::decode_obd2_response, nullptr, false),
		diagnostic_message_t(0x05, "engine.coolant.temperature", -40, 215, DEGREES_CELSIUS, 1,  decoder_t::decode_obd2_response, nullptr, false),
		diagnostic_message_t(0x0a, "fuel.pressure", 0, 765, KPA, 1, decoder_t::decode_obd2_response, nullptr, false),
		diagnostic_message_t(0x0b, "intake.manifold.pressure", 0, 255, KPA, 1,  decoder_t::decode_obd2_response, nullptr, false),
		diagnostic_message_t(0x0c, "engine.speed", 0, 16383, RPM, 5,  decoder_t::decode_obd2_response, nullptr, false),
		diagnostic_message_t(0x0d, "vehicle.speed", 0, 255, KM_H, 5,  decoder_t::decode_obd2_response, nullptr, false),
		diagnostic_message_t(0x0f, "intake.air.temperature", -40, 215, DEGREES_CELSIUS, 1,  decoder_t::decode_obd2_response, nullptr, false),
		diagnostic_message_t(0x10, "mass.airflow", 0, 655, GRAMS_SEC, 5,  decoder_t::decode_obd2_response, nullptr, false),
		diagnostic_message_t(0x11, "throttle.position", 0, 100, POURCENT, 5,  decoder_t::decode_obd2_response, nullptr, false),
		diagnostic_message_t(0x1f, "running.time", 0, 65535, SECONDS, 1,  decoder_t::decode_obd2_response, nullptr, false),
		diagnostic_message_t(0x2d, "EGR.error", -100, 99, POURCENT, 0,  decoder_t::decode_obd2_response, nullptr, false),
		diagnostic_message_t(0x2f, "fuel.level", 0, 100, POURCENT, 1, decoder_t::decode_obd2_response, nullptr, false),
		diagnostic_message_t(0x33, "barometric.pressure", 0, 255, KPA, 1, decoder_t::decode_obd2_response, nullptr, false),
		diagnostic_message_t(0x4c, "commanded.throttle.position", 0, 100, POURCENT, 1, decoder_t::decode_obd2_response, nullptr, false),
		diagnostic_message_t(0x52, "ethanol.fuel.percentage", 0, 100, POURCENT, 1, decoder_t::decode_obd2_response, nullptr, false),
		diagnostic_message_t(0x5a, "accelerator.pedal.position", 0, 100, POURCENT, 5, decoder_t::decode_obd2_response, nullptr, false),
		diagnostic_message_t(0x5b, "hybrid.battery-pack.remaining.life", 0, 100, POURCENT, 5, decoder_t::decode_obd2_response, nullptr, false),
		diagnostic_message_t(0x5c, "engine.oil.temperature",-40, 210, DEGREES_CELSIUS, 1, decoder_t::decode_obd2_response, nullptr, false),
		diagnostic_message_t(0x63, "engine.torque", 0, 65535, NM, 1, decoder_t::decode_obd2_response, nullptr, false)}
	}
{
}

const std::string configuration_t::get_diagnostic_bus() const
{
	return "vcan0";
}