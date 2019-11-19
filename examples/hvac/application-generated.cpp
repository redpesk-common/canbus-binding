#include "application.hpp"
#include "../can/can-decoder.hpp"
#include "../can/can-encoder.hpp"

application_t::application_t()
	: can_bus_manager_{utils::config_parser_t{"/etc/dev-mapping.conf"}}
	, message_set_{
		{std::make_shared<message_set_t>(message_set_t{0,"example",
			{ // beginning message_definition_ vector
				{std::make_shared<message_definition_t>(message_definition_t{"ls",0x30,"",8,0,true,frequency_clock_t(5.00000f),true,
					{ // beginning signals vector
						{std::make_shared<signal_t> (signal_t{
							"hvac.fan.speed",// generic_name
							32,// bit_position
							8,// bit_size
							23.5294f,// factor
							0.00000f,// offset
							0,// min_value
							0,// max_value
							frequency_clock_t(0.00000f),// frequency
							true,// send_same
							false,// force_send_changed
							{
							},// states
							false,// writable
							nullptr,// decoder
							nullptr,// encoder
							false,// received
							std::make_pair<bool, int>(false, 0),// multiplex
							false,// is_big_endian
							static_cast<sign_t>(0),// signed
							-1,// bit_sign_position
							""// unit
						})},
						{std::make_shared<signal_t> (signal_t{
							"hvac.temperature.left",// generic_name
							0,// bit_position
							8,// bit_size
							1.00000f,// factor
							0.00000f,// offset
							0,// min_value
							0,// max_value
							frequency_clock_t(0.00000f),// frequency
							true,// send_same
							false,// force_send_changed
							{
							},// states
							false,// writable
							nullptr,// decoder
							nullptr,// encoder
							false,// received
							std::make_pair<bool, int>(false, 0),// multiplex
							false,// is_big_endian
							static_cast<sign_t>(0),// signed
							-1,// bit_sign_position
							""// unit
						})},
						{std::make_shared<signal_t> (signal_t{
							"hvac.temperature.right",// generic_name
							8,// bit_position
							8,// bit_size
							1.00000f,// factor
							0.00000f,// offset
							0,// min_value
							0,// max_value
							frequency_clock_t(0.00000f),// frequency
							true,// send_same
							false,// force_send_changed
							{
							},// states
							false,// writable
							nullptr,// decoder
							nullptr,// encoder
							false,// received
							std::make_pair<bool, int>(false, 0),// multiplex
							false,// is_big_endian
							static_cast<sign_t>(0),// signed
							-1,// bit_sign_position
							""// unit
						})},
						{std::make_shared<signal_t> (signal_t{
							"hvac.temperature.average",// generic_name
							16,// bit_position
							8,// bit_size
							1.00000f,// factor
							0.00000f,// offset
							0,// min_value
							0,// max_value
							frequency_clock_t(0.00000f),// frequency
							true,// send_same
							false,// force_send_changed
							{
							},// states
							false,// writable
							nullptr,// decoder
							nullptr,// encoder
							false,// received
							std::make_pair<bool, int>(false, 0),// multiplex
							false,// is_big_endian
							static_cast<sign_t>(0),// signed
							-1,// bit_sign_position
							""// unit
						})}
					} // end signals vector
				})} // end message_definition entry

		}, // end message_definition vector
			{ // beginning diagnostic_messages_ vector

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
	return "";
}


