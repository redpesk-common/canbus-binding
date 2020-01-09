#include <binding/application.hpp>
#include <can/can-decoder.hpp>
#include <can/can-encoder.hpp>

extern "C" {
CTLP_CAPI_REGISTER("example");

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

std::shared_ptr<message_set_t> cms = std::make_shared<message_set_t>(message_set_t{0,"example",
	{ // beginning message_definition_ vector
		{std::make_shared<message_definition_t>(message_definition_t{"hs",0x128,"ECM_z_5D2",8,0,frequency_clock_t(5.00000f),true,
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
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
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
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
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
					decoder_t::gnedSteeringWheelAngle,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
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
					decoder_t::der,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
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
					decoder_t::der,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
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
			decoder_t::Pid,
			nullptr,
			true,
			false
		})}
,		{std::make_shared<diagnostic_message_t>(diagnostic_message_t{
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
,		{std::make_shared<diagnostic_message_t>(diagnostic_message_t{
			6,
			"",
			0,
			0,
			UNIT::INVALID,
			1.00000f,
			decoder_t::agRequest,
			nullptr,
			true,
			false
		})}

	} // end diagnostic_messages_ vector
}); // end message_set entry

CTLP_ONLOAD(plugin, handle) {
	afb_api_t api = (afb_api_t) plugin->api;
	CtlConfigT* CtlConfig = (CtlConfigT*) afb_api_get_userdata(api);
	application_t* app = (application_t*) getExternalData(CtlConfig);

	return app->add_message_set(cms);
}


}
