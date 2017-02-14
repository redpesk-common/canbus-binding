/*
 * Copyright (C) 2015, 2016 "IoT.bzh"
 * Author "Romain Forlot" <romain.forlot@iot.bzh>
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

/********************************************************************************
*
*		CanBus method implementation
*
*********************************************************************************/

CanBus_c::CanBus_c(afb_binding_interface *itf, const std:string& dev_name)
{
	interface = itf;
	deviceName = dev_name;
}

int CanBus_c::open()
{
	const int canfd_on = 1;
	struct ifreq ifr;
	struct timeval timeout = {1, 0};

	DEBUG(interface, "open_can_dev: CAN Handler socket : %d", socket);
	if (socket >= 0)
		close(socket);

	socket = socket(PF_CAN, SOCK_RAW, CAN_RAW);
	if (socket < 0)
	{
		ERROR(interface, "open_can_dev: socket could not be created");
	}
	else
	{
		/* Set timeout for read */
		setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));
		/* try to switch the socket into CAN_FD mode */
		if (setsockopt(socket, SOL_CAN_RAW, CAN_RAW_FD_FRAMES, &canfd_on, sizeof(canfd_on)) < 0)
		{
			NOTICE(interface, "open_can_dev: Can not switch into CAN Extended frame format.");
			is_fdmode_on = false;
		} else {
			is_fdmode_on = true;
		}

		/* Attempts to open a socket to CAN bus */
		strcpy(ifr.ifr_name, device);
		if(ioctl(socket, SIOCGIFINDEX, &ifr) < 0)
			ERROR(interface, "open_can_dev: ioctl failed");
		else
		{
			txAddress.can_family = AF_CAN;
			txAddress.can_ifindex = ifr.ifr_ifindex;

			/* And bind it to txAddress */
			if (bind(socket, (struct sockaddr *)&txAddress, sizeof(txAddress)) < 0)
			{
				ERROR(interface, "open_can_dev: bind failed");
			}
			else
			{
				fcntl(socket, F_SETFL, O_NONBLOCK);
				return 0;
			}
		}
		close(socket);
		socket = -1;
	}
	return -1;
}

int CanBus_c::close()
{
	close(socket);
	socket = -1;
}

void CanBus_c::start_threads()
{
	std::queue <CanMessage_c> can_message_q;

	th_reading = std::thread(can_reader, interface, socket, can_message_q);
	th_decoding = std::thread(can_decoder, interface, can_message_q, can_message_q);
	th_pushing = std::thread(can_event_push, interface, can_message_q);
}

/*
 * Send a can message from a CanMessage_c object.
 */
int CanBus_c::send_can_message(CanMessage_c can_msg)
{
	int nbytes;
	canfd_frame *f;

	f = can_msg.convert_to_canfd_frame();

	if(socket >= 0)
	{
		nbytes = sendto(socket, &f, sizeof(struct canfd_frame), 0,
				(struct sockaddr*)&txAddress, sizeof(txAddress));
				
		if (nbytes == -1)
		{
			ERROR(interface, "send_can_message: Sending CAN frame failed.");
			return -1;
		}
		return nbytes;
	}
	else
	{
		ERROR(interface, "send_can_message: socket not initialized. Attempt to reopen can device socket.");
		open_can_dev();
	}
	return 0;
}

/*
 * Get a CanMessage from can_message_q and return it
 * then point to the next CanMessage in queue.
 * 
 * Return the next queue element or NULL if queue is empty.
 */
CanMessage_c* CanBus_c::next_can_message()
{
	if(! can_message_q.empty())
	{
		CanMessage_c can_msg = can_message_q.front();
		can_message_q.pop()
		return &can_msg;
	}

	return nullptr;
}

void CanBus_c::insert_new_can_message(CanMessage_c *can_msg)
{
	can_message_q.push(can_msg);
}

/*
 * Get a VehicleMessage from vehicle_message_q and return it
 * then point to the next VehicleMessage in queue.
 * 
 * Return the next queue element or NULL if queue is empty.
 */
openxc_VehicleMessage* CanBus_c::next_vehicle_message()
{
	if(! vehicle_message_q.empty())
	{
		openxc_VehicleMessage v_msg = vehicle_message_q.front();
		vehicle_message_q.pop()
		return &v_msg;
	}

	return nullptr;
}

void CanBus_c::insert_new_vehicle_message(openxc_VehicleMessage *v_msg)
{
	vehicle_message_q.push(v_msg);
}
/********************************************************************************
*
*		CanMessage method implementation
*
*********************************************************************************/

uint32_t CanMessage_c::get_id()
{
	return id;
}

int CanMessage_c::get_format()
{
	return format;
}

uint8_t CanMessage_c::get_data()
{
	return data;
}
uint8_t CanMessage_c::get_lenght()
{
	return lenght;
}

void CanMessage_c::set_id(uint32_t new_id)
{
	switch(format):
		case CanMessageFormat::SIMPLE:
			id = new_id & CAN_SFF_MASK;
		case CanMessageFormat::EXTENDED:
			id = new_id & CAN_EFF_MASK;
		default:
			ERROR(interface, "ERROR: Can set id, not a compatible format or format not set prior to set id.");
}

void CanMessage_c::set_format(CanMessageFormat new_format)
{
	if(new_format == CanMessageFormat::SIMPLE || new_format == CanMessageFormat::EXTENDED)
		format = new_format;
	else
		ERROR(interface, "ERROR: Can set format, wrong format chosen");
}

void CanMessage_c::set_data(uint8_t new_data)
{
	data = new_data;
}

void CanMessage_c::set_lenght(uint8_t new_length)
{
	lenght = new_lenght;
}

/*
 * This is the preferred way to initialize a CanMessage object 
 * from a read canfd_frame message.
 * 
 * params: canfd_frame pointer
 */
void CanMessage_c::convert_from_canfd_frame(canfd_frame *frame)
{
	
	lenght = (canfd_frame->len > maxdlen) ? maxdlen : canfd_frame->len;

	switch (canfd_frame->can_id): 
		case (canfd_frame->can_id & CAN_ERR_FLAG):
			id = canfd_frame->can_id & (CAN_ERR_MASK|CAN_ERR_FLAG);
			break;
		case (canfd_frame->can_id & CAN_EFF_FLAG):
			id = canfd_frame->can_id & CAN_EFF_MASK;
			format = CanMessageFormat::EXTENDED;
			break;
		default:
			format = CanMessageFormat::STANDARD;
			id = canfd_frame->can_id & CAN_SFF_MASK;
			break;

	if (sizeof(canfd_frame->data) <= sizeof(data))
	{
		memcpy(data, canfd_frame->data, lenght);
		return 0;
	} else if (sizeof(canfd_frame->data) >= CAN_MAX_DLEN)
		ERROR(interface, "CanMessage_c: canfd_frame data too long to be stored into CanMessage object");
}

canfd_frame* convert_to_canfd_frame()
{
	canfd_frame frame;

	frame.id = can_msg.get_id();
	frame.len = can_msg.get_lenght();
	frame.data = can_msg.get_data();

	return &frame;
}
