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


#include "socketcan-j1939-data.hpp"
#include "socketcan-j1939-addressclaiming.hpp"
#include "../../binding/application.hpp"

namespace utils
{
    /**
     * @brief Open a socket for receive or send data
     *
     * @param device_name The device name to open the socket
     * @param pgn The pgn to receive only him
     * @return int Return the number of the socket
     */
    int socketcan_j1939_data_t::open(std::string device_name, pgn_t pgn)
    {
        int ret = socketcan_j1939_t::open(device_name,htole64(J1939_NAME_ECU),pgn,J1939_NO_ADDR);
        if(ret >= 0)
        {
            if(tx_address_.can_addr.j1939.pgn != J1939_NO_PGN)
            {
                add_filter(J1939_NO_NAME,tx_address_.can_addr.j1939.pgn,J1939_NO_ADDR,J1939_NO_NAME,J1939_NO_PGN,J1939_NO_ADDR);
            }
            define_opt();
        }
        return ret;
    }

    /**
     * @brief Write a message but check if the address claiming is operation before
     *
     * @param obj A j1939 message
     * @return int 0 if the write is ok
     */
    int socketcan_j1939_data_t::write_message(message_t& obj)
    {
        std::unique_lock<std::mutex> lock(mutex_claiming_);
        application_t &application = application_t::instance();
        socketcan_j1939_addressclaiming_t *socket_addr_claimed = static_cast<socketcan_j1939_addressclaiming_t*>(application.get_socket_address_claiming().get());
        while(socket_addr_claimed->get_state() != claiming_state::OPERATIONAL)
        {
            socketcan_j1939_t::signal_address_claiming_.wait(lock);
            if(socket_addr_claimed->get_state() == claiming_state::INVALID)
            {
                AFB_ERROR("Invalid state");
                return -1;
            }
        }
        return socketcan_j1939_t::write_message(obj);
    }
}
