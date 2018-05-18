#include "application.hpp"
#include "../can/can-decoder.hpp"


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
	, can_message_set_{
		{std::make_shared<can_message_set_t>(can_message_set_t{0,"example",
			{ // beginning can_message_definition_ vector
				{std::make_shared<can_message_definition_t>(can_message_definition_t{"hs",0x128,can_message_format_t::STANDARD,frequency_clock_t(5.00000f),true,
					{ // beginning can_signals vector
						{std::make_shared<can_signal_t> (can_signal_t{
							"engine_speed",
							12,
							8,
							1.00000f,
							0.00000,
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
						})},
						{std::make_shared<can_signal_t> (can_signal_t{
							"GearshiftPosition",
							41,
							3,
							1.00000f,
							0.00000,
							0,
							0,
							frequency_clock_t(0.00000f),
							true,
							false,
							{
								{1,"FIRST"},
								{4,"FOURTH"},
								{6,"NEUTRAL"},
								{5,"REVERSE"},
								{2,"SECOND"},
								{3,"THIRD"}
							},
							false,
							nullptr,
							nullptr,
							false
						})},
						{std::make_shared<can_signal_t> (can_signal_t{
							"SteeringWheelAngle",
							52,
							12,
							0.153920f,
							0.00000,
							0,
							0,
							frequency_clock_t(0.00000f),
							true,
							false,
							{
							},
							false,
							handleUnsignedSteeringWheelAngle,
							nullptr,
							false
						})},
						{std::make_shared<can_signal_t> (can_signal_t{
							"steering_wheel_angle_error",
							44,
							12,
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
							ignoreDecoder,
							nullptr,
							false
						})},
						{std::make_shared<can_signal_t> (can_signal_t{
							"steering_angle_sign",
							52,
							12,
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
							ignoreDecoder,
							nullptr,
							false
						})}
					} // end can_signals vector
				})} // end can_message_definition entry

		}, // end can_message_definition vector
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


