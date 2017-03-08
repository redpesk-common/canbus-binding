
/////////////////////////////////////////////////////////////////////////////////////
//							THIS IS AN EXAMPLE
/////////////////////////////////////////////////////////////////////////////////////

#include <configuration.hpp>

#define OBD2_FUNCTIONAL_BROADCAST_ID 0x7df


std::vector<can_message_set_t> CAN_MESSAGE_SET =
{
	{0, "example_messages", 1, 2, 2, 0, 19},
};

std::vector<std::vector<can_message_definition_t>> CAN_MESSAGES_DEFINTION = {
	{
		{0x620, can_message_format::STANDARD, {10, 0, nullptr}, false, (uint8_t)NULL},
		{0x442, can_message_format::STANDARD, {10, 0, nullptr}, false, (uint8_t)NULL},
	},
};

/**
 * @brief Dumb SIGNALS array. It is composed by can_message_set_t
 * SIGNALS[MESSAGE_SET_ID][can_signal_t]
 */
std::vector<std::vector<can_signal_t>> SIGNALS = {
	{
		{&(CAN_MESSAGES_DEFINTION[MESSAGE_SET_ID][0]), "can.driver_door.open", 2, 4, 1.000000, 0.000000, 0.000000, 0.000000, {10, 0, nullptr}, false, true, nullptr, 0, false, decoder_t::booleanDecoder, nullptr, false, 0.0},
		{&(CAN_MESSAGES_DEFINTION[MESSAGE_SET_ID][1]), "can.driver_door.close", 0, 4, 1.000000, 0.000000, 0.000000, 0.000000, {10, 0, nullptr}, false, true, nullptr, 0, false, decoder_t::booleanDecoder, nullptr, false, 0.0}
	},
};