#include "application.hpp"
#include "../can/can-decoder.hpp"
#include "../can/can-encoder.hpp"


// >>>>> handlers.cpp >>>>>
#include "can/canread.h"

using openxc::can::read::publishNumericalMessage;

void handleSteeringWheelMessage(CanMessage* message,
        CanSignal* signals, int signalCount, Pipeline* pipeline) {
    publishNumericalMessage("latitude", 42.0, pipeline);
}

openxc_DynamicField handleInverted(CanSignal* signal, CanSignal* signals,
        int signalCount, float value, bool* send) {
    return openxc::payload::wrapNumber(value * -1);
}

void initializeMyStuff() { }

void initializeOtherStuff() { }

void myLooper() {
    // this function will be called once, each time through the main loop, after
    // all CAN message processing has been completed
}

// <<<<< handlers.cpp <<<<<

application_t::application_t()
	: can_bus_manager_{utils::config_parser_t{"/etc/dev-mapping.conf"}}
	, message_set_{
		{std::make_shared<message_set_t>(message_set_t{0,"example",
			{ // beginning message_definition_ vector
				{std::make_shared<message_definition_t>(message_definition_t{"hs", 0x128,"ECM_z_5D2", 0, false, message_format_t::STANDARD, frequency_clock_t(5.00000f), true,
					{ // beginning signals vector
						{std::make_shared<signal_t> (signal_t{
							"engine_speed",// generic_name
							12,// bit_position
							8,// bit_size
							1.00000f,// factor
							0.00000f,// offset
							0,// min_value
							0,// max_value
							frequency_clock_t(15.0000f),// frequency
							true,// send_same
							false,// force_send_changed
							{
							},// states
							false,// writable
							nullptr,// decoder
							nullptr,// encoder
							false,// received
							std::make_pair<bool, int>(false, 0),// multiplex
							0,// is_big_endian
							0,// is_signed
							""// unit
						})},
						{std::make_shared<signal_t> (signal_t{
							"GearshiftPosition",// generic_name
							41,// bit_position
							3,// bit_size
							1.00000f,// factor
							0.00000f,// offset
							0,// min_value
							0,// max_value
							frequency_clock_t(0.00000f),// frequency
							true,// send_same
							false,// force_send_changed
							{
								{1,"FIRST"},
								{4,"FOURTH"},
								{6,"NEUTRAL"},
								{5,"REVERSE"},
								{2,"SECOND"},
								{3,"THIRD"}
							},// states
							false,// writable
							decoder_t::decode_state,// decoder
							nullptr,// encoder
							false,// received
							std::make_pair<bool, int>(false, 0),// multiplex
							0,// is_big_endian
							0,// is_signed
							""// unit
						})},
						{std::make_shared<signal_t> (signal_t{
							"SteeringWheelAngle",// generic_name
							52,// bit_position
							12,// bit_size
							0.153920f,// factor
							0.00000f,// offset
							0,// min_value
							0,// max_value
							frequency_clock_t(0.00000f),// frequency
							true,// send_same
							false,// force_send_changed
							{
							},// states
							false,// writable
							handleUnsignedSteeringWheelAngle,// decoder
							nullptr,// encoder
							false,// received
							std::make_pair<bool, int>(false, 0),// multiplex
							0,// is_big_endian
							0,// is_signed
							""// unit
						})},
						{std::make_shared<signal_t> (signal_t{
							"steering_wheel_angle_error",// generic_name
							44,// bit_position
							12,// bit_size
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
							ignoreDecoder,// decoder
							nullptr,// encoder
							false,// received
							std::make_pair<bool, int>(false, 0),// multiplex
							0,// is_big_endian
							0,// is_signed
							""// unit
						})},
						{std::make_shared<signal_t> (signal_t{
							"steering_angle_sign",// generic_name
							52,// bit_position
							12,// bit_size
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
							ignoreDecoder,// decoder
							nullptr,// encoder
							false,// received
							std::make_pair<bool, int>(false, 0),// multiplex
							0,// is_big_endian
							0,// is_signed
							""// unit
						})}
					} // end signals vector
				})} // end message_definition entry

		}, // end message_definition vector
			{ // beginning diagnostic_messages_ vector
				{std::make_shared<diagnostic_message_t>(diagnostic_message_t{
					12,
					"",
					0,
					0,
					UNIT::INVALID,
					1.00000f,
					handleObd2Pid,
					nullptr,
					true,
					false
				})}
,				{std::make_shared<diagnostic_message_t>(diagnostic_message_t{
					12,
					"",
					0,
					0,
					UNIT::INVALID,
					1.00000f,
					nullptr,
					nullptr,
					true,
					false
				})}
,				{std::make_shared<diagnostic_message_t>(diagnostic_message_t{
					6,
					"",
					0,
					0,
					UNIT::INVALID,
					1.00000f,
					handleMyDiagRequest,
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
		vect_ptr_msg_def_t messages_definition = cms->get_messages_definition();
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


