#include <low-can/binding/application.hpp>
#include <low-can/can/can-decoder.hpp>
#include <low-can/can/can-encoder.hpp>

#include <ctl-config.h>


// >>>>> signals-header-template.cpp >>>>>
/*******************************************************************************
*
* Copyright (C) 2019 IoT.bzh Company
* Contact: https://www.iot.bzh/licensing
*
* This file is part of the rp-webserver module of the RedPesk project.
*
* $RP_BEGIN_LICENSE$
* Commercial License Usage
*  Licensees holding valid commercial IoT.bzh licenses may use this file in
*  accordance with the commercial license agreement provided with the
*  Software or, alternatively, in accordance with the terms contained in
*  a written agreement between you and The IoT.bzh Company. For licensing terms
*  and conditions see https://www.iot.bzh/terms-conditions. For further
*  information use the contact form at https://www.iot.bzh/contact.
*
* GNU General Public License Usage
*  Alternatively, this file may be used under the terms of the GNU General
*  Public license version 3. This license is as published by the Free Software
*  Foundation and appearing in the file LICENSE.GPLv3 included in the packaging
*  of this file. Please review the following information to ensure the GNU
*  General Public License requirements will be met
*  https://www.gnu.org/licenses/gpl-3.0.html.
* $RP_END_LICENSE$
*
*******************************************************************************/

#include <low-can/utils/signals.hpp>
#include <low-can/utils/openxc-utils.hpp>
#include <low-can/can/can-decoder.hpp>

openxc_DynamicField decode_example(signal_t &signal, std::shared_ptr<message_t> message, bool *send) //Defining a new decoder for our signals
{
    float value = decoder_t::parse_signal_bitfield(signal, message); //Getting signal value
    float modified_value = atan2f32(value, value*M_1_PIf32); //Processing the value. The calculation has no purpose, it's juste for the example.
    openxc_DynamicField decoded_value = build_DynamicField((double) modified_value);
    return decoded_value; //Return the value
}
// <<<<< signals-header-template.cpp <<<<<

extern "C" {
CTLP_CAPI_REGISTER("agl-virtual-car");

std::shared_ptr<message_set_t> cms = std::make_shared<message_set_t>(message_set_t{0,"AGL Virtual Car",
	{ // beginning message_definition_ vector
		{std::make_shared<message_definition_t>(message_definition_t{"hs",0x3D9,"",8,0,frequency_clock_t(5.00000f),true,
			{ // beginning signals vector
				{std::make_shared<signal_t> (signal_t{
					"engine.speed",// generic_name
					16,// bit_position
					16,// bit_size
					0.250000f,// factor
					0.00000f,// offset
					std::nanf(""),// min_value
					std::nanf(""),// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
					},// states
					true,// writable
					decoder_t::mple,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					""// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"fuel.level.low",// generic_name
					55,// bit_position
					1,// bit_size
					1.00000f,// factor
					0.00000f,// offset
					std::nanf(""),// min_value
					std::nanf(""),// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
					},// states
					true,// writable
					decoder_t::decode_boolean,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					""// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"fuel.level",// generic_name
					8,// bit_position
					8,// bit_size
					0.392157f,// factor
					0.00000f,// offset
					std::nanf(""),// min_value
					std::nanf(""),// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
					},// states
					true,// writable
					decode_example,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					""// unit
				})}
			} // end signals vector
		})} // end message_definition entry
,		{std::make_shared<message_definition_t>(message_definition_t{"hs",0x620,"",8,0,frequency_clock_t(5.00000f),true,
			{ // beginning signals vector
				{std::make_shared<signal_t> (signal_t{
					"doors.boot.open",// generic_name
					47,// bit_position
					1,// bit_size
					1.00000f,// factor
					0.00000f,// offset
					std::nanf(""),// min_value
					std::nanf(""),// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
					},// states
					true,// writable
					decoder_t::decode_boolean,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					""// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"doors.front_left.open",// generic_name
					43,// bit_position
					1,// bit_size
					1.00000f,// factor
					0.00000f,// offset
					std::nanf(""),// min_value
					std::nanf(""),// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
					},// states
					true,// writable
					decoder_t::decode_boolean,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					""// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"doors.front_right.open",// generic_name
					44,// bit_position
					1,// bit_size
					1.00000f,// factor
					0.00000f,// offset
					std::nanf(""),// min_value
					std::nanf(""),// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
					},// states
					true,// writable
					decoder_t::decode_boolean,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					""// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"doors.rear_left.open",// generic_name
					46,// bit_position
					1,// bit_size
					1.00000f,// factor
					0.00000f,// offset
					std::nanf(""),// min_value
					std::nanf(""),// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
					},// states
					true,// writable
					decoder_t::decode_boolean,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					""// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"doors.rear_right.open",// generic_name
					45,// bit_position
					4,// bit_size
					1.00000f,// factor
					0.00000f,// offset
					std::nanf(""),// min_value
					std::nanf(""),// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
					},// states
					true,// writable
					decoder_t::decode_boolean,// decoder
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
