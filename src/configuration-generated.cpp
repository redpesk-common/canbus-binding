#include "configuration.hpp"
#include "can/can-decoder.hpp"

configuration_t::configuration_t()
	: can_message_set_{{0, "example", 2, 2, 7, 1, 3}}
	, can_message_definition_
	{
		{
			can_message_definition_t(0, "can0", 0x128, can_message_format_t::STANDARD, frequency_clock_t(0.00000f), true),
			can_message_definition_t(0, "can0", 0x813, can_message_format_t::STANDARD, frequency_clock_t(0.00000f), true)
		}
	}
	, can_signals_
	{
		{
			{
				0,
				0,
				"engine_speed",
				12,
				8,
				1.00000f, 
				0, 
				0,
				0,
				frequency_clock_t(15.0000f),
				true,
				false,
				{
				},
				false,
				nullptr,
				nullptr,
				false
			},
			{
				0,
				0,
				"GearshiftPosition",
				41,
				3,
				1.00000f, 
				0, 
				0,
				0,
				frequency_clock_t(0.00000f),
				true,
				false,
				{
					{1, "FIRST"},
					{4, "FOURTH"},
					{6, "NEUTRAL"},
					{5, "REVERSE"},
					{2, "SECOND"},
					{3, "THIRD"}
				},
				false,
				nullptr,
				nullptr,
				false
			},
			{
				0,
				0,
				"SteeringWheelAngle",
				52,
				12,
				0.153920f, 
				0, 
				0,
				0,
				frequency_clock_t(0.00000f),
				true,
				false,
				{
				},
				false,
				nullptr,
				nullptr,
				false
			},
			{
				0,
				0,
				"steering_wheel_angle_error",
				44,
				12,
				1.00000f, 
				0, 
				0,
				0,
				frequency_clock_t(0.00000f),
				true,
				false,
				{
				},
				false,
				nullptr,
				nullptr,
				false
			},
			{
				0,
				0,
				"steering_angle_sign",
				52,
				12,
				1.00000f, 
				0, 
				0,
				0,
				frequency_clock_t(0.00000f),
				true,
				false,
				{
				},
				false,
				nullptr,
				nullptr,
				false
			}
		},
		{
			{
				0,
				1,
				"abc",
				52,
				12,
				0.153920f, 
				0, 
				0,
				0,
				frequency_clock_t(0.00000f),
				true,
				false,
				{
				},
				false,
				nullptr,
				nullptr,
				false
			},
			{
				0,
				1,
				"def",
				52,
				12,
				1.00000f, 
				0, 
				0,
				0,
				frequency_clock_t(0.00000f),
				true,
				false,
				{
				},
				false,
				decoder_t::ignoreDecoder,
				nullptr,
				false
			}
		}
	}
	//, obd2_signals_{/*...*/}
{
}

const std::string configuration_t::get_diagnostic_bus() const
{
	return "vcan0";
}
