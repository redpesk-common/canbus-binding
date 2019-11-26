#include "application.hpp"
#include "../can/can-decoder.hpp"
#include "../can/can-encoder.hpp"

application_t::application_t()
	: can_bus_manager_{utils::config_parser_t{"/etc/dev-mapping.conf"}}
	, message_set_{
		{std::make_shared<message_set_t>(message_set_t{0,"AGL Virtual Car",
			{ // beginning message_definition_ vector
				{std::make_shared<message_definition_t>(message_definition_t{"ls",0x30,"",0,false,message_format_t::EXTENDED,frequency_clock_t(5.00000f),true,
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
							0,// is_big_endian
							0,// is_signed
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
							0,// is_big_endian
							0,// is_signed
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
							0,// is_big_endian
							0,// is_signed
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
							0,// is_big_endian
							0,// is_signed
							""// unit
						})}
					} // end signals vector
				})} // end message_definition entry
,				{std::make_shared<message_definition_t>(message_definition_t{"hs",0x3D9,"",0,true,message_format_t::STANDARD,frequency_clock_t(5.00000f),true,
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
							0,// is_big_endian
							0,// is_signed
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
							0,// is_big_endian
							0,// is_signed
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
							0,// is_big_endian
							0,// is_signed
							""// unit
						})}
					} // end signals vector
				})} // end message_definition entry
,				{std::make_shared<message_definition_t>(message_definition_t{"hs",0x3E9,"",0,false,message_format_t::STANDARD,frequency_clock_t(5.00000f),true,
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
							0,// is_big_endian
							0,// is_signed
							""// unit
						})}
					} // end signals vector
				})} // end message_definition entry
,				{std::make_shared<message_definition_t>(message_definition_t{"hs",0x4D1,"",0,false,message_format_t::STANDARD,frequency_clock_t(5.00000f),true,
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
							false,// writable
							decoder_t::decode_boolean,// decoder
							nullptr,// encoder
							false,// received
							std::make_pair<bool, int>(false, 0),// multiplex
							0,// is_big_endian
							0,// is_signed
							""// unit
						})}
					} // end signals vector
				})} // end message_definition entry
,				{std::make_shared<message_definition_t>(message_definition_t{"hs",0x620,"",0,false,message_format_t::STANDARD,frequency_clock_t(5.00000f),true,
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
							false,// writable
							decoder_t::decode_boolean,// decoder
							nullptr,// encoder
							false,// received
							std::make_pair<bool, int>(false, 0),// multiplex
							0,// is_big_endian
							0,// is_signed
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
							false,// writable
							decoder_t::decode_boolean,// decoder
							nullptr,// encoder
							false,// received
							std::make_pair<bool, int>(false, 0),// multiplex
							0,// is_big_endian
							0,// is_signed
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
							false,// writable
							decoder_t::decode_boolean,// decoder
							nullptr,// encoder
							false,// received
							std::make_pair<bool, int>(false, 0),// multiplex
							0,// is_big_endian
							0,// is_signed
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
							false,// writable
							decoder_t::decode_boolean,// decoder
							nullptr,// encoder
							false,// received
							std::make_pair<bool, int>(false, 0),// multiplex
							0,// is_big_endian
							0,// is_signed
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
							false,// writable
							decoder_t::decode_boolean,// decoder
							nullptr,// encoder
							false,// received
							std::make_pair<bool, int>(false, 0),// multiplex
							0,// is_big_endian
							0,// is_signed
							""// unit
						})}
					} // end signals vector
				})} // end message_definition entry
,				{std::make_shared<message_definition_t>(message_definition_t{"hs",0x799,"",0,false,message_format_t::STANDARD,frequency_clock_t(5.00000f),true,
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
							false,// writable
							decoder_t::decode_boolean,// decoder
							nullptr,// encoder
							false,// received
							std::make_pair<bool, int>(false, 0),// multiplex
							0,// is_big_endian
							0,// is_signed
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
							false,// writable
							decoder_t::decode_boolean,// decoder
							nullptr,// encoder
							false,// received
							std::make_pair<bool, int>(false, 0),// multiplex
							0,// is_big_endian
							0,// is_signed
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
							false,// writable
							decoder_t::decode_boolean,// decoder
							nullptr,// encoder
							false,// received
							std::make_pair<bool, int>(false, 0),// multiplex
							0,// is_big_endian
							0,// is_signed
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
							false,// writable
							decoder_t::decode_boolean,// decoder
							nullptr,// encoder
							false,// received
							std::make_pair<bool, int>(false, 0),// multiplex
							0,// is_big_endian
							0,// is_signed
							""// unit
						})}
					} // end signals vector
				})} // end message_definition entry
#ifdef USE_FEATURE_J1939
,				{std::make_shared<message_definition_t>(message_definition_t{"j1939",61442,"ETC1",8,false,message_format_t::J1939,frequency_clock_t(5.00000f),true,
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
							0,// is_big_endian
							0,// is_signed
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
							0,// is_big_endian
							0,// is_signed
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
							0,// is_big_endian
							0,// is_signed
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
							0,// is_big_endian
							0,// is_signed
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
							0,// is_big_endian
							0,// is_signed
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
							0,// is_big_endian
							0,// is_signed
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
							0,// is_big_endian
							0,// is_signed
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
							0,// is_big_endian
							0,// is_signed
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
							0,// is_big_endian
							0,// is_signed
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
							0,// is_big_endian
							0,// is_signed
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
							0,// is_big_endian
							0,// is_signed
							""// unit
						})}
					} // end signals vector
				})} // end message_definition entry
,				{std::make_shared<message_definition_t>(message_definition_t{"j1939",61444,"EEC1",8,false,message_format_t::J1939,frequency_clock_t(5.00000f),true,
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
							0,// is_big_endian
							0,// is_signed
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
							0,// is_big_endian
							0,// is_signed
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
							0,// is_big_endian
							0,// is_signed
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
							0,// is_big_endian
							0,// is_signed
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
							0,// is_big_endian
							0,// is_signed
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
							0,// is_big_endian
							0,// is_signed
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
							0,// is_big_endian
							0,// is_signed
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
							0,// is_big_endian
							0,// is_signed
							""// unit
						})}
					} // end signals vector
				})} // end message_definition entry
,				{std::make_shared<message_definition_t>(message_definition_t{"j1939",61449,"VDC2",8,false,message_format_t::J1939,frequency_clock_t(5.00000f),true,
					{ // beginning signals vector
						{std::make_shared<signal_t> (signal_t{
							"Lateral.Acceleration",// generic_name
							40,// bit_position
							16,// bit_size
							0.000488281f,// factor
							-15.6870f,// offset
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
							0,// is_big_endian
							0,// is_signed
							"m/s/s"// unit
						})},
						{std::make_shared<signal_t> (signal_t{
							"Longitudinal.Acceleration",// generic_name
							56,// bit_position
							8,// bit_size
							0.100000f,// factor
							-12.5000f,// offset
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
							0,// is_big_endian
							0,// is_signed
							"m/s/s"// unit
						})},
						{std::make_shared<signal_t> (signal_t{
							"Steer.Wheel.Angle",// generic_name
							0,// bit_position
							16,// bit_size
							0.000976562f,// factor
							-31.3740f,// offset
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
							0,// is_big_endian
							0,// is_signed
							"rad"// unit
						})},
						{std::make_shared<signal_t> (signal_t{
							"Steer.Wheel.Angle.Sensor.Type",// generic_name
							22,// bit_position
							2,// bit_size
							1.00000f,// factor
							0.00000f,// offset
							0,// min_value
							0,// max_value
							frequency_clock_t(0.00000f),// frequency
							true,// send_same
							false,// force_send_changed
							{
								{1,"AbsMeasuringPrinciple"},
								{3,"NotAvailable"},
								{0,"RelativeMeasuringPrinciple"},
								{2,"Reserved"}
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
							"Steer.Wheel.Turn.Counter",// generic_name
							16,// bit_position
							6,// bit_size
							1.00000f,// factor
							-32.0000f,// offset
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
							0,// is_big_endian
							0,// is_signed
							"turns"// unit
						})},
						{std::make_shared<signal_t> (signal_t{
							"Yaw.Rate",// generic_name
							24,// bit_position
							16,// bit_size
							0.000122070f,// factor
							-3.92000f,// offset
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
							0,// is_big_endian
							0,// is_signed
							"rad/s"// unit
						})}
					} // end signals vector
				})} // end message_definition entry
,				{std::make_shared<message_definition_t>(message_definition_t{"j1939",65215,"EBC2",8,false,message_format_t::J1939,frequency_clock_t(5.00000f),true,
					{ // beginning signals vector
						{std::make_shared<signal_t> (signal_t{
							"Front.Axle.Speed",// generic_name
							0,// bit_position
							16,// bit_size
							0.00390625f,// factor
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
							0,// is_big_endian
							0,// is_signed
							"km/h"// unit
						})},
						{std::make_shared<signal_t> (signal_t{
							"Relative.Speed.Front.Axle.Left.Wheel",// generic_name
							16,// bit_position
							8,// bit_size
							0.0625000f,// factor
							-7.81250f,// offset
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
							0,// is_big_endian
							0,// is_signed
							"km/h"// unit
						})},
						{std::make_shared<signal_t> (signal_t{
							"Relative.Speed.Rear.Axle1.Left.Wheel",// generic_name
							32,// bit_position
							8,// bit_size
							0.0625000f,// factor
							-7.81250f,// offset
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
							0,// is_big_endian
							0,// is_signed
							"km/h"// unit
						})},
						{std::make_shared<signal_t> (signal_t{
							"Relative.Speed.Rear.Axle2.Left.Wheel",// generic_name
							48,// bit_position
							8,// bit_size
							0.0625000f,// factor
							-7.81250f,// offset
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
							0,// is_big_endian
							0,// is_signed
							"km/h"// unit
						})},
						{std::make_shared<signal_t> (signal_t{
							"Rlative.Speed.Front.Axle.Right.Wheel",// generic_name
							24,// bit_position
							8,// bit_size
							0.0625000f,// factor
							-7.81250f,// offset
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
							0,// is_big_endian
							0,// is_signed
							"km/h"// unit
						})},
						{std::make_shared<signal_t> (signal_t{
							"Rlative.Speed.Rear.Axle1.Right.Wheel",// generic_name
							40,// bit_position
							8,// bit_size
							0.0625000f,// factor
							-7.81250f,// offset
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
							0,// is_big_endian
							0,// is_signed
							"km/h"// unit
						})},
						{std::make_shared<signal_t> (signal_t{
							"Rlative.Speed.Rear.Axle2.Right.Wheel",// generic_name
							56,// bit_position
							8,// bit_size
							0.0625000f,// factor
							-7.81250f,// offset
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
							0,// is_big_endian
							0,// is_signed
							"km/h"// unit
						})}
					} // end signals vector
				})} // end message_definition entry
,				{std::make_shared<message_definition_t>(message_definition_t{"j1939",65253,"HOURS",8,false,message_format_t::J1939,frequency_clock_t(5.00000f),true,
					{ // beginning signals vector
						{std::make_shared<signal_t> (signal_t{
							"Eng.Total.Hours.Of.Operation",// generic_name
							0,// bit_position
							32,// bit_size
							0.0500000f,// factor
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
							0,// is_big_endian
							0,// is_signed
							"hr"// unit
						})},
						{std::make_shared<signal_t> (signal_t{
							"Eng.Total.Revolutions",// generic_name
							32,// bit_position
							32,// bit_size
							1000.00f,// factor
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
							0,// is_big_endian
							0,// is_signed
							"r"// unit
						})}
					} // end signals vector
				})} // end message_definition entry
,				{std::make_shared<message_definition_t>(message_definition_t{"j1939",65262,"ET1",8,false,message_format_t::J1939,frequency_clock_t(5.00000f),true,
					{ // beginning signals vector
						{std::make_shared<signal_t> (signal_t{
							"Eng.Coolant.Temp",// generic_name
							0,// bit_position
							8,// bit_size
							0.0312500f,// factor
							-273.000f,// offset
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
							0,// is_big_endian
							0,// is_signed
							"deg C"// unit
						})},
						{std::make_shared<signal_t> (signal_t{
							"Eng.Fuel.Temp1",// generic_name
							8,// bit_position
							8,// bit_size
							1.00000f,// factor
							-40.0000f,// offset
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
							0,// is_big_endian
							0,// is_signed
							"deg C"// unit
						})},
						{std::make_shared<signal_t> (signal_t{
							"Eng.Intercooler.Temp",// generic_name
							48,// bit_position
							8,// bit_size
							1.00000f,// factor
							-40.0000f,// offset
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
							0,// is_big_endian
							0,// is_signed
							"deg C"// unit
						})},
						{std::make_shared<signal_t> (signal_t{
							"Eng.Intercooler.Thermostat.Opening",// generic_name
							56,// bit_position
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
							false,// writable
							nullptr,// decoder
							nullptr,// encoder
							false,// received
							std::make_pair<bool, int>(false, 0),// multiplex
							0,// is_big_endian
							0,// is_signed
							"%"// unit
						})},
						{std::make_shared<signal_t> (signal_t{
							"Eng.Oil.Temp1",// generic_name
							16,// bit_position
							16,// bit_size
							0.0312500f,// factor
							-273.000f,// offset
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
							0,// is_big_endian
							0,// is_signed
							"deg C"// unit
						})},
						{std::make_shared<signal_t> (signal_t{
							"Eng.Turbo.Oil.Temp",// generic_name
							32,// bit_position
							16,// bit_size
							0.0312500f,// factor
							-273.000f,// offset
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
							0,// is_big_endian
							0,// is_signed
							"deg C"// unit
						})}
					} // end signals vector
				})} // end message_definition entry
#endif
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
,				{std::make_shared<diagnostic_message_t>(diagnostic_message_t{
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
,				{std::make_shared<diagnostic_message_t>(diagnostic_message_t{
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
,				{std::make_shared<diagnostic_message_t>(diagnostic_message_t{
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
,				{std::make_shared<diagnostic_message_t>(diagnostic_message_t{
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
,				{std::make_shared<diagnostic_message_t>(diagnostic_message_t{
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
,				{std::make_shared<diagnostic_message_t>(diagnostic_message_t{
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
,				{std::make_shared<diagnostic_message_t>(diagnostic_message_t{
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
,				{std::make_shared<diagnostic_message_t>(diagnostic_message_t{
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
,				{std::make_shared<diagnostic_message_t>(diagnostic_message_t{
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
,				{std::make_shared<diagnostic_message_t>(diagnostic_message_t{
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
,				{std::make_shared<diagnostic_message_t>(diagnostic_message_t{
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
,				{std::make_shared<diagnostic_message_t>(diagnostic_message_t{
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
,				{std::make_shared<diagnostic_message_t>(diagnostic_message_t{
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
,				{std::make_shared<diagnostic_message_t>(diagnostic_message_t{
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
,				{std::make_shared<diagnostic_message_t>(diagnostic_message_t{
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
,				{std::make_shared<diagnostic_message_t>(diagnostic_message_t{
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
,				{std::make_shared<diagnostic_message_t>(diagnostic_message_t{
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
,				{std::make_shared<diagnostic_message_t>(diagnostic_message_t{
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
,				{std::make_shared<diagnostic_message_t>(diagnostic_message_t{
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
,				{std::make_shared<diagnostic_message_t>(diagnostic_message_t{
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
	return "hs";
}


