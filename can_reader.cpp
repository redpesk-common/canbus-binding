/*
 * Copyright (C) 2015, 2016 "IoT.bzh"
 * Author "Romain Forlot" <romain.forlot@iot.bzh>
 * Author "Loic Collignon" <loic.collignon@iot.bzh>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <linux/can.h>
#include <linux/can/raw.h>

#include <afb/afb-binding.h>

void can_reader(afb_binding_interface *interface, int socket, std::queue <canfd_frame>& canfd_frame_queue)
{
    ssize_t nbytes;
	int maxdlen;

	/* Test that socket is really opened */
	if ( socket < 0)
	{
		ERROR(interface, "read_can: Socket unavailable");
		return -1;
	}

    while(true)
    {
    	nbytes = read(socket, &canfd_frame, CANFD_MTU);

        switch(nbytes)
        {
            case CANFD_MTU:s
                DEBUG(interface, "read_can: Got an CAN FD frame with length %d", canfd_frame.len);
                maxdlen = CANFD_MAX_DLEN;
                break;
            case CAN_MTU:
                DEBUG(interface, "read_can: Got a legacy CAN frame with length %d", canfd_frame.len);
                maxdlen = CAN_MAX_DLEN;
                break;
            default:
                if (errno == ENETDOWN)
                    ERROR(interface, "read_can: %s interface down", device);
                
                ERROR(interface, "read_can: Error reading CAN bus");
                return -2;
        }

        /* 
         * TODO: thread handle
        if (parse_can_frame(can_message, &canfd_frame, maxdlen))
        {
            ERROR(interface, "read_can: Can't parse the can frame. ID: %i, DLC: %i, DATA: %s", 
            canfd_frame.can_id, canfd_frame.len, canfd_frame.data);
            return -4;
        }
        */
        canfd_frame_queue.push(canfd_frame);
    }
}