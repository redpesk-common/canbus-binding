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

#include <binding/application.hpp>
#include <can/can-decoder.hpp>
#include <can/can-encoder.hpp>

extern "C" {
CTLP_CAPI_REGISTER("nmea2000");

std::shared_ptr<message_set_t> cms = std::make_shared<message_set_t>(message_set_t{0,"NMEA2000",
	{ // beginning message_definition_ vector
		{std::make_shared<message_definition_t>(message_definition_t{"j1939",126992,"System.Time",8,136,frequency_clock_t(5.00000f),true,
			{ // beginning signals vector
				{std::make_shared<signal_t> (signal_t{
					"System.Time.Date",// generic_name
					32,// bit_position edited with low-can-generator
					16,// bit_size
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
					decoder_t::decode_date,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					"days"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"System.Time.Reserved",// generic_name
					48,// bit_position edited with low-can-generator
					4,// bit_size
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
					decoder_t::decode_bytes,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					""// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"System.Time.Sid",// generic_name
					56,// bit_position edited with low-can-generator
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
					"System.Time.Source",// generic_name
					52,// bit_position edited with low-can-generator
					4,// bit_size
					1.00000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
						{1,"GLONASS"},
						{0,"GPS"},
						{3,"Local Cesium clock"},
						{5,"Local Crystal clock"},
						{4,"Local Rubidium clock"},
						{2,"Radio Station"}
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
					"System.Time.Time",// generic_name
					0,// bit_position edited with low-can-generator
					32,// bit_size
					0.000100000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
					},// states
					false,// writable
					decoder_t::decode_time,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					"s"// unit
				})}
			} // end signals vector
		})} // end message_definition entry
,		{std::make_shared<message_definition_t>(message_definition_t{"j1939",127237,"Heading.Track.Control",21,136,frequency_clock_t(5.00000f),true,
			{ // beginning signals vector
				{std::make_shared<signal_t> (signal_t{
					"Heading.Track.Control.Commanded.Rudder.Angle",// generic_name
					128,// bit_position edited with low-can-generator
					16,// bit_size
					0.000100000f,// factor
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
					static_cast<sign_t>(2),// signed
					-1,// bit_sign_position
					"rad"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Heading.Track.Control.Commanded.Rudder.Direction",// generic_name
					144,// bit_position edited with low-can-generator
					3,// bit_size
					1.00000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
						{2,"Move to port"},
						{1,"Move to starboard"},
						{0,"No Order"}
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
					"Heading.Track.Control.Heading.Reference",// generic_name
					152,// bit_position edited with low-can-generator
					2,// bit_size
					1.00000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
						{2,"Error"},
						{1,"Magnetic"},
						{3,"Null"},
						{0,"True"}
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
					"Heading.Track.Control.Heading.To.Steer.Course",// generic_name
					112,// bit_position edited with low-can-generator
					16,// bit_size
					0.000100000f,// factor
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
					"rad"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Heading.Track.Control.Off.Heading.Limit",// generic_name
					64,// bit_position edited with low-can-generator
					16,// bit_size
					0.000100000f,// factor
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
					"rad"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Heading.Track.Control.Off.Heading.Limit.Exceeded",// generic_name
					164,// bit_position edited with low-can-generator
					2,// bit_size
					1.00000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
						{2,"Error"},
						{0,"No"},
						{3,"Unavailable"},
						{1,"Yes"}
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
					"Heading.Track.Control.Off.Track.Limit",// generic_name
					16,// bit_position edited with low-can-generator
					16,// bit_size
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
					static_cast<sign_t>(2),// signed
					-1,// bit_sign_position
					"m"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Heading.Track.Control.Off.Track.Limit.Exceeded",// generic_name
					162,// bit_position edited with low-can-generator
					2,// bit_size
					1.00000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
						{2,"Error"},
						{0,"No"},
						{3,"Unavailable"},
						{1,"Yes"}
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
					"Heading.Track.Control.Override",// generic_name
					160,// bit_position edited with low-can-generator
					2,// bit_size
					1.00000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
						{2,"Error"},
						{0,"No"},
						{3,"Unavailable"},
						{1,"Yes"}
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
					"Heading.Track.Control.Radius.Of.Turn.Order",// generic_name
					48,// bit_position edited with low-can-generator
					16,// bit_size
					0.000100000f,// factor
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
					static_cast<sign_t>(2),// signed
					-1,// bit_sign_position
					"rad"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Heading.Track.Control.Rate.Of.Turn.Order",// generic_name
					32,// bit_position edited with low-can-generator
					16,// bit_size
					3.12500e-05f,// factor
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
					static_cast<sign_t>(2),// signed
					-1,// bit_sign_position
					"rad/s"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Heading.Track.Control.Reserved",// generic_name
					147,// bit_position edited with low-can-generator
					5,// bit_size
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
					decoder_t::decode_bytes,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					""// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Heading.Track.Control.Rudder.Limit",// generic_name
					80,// bit_position edited with low-can-generator
					16,// bit_size
					0.000100000f,// factor
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
					"rad"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Heading.Track.Control.Rudder.Limit.Exceeded",// generic_name
					166,// bit_position edited with low-can-generator
					2,// bit_size
					1.00000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
						{2,"Error"},
						{0,"No"},
						{3,"Unavailable"},
						{1,"Yes"}
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
					"Heading.Track.Control.Steering.Mode",// generic_name
					157,// bit_position edited with low-can-generator
					3,// bit_size
					1.00000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
						{2,"Follow-up Device"},
						{4,"Heading Control"},
						{3,"Heading Control Standalone"},
						{0,"Main Steering"},
						{1,"Non-Follow-up Device"},
						{5,"Track Control"}
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
					"Heading.Track.Control.Track",// generic_name
					96,// bit_position edited with low-can-generator
					16,// bit_size
					0.000100000f,// factor
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
					"rad"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Heading.Track.Control.Turn.Mode",// generic_name
					154,// bit_position edited with low-can-generator
					3,// bit_size
					1.00000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
						{0,"Rudder Limit controlled"},
						{2,"radius controlled"},
						{1,"turn rate controlled"}
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
					"Heading.Track.Control.Vessel.Heading",// generic_name
					0,// bit_position edited with low-can-generator
					16,// bit_size
					0.000100000f,// factor
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
					"rad"// unit
				})}
			} // end signals vector
		})} // end message_definition entry
,		{std::make_shared<message_definition_t>(message_definition_t{"j1939",127245,"Rudder",8,136,frequency_clock_t(5.00000f),true,
			{ // beginning signals vector
				{std::make_shared<signal_t> (signal_t{
					"Rudder.Angle.Order",// generic_name
					32,// bit_position edited with low-can-generator
					16,// bit_size
					0.000100000f,// factor
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
					static_cast<sign_t>(2),// signed
					-1,// bit_sign_position
					"rad"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Rudder.Direction.Order",// generic_name
					54,// bit_position edited with low-can-generator
					2,// bit_size
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
					"Rudder.Instance",// generic_name
					56,// bit_position edited with low-can-generator
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
					"Rudder.Position",// generic_name
					16,// bit_position edited with low-can-generator
					16,// bit_size
					0.000100000f,// factor
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
					static_cast<sign_t>(2),// signed
					-1,// bit_sign_position
					"rad"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Rudder.Reserved",// generic_name
					48,// bit_position edited with low-can-generator
					6,// bit_size
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
					decoder_t::decode_bytes,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					""// unit
				})}
			} // end signals vector
		})} // end message_definition entry
,		{std::make_shared<message_definition_t>(message_definition_t{"j1939",127250,"Vessel.Heading",8,136,frequency_clock_t(5.00000f),true,
			{ // beginning signals vector
				{std::make_shared<signal_t> (signal_t{
					"Vessel.Heading.Deviation",// generic_name
					24,// bit_position edited with low-can-generator
					16,// bit_size
					0.000100000f,// factor
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
					static_cast<sign_t>(2),// signed
					-1,// bit_sign_position
					"rad"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Vessel.Heading.Heading",// generic_name
					40,// bit_position edited with low-can-generator
					16,// bit_size
					0.000100000f,// factor
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
					"rad"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Vessel.Heading.Reference",// generic_name
					6,// bit_position edited with low-can-generator
					2,// bit_size
					1.00000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
						{2,"Error"},
						{1,"Magnetic"},
						{3,"Null"},
						{0,"True"}
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
					"Vessel.Heading.Reserved",// generic_name
					0,// bit_position edited with low-can-generator
					6,// bit_size
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
					decoder_t::decode_bytes,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					""// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Vessel.Heading.Sid",// generic_name
					56,// bit_position edited with low-can-generator
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
					"Vessel.Heading.Variation",// generic_name
					8,// bit_position edited with low-can-generator
					16,// bit_size
					0.000100000f,// factor
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
					static_cast<sign_t>(2),// signed
					-1,// bit_sign_position
					"rad"// unit
				})}
			} // end signals vector
		})} // end message_definition entry
,		{std::make_shared<message_definition_t>(message_definition_t{"j1939",127251,"Rate.Of.Turn",5,136,frequency_clock_t(5.00000f),true,
			{ // beginning signals vector
				{std::make_shared<signal_t> (signal_t{
					"Rate.Of.Turn.Rate",// generic_name
					0,// bit_position edited with low-can-generator
					32,// bit_size
					3.12500e-08f,// factor
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
					static_cast<sign_t>(2),// signed
					-1,// bit_sign_position
					"rad/s"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Rate.Of.Turn.Sid",// generic_name
					32,// bit_position edited with low-can-generator
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
,		{std::make_shared<message_definition_t>(message_definition_t{"j1939",127258,"Magnetic.Variation",6,136,frequency_clock_t(5.00000f),true,
			{ // beginning signals vector
				{std::make_shared<signal_t> (signal_t{
					"Magnetic.Variation.Age.Of.Service",// generic_name
					16,// bit_position edited with low-can-generator
					16,// bit_size
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
					decoder_t::decode_date,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					"days"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Magnetic.Variation.Reserved",// generic_name
					32,// bit_position edited with low-can-generator
					4,// bit_size
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
					decoder_t::decode_bytes,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					""// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Magnetic.Variation.Sid",// generic_name
					40,// bit_position edited with low-can-generator
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
					"Magnetic.Variation.Source",// generic_name
					36,// bit_position edited with low-can-generator
					4,// bit_size
					1.00000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
						{3,"Automatic Calculation"},
						{1,"Automatic Chart"},
						{2,"Automatic Table"},
						{0,"Manual"},
						{4,"WMM 2000"},
						{5,"WMM 2005"},
						{6,"WMM 2010"},
						{7,"WMM 2015"},
						{8,"WMM 2020"}
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
					"Magnetic.Variation.Variation",// generic_name
					0,// bit_position edited with low-can-generator
					16,// bit_size
					0.000100000f,// factor
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
					static_cast<sign_t>(2),// signed
					-1,// bit_sign_position
					"rad"// unit
				})}
			} // end signals vector
		})} // end message_definition entry
,		{std::make_shared<message_definition_t>(message_definition_t{"j1939",128259,"Speed",6,136,frequency_clock_t(5.00000f),true,
			{ // beginning signals vector
				{std::make_shared<signal_t> (signal_t{
					"Speed.Sid",// generic_name
					40,// bit_position edited with low-can-generator
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
					"Speed.Speed.Direction",// generic_name
					4294967292,// bit_position edited with low-can-generator
					4,// bit_size
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
					"Speed.Speed.Ground.Referenced",// generic_name
					8,// bit_position edited with low-can-generator
					16,// bit_size
					0.0100000f,// factor
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
					"m/s"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Speed.Speed.Water.Referenced",// generic_name
					24,// bit_position edited with low-can-generator
					16,// bit_size
					0.0100000f,// factor
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
					"m/s"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Speed.Speed.Water.Referenced.Type",// generic_name
					0,// bit_position edited with low-can-generator
					8,// bit_size
					1.00000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
						{3,"Correlation (ultra sound)"},
						{2,"Doppler"},
						{4,"Electro Magnetic"},
						{0,"Paddle wheel"},
						{1,"Pitot tube"}
					},// states
					false,// writable
					decoder_t::decode_state,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					""// unit
				})}
			} // end signals vector
		})} // end message_definition entry
,		{std::make_shared<message_definition_t>(message_definition_t{"j1939",128267,"Water.Depth",5,136,frequency_clock_t(5.00000f),true,
			{ // beginning signals vector
				{std::make_shared<signal_t> (signal_t{
					"Water.Depth.Depth",// generic_name
					0,// bit_position edited with low-can-generator
					32,// bit_size
					0.0100000f,// factor
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
					"m"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Water.Depth.Offset",// generic_name
					4294967280,// bit_position edited with low-can-generator
					16,// bit_size
					0.00100000f,// factor
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
					static_cast<sign_t>(2),// signed
					-1,// bit_sign_position
					"m"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Water.Depth.Range",// generic_name
					4294967272,// bit_position edited with low-can-generator
					8,// bit_size
					10.0000f,// factor
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
					"m"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Water.Depth.Sid",// generic_name
					32,// bit_position edited with low-can-generator
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
,		{std::make_shared<message_definition_t>(message_definition_t{"j1939",128275,"Distance.Log",14,136,frequency_clock_t(5.00000f),true,
			{ // beginning signals vector
				{std::make_shared<signal_t> (signal_t{
					"Distance.Log.Date",// generic_name
					96,// bit_position edited with low-can-generator
					16,// bit_size
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
					decoder_t::decode_date,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					"days"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Distance.Log.Log",// generic_name
					32,// bit_position edited with low-can-generator
					32,// bit_size
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
					"m"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Distance.Log.Time",// generic_name
					64,// bit_position edited with low-can-generator
					32,// bit_size
					0.000100000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
					},// states
					false,// writable
					decoder_t::decode_time,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					"s"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Distance.Log.Trip.Log",// generic_name
					0,// bit_position edited with low-can-generator
					32,// bit_size
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
					"m"// unit
				})}
			} // end signals vector
		})} // end message_definition entry
,		{std::make_shared<message_definition_t>(message_definition_t{"j1939",129025,"Position.Rapid.Update",8,136,frequency_clock_t(5.00000f),true,
			{ // beginning signals vector
				{std::make_shared<signal_t> (signal_t{
					"Position.Rapid.Update.Latitude",// generic_name
					32,// bit_position edited with low-can-generator
					32,// bit_size
					1.00000e-07f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
					},// states
					false,// writable
					decoder_t::decode_noop,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(2),// signed
					-1,// bit_sign_position
					"deg"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Position.Rapid.Update.Longitude",// generic_name
					0,// bit_position edited with low-can-generator
					32,// bit_size
					1.00000e-07f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
					},// states
					false,// writable
					decoder_t::decode_noop,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(2),// signed
					-1,// bit_sign_position
					"deg"// unit
				})}
			} // end signals vector
		})} // end message_definition entry
,		{std::make_shared<message_definition_t>(message_definition_t{"j1939",129026,"Cog.Sog.Rapid.Update",8,136,frequency_clock_t(5.00000f),true,
			{ // beginning signals vector
				{std::make_shared<signal_t> (signal_t{
					"Cog.Sog.Rapid.Update.Cog",// generic_name
					32,// bit_position edited with low-can-generator
					16,// bit_size
					0.000100000f,// factor
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
					"rad"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Cog.Sog.Rapid.Update.Cog.Reference",// generic_name
					54,// bit_position edited with low-can-generator
					2,// bit_size
					1.00000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
						{2,"Error"},
						{1,"Magnetic"},
						{3,"Null"},
						{0,"True"}
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
					"Cog.Sog.Rapid.Update.Reserved",// generic_name
					0,// bit_position edited with low-can-generator
					16,// bit_size
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
					decoder_t::decode_bytes,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					""// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Cog.Sog.Rapid.Update.Sid",// generic_name
					56,// bit_position edited with low-can-generator
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
					"Cog.Sog.Rapid.Update.Sog",// generic_name
					16,// bit_position edited with low-can-generator
					16,// bit_size
					0.0100000f,// factor
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
					"m/s"// unit
				})}
			} // end signals vector
		})} // end message_definition entry
,		{std::make_shared<message_definition_t>(message_definition_t{"j1939",129029,"Gnss.Position.Data",51,136,frequency_clock_t(5.00000f),true,
			{ // beginning signals vector
				{std::make_shared<signal_t> (signal_t{
					"Gnss.Position.Data.Age.Of.Dgnss.Corrections",// generic_name
					32,// bit_position edited with low-can-generator
					16,// bit_size
					0.0100000f,// factor
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
					"s"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Gnss.Position.Data.Altitude",// generic_name
					160,// bit_position edited with low-can-generator
					64,// bit_size
					1.00000e-06f,// factor
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
					static_cast<sign_t>(2),// signed
					-1,// bit_sign_position
					"m"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Gnss.Position.Data.Date",// generic_name
					384,// bit_position edited with low-can-generator
					16,// bit_size
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
					decoder_t::decode_date,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					"days"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Gnss.Position.Data.Geoidal.Separation",// generic_name
					72,// bit_position edited with low-can-generator
					32,// bit_size
					0.0100000f,// factor
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
					static_cast<sign_t>(2),// signed
					-1,// bit_sign_position
					"m"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Gnss.Position.Data.Gnss.Type",// generic_name
					156,// bit_position edited with low-can-generator
					4,// bit_size
					1.00000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
						{5,"Chayka"},
						{1,"GLONASS"},
						{0,"GPS"},
						{2,"GPS+GLONASS"},
						{3,"GPS+SBAS/WAAS"},
						{4,"GPS+SBAS/WAAS+GLONASS"},
						{8,"Galileo"},
						{6,"integrated"},
						{7,"surveyed"}
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
					"Gnss.Position.Data.Hdop",// generic_name
					120,// bit_position edited with low-can-generator
					16,// bit_size
					0.0100000f,// factor
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
					static_cast<sign_t>(2),// signed
					-1,// bit_sign_position
					""// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Gnss.Position.Data.Integrity",// generic_name
					150,// bit_position edited with low-can-generator
					2,// bit_size
					1.00000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
						{2,"Caution"},
						{0,"No integrity checking"},
						{1,"Safe"}
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
					"Gnss.Position.Data.Latitude",// generic_name
					288,// bit_position edited with low-can-generator
					64,// bit_size
					1.00000e-16f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
					},// states
					false,// writable
					decoder_t::decode_noop,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(2),// signed
					-1,// bit_sign_position
					"deg"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Gnss.Position.Data.Longitude",// generic_name
					224,// bit_position edited with low-can-generator
					64,// bit_size
					1.00000e-16f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
					},// states
					false,// writable
					decoder_t::decode_noop,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(2),// signed
					-1,// bit_sign_position
					"deg"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Gnss.Position.Data.Method",// generic_name
					152,// bit_position edited with low-can-generator
					4,// bit_size
					1.00000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
						{2,"DGNSS fix"},
						{6,"Estimated (DR) mode"},
						{1,"GNSS fix"},
						{7,"Manual Input"},
						{3,"Precise GNSS"},
						{4,"RTK Fixed Integer"},
						{5,"RTK float"},
						{8,"Simulate mode"},
						{0,"no GNSS"}
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
					"Gnss.Position.Data.Number.Of.Svs",// generic_name
					136,// bit_position edited with low-can-generator
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
					"Gnss.Position.Data.Pdop",// generic_name
					104,// bit_position edited with low-can-generator
					16,// bit_size
					0.0100000f,// factor
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
					static_cast<sign_t>(2),// signed
					-1,// bit_sign_position
					""// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Gnss.Position.Data.Reference.Station.Id",// generic_name
					48,// bit_position edited with low-can-generator
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
					nullptr,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					"None"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Gnss.Position.Data.Reference.Station.Type",// generic_name
					60,// bit_position edited with low-can-generator
					4,// bit_size
					1.00000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
						{5,"Chayka"},
						{1,"GLONASS"},
						{0,"GPS"},
						{2,"GPS+GLONASS"},
						{3,"GPS+SBAS/WAAS"},
						{4,"GPS+SBAS/WAAS+GLONASS"},
						{8,"Galileo"},
						{6,"integrated"},
						{7,"surveyed"}
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
					"Gnss.Position.Data.Reference.Stations",// generic_name
					64,// bit_position edited with low-can-generator
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
					"Gnss.Position.Data.Reserved",// generic_name
					144,// bit_position edited with low-can-generator
					6,// bit_size
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
					decoder_t::decode_bytes,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					""// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Gnss.Position.Data.Sid",// generic_name
					400,// bit_position edited with low-can-generator
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
					"Gnss.Position.Data.Time",// generic_name
					352,// bit_position edited with low-can-generator
					32,// bit_size
					0.000100000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
					},// states
					false,// writable
					decoder_t::decode_time,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					"s"// unit
				})}
			} // end signals vector
		})} // end message_definition entry
,		{std::make_shared<message_definition_t>(message_definition_t{"j1939",129033,"Time.Date",8,136,frequency_clock_t(5.00000f),true,
			{ // beginning signals vector
				{std::make_shared<signal_t> (signal_t{
					"Time.Date.Date",// generic_name
					48,// bit_position edited with low-can-generator
					16,// bit_size
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
					decoder_t::decode_date,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					"days"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Time.Date.Local.Offset",// generic_name
					0,// bit_position edited with low-can-generator
					16,// bit_size
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
					decoder_t::decode_noop,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(2),// signed
					-1,// bit_sign_position
					"minutes"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Time.Date.Time",// generic_name
					16,// bit_position edited with low-can-generator
					32,// bit_size
					0.000100000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
					},// states
					false,// writable
					decoder_t::decode_time,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					"s"// unit
				})}
			} // end signals vector
		})} // end message_definition entry
,		{std::make_shared<message_definition_t>(message_definition_t{"j1939",129038,"Ais.Class.APosition.Report",28,136,frequency_clock_t(5.00000f),true,
			{ // beginning signals vector
				{std::make_shared<signal_t> (signal_t{
					"Ais.Class.APosition.Report.Ais.Spare",// generic_name
					13,// bit_position edited with low-can-generator
					3,// bit_size
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
					decoder_t::decode_bytes,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					""// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Ais.Class.APosition.Report.Ais.Transceiver.Information",// generic_name
					56,// bit_position edited with low-can-generator
					5,// bit_size
					1.00000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
						{0,"Channel A VDL reception"},
						{2,"Channel A VDL transmission"},
						{1,"Channel B VDL reception"},
						{3,"Channel B VDL transmission"},
						{4,"Own information not broadcast"},
						{5,"Reserved"}
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
					"Ais.Class.APosition.Report.Cog",// generic_name
					96,// bit_position edited with low-can-generator
					16,// bit_size
					0.000100000f,// factor
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
					"rad"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Ais.Class.APosition.Report.Communication.State",// generic_name
					61,// bit_position edited with low-can-generator
					19,// bit_size
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
					decoder_t::decode_bytes,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					""// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Ais.Class.APosition.Report.Heading",// generic_name
					40,// bit_position edited with low-can-generator
					16,// bit_size
					0.000100000f,// factor
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
					"rad"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Ais.Class.APosition.Report.Latitude",// generic_name
					120,// bit_position edited with low-can-generator
					32,// bit_size
					1.00000e-07f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
					},// states
					false,// writable
					decoder_t::decode_noop,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(2),// signed
					-1,// bit_sign_position
					"deg"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Ais.Class.APosition.Report.Longitude",// generic_name
					152,// bit_position edited with low-can-generator
					32,// bit_size
					1.00000e-07f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
					},// states
					false,// writable
					decoder_t::decode_noop,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(2),// signed
					-1,// bit_sign_position
					"deg"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Ais.Class.APosition.Report.Message.Id",// generic_name
					218,// bit_position edited with low-can-generator
					6,// bit_size
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
					"Ais.Class.APosition.Report.Nav.Status",// generic_name
					20,// bit_position edited with low-can-generator
					4,// bit_size
					1.00000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
						{14,"AIS-SART"},
						{6,"Aground"},
						{1,"At anchor"},
						{4,"Constrained by her draught"},
						{7,"Engaged in Fishing"},
						{9,"Hazardous material - High Speed"},
						{10,"Hazardous material - Wing in Ground"},
						{5,"Moored"},
						{2,"Not under command"},
						{3,"Restricted manoeuverability"},
						{8,"Under way sailing"},
						{0,"Under way using engine"}
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
					"Ais.Class.APosition.Report.Position.Accuracy",// generic_name
					119,// bit_position edited with low-can-generator
					1,// bit_size
					1.00000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
						{1,"High"},
						{0,"Low"}
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
					"Ais.Class.APosition.Report.Raim",// generic_name
					118,// bit_position edited with low-can-generator
					1,// bit_size
					1.00000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
						{1,"in use"},
						{0,"not in use"}
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
					"Ais.Class.APosition.Report.Rate.Of.Turn",// generic_name
					24,// bit_position edited with low-can-generator
					16,// bit_size
					3.12500e-05f,// factor
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
					static_cast<sign_t>(2),// signed
					-1,// bit_sign_position
					"rad/s"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Ais.Class.APosition.Report.Repeat.Indicator",// generic_name
					216,// bit_position edited with low-can-generator
					2,// bit_size
					1.00000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
						{3,"Final retransmission"},
						{1,"First retransmission"},
						{0,"Initial"},
						{2,"Second retransmission"}
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
					"Ais.Class.APosition.Report.Reserved",// generic_name
					8,// bit_position edited with low-can-generator
					5,// bit_size
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
					decoder_t::decode_bytes,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					""// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Ais.Class.APosition.Report.Sequence.Id",// generic_name
					0,// bit_position edited with low-can-generator
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
					decoder_t::decode_noop,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					""// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Ais.Class.APosition.Report.Sog",// generic_name
					80,// bit_position edited with low-can-generator
					16,// bit_size
					0.0100000f,// factor
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
					"m/s"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Ais.Class.APosition.Report.Special.Maneuver.Indicator",// generic_name
					18,// bit_position edited with low-can-generator
					2,// bit_size
					1.00000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
						{2,"Engaged in special maneuver"},
						{0,"Not available"},
						{1,"Not engaged in special maneuver"},
						{3,"Reserverd"}
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
					"Ais.Class.APosition.Report.Time.Stamp",// generic_name
					112,// bit_position edited with low-can-generator
					6,// bit_size
					1.00000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
						{62,"Dead reckoning mode"},
						{61,"Manual input mode"},
						{60,"Not available"},
						{63,"Positioning system is inoperative"}
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
					"Ais.Class.APosition.Report.User.Id",// generic_name
					184,// bit_position edited with low-can-generator
					32,// bit_size
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
					decoder_t::decode_noop,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					"MMSI"// unit
				})}
			} // end signals vector
		})} // end message_definition entry
,		{std::make_shared<message_definition_t>(message_definition_t{"j1939",129039,"Ais.Class.BPosition.Report",26,136,frequency_clock_t(5.00000f),true,
			{ // beginning signals vector
				{std::make_shared<signal_t> (signal_t{
					"Ais.Class.BPosition.Report.Ais.Communication.State",// generic_name
					7,// bit_position edited with low-can-generator
					1,// bit_size
					1.00000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
						{1,"ITDMA"},
						{0,"SOTDMA"}
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
					"Ais.Class.BPosition.Report.Ais.Mode",// generic_name
					8,// bit_position edited with low-can-generator
					1,// bit_size
					1.00000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
						{1,"Assigned"},
						{0,"Autonomous"}
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
					"Ais.Class.BPosition.Report.Ais.Transceiver.Information",// generic_name
					40,// bit_position edited with low-can-generator
					5,// bit_size
					1.00000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
						{0,"Channel A VDL reception"},
						{2,"Channel A VDL transmission"},
						{1,"Channel B VDL reception"},
						{3,"Channel B VDL transmission"},
						{4,"Own information not broadcast"},
						{5,"Reserved"}
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
					"Ais.Class.BPosition.Report.Band",// generic_name
					10,// bit_position edited with low-can-generator
					1,// bit_size
					1.00000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
						{1,"entire marine band"},
						{0,"top 525 kHz of marine band"}
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
					"Ais.Class.BPosition.Report.Can.Handle.Msg22",// generic_name
					9,// bit_position edited with low-can-generator
					1,// bit_size
					1.00000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
						{2,"Error"},
						{0,"No"},
						{3,"Unavailable"},
						{1,"Yes"}
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
					"Ais.Class.BPosition.Report.Cog",// generic_name
					80,// bit_position edited with low-can-generator
					16,// bit_size
					0.000100000f,// factor
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
					"rad"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Ais.Class.BPosition.Report.Communication.State",// generic_name
					45,// bit_position edited with low-can-generator
					19,// bit_size
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
					decoder_t::decode_bytes,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					""// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Ais.Class.BPosition.Report.Dsc",// generic_name
					11,// bit_position edited with low-can-generator
					1,// bit_size
					1.00000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
						{2,"Error"},
						{0,"No"},
						{3,"Unavailable"},
						{1,"Yes"}
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
					"Ais.Class.BPosition.Report.Heading",// generic_name
					24,// bit_position edited with low-can-generator
					16,// bit_size
					0.000100000f,// factor
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
					"rad"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Ais.Class.BPosition.Report.Integrated.Display",// generic_name
					12,// bit_position edited with low-can-generator
					1,// bit_size
					1.00000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
						{2,"Error"},
						{0,"No"},
						{3,"Unavailable"},
						{1,"Yes"}
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
					"Ais.Class.BPosition.Report.Latitude",// generic_name
					104,// bit_position edited with low-can-generator
					32,// bit_size
					1.00000e-07f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
					},// states
					false,// writable
					decoder_t::decode_noop,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(2),// signed
					-1,// bit_sign_position
					"deg"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Ais.Class.BPosition.Report.Longitude",// generic_name
					136,// bit_position edited with low-can-generator
					32,// bit_size
					1.00000e-07f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
					},// states
					false,// writable
					decoder_t::decode_noop,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(2),// signed
					-1,// bit_sign_position
					"deg"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Ais.Class.BPosition.Report.Message.Id",// generic_name
					202,// bit_position edited with low-can-generator
					6,// bit_size
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
					"Ais.Class.BPosition.Report.Position.Accuracy",// generic_name
					103,// bit_position edited with low-can-generator
					1,// bit_size
					1.00000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
						{1,"High"},
						{0,"Low"}
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
					"Ais.Class.BPosition.Report.Raim",// generic_name
					102,// bit_position edited with low-can-generator
					1,// bit_size
					1.00000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
						{1,"in use"},
						{0,"not in use"}
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
					"Ais.Class.BPosition.Report.Regional.Application",// generic_name
					14,// bit_position edited with low-can-generator
					2,// bit_size
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
					"Ais.Class.BPosition.Report.Repeat.Indicator",// generic_name
					200,// bit_position edited with low-can-generator
					2,// bit_size
					1.00000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
						{3,"Final retransmission"},
						{1,"First retransmission"},
						{0,"Initial"},
						{2,"Second retransmission"}
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
					"Ais.Class.BPosition.Report.Sog",// generic_name
					64,// bit_position edited with low-can-generator
					16,// bit_size
					0.0100000f,// factor
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
					"m/s"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Ais.Class.BPosition.Report.Time.Stamp",// generic_name
					96,// bit_position edited with low-can-generator
					6,// bit_size
					1.00000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
						{62,"Dead reckoning mode"},
						{61,"Manual input mode"},
						{60,"Not available"},
						{63,"Positioning system is inoperative"}
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
					"Ais.Class.BPosition.Report.Unit.Type",// generic_name
					13,// bit_position edited with low-can-generator
					1,// bit_size
					1.00000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
						{1,"CS"},
						{0,"SOTDMA"}
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
					"Ais.Class.BPosition.Report.User.Id",// generic_name
					168,// bit_position edited with low-can-generator
					32,// bit_size
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
					decoder_t::decode_noop,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					"MMSI"// unit
				})}
			} // end signals vector
		})} // end message_definition entry
,		{std::make_shared<message_definition_t>(message_definition_t{"j1939",129040,"Ais.Class.BExtended.Position.Report",33,136,frequency_clock_t(5.00000f),true,
			{ // beginning signals vector
				{std::make_shared<signal_t> (signal_t{
					"Ais.Class.BExtended.Position.Report.Ais.Mode",// generic_name
					4294967142,// bit_position edited with low-can-generator
					1,// bit_size
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
					"Ais.Class.BExtended.Position.Report.Ais.Raim.Flag",// generic_name
					158,// bit_position edited with low-can-generator
					1,// bit_size
					1.00000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
						{1,"in use"},
						{0,"not in use"}
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
					"Ais.Class.BExtended.Position.Report.Ais.Transceiver.Information",// generic_name
					4294967133,// bit_position edited with low-can-generator
					5,// bit_size
					1.00000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
						{0,"Channel A VDL reception"},
						{2,"Channel A VDL transmission"},
						{1,"Channel B VDL reception"},
						{3,"Channel B VDL transmission"},
						{4,"Own information not broadcast"},
						{5,"Reserved"}
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
					"Ais.Class.BExtended.Position.Report.Beam",// generic_name
					40,// bit_position edited with low-can-generator
					16,// bit_size
					0.100000f,// factor
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
					"m"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Ais.Class.BExtended.Position.Report.Cog",// generic_name
					136,// bit_position edited with low-can-generator
					16,// bit_size
					0.000100000f,// factor
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
					"rad"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Ais.Class.BExtended.Position.Report.Dte",// generic_name
					4294967143,// bit_position edited with low-can-generator
					1,// bit_size
					1.00000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
						{0,"Available"},
						{1,"Not available"}
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
					"Ais.Class.BExtended.Position.Report.Gnss.Type",// generic_name
					72,// bit_position edited with low-can-generator
					4,// bit_size
					1.00000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
						{5,"Chayka"},
						{2,"GLONASS"},
						{1,"GPS"},
						{3,"GPS+GLONASS"},
						{8,"Galileo"},
						{4,"Loran-C"},
						{6,"integrated"},
						{7,"surveyed"},
						{0,"undefined"}
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
					"Ais.Class.BExtended.Position.Report.Latitude",// generic_name
					160,// bit_position edited with low-can-generator
					32,// bit_size
					1.00000e-07f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
					},// states
					false,// writable
					decoder_t::decode_noop,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(2),// signed
					-1,// bit_sign_position
					"deg"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Ais.Class.BExtended.Position.Report.Length",// generic_name
					56,// bit_position edited with low-can-generator
					16,// bit_size
					0.100000f,// factor
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
					"m"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Ais.Class.BExtended.Position.Report.Longitude",// generic_name
					192,// bit_position edited with low-can-generator
					32,// bit_size
					1.00000e-07f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
					},// states
					false,// writable
					decoder_t::decode_noop,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(2),// signed
					-1,// bit_sign_position
					"deg"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Ais.Class.BExtended.Position.Report.Message.Id",// generic_name
					258,// bit_position edited with low-can-generator
					6,// bit_size
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
					"Ais.Class.BExtended.Position.Report.Name",// generic_name
					4294967144,// bit_position edited with low-can-generator
					160,// bit_size
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
					decoder_t::decode_ascii,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					""// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Ais.Class.BExtended.Position.Report.Position.Accuracy",// generic_name
					159,// bit_position edited with low-can-generator
					1,// bit_size
					1.00000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
						{1,"High"},
						{0,"Low"}
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
					"Ais.Class.BExtended.Position.Report.Position.Reference.From.Bow",// generic_name
					8,// bit_position edited with low-can-generator
					16,// bit_size
					0.100000f,// factor
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
					"m"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Ais.Class.BExtended.Position.Report.Position.Reference.From.Starboard",// generic_name
					24,// bit_position edited with low-can-generator
					16,// bit_size
					0.100000f,// factor
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
					"m"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Ais.Class.BExtended.Position.Report.Regional.Application",// generic_name
					108,// bit_position edited with low-can-generator
					4,// bit_size
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
					"Ais.Class.BExtended.Position.Report.Repeat.Indicator",// generic_name
					256,// bit_position edited with low-can-generator
					2,// bit_size
					1.00000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
						{3,"Final retransmission"},
						{1,"First retransmission"},
						{0,"Initial"},
						{2,"Second retransmission"}
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
					"Ais.Class.BExtended.Position.Report.Reserved",// generic_name
					4294967138,// bit_position edited with low-can-generator
					4,// bit_size
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
					decoder_t::decode_bytes,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					""// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Ais.Class.BExtended.Position.Report.Sog",// generic_name
					120,// bit_position edited with low-can-generator
					16,// bit_size
					0.0100000f,// factor
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
					"m/s"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Ais.Class.BExtended.Position.Report.Time.Stamp",// generic_name
					152,// bit_position edited with low-can-generator
					6,// bit_size
					1.00000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
						{62,"Dead reckoning mode"},
						{61,"Manual input mode"},
						{60,"Not available"},
						{63,"Positioning system is inoperative"}
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
					"Ais.Class.BExtended.Position.Report.True.Heading",// generic_name
					80,// bit_position edited with low-can-generator
					16,// bit_size
					0.000100000f,// factor
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
					"rad"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Ais.Class.BExtended.Position.Report.Type.Of.Ship",// generic_name
					96,// bit_position edited with low-can-generator
					8,// bit_size
					1.00000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
						{54,"Anti-pollution"},
						{70,"Cargo ship"},
						{79,"Cargo ship (no additional information)"},
						{71,"Cargo ship carrying dangerous goods"},
						{72,"Cargo ship hazard cat B"},
						{73,"Cargo ship hazard cat C"},
						{74,"Cargo ship hazard cat D"},
						{34,"Engaged in diving operations"},
						{33,"Engaged in dredging or underwater operations"},
						{35,"Engaged in military operations"},
						{30,"Fishing"},
						{40,"High speed craft"},
						{49,"High speed craft (no additional information)"},
						{41,"High speed craft carrying dangerous goods"},
						{42,"High speed craft hazard cat B"},
						{43,"High speed craft hazard cat C"},
						{44,"High speed craft hazard cat D"},
						{55,"Law enforcement"},
						{58,"Medical"},
						{90,"Other"},
						{99,"Other (no additional information)"},
						{91,"Other carrying dangerous goods"},
						{92,"Other hazard cat B"},
						{93,"Other hazard cat C"},
						{94,"Other hazard cat D"},
						{60,"Passenger ship"},
						{69,"Passenger ship (no additional information)"},
						{50,"Pilot vessel"},
						{37,"Pleasure"},
						{53,"Port tender"},
						{59,"RR Resolution No.18"},
						{51,"SAR"},
						{36,"Sailing"},
						{56,"Spare"},
						{57,"Spare #2"},
						{80,"Tanker"},
						{89,"Tanker (no additional information)"},
						{81,"Tanker carrying dangerous goods"},
						{82,"Tanker hazard cat B"},
						{83,"Tanker hazard cat C"},
						{84,"Tanker hazard cat D"},
						{31,"Towing"},
						{32,"Towing exceeds 200m or wider than 25m"},
						{52,"Tug"},
						{20,"Wing In Ground"},
						{29,"Wing In Ground (no other information)"},
						{0,"unavailable"}
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
					"Ais.Class.BExtended.Position.Report.User.Id",// generic_name
					224,// bit_position edited with low-can-generator
					32,// bit_size
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
					decoder_t::decode_noop,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					"MMSI"// unit
				})}
			} // end signals vector
		})} // end message_definition entry
,		{std::make_shared<message_definition_t>(message_definition_t{"j1939",129044,"Datum",24,136,frequency_clock_t(5.00000f),true,
			{ // beginning signals vector
				{std::make_shared<signal_t> (signal_t{
					"Datum.Delta.Altitude",// generic_name
					64,// bit_position edited with low-can-generator
					32,// bit_size
					1.00000e-06f,// factor
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
					static_cast<sign_t>(2),// signed
					-1,// bit_sign_position
					"m"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Datum.Delta.Latitude",// generic_name
					128,// bit_position edited with low-can-generator
					32,// bit_size
					1.00000e-07f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
					},// states
					false,// writable
					decoder_t::decode_noop,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(2),// signed
					-1,// bit_sign_position
					"deg"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Datum.Delta.Longitude",// generic_name
					96,// bit_position edited with low-can-generator
					32,// bit_size
					1.00000e-07f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
					},// states
					false,// writable
					decoder_t::decode_noop,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(2),// signed
					-1,// bit_sign_position
					"deg"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Datum.Local.Datum",// generic_name
					160,// bit_position edited with low-can-generator
					32,// bit_size
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
					decoder_t::decode_ascii,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					""// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Datum.Reference.Datum",// generic_name
					32,// bit_position edited with low-can-generator
					32,// bit_size
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
					decoder_t::decode_ascii,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					""// unit
				})}
			} // end signals vector
		})} // end message_definition entry
,		{std::make_shared<message_definition_t>(message_definition_t{"j1939",129283,"Cross.Track.Error",6,136,frequency_clock_t(5.00000f),true,
			{ // beginning signals vector
				{std::make_shared<signal_t> (signal_t{
					"Cross.Track.Error.Navigation.Terminated",// generic_name
					32,// bit_position edited with low-can-generator
					2,// bit_size
					1.00000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
						{2,"Error"},
						{0,"No"},
						{3,"Unavailable"},
						{1,"Yes"}
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
					"Cross.Track.Error.Reserved",// generic_name
					34,// bit_position edited with low-can-generator
					2,// bit_size
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
					decoder_t::decode_bytes,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					""// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Cross.Track.Error.Sid",// generic_name
					40,// bit_position edited with low-can-generator
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
					"Cross.Track.Error.Xte",// generic_name
					0,// bit_position edited with low-can-generator
					32,// bit_size
					0.0100000f,// factor
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
					static_cast<sign_t>(2),// signed
					-1,// bit_sign_position
					"m"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Cross.Track.Error.Xte.Mode",// generic_name
					36,// bit_position edited with low-can-generator
					4,// bit_size
					1.00000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
						{0,"Autonomous"},
						{1,"Differential enhanced"},
						{2,"Estimated"},
						{4,"Manual"},
						{3,"Simulator"}
					},// states
					false,// writable
					decoder_t::decode_state,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					""// unit
				})}
			} // end signals vector
		})} // end message_definition entry
,		{std::make_shared<message_definition_t>(message_definition_t{"j1939",129284,"Navigation.Data",34,136,frequency_clock_t(5.00000f),true,
			{ // beginning signals vector
				{std::make_shared<signal_t> (signal_t{
					"Navigation.Data.Arrival.Circle.Entered",// generic_name
					226,// bit_position edited with low-can-generator
					2,// bit_size
					1.00000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
						{2,"Error"},
						{0,"No"},
						{3,"Unavailable"},
						{1,"Yes"}
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
					"Navigation.Data.Bearing.Origin.To.Destination.Waypoint",// generic_name
					160,// bit_position edited with low-can-generator
					16,// bit_size
					0.000100000f,// factor
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
					"rad"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Navigation.Data.Bearing.Position.To.Destination.Waypoint",// generic_name
					144,// bit_position edited with low-can-generator
					16,// bit_size
					0.000100000f,// factor
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
					"rad"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Navigation.Data.Calculation.Type",// generic_name
					224,// bit_position edited with low-can-generator
					2,// bit_size
					1.00000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
						{0,"Great Circle"},
						{1,"Rhumb Line"}
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
					"Navigation.Data.Course.Bearing.Reference",// generic_name
					230,// bit_position edited with low-can-generator
					2,// bit_size
					1.00000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
						{2,"Error"},
						{1,"Magnetic"},
						{3,"Null"},
						{0,"True"}
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
					"Navigation.Data.Destination.Latitude",// generic_name
					48,// bit_position edited with low-can-generator
					32,// bit_size
					1.00000e-07f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
					},// states
					false,// writable
					decoder_t::decode_noop,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(2),// signed
					-1,// bit_sign_position
					"deg"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Navigation.Data.Destination.Longitude",// generic_name
					16,// bit_position edited with low-can-generator
					32,// bit_size
					1.00000e-07f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
					},// states
					false,// writable
					decoder_t::decode_noop,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(2),// signed
					-1,// bit_sign_position
					"deg"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Navigation.Data.Destination.Waypoint.Number",// generic_name
					80,// bit_position edited with low-can-generator
					32,// bit_size
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
					"Navigation.Data.Distance.To.Waypoint",// generic_name
					232,// bit_position edited with low-can-generator
					32,// bit_size
					0.0100000f,// factor
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
					"m"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Navigation.Data.Eta.Date",// generic_name
					176,// bit_position edited with low-can-generator
					16,// bit_size
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
					decoder_t::decode_date,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					"days"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Navigation.Data.Eta.Time",// generic_name
					192,// bit_position edited with low-can-generator
					32,// bit_size
					0.000100000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
					},// states
					false,// writable
					decoder_t::decode_time,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					"s"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Navigation.Data.Origin.Waypoint.Number",// generic_name
					112,// bit_position edited with low-can-generator
					32,// bit_size
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
					"Navigation.Data.Perpendicular.Crossed",// generic_name
					228,// bit_position edited with low-can-generator
					2,// bit_size
					1.00000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
						{2,"Error"},
						{0,"No"},
						{3,"Unavailable"},
						{1,"Yes"}
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
					"Navigation.Data.Sid",// generic_name
					264,// bit_position edited with low-can-generator
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
					"Navigation.Data.Waypoint.Closing.Velocity",// generic_name
					0,// bit_position edited with low-can-generator
					16,// bit_size
					0.0100000f,// factor
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
					static_cast<sign_t>(2),// signed
					-1,// bit_sign_position
					"m/s"// unit
				})}
			} // end signals vector
		})} // end message_definition entry
,		{std::make_shared<message_definition_t>(message_definition_t{"j1939",129291,"Set.Drift.Rapid.Update",8,136,frequency_clock_t(5.00000f),true,
			{ // beginning signals vector
				{std::make_shared<signal_t> (signal_t{
					"Set.Drift.Rapid.Update.Drift",// generic_name
					16,// bit_position edited with low-can-generator
					16,// bit_size
					0.0100000f,// factor
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
					"m/s"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Set.Drift.Rapid.Update.Reserved",// generic_name
					48,// bit_position edited with low-can-generator
					6,// bit_size
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
					decoder_t::decode_bytes,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					""// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Set.Drift.Rapid.Update.Set",// generic_name
					32,// bit_position edited with low-can-generator
					16,// bit_size
					0.000100000f,// factor
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
					"rad"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Set.Drift.Rapid.Update.Set.Reference",// generic_name
					54,// bit_position edited with low-can-generator
					2,// bit_size
					1.00000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
						{2,"Error"},
						{1,"Magnetic"},
						{3,"Null"},
						{0,"True"}
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
					"Set.Drift.Rapid.Update.Sid",// generic_name
					56,// bit_position edited with low-can-generator
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
,		{std::make_shared<message_definition_t>(message_definition_t{"j1939",129539,"Gnss.Dops",8,136,frequency_clock_t(5.00000f),true,
			{ // beginning signals vector
				{std::make_shared<signal_t> (signal_t{
					"Gnss.Dops.Actual.Mode",// generic_name
					50,// bit_position edited with low-can-generator
					3,// bit_size
					1.00000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
						{0,"1D"},
						{1,"2D"},
						{2,"3D"},
						{3,"Auto"},
						{6,"Error"},
						{5,"Reserved"}
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
					"Gnss.Dops.Desired.Mode",// generic_name
					53,// bit_position edited with low-can-generator
					3,// bit_size
					1.00000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
						{0,"1D"},
						{1,"2D"},
						{2,"3D"},
						{3,"Auto"},
						{6,"Error"},
						{5,"Reserved"}
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
					"Gnss.Dops.Hdop",// generic_name
					32,// bit_position edited with low-can-generator
					16,// bit_size
					0.0100000f,// factor
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
					static_cast<sign_t>(2),// signed
					-1,// bit_sign_position
					""// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Gnss.Dops.Reserved",// generic_name
					48,// bit_position edited with low-can-generator
					2,// bit_size
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
					decoder_t::decode_bytes,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					""// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Gnss.Dops.Sid",// generic_name
					56,// bit_position edited with low-can-generator
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
					"Gnss.Dops.Tdop",// generic_name
					0,// bit_position edited with low-can-generator
					16,// bit_size
					0.0100000f,// factor
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
					static_cast<sign_t>(2),// signed
					-1,// bit_sign_position
					""// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Gnss.Dops.Vdop",// generic_name
					16,// bit_position edited with low-can-generator
					16,// bit_size
					0.0100000f,// factor
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
					static_cast<sign_t>(2),// signed
					-1,// bit_sign_position
					""// unit
				})}
			} // end signals vector
		})} // end message_definition entry
,		{std::make_shared<message_definition_t>(message_definition_t{"j1939",129540,"Gnss.Sats.In.View",255,136,frequency_clock_t(5.00000f),true,
			{ // beginning signals vector
				{std::make_shared<signal_t> (signal_t{
					"Gnss.Sats.In.View.Azimuth",// generic_name
					1976,// bit_position edited with low-can-generator
					16,// bit_size
					0.000100000f,// factor
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
					"rad"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Gnss.Sats.In.View.Elevation",// generic_name
					1992,// bit_position edited with low-can-generator
					16,// bit_size
					0.000100000f,// factor
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
					"rad"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Gnss.Sats.In.View.Mode",// generic_name
					2030,// bit_position edited with low-can-generator
					2,// bit_size
					1.00000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
						{3,"Range residuals used to calculate position"}
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
					"Gnss.Sats.In.View.Prn",// generic_name
					2008,// bit_position edited with low-can-generator
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
					"Gnss.Sats.In.View.Range.Residuals",// generic_name
					1928,// bit_position edited with low-can-generator
					32,// bit_size
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
					static_cast<sign_t>(2),// signed
					-1,// bit_sign_position
					""// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Gnss.Sats.In.View.Reserved",// generic_name
					1920,// bit_position edited with low-can-generator
					4,// bit_size
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
					decoder_t::decode_bytes,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					""// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Gnss.Sats.In.View.Sats.In.View",// generic_name
					2016,// bit_position edited with low-can-generator
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
					"Gnss.Sats.In.View.Sid",// generic_name
					2032,// bit_position edited with low-can-generator
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
					"Gnss.Sats.In.View.Snr",// generic_name
					1960,// bit_position edited with low-can-generator
					16,// bit_size
					0.0100000f,// factor
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
					"dB"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Gnss.Sats.In.View.Status",// generic_name
					1924,// bit_position edited with low-can-generator
					4,// bit_size
					1.00000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
						{0,"Not tracked"},
						{3,"Not tracked+Diff"},
						{1,"Tracked"},
						{4,"Tracked+Diff"},
						{2,"Used"},
						{5,"Used+Diff"}
					},// states
					false,// writable
					decoder_t::decode_state,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					""// unit
				})}
			} // end signals vector
		})} // end message_definition entry
,		{std::make_shared<message_definition_t>(message_definition_t{"j1939",129793,"Ais.Utc.And.Date.Report",26,136,frequency_clock_t(5.00000f),true,
			{ // beginning signals vector
				{std::make_shared<signal_t> (signal_t{
					"Ais.Utc.And.Date.Report.Ais.Transceiver.Information",// generic_name
					40,// bit_position edited with low-can-generator
					5,// bit_size
					1.00000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
						{0,"Channel A VDL reception"},
						{2,"Channel A VDL transmission"},
						{1,"Channel B VDL reception"},
						{3,"Channel B VDL transmission"},
						{4,"Own information not broadcast"},
						{5,"Reserved"}
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
					"Ais.Utc.And.Date.Report.Communication.State",// generic_name
					45,// bit_position edited with low-can-generator
					19,// bit_size
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
					decoder_t::decode_bytes,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					""// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Ais.Utc.And.Date.Report.Gnss.Type",// generic_name
					16,// bit_position edited with low-can-generator
					4,// bit_size
					1.00000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
						{5,"Chayka"},
						{2,"GLONASS"},
						{1,"GPS"},
						{3,"GPS+GLONASS"},
						{8,"Galileo"},
						{4,"Loran-C"},
						{6,"integrated"},
						{7,"surveyed"},
						{0,"undefined"}
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
					"Ais.Utc.And.Date.Report.Latitude",// generic_name
					104,// bit_position edited with low-can-generator
					32,// bit_size
					1.00000e-07f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
					},// states
					false,// writable
					decoder_t::decode_noop,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(2),// signed
					-1,// bit_sign_position
					"deg"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Ais.Utc.And.Date.Report.Longitude",// generic_name
					136,// bit_position edited with low-can-generator
					32,// bit_size
					1.00000e-07f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
					},// states
					false,// writable
					decoder_t::decode_noop,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(2),// signed
					-1,// bit_sign_position
					"deg"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Ais.Utc.And.Date.Report.Message.Id",// generic_name
					202,// bit_position edited with low-can-generator
					6,// bit_size
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
					"Ais.Utc.And.Date.Report.Position.Accuracy",// generic_name
					103,// bit_position edited with low-can-generator
					1,// bit_size
					1.00000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
						{1,"High"},
						{0,"Low"}
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
					"Ais.Utc.And.Date.Report.Position.Date",// generic_name
					24,// bit_position edited with low-can-generator
					16,// bit_size
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
					decoder_t::decode_date,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					"days"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Ais.Utc.And.Date.Report.Position.Time",// generic_name
					64,// bit_position edited with low-can-generator
					32,// bit_size
					0.000100000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
					},// states
					false,// writable
					decoder_t::decode_time,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					"s"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Ais.Utc.And.Date.Report.Raim",// generic_name
					102,// bit_position edited with low-can-generator
					1,// bit_size
					1.00000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
						{1,"in use"},
						{0,"not in use"}
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
					"Ais.Utc.And.Date.Report.Repeat.Indicator",// generic_name
					200,// bit_position edited with low-can-generator
					2,// bit_size
					1.00000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
						{3,"Final retransmission"},
						{1,"First retransmission"},
						{0,"Initial"},
						{2,"Second retransmission"}
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
					"Ais.Utc.And.Date.Report.Reserved",// generic_name
					20,// bit_position edited with low-can-generator
					4,// bit_size
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
					decoder_t::decode_bytes,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					""// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Ais.Utc.And.Date.Report.Spare",// generic_name
					8,// bit_position edited with low-can-generator
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
					decoder_t::decode_bytes,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					""// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Ais.Utc.And.Date.Report.User.Id",// generic_name
					168,// bit_position edited with low-can-generator
					32,// bit_size
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
					decoder_t::decode_noop,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					"MMSI"// unit
				})}
			} // end signals vector
		})} // end message_definition entry
,		{std::make_shared<message_definition_t>(message_definition_t{"j1939",129794,"Ais.Class.AStatic.And.Voyage.Related.Data",24,136,frequency_clock_t(5.00000f),true,
			{ // beginning signals vector
				{std::make_shared<signal_t> (signal_t{
					"Ais.Class.AStatic.And.Voyage.Related.Data.Ais.Transceiver.Information",// generic_name
					4294966891,// bit_position edited with low-can-generator
					5,// bit_size
					1.00000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
						{0,"Channel A VDL reception"},
						{2,"Channel A VDL transmission"},
						{1,"Channel B VDL reception"},
						{3,"Channel B VDL transmission"},
						{4,"Own information not broadcast"},
						{5,"Reserved"}
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
					"Ais.Class.AStatic.And.Voyage.Related.Data.Ais.Version.Indicator",// generic_name
					4294966902,// bit_position edited with low-can-generator
					2,// bit_size
					1.00000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
						{0,"ITU-R M.1371-1"},
						{1,"ITU-R M.1371-3"}
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
					"Ais.Class.AStatic.And.Voyage.Related.Data.Beam",// generic_name
					4294967160,// bit_position edited with low-can-generator
					16,// bit_size
					0.100000f,// factor
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
					"m"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Ais.Class.AStatic.And.Voyage.Related.Data.Callsign",// generic_name
					64,// bit_position edited with low-can-generator
					56,// bit_size
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
					decoder_t::decode_ascii,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					""// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Ais.Class.AStatic.And.Voyage.Related.Data.Destination",// generic_name
					4294966904,// bit_position edited with low-can-generator
					160,// bit_size
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
					decoder_t::decode_ascii,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					""// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Ais.Class.AStatic.And.Voyage.Related.Data.Draft",// generic_name
					4294967064,// bit_position edited with low-can-generator
					16,// bit_size
					0.0100000f,// factor
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
					"m"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Ais.Class.AStatic.And.Voyage.Related.Data.Dte",// generic_name
					4294966897,// bit_position edited with low-can-generator
					1,// bit_size
					1.00000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
						{0,"available"},
						{1,"not available"}
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
					"Ais.Class.AStatic.And.Voyage.Related.Data.Eta.Date",// generic_name
					4294967112,// bit_position edited with low-can-generator
					16,// bit_size
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
					decoder_t::decode_date,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					"days"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Ais.Class.AStatic.And.Voyage.Related.Data.Eta.Time",// generic_name
					4294967080,// bit_position edited with low-can-generator
					32,// bit_size
					0.000100000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
					},// states
					false,// writable
					decoder_t::decode_time,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					"s"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Ais.Class.AStatic.And.Voyage.Related.Data.Gnss.Type",// generic_name
					4294966898,// bit_position edited with low-can-generator
					4,// bit_size
					1.00000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
						{5,"Chayka"},
						{2,"GLONASS"},
						{1,"GPS"},
						{3,"GPS+GLONASS"},
						{8,"Galileo"},
						{4,"Loran-C"},
						{6,"integrated"},
						{7,"surveyed"},
						{0,"undefined"}
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
					"Ais.Class.AStatic.And.Voyage.Related.Data.Imo.Number",// generic_name
					120,// bit_position edited with low-can-generator
					32,// bit_size
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
					decoder_t::decode_noop,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					""// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Ais.Class.AStatic.And.Voyage.Related.Data.Length",// generic_name
					4294967176,// bit_position edited with low-can-generator
					16,// bit_size
					0.100000f,// factor
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
					"m"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Ais.Class.AStatic.And.Voyage.Related.Data.Message.Id",// generic_name
					186,// bit_position edited with low-can-generator
					6,// bit_size
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
					"Ais.Class.AStatic.And.Voyage.Related.Data.Name",// generic_name
					4294967200,// bit_position edited with low-can-generator
					160,// bit_size
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
					decoder_t::decode_ascii,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					""// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Ais.Class.AStatic.And.Voyage.Related.Data.Position.Reference.From.Bow",// generic_name
					4294967128,// bit_position edited with low-can-generator
					16,// bit_size
					0.100000f,// factor
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
					"m"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Ais.Class.AStatic.And.Voyage.Related.Data.Position.Reference.From.Starboard",// generic_name
					4294967144,// bit_position edited with low-can-generator
					16,// bit_size
					0.100000f,// factor
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
					"m"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Ais.Class.AStatic.And.Voyage.Related.Data.Repeat.Indicator",// generic_name
					184,// bit_position edited with low-can-generator
					2,// bit_size
					1.00000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
						{3,"Final retransmission"},
						{1,"First retransmission"},
						{0,"Initial"},
						{2,"Second retransmission"}
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
					"Ais.Class.AStatic.And.Voyage.Related.Data.Reserved",// generic_name
					4294966896,// bit_position edited with low-can-generator
					1,// bit_size
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
					decoder_t::decode_bytes,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					""// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Ais.Class.AStatic.And.Voyage.Related.Data.Type.Of.Ship",// generic_name
					4294967192,// bit_position edited with low-can-generator
					8,// bit_size
					1.00000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
						{54,"Anti-pollution"},
						{70,"Cargo ship"},
						{79,"Cargo ship (no additional information)"},
						{71,"Cargo ship carrying dangerous goods"},
						{72,"Cargo ship hazard cat B"},
						{73,"Cargo ship hazard cat C"},
						{74,"Cargo ship hazard cat D"},
						{34,"Engaged in diving operations"},
						{33,"Engaged in dredging or underwater operations"},
						{35,"Engaged in military operations"},
						{30,"Fishing"},
						{40,"High speed craft"},
						{49,"High speed craft (no additional information)"},
						{41,"High speed craft carrying dangerous goods"},
						{42,"High speed craft hazard cat B"},
						{43,"High speed craft hazard cat C"},
						{44,"High speed craft hazard cat D"},
						{55,"Law enforcement"},
						{58,"Medical"},
						{90,"Other"},
						{99,"Other (no additional information)"},
						{91,"Other carrying dangerous goods"},
						{92,"Other hazard cat B"},
						{93,"Other hazard cat C"},
						{94,"Other hazard cat D"},
						{60,"Passenger ship"},
						{69,"Passenger ship (no additional information)"},
						{50,"Pilot vessel"},
						{37,"Pleasure"},
						{53,"Port tender"},
						{59,"RR Resolution No.18"},
						{51,"SAR"},
						{36,"Sailing"},
						{56,"Spare"},
						{57,"Spare #2"},
						{80,"Tanker"},
						{89,"Tanker (no additional information)"},
						{81,"Tanker carrying dangerous goods"},
						{82,"Tanker hazard cat B"},
						{83,"Tanker hazard cat C"},
						{84,"Tanker hazard cat D"},
						{31,"Towing"},
						{32,"Towing exceeds 200m or wider than 25m"},
						{52,"Tug"},
						{20,"Wing In Ground"},
						{29,"Wing In Ground (no other information)"},
						{0,"unavailable"}
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
					"Ais.Class.AStatic.And.Voyage.Related.Data.User.Id",// generic_name
					152,// bit_position edited with low-can-generator
					32,// bit_size
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
					decoder_t::decode_noop,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					"MMSI"// unit
				})}
			} // end signals vector
		})} // end message_definition entry
,		{std::make_shared<message_definition_t>(message_definition_t{"j1939",129798,"Ais.Sar.Aircraft.Position.Report",8,136,frequency_clock_t(5.00000f),true,
			{ // beginning signals vector
				{std::make_shared<signal_t> (signal_t{
					"Ais.Sar.Aircraft.Position.Report.Ais.Transceiver.Information",// generic_name
					4294967192,// bit_position edited with low-can-generator
					5,// bit_size
					1.00000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
						{0,"Channel A VDL reception"},
						{2,"Channel A VDL transmission"},
						{1,"Channel B VDL reception"},
						{3,"Channel B VDL transmission"},
						{4,"Own information not broadcast"},
						{5,"Reserved"}
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
					"Ais.Sar.Aircraft.Position.Report.Altitude",// generic_name
					4294967128,// bit_position edited with low-can-generator
					64,// bit_size
					1.00000e-06f,// factor
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
					static_cast<sign_t>(2),// signed
					-1,// bit_sign_position
					"m"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Ais.Sar.Aircraft.Position.Report.Cog",// generic_name
					4294967232,// bit_position edited with low-can-generator
					16,// bit_size
					0.000100000f,// factor
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
					"rad"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Ais.Sar.Aircraft.Position.Report.Communication.State",// generic_name
					4294967197,// bit_position edited with low-can-generator
					19,// bit_size
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
					decoder_t::decode_bytes,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					""// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Ais.Sar.Aircraft.Position.Report.Dte",// generic_name
					4294967119,// bit_position edited with low-can-generator
					1,// bit_size
					1.00000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
						{0,"Available"},
						{1,"Not available"}
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
					"Ais.Sar.Aircraft.Position.Report.Latitude",// generic_name
					4294967256,// bit_position edited with low-can-generator
					32,// bit_size
					1.00000e-07f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
					},// states
					false,// writable
					decoder_t::decode_noop,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(2),// signed
					-1,// bit_sign_position
					"deg"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Ais.Sar.Aircraft.Position.Report.Longitude",// generic_name
					4294967288,// bit_position edited with low-can-generator
					32,// bit_size
					1.00000e-07f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
					},// states
					false,// writable
					decoder_t::decode_noop,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(2),// signed
					-1,// bit_sign_position
					"deg"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Ais.Sar.Aircraft.Position.Report.Message.Id",// generic_name
					58,// bit_position edited with low-can-generator
					6,// bit_size
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
					"Ais.Sar.Aircraft.Position.Report.Position.Accuracy",// generic_name
					4294967255,// bit_position edited with low-can-generator
					1,// bit_size
					1.00000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
						{1,"High"},
						{0,"Low"}
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
					"Ais.Sar.Aircraft.Position.Report.Raim",// generic_name
					4294967254,// bit_position edited with low-can-generator
					1,// bit_size
					1.00000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
						{1,"in use"},
						{0,"not in use"}
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
					"Ais.Sar.Aircraft.Position.Report.Repeat.Indicator",// generic_name
					56,// bit_position edited with low-can-generator
					2,// bit_size
					1.00000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
						{3,"Final retransmission"},
						{1,"First retransmission"},
						{0,"Initial"},
						{2,"Second retransmission"}
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
					"Ais.Sar.Aircraft.Position.Report.Reserved",// generic_name
					4294967112,// bit_position edited with low-can-generator
					7,// bit_size
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
					decoder_t::decode_bytes,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					""// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Ais.Sar.Aircraft.Position.Report.Reserved.For.Regional.Applications",// generic_name
					4294967120,// bit_position edited with low-can-generator
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
					decoder_t::decode_bytes,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					""// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Ais.Sar.Aircraft.Position.Report.Sog",// generic_name
					4294967216,// bit_position edited with low-can-generator
					16,// bit_size
					0.100000f,// factor
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
					"m/s"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Ais.Sar.Aircraft.Position.Report.Time.Stamp",// generic_name
					4294967248,// bit_position edited with low-can-generator
					6,// bit_size
					1.00000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
						{62,"Dead reckoning mode"},
						{61,"Manual input mode"},
						{60,"Not available"},
						{63,"Positioning system is inoperative"}
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
					"Ais.Sar.Aircraft.Position.Report.User.Id",// generic_name
					24,// bit_position edited with low-can-generator
					32,// bit_size
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
					decoder_t::decode_noop,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					"MMSI"// unit
				})}
			} // end signals vector
		})} // end message_definition entry
,		{std::make_shared<message_definition_t>(message_definition_t{"j1939",129809,"Ais.Class.BStatic.Data.Msg24.Part.A",27,136,frequency_clock_t(5.00000f),true,
			{ // beginning signals vector
				{std::make_shared<signal_t> (signal_t{
					"Ais.Class.BStatic.Data.Msg24.Part.A.Ais.Transceiver.Information",// generic_name
					11,// bit_position edited with low-can-generator
					5,// bit_size
					1.00000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
						{0,"Channel A VDL reception"},
						{2,"Channel A VDL transmission"},
						{1,"Channel B VDL reception"},
						{3,"Channel B VDL transmission"},
						{4,"Own information not broadcast"},
						{5,"Reserved"}
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
					"Ais.Class.BStatic.Data.Msg24.Part.A.Message.Id",// generic_name
					210,// bit_position edited with low-can-generator
					6,// bit_size
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
					"Ais.Class.BStatic.Data.Msg24.Part.A.Name",// generic_name
					16,// bit_position edited with low-can-generator
					160,// bit_size
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
					decoder_t::decode_ascii,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					""// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Ais.Class.BStatic.Data.Msg24.Part.A.Repeat.Indicator",// generic_name
					208,// bit_position edited with low-can-generator
					2,// bit_size
					1.00000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
						{3,"Final retransmission"},
						{1,"First retransmission"},
						{0,"Initial"},
						{2,"Second retransmission"}
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
					"Ais.Class.BStatic.Data.Msg24.Part.A.Reserved",// generic_name
					8,// bit_position edited with low-can-generator
					3,// bit_size
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
					decoder_t::decode_bytes,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					""// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Ais.Class.BStatic.Data.Msg24.Part.A.Sequence.Id",// generic_name
					0,// bit_position edited with low-can-generator
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
					decoder_t::decode_noop,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					""// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Ais.Class.BStatic.Data.Msg24.Part.A.User.Id",// generic_name
					176,// bit_position edited with low-can-generator
					32,// bit_size
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
					decoder_t::decode_noop,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					"MMSI"// unit
				})}
			} // end signals vector
		})} // end message_definition entry
,		{std::make_shared<message_definition_t>(message_definition_t{"j1939",129810,"Ais.Class.BStatic.Data.Msg24.Part.B",34,136,frequency_clock_t(5.00000f),true,
			{ // beginning signals vector
				{std::make_shared<signal_t> (signal_t{
					"Ais.Class.BStatic.Data.Msg24.Part.B.Ais.Transceiver.Information",// generic_name
					3,// bit_position edited with low-can-generator
					5,// bit_size
					1.00000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
						{0,"Channel A VDL reception"},
						{2,"Channel A VDL transmission"},
						{1,"Channel B VDL reception"},
						{3,"Channel B VDL transmission"},
						{4,"Own information not broadcast"},
						{5,"Reserved"}
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
					"Ais.Class.BStatic.Data.Msg24.Part.B.Beam",// generic_name
					80,// bit_position edited with low-can-generator
					16,// bit_size
					0.100000f,// factor
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
					"m"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Ais.Class.BStatic.Data.Msg24.Part.B.Callsign",// generic_name
					112,// bit_position edited with low-can-generator
					56,// bit_size
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
					decoder_t::decode_ascii,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					""// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Ais.Class.BStatic.Data.Msg24.Part.B.Length",// generic_name
					96,// bit_position edited with low-can-generator
					16,// bit_size
					0.100000f,// factor
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
					"m"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Ais.Class.BStatic.Data.Msg24.Part.B.Message.Id",// generic_name
					266,// bit_position edited with low-can-generator
					6,// bit_size
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
					"Ais.Class.BStatic.Data.Msg24.Part.B.Mothership.User.Id",// generic_name
					16,// bit_position edited with low-can-generator
					32,// bit_size
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
					decoder_t::decode_noop,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					"MMSI"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Ais.Class.BStatic.Data.Msg24.Part.B.Position.Reference.From.Bow",// generic_name
					48,// bit_position edited with low-can-generator
					16,// bit_size
					0.100000f,// factor
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
					"m"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Ais.Class.BStatic.Data.Msg24.Part.B.Position.Reference.From.Starboard",// generic_name
					64,// bit_position edited with low-can-generator
					16,// bit_size
					0.100000f,// factor
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
					"m"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Ais.Class.BStatic.Data.Msg24.Part.B.Repeat.Indicator",// generic_name
					264,// bit_position edited with low-can-generator
					2,// bit_size
					1.00000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
						{3,"Final retransmission"},
						{1,"First retransmission"},
						{0,"Initial"},
						{2,"Second retransmission"}
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
					"Ais.Class.BStatic.Data.Msg24.Part.B.Reserved",// generic_name
					0,// bit_position edited with low-can-generator
					3,// bit_size
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
					decoder_t::decode_bytes,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					""// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Ais.Class.BStatic.Data.Msg24.Part.B.Sequence.Id",// generic_name
					4294967288,// bit_position edited with low-can-generator
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
					decoder_t::decode_noop,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					""// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Ais.Class.BStatic.Data.Msg24.Part.B.Spare",// generic_name
					8,// bit_position edited with low-can-generator
					6,// bit_size
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
					decoder_t::decode_noop,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					""// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Ais.Class.BStatic.Data.Msg24.Part.B.Type.Of.Ship",// generic_name
					224,// bit_position edited with low-can-generator
					8,// bit_size
					1.00000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
						{54,"Anti-pollution"},
						{70,"Cargo ship"},
						{79,"Cargo ship (no additional information)"},
						{71,"Cargo ship carrying dangerous goods"},
						{72,"Cargo ship hazard cat B"},
						{73,"Cargo ship hazard cat C"},
						{74,"Cargo ship hazard cat D"},
						{34,"Engaged in diving operations"},
						{33,"Engaged in dredging or underwater operations"},
						{35,"Engaged in military operations"},
						{30,"Fishing"},
						{40,"High speed craft"},
						{49,"High speed craft (no additional information)"},
						{41,"High speed craft carrying dangerous goods"},
						{42,"High speed craft hazard cat B"},
						{43,"High speed craft hazard cat C"},
						{44,"High speed craft hazard cat D"},
						{55,"Law enforcement"},
						{58,"Medical"},
						{90,"Other"},
						{99,"Other (no additional information)"},
						{91,"Other carrying dangerous goods"},
						{92,"Other hazard cat B"},
						{93,"Other hazard cat C"},
						{94,"Other hazard cat D"},
						{60,"Passenger ship"},
						{69,"Passenger ship (no additional information)"},
						{50,"Pilot vessel"},
						{37,"Pleasure"},
						{53,"Port tender"},
						{59,"RR Resolution No.18"},
						{51,"SAR"},
						{36,"Sailing"},
						{56,"Spare"},
						{57,"Spare #2"},
						{80,"Tanker"},
						{89,"Tanker (no additional information)"},
						{81,"Tanker carrying dangerous goods"},
						{82,"Tanker hazard cat B"},
						{83,"Tanker hazard cat C"},
						{84,"Tanker hazard cat D"},
						{31,"Towing"},
						{32,"Towing exceeds 200m or wider than 25m"},
						{52,"Tug"},
						{20,"Wing In Ground"},
						{29,"Wing In Ground (no other information)"},
						{0,"unavailable"}
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
					"Ais.Class.BStatic.Data.Msg24.Part.B.User.Id",// generic_name
					232,// bit_position edited with low-can-generator
					32,// bit_size
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
					decoder_t::decode_noop,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					"MMSI"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Ais.Class.BStatic.Data.Msg24.Part.B.Vendor.Id",// generic_name
					168,// bit_position edited with low-can-generator
					56,// bit_size
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
					decoder_t::decode_ascii,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					""// unit
				})}
			} // end signals vector
		})} // end message_definition entry
,		{std::make_shared<message_definition_t>(message_definition_t{"j1939",130306,"Wind.Data",8,136,frequency_clock_t(5.00000f),true,
			{ // beginning signals vector
				{std::make_shared<signal_t> (signal_t{
					"Wind.Data.Reference",// generic_name
					21,// bit_position edited with low-can-generator
					3,// bit_size
					1.00000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
						{2,"Apparent"},
						{1,"Magnetic (ground referenced to Magnetic North)"},
						{3,"True (boat referenced)"},
						{0,"True (ground referenced to North)"},
						{4,"True (water referenced)"}
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
					"Wind.Data.Reserved",// generic_name
					0,// bit_position edited with low-can-generator
					21,// bit_size
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
					decoder_t::decode_bytes,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					""// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Wind.Data.Sid",// generic_name
					56,// bit_position edited with low-can-generator
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
					"Wind.Data.Wind.Angle",// generic_name
					24,// bit_position edited with low-can-generator
					16,// bit_size
					0.000100000f,// factor
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
					"rad"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Wind.Data.Wind.Speed",// generic_name
					40,// bit_position edited with low-can-generator
					16,// bit_size
					0.0100000f,// factor
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
					"m/s"// unit
				})}
			} // end signals vector
		})} // end message_definition entry
,		{std::make_shared<message_definition_t>(message_definition_t{"j1939",130310,"Environmental.Parameters",8,136,frequency_clock_t(5.00000f),true,
			{ // beginning signals vector
				{std::make_shared<signal_t> (signal_t{
					"Environmental.Parameters.Atmospheric.Pressure",// generic_name
					8,// bit_position edited with low-can-generator
					16,// bit_size
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
					decoder_t::decode_noop,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					"hPa"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Environmental.Parameters.Outside.Ambient.Air.Temperature",// generic_name
					24,// bit_position edited with low-can-generator
					16,// bit_size
					0.0100000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
					},// states
					false,// writable
					decoder_t::decode_noop,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					"K"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Environmental.Parameters.Reserved",// generic_name
					0,// bit_position edited with low-can-generator
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
					decoder_t::decode_bytes,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					""// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Environmental.Parameters.Sid",// generic_name
					56,// bit_position edited with low-can-generator
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
					"Environmental.Parameters.Water.Temperature",// generic_name
					40,// bit_position edited with low-can-generator
					16,// bit_size
					0.0100000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
					},// states
					false,// writable
					decoder_t::decode_noop,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					"K"// unit
				})}
			} // end signals vector
		})} // end message_definition entry
,		{std::make_shared<message_definition_t>(message_definition_t{"j1939",130311,"Environmental.Parameters",8,136,frequency_clock_t(5.00000f),true,
			{ // beginning signals vector
				{std::make_shared<signal_t> (signal_t{
					"Environmental.Parameters.Atmospheric.Pressure",// generic_name
					0,// bit_position edited with low-can-generator
					16,// bit_size
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
					decoder_t::decode_noop,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					"hPa"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Environmental.Parameters.Humidity",// generic_name
					16,// bit_position edited with low-can-generator
					16,// bit_size
					0.00400000f,// factor
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
					static_cast<sign_t>(2),// signed
					-1,// bit_sign_position
					"%"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Environmental.Parameters.Humidity.Source",// generic_name
					48,// bit_position edited with low-can-generator
					2,// bit_size
					1.00000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
						{0,"Inside"},
						{1,"Outside"}
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
					"Environmental.Parameters.Sid",// generic_name
					56,// bit_position edited with low-can-generator
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
					"Environmental.Parameters.Temperature",// generic_name
					32,// bit_position edited with low-can-generator
					16,// bit_size
					0.0100000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
					},// states
					false,// writable
					decoder_t::decode_noop,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					"K"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Environmental.Parameters.Temperature.Source",// generic_name
					50,// bit_position edited with low-can-generator
					6,// bit_size
					1.00000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
						{10,"Apparent Wind Chill Temperature"},
						{6,"Bait Well Temperature"},
						{9,"Dew Point Temperature"},
						{3,"Engine Room Temperature"},
						{14,"Exhaust Gas Temperature"},
						{13,"Freezer Temperature"},
						{12,"Heat Index Temperature"},
						{8,"Heating System Temperature"},
						{2,"Inside Temperature"},
						{5,"Live Well Temperature"},
						{4,"Main Cabin Temperature"},
						{1,"Outside Temperature"},
						{7,"Refridgeration Temperature"},
						{0,"Sea Temperature"},
						{11,"Theoretical Wind Chill Temperature"}
					},// states
					false,// writable
					decoder_t::decode_state,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					""// unit
				})}
			} // end signals vector
		})} // end message_definition entry
,		{std::make_shared<message_definition_t>(message_definition_t{"j1939",130312,"Temperature",8,136,frequency_clock_t(5.00000f),true,
			{ // beginning signals vector
				{std::make_shared<signal_t> (signal_t{
					"Temperature.Actual.Temperature",// generic_name
					24,// bit_position edited with low-can-generator
					16,// bit_size
					0.0100000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
					},// states
					false,// writable
					decoder_t::decode_noop,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					"K"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Temperature.Instance",// generic_name
					48,// bit_position edited with low-can-generator
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
					"Temperature.Set.Temperature",// generic_name
					8,// bit_position edited with low-can-generator
					16,// bit_size
					0.0100000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
					},// states
					false,// writable
					decoder_t::decode_noop,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					"K"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Temperature.Sid",// generic_name
					56,// bit_position edited with low-can-generator
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
					"Temperature.Source",// generic_name
					40,// bit_position edited with low-can-generator
					8,// bit_size
					1.00000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
						{10,"Apparent Wind Chill Temperature"},
						{6,"Bait Well Temperature"},
						{9,"Dew Point Temperature"},
						{3,"Engine Room Temperature"},
						{14,"Exhaust Gas Temperature"},
						{13,"Freezer Temperature"},
						{12,"Heat Index Temperature"},
						{8,"Heating System Temperature"},
						{2,"Inside Temperature"},
						{5,"Live Well Temperature"},
						{4,"Main Cabin Temperature"},
						{1,"Outside Temperature"},
						{7,"Refridgeration Temperature"},
						{0,"Sea Temperature"},
						{11,"Theoretical Wind Chill Temperature"}
					},// states
					false,// writable
					decoder_t::decode_state,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					""// unit
				})}
			} // end signals vector
		})} // end message_definition entry
,		{std::make_shared<message_definition_t>(message_definition_t{"j1939",60160,"Iso.Transport.Protocol.Data.Transfer",8,136,frequency_clock_t(5.00000f),true,
			{ // beginning signals vector
				{std::make_shared<signal_t> (signal_t{
					"Iso.Transport.Protocol.Data.Transfer.Data",// generic_name
					0,// bit_position edited with low-can-generator
					56,// bit_size
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
					"Iso.Transport.Protocol.Data.Transfer.Sid",// generic_name
					56,// bit_position edited with low-can-generator
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
,		{std::make_shared<message_definition_t>(message_definition_t{"j1939",60416,"Iso.Transport.Protocol.Connection.Management.Abort",8,136,frequency_clock_t(5.00000f),true,
			{ // beginning signals vector
				{std::make_shared<signal_t> (signal_t{
					"Iso.Transport.Protocol.Connection.Management.Abort.Group.Function.Code",// generic_name
					56,// bit_position edited with low-can-generator
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
					"Iso.Transport.Protocol.Connection.Management.Abort.Pgn",// generic_name
					8,// bit_position edited with low-can-generator
					24,// bit_size
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
					decoder_t::decode_noop,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					""// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Iso.Transport.Protocol.Connection.Management.Abort.Reason",// generic_name
					48,// bit_position edited with low-can-generator
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
					decoder_t::decode_bytes,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					""// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Iso.Transport.Protocol.Connection.Management.Abort.Reserved",// generic_name
					32,// bit_position edited with low-can-generator
					16,// bit_size
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
					decoder_t::decode_bytes,// decoder
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
