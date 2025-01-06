/*******************************************************************************
*
* Copyright (C) 2019-2025 IoT.bzh Company
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

#include <canbus-binding/utils/signals.hpp>
#include <canbus-binding/utils/openxc-utils.hpp>
#include <canbus-binding/can/can-decoder.hpp>

openxc_DynamicField decode_example(signal_t &signal, std::shared_ptr<message_t> message, bool *send) //Defining a new decoder for our signals,
                                                                                                    //note that the prototype must be the same
{
    float value = decoder_t::parse_signal_bitfield(signal, message); //Getting signal value
    float modified_value = atan2f32(value, value*M_1_PIf32); //Processing the value. The calculation has no purpose, it's juste for the example.
    openxc_DynamicField decoded_value = build_DynamicField((double) modified_value);
    return decoded_value; //Return the value
}

// This decoder can now be added to the json config file to use it to parse the recieved signals.
