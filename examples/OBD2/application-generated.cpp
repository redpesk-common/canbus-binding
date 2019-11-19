#include "application.hpp"
#include "../can/can-decoder.hpp"
#include "../can/can-encoder.hpp"

application_t::application_t()
	: can_bus_manager_{utils::config_parser_t{"/etc/dev-mapping.conf"}}
	, message_set_{
		{std::make_shared<message_set_t>(message_set_t{0,"example",
			{ // beginning message_definition_ vector

		}, // end message_definition vector
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
		})} // end message_set entry
	} // end message_set vector
{
	for(std::shared_ptr<message_set_t> cms: message_set_)
	{
		std::vector<std::shared_ptr<message_definition_t>> messages_definition = cms->get_messages_definition();
		for(std::shared_ptr<message_definition_t> cmd : messages_definition)
		{
			cmd->set_parent(cms);
			std::vector<std::shared_ptr<signal_t>> signals = cmd->get_signals();
			for(std::shared_ptr<signal_t> sig: signals)
			{
				sig->set_parent(cmd);
			}
		}

		std::vector<std::shared_ptr<diagnostic_message_t>> diagnostic_messages = cms->get_diagnostic_messages();
		for(std::shared_ptr<diagnostic_message_t> dm : diagnostic_messages)
		{
			dm->set_parent(cms);
		}
	}
		}

const std::string application_t::get_diagnostic_bus() const
{
	return "hs";
}


