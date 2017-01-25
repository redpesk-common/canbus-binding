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
#define _GNU_SOURCE

#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <sys/time.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <math.h>
#include <fcntl.h>
#include <systemd/sd-event.h>
#include <errno.h>

#include <json-c/json.h>
#include <openxc.pb.h>

#include <afb/afb-binding.h>
#include <afb/afb-service-itf.h>

/*****************************************************************************************/
/*****************************************************************************************/
/**											**/
/**											**/
/**	   SECTION: GLOBAL VARIABLES							**/
/**											**/
/**											**/
/*****************************************************************************************/
/*****************************************************************************************/
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

/* CAN variable initialization */
struct canfd_frame canfd_frame;

struct can_handler {
	int socket;
	char *device;
	openxc_CanMessage *msg;
	struct sockaddr_can txAddress;
};

/*
 * each generated event
 */
struct event {
	struct event *next;	/* link for the same period */
	const char *name;	/* name of the event */
	struct afb_event event;	/* the event for the binder */
	enum type type;		/* the type of data expected */
	int id;			/* id of the event for unsubscribe */
};

/*****************************************************************************************/
/*****************************************************************************************/
/**											**/
/**											**/
/**	   SECTION: UTILITY FUNCTIONS							**/
/**											**/
/**											**/
/*****************************************************************************************/
/*****************************************************************************************/

/*
 * @brief Retry a function 3 times
 *
 * @param int function(): function that return an int wihtout any parameter
 *
 * @ return : 0 if ok, -1 if failed
 *
 */
static int retry( int(*func)());
static int retry( int(*func)())
{
	int i;

	for (i=0;i<4;i++)
	{
		if ( (*func)() >= 0)
		{
			return 0;
		}
		usleep(100000);
	}
	return -1;
}

/*****************************************************************************************/
/*****************************************************************************************/
/**											**/
/**											**/
/**	   SECTION: HANDLE CAN DEVICE							**/
/**											**/
/**											**/
/*****************************************************************************************/
/*****************************************************************************************/
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

static void canread_frame_parse(struct canfd_frame *canfd_frame, int maxdlen);

/*
 * names of the types
 */
static const char * const type_NAMES[type_size] = {
	"OBDII",
	"CAN"
};


// Initialize default can_handler values
static struct can_handler can_handler = {
	.socket = -1,
	.device = "vcan0",
};


/*
 * open the can socket
 */
static int open_can_dev()
{
	const int canfd_on = 1;
	struct ifreq ifr;
	struct timeval timeout = {1,0};
	openxc_CanMessage can_msg = {
		.has_bus = false,
		.has_id = false,
		.has_data = false,
		.has_frame_format = false,
	};

	DEBUG(interface, "open_can_dev: CAN Handler socket : %d", can_handler.socket);
	close(can_handler.socket);

	can_handler.socket = socket(PF_CAN, SOCK_RAW, CAN_RAW);
	if (can_handler.socket < 0)
	{
		ERROR(interface, "open_can_dev: socket could not be created");
	}
	else
	{
		/* Set timeout for read */
		setsockopt(can_handler.socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));
		/* try to switch the socket into CAN_FD mode */
		can_msg.has_frame_format = true;
		if (setsockopt(can_handler.socket, SOL_CAN_RAW, CAN_RAW_FD_FRAMES, &canfd_on, sizeof(canfd_on)) < 0)
		{
			NOTICE(interface, "open_can_dev: Can not switch into CAN Extended frame format.");
			can_msg.frame_format = openxc_CanMessage_FrameFormat_STANDARD;
		} else {
			can_msg.frame_format = openxc_CanMessage_FrameFormat_EXTENDED;
		}

		/* Attempts to open a socket to CAN bus */
		strcpy(ifr.ifr_name, can_handler.device);
		if(ioctl(can_handler.socket, SIOCGIFINDEX, &ifr) < 0)
		{
			ERROR(interface, "open_can_dev: ioctl failed");
		}
		else
		{
			can_handler.txAddress.can_family = AF_CAN;
			can_handler.txAddress.can_ifindex = ifr.ifr_ifindex;

			/* And bind it to txAddress */
			if (bind(can_handler.socket, (struct sockaddr *)&can_handler.txAddress, sizeof(can_handler.txAddress)) < 0)
			{
				ERROR(interface, "open_can_dev: bind failed");
			}
			else
			{
				fcntl(can_handler.socket, F_SETFL, O_NONBLOCK);
				can_handler.msg = &can_msg;
				return 0;
			}
		}
		close(can_handler.socket);
		can_handler.socket = -1;
	}
	return -1;
}

static int write_can()
{
	ssize_t nbytes;
	int rc;

	rc = can_handler.socket;
	if (rc >= 0)
	{
/*
 * TODO change old hvac write can frame to generic on_event
 */
		nbytes = sendto(can_handler.socket, &canfd_frame, sizeof(struct canfd_frame), 0,
			    (struct sockaddr*)&can_handler.txAddress, sizeof(can_handler.txAddress));
		if (nbytes < 0)
		{
			ERROR(interface, "write_can: Sending CAN frame failed.");
		}
	}
	else
	{
		ERROR(interface, "write_can: socket not initialized. Attempt to reopen can device socket.");
		retry(open_can_dev);
	}
	return rc;
}

/*
 * Read on CAN bus and return how much bytes has been read.
 */
static int read_can(openxc_CanMessage *can_message)
{
	ssize_t nbytes;
	int maxdlen;

	nbytes = read(can_handler.socket, &canfd_frame, CANFD_MTU);

	if (nbytes == CANFD_MTU)
	{
		DEBUG(interface, "read_can: Got an CAN FD frame with length %d", canfd_frame.len);
	}
	else if (nbytes == CAN_MTU)
	{
		DEBUG(interface, "read_can: Got a legacy CAN frame with length %d", canfd_frame.len);
	}
	else
	{
	 	if (errno == ENETDOWN)
			ERROR(interface, "read_can: %s interface down", can_handler.device);
		ERROR(interface, "read_can: Error reading CAN bus");
		return -1;
	}

	/* CAN frame integrity check */
	if ((size_t)nbytes == CAN_MTU)
		maxdlen = CAN_MAX_DLEN;
	else if ((size_t)nbytes == CANFD_MTU)
		maxdlen = CANFD_MAX_DLEN;
	else
	{
		ERROR(interface, "read_can: CAN frame incomplete");
		return -2;
	}

	canread_frame_parse(&canfd_frame, maxdlen);
}

/*
 * Parse the CAN frame data payload as a CAN packet
 * TODO: parse as an OpenXC Can Message. Don't translate as ASCII and put bytes
 * directly into openxc_CanMessage
 */
static void canread_frame_parse(struct canfd_frame *canfd_frame, int maxdlen)
{
	int i,offset;
	int len = (canfd_frame->len > maxdlen) ? maxdlen : canfd_frame->len;
	char buf[CL_CFSZ];

	if (canfd_frame->can_id & CAN_ERR_FLAG)
	{
		put_eff_id(buf, canfd_frame->can_id & (CAN_ERR_MASK|CAN_ERR_FLAG));
		buf[8] = '#';
		offset = 9;
	} else if (canfd_frame->can_id & CAN_EFF_FLAG)
	{
		put_eff_id(buf, canfd_frame->can_id & CAN_EFF_MASK);
		buf[8] = '#';
		offset = 9;
	} else {
		put_sff_id(buf, canfd_frame->can_id & CAN_SFF_MASK);
		buf[3] = '#';
		offset = 4;
	}

	/* standard CAN frames may have RTR enabled. There are no ERR frames with RTR */
	if (maxdlen == CAN_MAX_DLEN && canfd_frame->can_id & CAN_RTR_FLAG)
	{
		buf[offset++] = 'R';
		/* print a given CAN 2.0B DLC if it's not zero */
		if (canfd_frame->len && canfd_frame->len <= CAN_MAX_DLC)
			buf[offset++] = hex_asc_upper[canfd_frame->len & 0xF];

		buf[offset] = 0;
		return;
	}

	if (maxdlen == CANFD_MAX_DLEN)
	{
		/* add CAN FD specific escape char and flags */
		buf[offset++] = '#';
		buf[offset++] = hex_asc_upper[canfd_frame->flags & 0xF];
	}

	for (i = 0; i < len; i++)
	{
		put_hex_byte(buf + offset, canfd_frame->data[i]);
		offset += 2;
	}

	buf[offset] = 0;
	return;
}

/***************************************************************************************/
/***************************************************************************************/
/**                                                                                   **/
/**                                                                                   **/
/**       SECTION: MANAGING EVENTS                                                    **/
/**                                                                                   **/
/**                                                                                   **/
/***************************************************************************************/
/***************************************************************************************/
static int connect_to_event_loop();

/*
 * called on an event on the CAN bus
 */
static int on_event(sd_event_source *s, int fd, uint32_t revents, void *userdata)
{
	openxc_CanMessage can_message;

	/* read available data */
	if ((revents & EPOLLIN) != 0)
	{
		read_can(&can_message);
//		event_send();
	}

	/* check if error or hangup */
	if ((revents & (EPOLLERR|EPOLLRDHUP|EPOLLHUP)) != 0)
	{
		sd_event_source_unref(s);
		close(fd);
		connect_to_event_loop();
	}

	return 0;
}

/*
 * get or create an event handler for the type
 * TODO: implement function and handle retrieve or create an event as needed
 *
static struct event *event_get(enum type type)
{

}
*/

static struct event *event_of_id(int id)
{

}

/*****************************************************************************************/
/*****************************************************************************************/
/**											**/
/**											**/
/**	   SECTION: BINDING VERBS IMPLEMENTATION					**/
/**											**/
/**											**/
/*****************************************************************************************/
/*****************************************************************************************/
/*
 * Returns the type corresponding to the given name
 */
static enum type type_of_name(const char *name)
{
	enum type result;
	if (name == NULL)
		return type_DEFAULT;
	for (result = 0 ; result < type_size; result++)
		if (strcmp(type_NAMES[result], name) == 0)
			return result;
	return type_INVALID;
}

/*
 * extract a valid type from the request
 */
static int get_type_for_req(struct afb_req req, enum type *type)
{
	if ((*type = type_of_name(afb_req_value(req, "type"))) != type_INVALID)
		return 1;
	afb_req_fail(req, "unknown-type", NULL);
	return 0;
}

/*
 * subscribe to notification of new CAN messages
 *
 * parameters of the subscription are:
 *
 *    TODO type: string:  choose between CAN and OBDII messages formats.
 *
 * returns an object with 2 fields:
 *
 *    name:   string:  the name of the event without its prefix
 *    id:     integer: a numeric identifier of the event to be used for unsubscribing
 */
static void subscribe(struct afb_req req)
{
	enum type type;
	const char *period;
	struct event *can_sig;
	struct json_object *json;

	if (get_type_for_req(req, &type))
	{
		can_sig->event = afb_daemon_make_event(interface->daemon, type_NAMES[type]);
		if (can_sig == NULL)
			afb_req_fail(req, "out-of-memory", NULL);
		else if (afb_req_subscribe(req, can_sig->event) != 0)
			afb_req_fail_f(req, "failed", "afb_req_subscribe returned an error: %m");
		else
		{
			json = json_object_new_object();
			json_object_object_add(json, "name", json_object_new_string(event->name));
			json_object_object_add(json, "id", json_object_new_int(event->id));
			afb_req_success(req, json, NULL);
		}
	}
}

/*
 * unsubscribe a previous subscription
 *
 * parameters of the unsubscription are:
 *
 *    id:   integer: the numeric identifier of the event as returned when subscribing
 */
static void unsubscribe(struct afb_req req)
{
	const char *id;
	struct event *event;

	id = afb_req_value(req, "id");
	if (id == NULL)
		afb_req_fail(req, "missing-id", NULL);
	else
	{
		event = event_of_id(atoi(id));
		if (event == NULL)
			afb_req_fail(req, "bad-id", NULL);
		else
		{
			afb_req_unsubscribe(req, event->event);
			afb_req_success(req, NULL, NULL);
		}
	}
}

static int connect_to_event_loop()
{
	sd_event_source *source;
	int rc;

	retry(open_can_dev);

	if (can_handler.socket < 0)
	{
		return can_handler.socket;
	}

	rc = sd_event_add_io(afb_daemon_get_event_loop(interface->daemon), &source, can_handler.socket, EPOLLIN, on_event, NULL);
	if (rc < 0)
	{
		close(can_handler.socket);
		ERROR(interface, "Can't connect CAN bus %s to the event loop", can_handler.device);
	} else
	{
		NOTICE(interface, "Connected CAN bus %s to the event loop", can_handler.device);
	}

	return rc;
}


// TODO: Have to change session management flag to AFB_SESSION_CHECK to use token auth
static const struct afb_verb_desc_v1 verbs[]=
{
  { .name= "subscribe",    .session= AFB_SESSION_NONE, .callback= subscribe,    .info= "subscribe to notification of CAN bus messages." },
  { .name= "unsubscribe",  .session= AFB_SESSION_NONE, .callback= unsubscribe,  .info= "unsubscribe a previous subscription." },
	{NULL}
};

static const struct afb_binding binding_desc = {
	.type = AFB_BINDING_VERSION_1,
	.v1 = {
		.info = "CAN bus service",
		.prefix = "can",
		.verbs = verbs
	}
};

const struct afb_binding *afbBindingV1Register (const struct afb_binding_interface *itf)
{
	interface = itf;

	return &binding_desc;
}

int afbBindingV1ServiceInit(struct afb_service service)
{
	return connect_to_event_loop();
}
