#include <binding/application.hpp>
#include <can/can-decoder.hpp>
#include <can/can-encoder.hpp>

extern "C"
{

CTLP_CAPI_REGISTER("j1939-signals");

std::shared_ptr<message_set_t> cms = std::make_shared<message_set_t>(message_set_t{0,"Virtual J1939",
			{ // beginning message_definition_ vector
				{std::make_shared<message_definition_t>(message_definition_t{"j1939",61442,"ETC1",8,4,true,frequency_clock_t(5.00000f),true,
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
							false,// is_big_endian
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
							false,// is_big_endian
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
							false,// is_big_endian
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
							false,// is_big_endian
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
							false,// is_big_endian
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
							false,// is_big_endian
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
							false,// is_big_endian
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
							false,// is_big_endian
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
							false,// is_big_endian
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
							false,// is_big_endian
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
							false,// is_big_endian
							static_cast<sign_t>(0),// signed
							-1,// bit_sign_position
							""// unit
						})}
					} // end signals vector
				})} // end message_definition entry
,				{std::make_shared<message_definition_t>(message_definition_t{"j1939",61444,"EEC1",8,4,true,frequency_clock_t(5.00000f),true,
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
							false,// is_big_endian
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
							false,// is_big_endian
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
							false,// is_big_endian
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
							false,// is_big_endian
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
							false,// is_big_endian
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
							false,// is_big_endian
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
							false,// is_big_endian
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
							false,// is_big_endian
							static_cast<sign_t>(0),// signed
							-1,// bit_sign_position
							""// unit
						})}
					} // end signals vector
				})} // end message_definition entry
,				{std::make_shared<message_definition_t>(message_definition_t{"j1939",61449,"VDC2",8,4,true,frequency_clock_t(5.00000f),true,
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
							false,// is_big_endian
							static_cast<sign_t>(0),// signed
							-1,// bit_sign_position
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
							false,// is_big_endian
							static_cast<sign_t>(0),// signed
							-1,// bit_sign_position
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
							false,// is_big_endian
							static_cast<sign_t>(0),// signed
							-1,// bit_sign_position
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
							false,// is_big_endian
							static_cast<sign_t>(0),// signed
							-1,// bit_sign_position
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
							false,// is_big_endian
							static_cast<sign_t>(0),// signed
							-1,// bit_sign_position
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
							false,// is_big_endian
							static_cast<sign_t>(0),// signed
							-1,// bit_sign_position
							"rad/s"// unit
						})}
					} // end signals vector
				})} // end message_definition entry
,				{std::make_shared<message_definition_t>(message_definition_t{"j1939",65215,"EBC2",8,4,true,frequency_clock_t(5.00000f),true,
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
							false,// is_big_endian
							static_cast<sign_t>(0),// signed
							-1,// bit_sign_position
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
							false,// is_big_endian
							static_cast<sign_t>(0),// signed
							-1,// bit_sign_position
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
							false,// is_big_endian
							static_cast<sign_t>(0),// signed
							-1,// bit_sign_position
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
							false,// is_big_endian
							static_cast<sign_t>(0),// signed
							-1,// bit_sign_position
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
							false,// is_big_endian
							static_cast<sign_t>(0),// signed
							-1,// bit_sign_position
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
							false,// is_big_endian
							static_cast<sign_t>(0),// signed
							-1,// bit_sign_position
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
							false,// is_big_endian
							static_cast<sign_t>(0),// signed
							-1,// bit_sign_position
							"km/h"// unit
						})}
					} // end signals vector
				})} // end message_definition entry
,				{std::make_shared<message_definition_t>(message_definition_t{"j1939",65253,"HOURS",8,4,true,frequency_clock_t(5.00000f),true,
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
							false,// is_big_endian
							static_cast<sign_t>(0),// signed
							-1,// bit_sign_position
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
							false,// is_big_endian
							static_cast<sign_t>(0),// signed
							-1,// bit_sign_position
							"r"// unit
						})}
					} // end signals vector
				})} // end message_definition entry
,				{std::make_shared<message_definition_t>(message_definition_t{"j1939",65262,"ET1",8,4,true,frequency_clock_t(5.00000f),true,
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
							false,// is_big_endian
							static_cast<sign_t>(0),// signed
							-1,// bit_sign_position
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
							false,// is_big_endian
							static_cast<sign_t>(0),// signed
							-1,// bit_sign_position
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
							false,// is_big_endian
							static_cast<sign_t>(0),// signed
							-1,// bit_sign_position
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
							false,// is_big_endian
							static_cast<sign_t>(0),// signed
							-1,// bit_sign_position
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
							false,// is_big_endian
							static_cast<sign_t>(0),// signed
							-1,// bit_sign_position
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
							false,// is_big_endian
							static_cast<sign_t>(0),// signed
							-1,// bit_sign_position
							"deg C"// unit
						})}
					} // end signals vector
				})} // end message_definition entry

		}, // end message_definition vector
			{ // beginning diagnostic_messages_ vector

			} // end diagnostic_messages_ vector
		});

CTLP_ONLOAD(plugin, handle)
{
	afb_api_t api = (afb_api_t) plugin->api;
	CtlConfigT* CtlConfig = (CtlConfigT*) afb_api_get_userdata(api);
	application_t* app = (application_t*) getExternalData(CtlConfig);

	return app->add_message_set(cms);
}

}
