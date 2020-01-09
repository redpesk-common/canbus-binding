#include <binding/application.hpp>
#include <can/can-decoder.hpp>
#include <can/can-encoder.hpp>

extern "C" {
CTLP_CAPI_REGISTER("example");

std::shared_ptr<message_set_t> cms = std::make_shared<message_set_t>(message_set_t{0,"example",
	{ // beginning message_definition_ vector
		{std::make_shared<message_definition_t>(message_definition_t{"ls",0x30,"",8,0,frequency_clock_t(5.00000f),true,
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
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					""// unit
				})}
			} // end signals vector
		})} // end message_definition entry
	}, // end message_definition vector
	{ // beginning diagnostic_messages_ vector

	} // end diagnostic_messages_ vector
}); // end message_set entry

CTLP_ONLOAD(plugin, handle) {
	afb_api_t api = (afb_api_t) plugin->api;
	CtlConfigT* CtlConfig = (CtlConfigT*) afb_api_get_userdata(api);
	application_t* app = (application_t*) getExternalData(CtlConfig);

	return app->add_message_set(cms);
}


}
