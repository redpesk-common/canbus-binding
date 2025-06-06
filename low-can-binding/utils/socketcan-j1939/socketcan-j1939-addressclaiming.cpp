/*
 * Copyright (C) 2018-2025 IoT.bzh Company
 * Author: "Arthur Guyader" <arthur.guyader@iot.bzh>
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

#include <unistd.h>
#include <cstring>

#include <canbus-binding/utils/socketcan-j1939/socketcan-j1939-addressclaiming.hpp>

#include <canbus-binding/can/message/j1939-message.hpp>

namespace utils
{
	/**
	 * @brief Construct a new socketcan j1939 addressclaiming t::socketcan j1939 addressclaiming t object
	 *
	 */
	socketcan_j1939_addressclaiming_t::socketcan_j1939_addressclaiming_t():
	socketcan_j1939_t(),
	table_j1939_address_{{std::make_pair(0, false)}},
	current_address_{J1939_IDLE_ADDR},
	signal_stop_{false},
	claiming_state_{claiming_state::INITIAL}
	{}

	/**
	 * @brief Allows to read the claiming message and react to them
	 *
	 * @return std::shared_ptr<message_t> Return an INVALID j1939 message because no need of return (only signature of the function)
	 */
	std::unique_ptr<message_t> socketcan_j1939_addressclaiming_t::read_message()
	{
		AFB_DEBUG("[socketcan_j1939_addressclaiming_t] read_message");
		std::unique_ptr<message_t> invalid_message = std::make_unique<j1939_message_t>();
		std::unique_ptr<message_t> m;
		j1939_message_t* jm;

		m = socketcan_j1939_t::read_message();
		jm = static_cast<j1939_message_t*>(m.get());

		if(jm->get_pgn() == J1939_PGN_ADDRESS_CLAIMED)
		{
			if(jm->get_addr() >= J1939_IDLE_ADDR)
			{
				AFB_DEBUG("Get invalid address claiming by name (maybe logout) : %x",(unsigned int)jm->get_name());
				return invalid_message;
			}

			if(jm->get_name() == htole64(j1939_name_))
			{
				AFB_DEBUG("Get own address claiming");
				return invalid_message;
			}

			AFB_DEBUG("Get address claiming from %x",(unsigned int)jm->get_name());

			if(jm->get_addr() != current_address_)
			{
				save_addr_name(jm->get_addr(), jm->get_name());
				return invalid_message;
			}

			if(claiming_state_ == claiming_state::CLAIMING)
			{
				if(jm->get_name() > htole64(j1939_name_))
				{
					AFB_WARNING("Error from %x to use j1939 protocol",(unsigned int)htole64(j1939_name_));
					return invalid_message;
				}

				save_addr_name(jm->get_addr(), jm->get_name());


				if(timer_handle_)
				{
						afb_timer_unref(timer_handle_);
						timer_handle_ = nullptr;
				}

				if(claim_address(false, true) < 0)
				{
					AFB_ERROR("Claim address failed");
					change_state(claiming_state::INVALID);
					return invalid_message;
				}
			}
			else if(claiming_state_ == claiming_state::OPERATIONAL)
			{
				AFB_DEBUG("Address colision");
				if(jm->get_name() > htole64(j1939_name_))
				{
					if(claim_address(false, false) < 0)
					{
						AFB_ERROR("Claim address failed");
						change_state(claiming_state::INVALID);
						return invalid_message;
					}
					return invalid_message;
				}

				save_addr_name(jm->get_addr(), jm->get_name());

				if(claim_address(false, true) < 0)
				{
						AFB_ERROR("Claim address failed");
						change_state(claiming_state::INVALID);
						return invalid_message;
				}
			}
		}
		return invalid_message;
	}

	/**
	 * @brief Initialize the table j1939 with the valid address posible
	 *
	 */
	void socketcan_j1939_addressclaiming_t::initialize_table_j1939_address()
	{
		int start_addr = 128;
		int end_addr = 247;
		if(start_addr < 0)
		{
			AFB_ERROR("[socketcan-j1939-addressclaiming][initialize_table_j1939_address] Invalid start address");
		}
		else
		{
			if(end_addr >= J1939_IDLE_ADDR)
			{
				AFB_ERROR("[socketcan-j1939-addressclaiming][initialize_table_j1939_address] Invalid end address");
			}
			else
			{
				for (int i = start_addr; i <= end_addr; i++)
					table_j1939_address_[i] = std::make_pair(0, true);
			}
		}
	}

	/**
	 * @brief Save at an address a name
	 *
	 * @param addr The adress where you want to save name
	 * @param name The name of the ECU that is in the address
	 * @return int 0 if save is ok
	 */
	int socketcan_j1939_addressclaiming_t::save_addr_name(uint8_t addr, name_t name)
	{
		if(addr < J1939_IDLE_ADDR)
		{
			if(table_j1939_address_[addr].first < name)
			{
				table_j1939_address_[addr].first = name;
				AFB_DEBUG("[socketcan-j1939-addressclaiming][save_addr_name] NAME : %x <--> ADDR : %d",(unsigned int)name, addr);
			}
			else if(table_j1939_address_[addr].first == name)
			{
				AFB_WARNING("Name %x has already adress %d",(unsigned int)name, addr);
			}
		}
		else
		{
			AFB_ERROR("Invalid address to save");
			return -1;
		}
		return 0;
	}

	/**
	 * @brief Choose new address for the ECU check in the table the best place to claim
	 *
	 * @return uint8_t The new address choosen
	 */
	uint8_t socketcan_j1939_addressclaiming_t::choose_new_address()
	{
		for (int i = 0; i < J1939_IDLE_ADDR; i++)
		{
			if(table_j1939_address_[i].second && i!=current_address_)
			{
				if(	table_j1939_address_[i].first >= htole64(j1939_name_) || table_j1939_address_[i].first == 0)
					return (uint8_t) i;
			}
		}
		return J1939_IDLE_ADDR;
	}

	/**
	 * @brief The function that destoy the timer
	 *
	 * @param timer_context The timer context to destroy
	 * @return int
	 */
	int socketcan_j1939_addressclaiming_t::free_timer_handle(void *timer_context)
	{
		socketcan_j1939_addressclaiming_t *addressclaiming_socket = (socketcan_j1939_addressclaiming_t*) timer_context;
		addressclaiming_socket->timer_handle_ = nullptr;
		return 0;
	}

	/**
	 * @brief The function is call when at the end of the timer, the socket has don't receive
	 *
	 * @param timerhandle The timerhandle of the timer
	 * @return int 1 it's all it's ok
	 */
	void socketcan_j1939_addressclaiming_t::no_response_claiming(afb_timer_t timerhandle, void *closure, unsigned decount)
	{
		socketcan_j1939_addressclaiming_t *addressclaiming_socket = (socketcan_j1939_addressclaiming_t*) closure;

		// If the cache is cleared :
		addressclaiming_socket->change_state(claiming_state::OPERATIONAL);
		addressclaiming_socket->save_addr_name(addressclaiming_socket->current_address_, htole64(addressclaiming_socket->get_j1939_name()));
		AFB_DEBUG("Get address %d for this ecu", addressclaiming_socket->current_address_);
	}

	/**
	 * @brief Launch timer when an address is claimed
	 *
	 */
	void socketcan_j1939_addressclaiming_t::launch_timer()
	{
		afb_timer_create(&timer_handle_, 0, 0, 250, 1, 250, 10, no_response_claiming, (void *)this, 1);
	}

	/**
	 * @brief Allows to claim a new address
	 *
	 * @param first_claim If true, the socket is open
	 * @param new_address If true, claim a new address, else only resend a claim with same address
	 * @return int -1 if fail
	 */
	int socketcan_j1939_addressclaiming_t::claim_address(bool first_claim, bool new_address)
	{
		if(new_address)
		{
			AFB_DEBUG("New address");
			current_address_ = choose_new_address();
			change_state(claiming_state::CLAIMING);
			launch_timer();
		}

		if(current_address_ == J1939_IDLE_ADDR)
		{
			AFB_ERROR("No address left");
			return -1;
		}

		if(first_claim)
		{
			int ret = socketcan_j1939_t::open(device_name_, htole64(j1939_name_), J1939_NO_PGN, current_address_);

			if(ret < 0)
			{
				AFB_ERROR("Error open socket address claiming");
				return -1;
			}

			AFB_DEBUG("[socketcan-j1939-addressclaiming][claim_address] Success open socket address claiming");
			add_filter(J1939_NO_NAME, J1939_PGN_ADDRESS_CLAIMED, J1939_NO_ADDR, J1939_NO_NAME, J1939_PGN_PDU1_MAX, J1939_NO_ADDR);
			define_opt();
		}
		else
		{
			tx_address_.can_addr.j1939.addr = current_address_;
			if(bind((struct sockaddr *)&tx_address_, sizeof(tx_address_)) < 0)
			{
				AFB_ERROR("rebind() fail");
				return -1;
			}
		}

		uint64_t name = htole64(j1939_name_);
		uint8_t dat[8] = {0};
		memcpy(dat, &name, 8);
		struct sockaddr_can sockname;
		memset(&sockname, 0, sizeof(sockname));
		sockname.can_family = AF_CAN;
		sockname.can_addr.j1939.pgn = J1939_PGN_ADDRESS_CLAIMED;
		sockname.can_addr.j1939.addr = J1939_NO_ADDR;
		socklen_t socklen = sizeof(sockname);

		ssize_t ret = sendto(socket_, &dat, sizeof(dat), 0, (const struct sockaddr *)&sockname, socklen);

		if(ret < 0 )
		{
			AFB_ERROR("Address claimed fail : %s", strerror(errno));
			return -1;
		}

		AFB_DEBUG("[socketcan-j1939-addressclaiming][claim_address] Send address claiming request");

		return (int)ret;
	}

/*	int socketcan_j1939_addressclaiming_t::pgn_request()
	{
		static const uint8_t dat[3] = { 0, 0xee, 0, };
		static struct sockaddr_can peername;
		peername.can_family = AF_CAN;
		peername.can_addr.j1939.pgn = J1939_PGN_REQUEST;
		peername.can_addr.j1939.addr = J1939_NO_ADDR;
		int ret = sendto(socket_, dat, sizeof(dat), 0, (const struct sockaddr *)&peername, sizeof(peername));

		if(ret < 0)
			AFB_ERROR("Error pgn_request()");

		return ret;
	}
	*/

	/**
	 * @brief Return the address associate to a name
	 *
	 * @param name The name you are looking for
	 * @return uint8_t The address if it is present, else J1939_IDLE_ADDR
	 */
	uint8_t socketcan_j1939_addressclaiming_t::get_addr_table(name_t name)
	{
		for(int i = 0; i < J1939_IDLE_ADDR; i++)
		{
			if(table_j1939_address_[i].first == name)
				return (uint8_t) i;
		}
		return J1939_IDLE_ADDR;
	}

	/**
	 * @brief Allows to open a J1939 socket address claiming
	 *
	 * @param device_name The name of the device on which to open the socket
	 * @param pgn NO_PGN
	 * @return int Return 0 if ok else -1
	 */
	int socketcan_j1939_addressclaiming_t::open(std::string device_name, name_t j1939_name_, pgn_t pgn)
	{
		device_name_ = device_name;
		set_j1939_name(j1939_name_);
		initialize_table_j1939_address();
		if(claim_address(true, true) < 0)
		{
			AFB_ERROR("Claim address failed");
			return -1;
		}
		return 0;
	}

	/**
	 * @brief Allows to change the state of the socket address claiming
	 * When the state change a mutex is lock
	 *
	 * @param new_state The new state
	 */
	void socketcan_j1939_addressclaiming_t::change_state(claiming_state new_state)
	{
		std::unique_lock<std::mutex> lock(socketcan_j1939_t::mutex_claiming_);
		claiming_state_ = new_state;
		socketcan_j1939_t::signal_address_claiming_.notify_one();
	}

	/**
	 * @brief Allows to get the states of the socket
	 *
	 * @return claiming_state The state of the socket
	 */
	claiming_state socketcan_j1939_addressclaiming_t::get_state()
	{
		return claiming_state_;
	}

}
