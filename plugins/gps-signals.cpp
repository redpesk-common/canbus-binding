#include <binding/application.hpp>
#include <can/can-decoder.hpp>
#include <can/can-encoder.hpp>

extern "C" {
CTLP_CAPI_REGISTER("gps");

std::shared_ptr<message_set_t> cms = std::make_shared<message_set_t>(message_set_t{0,"gps",
	{ // beginning message_definition_ vector
		{std::make_shared<message_definition_t>(message_definition_t{"hs",0x123,"gps.simulateur",8,0,frequency_clock_t(5.00000f),true,
			{ // beginning signals vector
				{std::make_shared<signal_t> (signal_t{
					"gps.simulateur.bearing",// generic_name
					54,// bit_position
					10,// bit_size
					0.500000f,// factor
					0.00000f,// offset
					std::nanf(""),// min_value
					std::nanf(""),// max_value
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
					"gps.simulateur.latitude",// generic_name
					22,// bit_position
					22,// bit_size
					0.000100000f,// factor
					-90.0000f,// offset
					std::nanf(""),// min_value
					std::nanf(""),// max_value
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
					"gps.simulateur.longitude",// generic_name
					0,// bit_position
					22,// bit_size
					0.000100000f,// factor
					-180.000f,// offset
					std::nanf(""),// min_value
					std::nanf(""),// max_value
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
					"gps.simulateur.speed",// generic_name
					44,// bit_position
					10,// bit_size
					0.100000f,// factor
					0.00000f,// offset
					std::nanf(""),// min_value
					std::nanf(""),// max_value
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
,		{std::make_shared<message_definition_t>(message_definition_t{"hs",0x124,"gps.attack",8,0,frequency_clock_t(5.00000f),true,
			{ // beginning signals vector
				{std::make_shared<signal_t> (signal_t{
					"gps.attack.bearing",// generic_name
					54,// bit_position
					10,// bit_size
					0.500000f,// factor
					0.00000f,// offset
					std::nanf(""),// min_value
					std::nanf(""),// max_value
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
					"gps.attack.latitude",// generic_name
					22,// bit_position
					22,// bit_size
					0.000100000f,// factor
					-90.0000f,// offset
					std::nanf(""),// min_value
					std::nanf(""),// max_value
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
					"gps.attack.longitude",// generic_name
					0,// bit_position
					22,// bit_size
					0.000100000f,// factor
					-180.000f,// offset
					std::nanf(""),// min_value
					std::nanf(""),// max_value
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
					"gps.attack.speed",// generic_name
					44,// bit_position
					10,// bit_size
					0.100000f,// factor
					0.00000f,// offset
					std::nanf(""),// min_value
					std::nanf(""),// max_value
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
,		{std::make_shared<message_definition_t>(message_definition_t{"hs",0x125,"gps.accelero",8,0,frequency_clock_t(5.00000f),true,
			{ // beginning signals vector
				{std::make_shared<signal_t> (signal_t{
					"gps.accelero.bearing",// generic_name
					54,// bit_position
					10,// bit_size
					0.500000f,// factor
					0.00000f,// offset
					std::nanf(""),// min_value
					std::nanf(""),// max_value
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
					"gps.accelero.latitude",// generic_name
					22,// bit_position
					22,// bit_size
					0.000100000f,// factor
					-90.0000f,// offset
					std::nanf(""),// min_value
					std::nanf(""),// max_value
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
					"gps.accelero.longitude",// generic_name
					0,// bit_position
					22,// bit_size
					0.000100000f,// factor
					-180.000f,// offset
					std::nanf(""),// min_value
					std::nanf(""),// max_value
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
					"gps.accelero.speed",// generic_name
					44,// bit_position
					10,// bit_size
					0.100000f,// factor
					0.00000f,// offset
					std::nanf(""),// min_value
					std::nanf(""),// max_value
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
