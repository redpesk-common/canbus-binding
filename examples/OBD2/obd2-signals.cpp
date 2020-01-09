#include <binding/application.hpp>
#include <can/can-decoder.hpp>
#include <can/can-encoder.hpp>

extern "C" {
CTLP_CAPI_REGISTER("example");

std::shared_ptr<message_set_t> cms = std::make_shared<message_set_t>(message_set_t{0,"example",
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
,		{std::make_shared<diagnostic_message_t>(diagnostic_message_t{
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
,		{std::make_shared<diagnostic_message_t>(diagnostic_message_t{
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
,		{std::make_shared<diagnostic_message_t>(diagnostic_message_t{
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
,		{std::make_shared<diagnostic_message_t>(diagnostic_message_t{
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
,		{std::make_shared<diagnostic_message_t>(diagnostic_message_t{
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
,		{std::make_shared<diagnostic_message_t>(diagnostic_message_t{
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
,		{std::make_shared<diagnostic_message_t>(diagnostic_message_t{
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
,		{std::make_shared<diagnostic_message_t>(diagnostic_message_t{
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
,		{std::make_shared<diagnostic_message_t>(diagnostic_message_t{
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
,		{std::make_shared<diagnostic_message_t>(diagnostic_message_t{
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
,		{std::make_shared<diagnostic_message_t>(diagnostic_message_t{
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
,		{std::make_shared<diagnostic_message_t>(diagnostic_message_t{
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
,		{std::make_shared<diagnostic_message_t>(diagnostic_message_t{
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
,		{std::make_shared<diagnostic_message_t>(diagnostic_message_t{
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
,		{std::make_shared<diagnostic_message_t>(diagnostic_message_t{
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
,		{std::make_shared<diagnostic_message_t>(diagnostic_message_t{
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
,		{std::make_shared<diagnostic_message_t>(diagnostic_message_t{
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
,		{std::make_shared<diagnostic_message_t>(diagnostic_message_t{
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
,		{std::make_shared<diagnostic_message_t>(diagnostic_message_t{
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
,		{std::make_shared<diagnostic_message_t>(diagnostic_message_t{
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
}); // end message_set entry

CTLP_ONLOAD(plugin, handle) {
	afb_api_t api = (afb_api_t) plugin->api;
	CtlConfigT* CtlConfig = (CtlConfigT*) afb_api_get_userdata(api);
	application_t* app = (application_t*) getExternalData(CtlConfig);

	return app->add_message_set(cms);
}


}
