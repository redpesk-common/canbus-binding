/*
 * Copyright (C) 2015, 2016 "IoT.bzh"
 * Author "Romain Forlot" <romain.forlot@iot.bzh>
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

/* max. number of CAN interfaces given on the cmdline */
#define MAXSOCK 16

/* buffer sizes for CAN frame string representations */
#define CL_ID (sizeof("12345678##1"))
#define CL_DATA sizeof(".AA")
#define CL_BINDATA sizeof(".10101010")

 /* CAN FD ASCII hex short representation with DATA_SEPERATORs */
#define CL_CFSZ (2*CL_ID + 64*CL_DATA)

#define CANID_DELIM '#'

/*
 * Interface between the daemon and the binding
 */
static const struct afb_binding_interface *interface;

/*
 * the type of position expected
 *
 * here, this type is the selection of protocol
 */
enum type {
	type_OBDII,
	type_CAN,
	type_DEFAULT = type_CAN,
	type_INVALID = -1
};

#define type_size sizeof(enum type)-2

/*
 * names of the types
 */
static const char * const type_NAMES[type_size] = {
	"OBDII",
	"CAN"
};

/* CAN variable initialization */
struct canfd_frame canfd_frame;

struct can_handler {
	int socket;
	char *device;
	bool is_fdmode_on;
	struct sockaddr_can txAddress;
};

/*
 * each generated event
 */
typedef struct _event event;
struct _event {
	event *next;			/* link for the next event */
	const char *name;		/* name of the event */
	struct afb_event afb_event;	/* the event for the binder */
	openxc_CanMessage can_message;	/* value for the can_message */
};

/*
 * each can event, will browse by the id
 */
typedef struct _can_event can_event;
struct _can_event {
	can_event *next;	/* Link to the next other can message */
	event *events;		/* events for the can message */
	uint32_t id;		/* id of the event for unsubscribe */
	enum type type;		/* the type of data expected */
};

can_event *can_events_list;

/* TODO : Add help comments :p */
static int connect_to_event_loop();

const char hex_asc_upper[] = "0123456789ABCDEF";

#define hex_asc_upper_lo(x) hex_asc_upper[((x) & 0x0F)]
#define hex_asc_upper_hi(x) hex_asc_upper[((x) & 0xF0) >> 4]

static inline void put_hex_byte(char *buf, __u8 byte)
{
	buf[0] = hex_asc_upper_hi(byte);
	buf[1] = hex_asc_upper_lo(byte);
}

static inline void _put_id(char *buf, int end_offset, canid_t id)
{
	/* build 3 (SFF) or 8 (EFF) digit CAN identifier */
	while (end_offset >= 0) {
		buf[end_offset--] = hex_asc_upper[id & 0xF];
		id >>= 4;
	}
}

#define put_sff_id(buf, id) _put_id(buf, 2, id)
#define put_eff_id(buf, id) _put_id(buf, 7, id)

static void parse_can_frame(openxc_CanMessage *can_message, struct canfd_frame *canfd_frame, int maxdlen);


// Initialize default can_handler values
static struct can_handler can_handler = {
	.socket = -1,
	.device = "vcan0",
	.is_fdmode_on = false,
};

static void send_event();
