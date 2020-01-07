/*
 * Copyright (C) 2015, 2019 "IoT.bzh"
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

#define CAN_MESSAGE_SIZE 8

#define MAX_BCM_CAN_FRAMES 257
#define MAX_ISOTP_FRAMES 4096

#define J1939_CAN_ID CAN_EFF_FLAG
#define J1939_CAN_MASK (CAN_EFF_FLAG | CAN_RTR_FLAG)

#define CAN_ISOTP_MAX_DLEN 4096

#define NO_CAN_ID 0xFFFFFFFFU

/**
 * FLAGS defining how to open socket and how you could decode the frame
 */
#define CAN_PROTOCOL		   0x0001
#define CAN_PROTOCOL_WITH_FD_FRAME 0x0002
#define J1939_ADDR_CLAIM_PROTOCOL  0x0004
#define J1939_PROTOCOL		   0x0008
#define ISOTP_PROTOCOL		   0x0010
#define ISOTP_SEND		   0x0020
#define ISOTP_RECEIVE		   0x0040
#define BYTE_FRAME_IS_BIG_ENDIAN   0x0080
#define BIT_POSITION_REVERSED	   0x0100
#define CONTINENTAL_BIT_POSITION   0x0200
#define INVALID_FLAG		   0x8000
