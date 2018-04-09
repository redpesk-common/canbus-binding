#include "application.hpp"
#include "../can/can-decoder.hpp"

application_t::application_t()
	: can_bus_manager_{utils::config_parser_t{"/etc/dev-mapping.conf"}}
	, can_message_set_{
		{std::make_shared<can_message_set_t>(can_message_set_t{0,"example",
			{ // beginning can_message_definition_ vector
				{std::make_shared<can_message_definition_t>(can_message_definition_t{"hs",0x3D9,can_message_format_t::STANDARD,frequency_clock_t(5.00000f),true,
					{ // beginning can_signals vector
						{std::make_shared<can_signal_t> (can_signal_t{
							"engine.speed",
							16,
							16,
							0.250000f,
							0.00000,
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
						})},
						{std::make_shared<can_signal_t> (can_signal_t{
							"fuel.level.low",
							55,
							1,
							1.00000f,
							0.00000,
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
						})},
						{std::make_shared<can_signal_t> (can_signal_t{
							"fuel.level",
							8,
							8,
							0.392157f,
							0.00000,
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
						})}
					} // end can_signals vector
				})} // end can_message_definition entry
,				{std::make_shared<can_message_definition_t>(can_message_definition_t{"hs",0x3E9,can_message_format_t::STANDARD,frequency_clock_t(5.00000f),true,
					{ // beginning can_signals vector
						{std::make_shared<can_signal_t> (can_signal_t{
							"vehicle.average.speed",
							0,
							15,
							0.0156250f,
							0.00000,
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
						})}
					} // end can_signals vector
				})} // end can_message_definition entry
,				{std::make_shared<can_message_definition_t>(can_message_definition_t{"hs",0x4D1,can_message_format_t::STANDARD,frequency_clock_t(5.00000f),true,
					{ // beginning can_signals vector
						{std::make_shared<can_signal_t> (can_signal_t{
							"engine.oil.temp",
							16,
							8,
							1.00000f,
							0.00000,
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
						})},
						{std::make_shared<can_signal_t> (can_signal_t{
							"engine.oil.temp.high",
							7,
							1,
							1.00000f,
							0.00000,
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
						})}
					} // end can_signals vector
				})} // end can_message_definition entry
,				{std::make_shared<can_message_definition_t>(can_message_definition_t{"hs",0x620,can_message_format_t::STANDARD,frequency_clock_t(5.00000f),true,
					{ // beginning can_signals vector
						{std::make_shared<can_signal_t> (can_signal_t{
							"doors.boot.open",
							47,
							1,
							1.00000f,
							0.00000,
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
						})},
						{std::make_shared<can_signal_t> (can_signal_t{
							"doors.front_left.open",
							43,
							1,
							1.00000f,
							0.00000,
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
						})},
						{std::make_shared<can_signal_t> (can_signal_t{
							"doors.front_right.open",
							44,
							1,
							1.00000f,
							0.00000,
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
						})},
						{std::make_shared<can_signal_t> (can_signal_t{
							"doors.rear_left.open",
							46,
							1,
							1.00000f,
							0.00000,
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
						})},
						{std::make_shared<can_signal_t> (can_signal_t{
							"doors.rear_right.open",
							45,
							4,
							1.00000f,
							0.00000,
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
						})}
					} // end can_signals vector
				})} // end can_message_definition entry
,				{std::make_shared<can_message_definition_t>(can_message_definition_t{"hs",0x799,can_message_format_t::STANDARD,frequency_clock_t(5.00000f),true,
					{ // beginning can_signals vector
						{std::make_shared<can_signal_t> (can_signal_t{
							"windows.front_left.open",
							43,
							1,
							1.00000f,
							0.00000,
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
						})},
						{std::make_shared<can_signal_t> (can_signal_t{
							"windows.front_right.open",
							44,
							1,
							1.00000f,
							0.00000,
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
						})},
						{std::make_shared<can_signal_t> (can_signal_t{
							"windows.rear_left.open",
							46,
							1,
							1.00000f,
							0.00000,
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
						})},
						{std::make_shared<can_signal_t> (can_signal_t{
							"windows.rear_right.open",
							45,
							4,
							1.00000f,
							0.00000,
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
						})}
					} // end can_signals vector
				})} // end can_message_definition entry

		}, // end can_message_definition vector
			{ // beginning diagnostic_messages_ vector
				{std::make_shared<diagnostic_message_t>(diagnostic_message_t{
					4,
					"engine.load",
					0,
					0,
					UNIT::INVALID,
					5.00000f,
					decoder_t::decode_obd2_response,
					nullptr,
					true,
					false
				})}
,				{std::make_shared<diagnostic_message_t>(diagnostic_message_t{
					5,
					"engine.coolant.temperature",
					0,
					0,
					UNIT::INVALID,
					1.00000f,
					decoder_t::decode_obd2_response,
					nullptr,
					true,
					false
				})}
,				{std::make_shared<diagnostic_message_t>(diagnostic_message_t{
					10,
					"fuel.pressure",
					0,
					0,
					UNIT::INVALID,
					1.00000f,
					decoder_t::decode_obd2_response,
					nullptr,
					true,
					false
				})}
,				{std::make_shared<diagnostic_message_t>(diagnostic_message_t{
					11,
					"intake.manifold.pressure",
					0,
					0,
					UNIT::INVALID,
					1.00000f,
					decoder_t::decode_obd2_response,
					nullptr,
					true,
					false
				})}
,				{std::make_shared<diagnostic_message_t>(diagnostic_message_t{
					12,
					"engine.speed",
					0,
					0,
					UNIT::INVALID,
					5.00000f,
					decoder_t::decode_obd2_response,
					nullptr,
					true,
					false
				})}
,				{std::make_shared<diagnostic_message_t>(diagnostic_message_t{
					13,
					"vehicle.speed",
					0,
					0,
					UNIT::INVALID,
					5.00000f,
					decoder_t::decode_obd2_response,
					nullptr,
					true,
					false
				})}
,				{std::make_shared<diagnostic_message_t>(diagnostic_message_t{
					15,
					"intake.air.temperature",
					0,
					0,
					UNIT::INVALID,
					1.00000f,
					decoder_t::decode_obd2_response,
					nullptr,
					true,
					false
				})}
,				{std::make_shared<diagnostic_message_t>(diagnostic_message_t{
					16,
					"mass.airflow",
					0,
					0,
					UNIT::INVALID,
					5.00000f,
					decoder_t::decode_obd2_response,
					nullptr,
					true,
					false
				})}
,				{std::make_shared<diagnostic_message_t>(diagnostic_message_t{
					17,
					"throttle.position",
					0,
					0,
					UNIT::INVALID,
					5.00000f,
					decoder_t::decode_obd2_response,
					nullptr,
					true,
					false
				})}
,				{std::make_shared<diagnostic_message_t>(diagnostic_message_t{
					31,
					"running.time",
					0,
					0,
					UNIT::INVALID,
					1.00000f,
					decoder_t::decode_obd2_response,
					nullptr,
					true,
					false
				})}
,				{std::make_shared<diagnostic_message_t>(diagnostic_message_t{
					45,
					"EGR.error",
					0,
					0,
					UNIT::INVALID,
					0.00000f,
					decoder_t::decode_obd2_response,
					nullptr,
					true,
					false
				})}
,				{std::make_shared<diagnostic_message_t>(diagnostic_message_t{
					47,
					"fuel.level",
					0,
					0,
					UNIT::INVALID,
					1.00000f,
					decoder_t::decode_obd2_response,
					nullptr,
					true,
					false
				})}
,				{std::make_shared<diagnostic_message_t>(diagnostic_message_t{
					51,
					"barometric.pressure",
					0,
					0,
					UNIT::INVALID,
					1.00000f,
					decoder_t::decode_obd2_response,
					nullptr,
					true,
					false
				})}
,				{std::make_shared<diagnostic_message_t>(diagnostic_message_t{
					70,
					"ambient.air.temperature",
					0,
					0,
					UNIT::INVALID,
					1.00000f,
					decoder_t::decode_obd2_response,
					nullptr,
					true,
					false
				})}
,				{std::make_shared<diagnostic_message_t>(diagnostic_message_t{
					76,
					"commanded.throttle.position",
					0,
					0,
					UNIT::INVALID,
					1.00000f,
					decoder_t::decode_obd2_response,
					nullptr,
					true,
					false
				})}
,				{std::make_shared<diagnostic_message_t>(diagnostic_message_t{
					82,
					"ethanol.fuel.percentage",
					0,
					0,
					UNIT::INVALID,
					1.00000f,
					decoder_t::decode_obd2_response,
					nullptr,
					true,
					false
				})}
,				{std::make_shared<diagnostic_message_t>(diagnostic_message_t{
					90,
					"accelerator.pedal.position",
					0,
					0,
					UNIT::INVALID,
					5.00000f,
					decoder_t::decode_obd2_response,
					nullptr,
					true,
					false
				})}
,				{std::make_shared<diagnostic_message_t>(diagnostic_message_t{
					91,
					"hybrid.battery-pack.remaining.life",
					0,
					0,
					UNIT::INVALID,
					5.00000f,
					decoder_t::decode_obd2_response,
					nullptr,
					true,
					false
				})}
,				{std::make_shared<diagnostic_message_t>(diagnostic_message_t{
					92,
					"engine.oil.temperature",
					0,
					0,
					UNIT::INVALID,
					1.00000f,
					decoder_t::decode_obd2_response,
					nullptr,
					true,
					false
				})}
,				{std::make_shared<diagnostic_message_t>(diagnostic_message_t{
					94,
					"engine.fuel.rate",
					0,
					0,
					UNIT::INVALID,
					1.00000f,
					decoder_t::decode_obd2_response,
					nullptr,
					true,
					false
				})}
,				{std::make_shared<diagnostic_message_t>(diagnostic_message_t{
					99,
					"engine.torque",
					0,
					0,
					UNIT::INVALID,
					1.00000f,
					decoder_t::decode_obd2_response,
					nullptr,
					true,
					false
				})}

			} // end diagnostic_messages_ vector
		})} // end can_message_set entry
	} // end can_message_set vector
{
	for(auto& cms: can_message_set_)
	{
		std::vector<std::shared_ptr<can_message_definition_t> >& can_messages_definition = cms->get_can_message_definition();
		for(auto& cmd : can_messages_definition)
		{
			cmd->set_parent(cms.get());
			std::vector<std::shared_ptr<can_signal_t> >& can_signals = cmd->get_can_signals();
			for(auto& sig: can_signals)
			{
				sig->set_parent(cmd.get());
			}
		}

		std::vector<std::shared_ptr<diagnostic_message_t> >& diagnostic_messages = cms->get_diagnostic_messages();
		for(auto& dm : diagnostic_messages)
		{
			dm->set_parent(cms.get());
		}
	}
		}

const std::string application_t::get_diagnostic_bus() const
{
	return "hs";
}


