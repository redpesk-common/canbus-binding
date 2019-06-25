#include "application.hpp"
#include "../can/can-decoder.hpp"
#include "../can/can-encoder.hpp"

application_t::application_t()
	: can_bus_manager_{utils::config_parser_t{"/etc/dev-mapping.conf"}}
	, message_set_{
		{std::make_shared<message_set_t>(message_set_t{0,"J1939",
			{ // beginning message_definition_ vector

				{std::make_shared<message_definition_t>(message_definition_t{"j1939",61442,0,message_format_t::J1939,frequency_clock_t(5.00000f),true,8,
					{ // beginning signals vector
						{std::make_shared<signal_t> (signal_t{
							"EngMomentaryOverspeedEnable",// generic_name
							32,// bit_position
							2,// bit_size
							1.00000f,// factor
							0.00000,// offset
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
							false,// writable
							nullptr,// decoder
							nullptr,// encoder
							false,// received
							std::make_pair<bool, int>(false, 0),// multiplex
							0,// is_big_endian
							0,// is_signed
							"",// unit
							"Command signal used to indicate that the engine speed may be boosted up to the maximum engine overspeed value to"// comment
						})},
						{std::make_shared<signal_t> (signal_t{
							"MomentaryEngMaxPowerEnable",// generic_name
							36,// bit_position
							2,// bit_size
							1.00000f,// factor
							0.00000,// offset
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
							false,// writable
							nullptr,// decoder
							nullptr,// encoder
							false,// received
							std::make_pair<bool, int>(false, 0),// multiplex
							0,// is_big_endian
							0,// is_signed
							"",// unit
							"Momentarily Requesting highest torque map from the engine control"// comment
						})},
						{std::make_shared<signal_t> (signal_t{
							"PercentClutchSlip",// generic_name
							24,// bit_position
							8,// bit_size
							0.400000f,// factor
							0.00000,// offset
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
							"%",// unit
							"Parameter which represents the ratio of input shaft speed to current engine speed (in percent)."// comment
						})},
						{std::make_shared<signal_t> (signal_t{
							"ProgressiveShiftDisable",// generic_name
							34,// bit_position
							2,// bit_size
							1.00000f,// factor
							0.00000,// offset
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
							false,// writable
							nullptr,// decoder
							nullptr,// encoder
							false,// received
							std::make_pair<bool, int>(false, 0),// multiplex
							0,// is_big_endian
							0,// is_signed
							"",// unit
							"Command signal used to indicate that progressive shifting by the engine should be disallowed."// comment
						})},
						{std::make_shared<signal_t> (signal_t{
							"SrcAddrssOfCtrllngDvcFrTrnsCntrl",// generic_name
							56,// bit_position
							8,// bit_size
							1.00000f,// factor
							0.00000,// offset
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
							"",// unit
							"The source address of the SAE J1939 device currently controlling the transmission."// comment
						})},
						{std::make_shared<signal_t> (signal_t{
							"TransDrivelineEngaged",// generic_name
							0,// bit_position
							2,// bit_size
							1.00000f,// factor
							0.00000,// offset
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
							false,// writable
							nullptr,// decoder
							nullptr,// encoder
							false,// received
							std::make_pair<bool, int>(false, 0),// multiplex
							0,// is_big_endian
							0,// is_signed
							"",// unit
							"Driveline engaged indicates the transmission controlled portion of the driveline is engaged sufficiently to allow a transfer"// comment
						})},
						{std::make_shared<signal_t> (signal_t{
							"TransInputShaftSpeed",// generic_name
							40,// bit_position
							16,// bit_size
							0.125000f,// factor
							0.00000,// offset
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
							"rpm",// unit
							"Rotational velocity of the primary shaft transferring power into the transmission."// comment
						})},
						{std::make_shared<signal_t> (signal_t{
							"TransOutputShaftSpeed",// generic_name
							8,// bit_position
							16,// bit_size
							0.125000f,// factor
							0.00000,// offset
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
							"rpm",// unit
							"Calculated speed of the transmission output shaft."// comment
						})},
						{std::make_shared<signal_t> (signal_t{
							"TransShiftInProcess",// generic_name
							4,// bit_position
							2,// bit_size
							1.00000f,// factor
							0.00000,// offset
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
							false,// writable
							nullptr,// decoder
							nullptr,// encoder
							false,// received
							std::make_pair<bool, int>(false, 0),// multiplex
							0,// is_big_endian
							0,// is_signed
							"",// unit
							"Indicates that the transmission is in process of shifting from the current gear to the selected gear."// comment
						})},
						{std::make_shared<signal_t> (signal_t{
							"TrnsTrqCnvrtrLckpTrnstnInPrcess",// generic_name
							6,// bit_position
							2,// bit_size
							1.00000f,// factor
							0.00000,// offset
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
							false,// writable
							nullptr,// decoder
							nullptr,// encoder
							false,// received
							std::make_pair<bool, int>(false, 0),// multiplex
							0,// is_big_endian
							0,// is_signed
							"",// unit
							"State signal indicating whether or not the transmission torque converter lock up clutch is transitioning between being"// comment
						})},
						{std::make_shared<signal_t> (signal_t{
							"TrnsTrqueConverterLockupEngaged",// generic_name
							2,// bit_position
							2,// bit_size
							1.00000f,// factor
							0.00000,// offset
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
							false,// writable
							nullptr,// decoder
							nullptr,// encoder
							false,// received
							std::make_pair<bool, int>(false, 0),// multiplex
							0,// is_big_endian
							0,// is_signed
							"",// unit
							"State signal which indicates whether the torque converter lockup is engaged."// comment
						})}
					} // end signals vector
				})}, // end message_definition entry

				{std::make_shared<message_definition_t>(message_definition_t{"j1939",61449,0,message_format_t::J1939,frequency_clock_t(5.00000f),true,8,
					{ // beginning signals vector
						{std::make_shared<signal_t> (signal_t{
							"LateralAcceleration",// generic_name
							40,// bit_position
							16,// bit_size
							0.000488281f,// factor
							-15.6870,// offset
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
							"m/s/s",// unit
							"Indicates a lateral acceleration of the vehicle."// comment
						})},
						{std::make_shared<signal_t> (signal_t{
							"LongitudinalAcceleration",// generic_name
							56,// bit_position
							8,// bit_size
							0.100000f,// factor
							-12.5000,// offset
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
							"m/s/s",// unit
							"Indicates the longitudinal acceleration of the vehicle."// comment
						})},
						{std::make_shared<signal_t> (signal_t{
							"SteerWheelAngle",// generic_name
							0,// bit_position
							16,// bit_size
							0.000976562f,// factor
							-31.3740,// offset
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
							"rad",// unit
							"The main operator`s steering wheel angle (on the steering column, not the actual wheel angle)."// comment
						})},
						{std::make_shared<signal_t> (signal_t{
							"SteerWheelAngleSensorType",// generic_name
							22,// bit_position
							2,// bit_size
							1.00000f,// factor
							0.00000,// offset
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
							nullptr,// decoder
							nullptr,// encoder
							false,// received
							std::make_pair<bool, int>(false, 0),// multiplex
							0,// is_big_endian
							0,// is_signed
							"",// unit
							"Indicates whether the steering wheel angle sensor is capable of absolute measuring of the number of steering wheel"// comment
						})},
						{std::make_shared<signal_t> (signal_t{
							"SteerWheelTurnCounter",// generic_name
							16,// bit_position
							6,// bit_size
							1.00000f,// factor
							-32.0000,// offset
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
							"turns",// unit
							"Indicates number of steering wheel turns, absolute position or relative position at ignition on."// comment
						})},
						{std::make_shared<signal_t> (signal_t{
							"YawRate",// generic_name
							24,// bit_position
							16,// bit_size
							0.000122070f,// factor
							-3.92000,// offset
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
							"rad/s",// unit
							"Indicates the rotation about the vertical axis."// comment
						})}
					} // end signals vector
				})}, // end message_definition entry

				{std::make_shared<message_definition_t>(message_definition_t{"j1939",65253,0,message_format_t::J1939,frequency_clock_t(5.00000f),true,8,
					{ // beginning signals vector
						{std::make_shared<signal_t> (signal_t{
							"EngTotalHoursOfOperation",// generic_name
							0,// bit_position
							32,// bit_size
							0.0500000f,// factor
							0.00000,// offset
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
							"hr",// unit
							"Accumulated time of operation of engine."// comment
						})},
						{std::make_shared<signal_t> (signal_t{
							"EngTotalRevolutions",// generic_name
							32,// bit_position
							32,// bit_size
							1000.00f,// factor
							0.00000,// offset
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
							"r",// unit
							"Accumulated number of revolutions of engine crankshaft during its operation."// comment
						})}
					} // end signals vector
				})}, // end message_definition entry


				{std::make_shared<message_definition_t>(message_definition_t{"j1939",65262,0,message_format_t::J1939,frequency_clock_t(5.00000f),true,8,
					{ // beginning signals vector
						{std::make_shared<signal_t> (signal_t{
							"EngCoolantTemp",// generic_name
							0,// bit_position
							8,// bit_size
							0.0312500f,// factor
							-273.000,// offset
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
							"deg C",// unit
							"Temperature of liquid found in engine cooling system."// comment
						})},
						{std::make_shared<signal_t> (signal_t{
							"EngFuelTemp1",// generic_name
							8,// bit_position
							8,// bit_size
							1.00000f,// factor
							-40.0000,// offset
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
							"deg C",// unit
							"Temperature of fuel (or gas) passing through the first fuel control system."// comment
						})},
						{std::make_shared<signal_t> (signal_t{
							"EngIntercoolerTemp",// generic_name
							48,// bit_position
							8,// bit_size
							1.00000f,// factor
							-40.0000,// offset
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
							"deg C",// unit
							"Temperature of liquid found in the intercooler located after the turbocharger."// comment
						})},
						{std::make_shared<signal_t> (signal_t{
							"EngIntercoolerThermostatOpening",// generic_name
							56,// bit_position
							8,// bit_size
							0.400000f,// factor
							0.00000,// offset
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
							"%",// unit
							"The current position of the thermostat used to regulate the temperature of the engine intercooler."// comment
						})},
						{std::make_shared<signal_t> (signal_t{
							"EngOilTemp1",// generic_name
							16,// bit_position
							16,// bit_size
							0.0312500f,// factor
							-273.000,// offset
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
							"deg C",// unit
							"Temperature of the engine lubricant."// comment
						})},
						{std::make_shared<signal_t> (signal_t{
							"EngTurboOilTemp",// generic_name
							32,// bit_position
							16,// bit_size
							0.0312500f,// factor
							-273.000,// offset
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
							"deg C",// unit
							"Temperature of the turbocharger lubricant."// comment
						})}
					} // end signals vector
				})} // end message_definition entry

		}, // end message_definition vector
			{ // beginning diagnostic_messages_ vector

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
	return "";
}


