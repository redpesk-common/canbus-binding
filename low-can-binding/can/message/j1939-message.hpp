/*
 * Copyright (C) 2018, 2019 "IoT.bzh"
 * Author "Arthur Guyader" <arthur.guyader@iot.bzh>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *	 http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once
#include <linux/can.h>
#include <linux/can/j1939.h>
#include "./message.hpp"

#define J1939_MAX_MULTIPACKETS 255
#define J1939_MAX_DLEN J1939_MAX_MULTIPACKETS * CAN_MAX_DLEN

class j1939_message_t : public message_t
{
    private:
        /* J1939 NAME
        *
        * bit 0-20	: Identity Number
        * bit 21-31	: Manufacturer Code
        * bit 32-34	: ECU Instance
        * bit 35-39	: Function Instance
        * bit 40-47	: Function
        * bit 48	: Reserved
        * bit 49-55	: Vehicle System
        * bit 56-59	: Vehicle System Instance
        * bit 60-62	: Industry Group
        * bit 63	: Arbitrary Address Capable
        */
        name_t name_;

        /* J1939 Parameter Group Number
        *
        * bit 0-7	: PDU Specific (PS)
        * bit 8-15	: PDU Format (PF)
        * bit 16	: Data Page (DP)
        * bit 17	: Reserved (R)
        * bit 19-31	: set to zero
        */
        pgn_t pgn_;


        /* J1939 Address
         */
        uint8_t addr_;

    public:
        j1939_message_t();
        j1939_message_t(uint8_t length, can_message_format_t format, std::vector<uint8_t>& data, uint64_t timestamp, name_t name, pgn_t pgn, uint8_t addr);
        uint64_t get_name() const;
        uint32_t get_pgn() const;
        uint8_t get_addr() const;
        static std::shared_ptr<j1939_message_t> convert_from_addr(struct sockaddr_can& addr, uint8_t (&data)[128], size_t nbytes, uint64_t timestamp);
        bool is_set();
        std::string get_debug_message();
        uint32_t get_id() const;
        struct bcm_msg get_bcm_msg();
		void set_bcm_msg(struct bcm_msg bcm_msg);
};


