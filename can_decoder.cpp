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
#include <queue>

#include <afb/afb-binding.h>

#include "can-utils.h"
#include "openxc.pb.h"

void decode_can_message(afb_binding_interface *interface, std::queue <CanMessage_t>& can_message_q, std::queue <openxc_VehicleMessage>& VehicleMessage_q)
{
    canfd_frame canfd_frame;

    while(true)
    {
        if(! canfd_frame_queue.empty())
        {
            canfd_frame = canfd_frame_queue.front();
            canfd_frame_queue.pop();
        }
    }
}
