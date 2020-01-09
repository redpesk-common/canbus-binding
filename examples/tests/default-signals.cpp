#include <binding/application.hpp>
#include <can/can-decoder.hpp>
#include <can/can-encoder.hpp>

extern "C" {
CTLP_CAPI_REGISTER("agl-virtual-car");

std::shared_ptr<message_set_t> cms = std::make_shared<message_set_t>(message_set_t{0,"AGL Virtual Car",
	{ // beginning message_definition_ vector
		{std::make_shared<message_definition_t>(message_definition_t{"ls",0x111,"",16,16,frequency_clock_t(5.00000f),true,
			{ // beginning signals vector
				{std::make_shared<signal_t> (signal_t{
					"sig1.1",// generic_name
					0,// bit_position
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
					true,// writable
					nullptr,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					""// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"sig1.2",// generic_name
					16,// bit_position
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
					true,// writable
					nullptr,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					""// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"sig2.1",// generic_name
					32,// bit_position
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
					true,// writable
					nullptr,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					""// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"sig2.2",// generic_name
					48,// bit_position
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
					true,// writable
					nullptr,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					""// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"sig3.1",// generic_name
					64,// bit_position
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
					true,// writable
					nullptr,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					""// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"sig3.2",// generic_name
					80,// bit_position
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
					true,// writable
					nullptr,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					""// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"sig4.1",// generic_name
					96,// bit_position
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
					true,// writable
					nullptr,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					""// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"sig4.2",// generic_name
					112,// bit_position
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
					true,// writable
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
,		{std::make_shared<message_definition_t>(message_definition_t{"ls",0x30,"",8,0,frequency_clock_t(5.00000f),true,
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
					true,// writable
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
					true,// writable
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
					true,// writable
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
					true,// writable
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
,		{std::make_shared<message_definition_t>(message_definition_t{"hs",0x3D9,"",8,2,frequency_clock_t(5.00000f),true,
			{ // beginning signals vector
				{std::make_shared<signal_t> (signal_t{
					"engine.speed",// generic_name
					16,// bit_position
					16,// bit_size
					0.250000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
					},// states
					true,// writable
					nullptr,// decoder
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
					0,// min_value
					0,// max_value
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
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
					},// states
					true,// writable
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
,		{std::make_shared<message_definition_t>(message_definition_t{"hs",0x3E9,"",8,0,frequency_clock_t(5.00000f),true,
			{ // beginning signals vector
				{std::make_shared<signal_t> (signal_t{
					"vehicle.average.speed",// generic_name
					0,// bit_position
					15,// bit_size
					0.0156250f,// factor
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
,		{std::make_shared<message_definition_t>(message_definition_t{"hs",0x4D1,"",8,0,frequency_clock_t(5.00000f),true,
			{ // beginning signals vector
				{std::make_shared<signal_t> (signal_t{
					"engine.oil.temp",// generic_name
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
					true,// writable
					nullptr,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					""// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"engine.oil.temp.high",// generic_name
					7,// bit_position
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
,		{std::make_shared<message_definition_t>(message_definition_t{"hs",0x620,"",8,0,frequency_clock_t(5.00000f),true,
			{ // beginning signals vector
				{std::make_shared<signal_t> (signal_t{
					"doors.boot.open",// generic_name
					47,// bit_position
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
					0,// min_value
					0,// max_value
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
					0,// min_value
					0,// max_value
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
					0,// min_value
					0,// max_value
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
					0,// min_value
					0,// max_value
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
,		{std::make_shared<message_definition_t>(message_definition_t{"hs",0x799,"",8,0,frequency_clock_t(5.00000f),true,
			{ // beginning signals vector
				{std::make_shared<signal_t> (signal_t{
					"windows.front_left.open",// generic_name
					43,// bit_position
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
					"windows.front_right.open",// generic_name
					44,// bit_position
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
					"windows.rear_left.open",// generic_name
					46,// bit_position
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
					"windows.rear_right.open",// generic_name
					45,// bit_position
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
,		{std::make_shared<message_definition_t>(message_definition_t{"j1939",130306,"Wind.Data",8,392,frequency_clock_t(5.00000f),true,
			{ // beginning signals vector
				{std::make_shared<signal_t> (signal_t{
					"Wind.Data.Reference",// generic_name
					40,// bit_position
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
					43,// bit_position
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
					"Wind.Data.Wind.Angle",// generic_name
					24,// bit_position
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
					8,// bit_position
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
,		{std::make_shared<message_definition_t>(message_definition_t{"j1939",61442,"ETC1",8,8,frequency_clock_t(5.00000f),true,
			{ // beginning signals vector
				{std::make_shared<signal_t> (signal_t{
					"Eng.Momentary.Overspeed.Enable",// generic_name
					32,// bit_position
					2,// bit_size
					1.00000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
						{0,"MomentaryEngOverspeedIsDisabled"},
						{1,"MomentaryEngOverspeedIsEnabled"},
						{2,"Reserved"},
						{3,"TakeNoAction"}
					},// states
					true,// writable
					decoder_t::decode_state,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					""// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Momentary.Eng.Max.Power.Enable",// generic_name
					36,// bit_position
					2,// bit_size
					1.00000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
						{2,"fault"},
						{1,"mmntarilyRqingMaxPowerAvailable"},
						{3,"notAvailable"},
						{0,"notRqingMaxPowerAvailable"}
					},// states
					true,// writable
					decoder_t::decode_state,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					""// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Percent.Clutch.Slip",// generic_name
					24,// bit_position
					8,// bit_size
					0.400000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
					},// states
					true,// writable
					nullptr,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					"%"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Progressive.Shift.Disable",// generic_name
					34,// bit_position
					2,// bit_size
					1.00000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
						{1,"ProgressiveShiftIsDisabled"},
						{0,"ProgressiveShiftIsNotDisabled"},
						{2,"Reserved"},
						{3,"TakeNoAction"}
					},// states
					true,// writable
					decoder_t::decode_state,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					""// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Src.Addrss.Of.Ctrllng.Dvc.Fr.Trns.Cntrl",// generic_name
					56,// bit_position
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
					true,// writable
					nullptr,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					""// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Trans.Driveline.Engaged",// generic_name
					0,// bit_position
					2,// bit_size
					1.00000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
						{0,"DrivelineDisengaged"},
						{1,"DrivelineEngaged"},
						{2,"Error"},
						{3,"NotAvailable"}
					},// states
					true,// writable
					decoder_t::decode_state,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					""// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Trans.Input.Shaft.Speed",// generic_name
					40,// bit_position
					16,// bit_size
					0.125000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
					},// states
					true,// writable
					nullptr,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					"rpm"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Trans.Output.Shaft.Speed",// generic_name
					8,// bit_position
					16,// bit_size
					0.125000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
					},// states
					true,// writable
					nullptr,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					"rpm"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Trans.Shift.In.Process",// generic_name
					4,// bit_position
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
						{3,"NotAvailable"},
						{1,"ShiftInProcess"},
						{0,"ShiftIsNotInProcess"}
					},// states
					true,// writable
					decoder_t::decode_state,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					""// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Trns.Trq.Cnvrtr.Lckp.Trnstn.In.Prcess",// generic_name
					6,// bit_position
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
						{3,"NotAvailable"},
						{1,"TransitionIsInProcess"},
						{0,"TransitionIsNotInProcess"}
					},// states
					true,// writable
					decoder_t::decode_state,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					""// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Trns.Trque.Converter.Lockup.Engaged",// generic_name
					2,// bit_position
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
						{3,"NotAvailable"},
						{0,"TorqueConverterLockupDisengaged"},
						{1,"TorqueConverterLockupEngaged"}
					},// states
					true,// writable
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
,		{std::make_shared<message_definition_t>(message_definition_t{"j1939",61444,"EEC1",8,8,frequency_clock_t(5.00000f),true,
			{ // beginning signals vector
				{std::make_shared<signal_t> (signal_t{
					"Actl.Eng.Prcnt.Trque.High.Resolution",// generic_name
					4,// bit_position
					4,// bit_size
					0.125000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
						{0,"0000"},
						{1,"0125"},
						{7,"0875"},
						{8,"1111NotAvailable"}
					},// states
					false,// writable
					decoder_t::decode_state,// decoder
					nullptr,// encoder
					false,// received
					std::make_pair<bool, int>(false, 0),// multiplex
					static_cast<sign_t>(0),// signed
					-1,// bit_sign_position
					"%"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Actual.Eng.Percent.Torque",// generic_name
					16,// bit_position
					8,// bit_size
					1.00000f,// factor
					-125.000f,// offset
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
					"%"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Drivers.Demand.Eng.Percent.Torque",// generic_name
					8,// bit_position
					8,// bit_size
					1.00000f,// factor
					-125.000f,// offset
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
					"%"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Eng.Demand.Percent.Torque",// generic_name
					56,// bit_position
					8,// bit_size
					1.00000f,// factor
					-125.000f,// offset
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
					"%"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Eng.Speed",// generic_name
					24,// bit_position
					16,// bit_size
					0.125000f,// factor
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
					"rpm"// unit
				})},
				{std::make_shared<signal_t> (signal_t{
					"Eng.Starter.Mode",// generic_name
					48,// bit_position
					4,// bit_size
					1.00000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
						{9,"1011Reserved"},
						{14,"error"},
						{15,"notAvailable"},
						{0,"startNotRqed"},
						{2,"starterActiveGearEngaged"},
						{1,"starterActiveGearNotEngaged"},
						{12,"starterInhibitedReasonUnknown"},
						{3,"strtFnshdStrtrNtActvAftrHvngBnA"},
						{6,"strtrInhbtdDTDrvlnEnggdOthrTrns"},
						{5,"strtrInhbtdDTEngNtReadyForStart"},
						{7,"strtrInhbtdDToActiveImmobilizer"},
						{4,"strtrInhbtdDToEngAlreadyRunning"},
						{8,"strtrInhbtdDueToStarterOvertemp"}
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
					"Eng.Torque.Mode",// generic_name
					0,// bit_position
					4,// bit_size
					1.00000f,// factor
					0.00000f,// offset
					0,// min_value
					0,// max_value
					frequency_clock_t(0.00000f),// frequency
					true,// send_same
					false,// force_send_changed
					{
						{7,"ABS control"},
						{5,"ASR control"},
						{1,"Accelerator pedal/operator selec"},
						{10,"Braking system"},
						{2,"Cruise control"},
						{9,"High speed governor"},
						{0,"Low idle governor/no request (de"},
						{15,"Not available"},
						{3,"PTO governor"},
						{11,"Remote accelerator"},
						{4,"Road speed governor"},
						{8,"Torque limiting"},
						{6,"Transmission control"}
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
					"Src.AddrssOf.Ctrllng.Dvc.Fr.Eng.Cntrl",// generic_name
					40,// bit_position
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
