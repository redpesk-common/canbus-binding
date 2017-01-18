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
 * each generated event
 */
struct event {
	struct event *next;	/* link for the same period */
	const char *name;	/* name of the event */
	struct afb_event event;	/* the event for the binder */
	enum type type;		/* the type of data expected */
	int id;			/* id of the event for unsubscribe */
};

struct can_handler {
	int socket;
	char *device;
	struct sockaddr_can txAddress;
};

static __u32 dropcnt[MAXSOCK];
static __u32 last_dropcnt[MAXSOCK];
char ctrlmsg[CMSG_SPACE(sizeof(struct timeval)) + CMSG_SPACE(sizeof(__u32))];
struct timeval tv;
struct iovec iov;
struct msghdr msg;
struct cmsghdr *cmsg;
struct canfd_frame can_frame;

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
 * Interface between the daemon and the binding
 */
static const struct afb_binding_interface *interface;

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
	.device = "vcan0"
};

/*
 * Parse the CAN frame data payload as a CAN packet
 * TODO: parse as an OpenXC Can Message
 */
int can_frame_parse(openxc_CanMessage *can_message, struct can_frame *can_frame)
{

}


/*
 * open the can socket
 */
static int open_can_dev()
{
	struct ifreq ifr;
	struct timeval timeout = {1,0};

	DEBUG(interface, "open_can_dev: CAN Handler socket : %d", can_handler.socket);
	close(can_handler.socket);

	can_handler.socket = socket(PF_CAN, SOCK_RAW, CAN_RAW);
	if (can_handler.socket < 0)
	{
		ERROR(interface, "open_can_dev: socket could not be created");
	}
	else
	{
		// Set timeout for read
		setsockopt(can_handler.socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));
		// Attempts to open a socket to CAN bus
		strcpy(ifr.ifr_name, can_handler.device);
		if(ioctl(can_handler.socket, SIOCGIFINDEX, &ifr) < 0)
		{
			ERROR(interface, "open_can_dev: ioctl failed");
		}
		else
		{
			can_handler.txAddress.can_family = AF_CAN;
			can_handler.txAddress.can_ifindex = ifr.ifr_ifindex;

			// And bind it to txAddress
			if (bind(can_handler.socket, (struct sockaddr *)&can_handler.txAddress, sizeof(can_handler.txAddress)) < 0)
			{
				ERROR(interface, "open_can_dev: bind failed");
			}
			else
			{
				fcntl(can_handler.socket, F_SETFL, O_NONBLOCK);
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
	int rc = 0;

	rc = can_handler.socket;
	if (rc >= 0)
	{
/*
 * TODO change old hvac write can frame to generic on_event
 */
		rc = sendto(can_handler.socket, &can_frame, sizeof(struct can_frame), 0,
			    (struct sockaddr*)&can_handler.txAddress, sizeof(can_handler.txAddress));
		if (rc < 0)
		{
			ERROR(interface, "Sending CAN frame failed.");
		}
	}
	else
	{
		ERROR(interface, "socket not initialized. Attempt to reopen can device socket.");
		retry(open_can_dev);
	}
	return rc;
}

/*
 * Read on CAN bus and return how much bytes has been read.
 */
static int read_can(openxc_CanMessage *can_message)
{
	int byte_read, maxdlen;
	iov.iov_base = &can_frame;
	msg.msg_name = &can_handler.txAddress;
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	msg.msg_control = &ctrlmsg;

	byte_read = recvmsg(can_handler.socket, &msg, 0);

	if (byte_read < 0)
	{
		if (errno == ENETDOWN) {
			ERROR(interface, "%s: interface down", can_handler.device);
		}
		ERROR(interface, "Error reading CAN bus");
		return -1;
	}

	// CAN frame integrity check
	if ((size_t)byte_read == CAN_MTU)
		maxdlen = CAN_MAX_DLEN;
	else if ((size_t)byte_read == CANFD_MTU)
		maxdlen = CANFD_MAX_DLEN;
	else
	{
		ERROR(interface, "CAN frame incomplete");
		return -2;
	}

	for (	cmsg = CMSG_FIRSTHDR(&msg);
		cmsg && (cmsg->cmsg_level == SOL_SOCKET);
		cmsg = CMSG_NXTHDR(&msg,cmsg))
	{
		if (cmsg->cmsg_type == SO_TIMESTAMP)
			tv = *(struct timeval *)CMSG_DATA(cmsg);
			else if (cmsg->cmsg_type == SO_RXQ_OVFL)
				dropcnt[can_handler.socket] = *(__u32 *)CMSG_DATA(cmsg);
	}

	// Check if there is a new CAN frame dropped.
	if (dropcnt[can_handler.socket] != last_dropcnt[can_handler.socket])
	{
		__u32 frames = dropcnt[can_handler.socket] - last_dropcnt[can_handler.socket];
		WARNING(interface, "DROPCOUNT: dropped %d CAN frame%s on '%s' socket (total drops %d)",
			frames, (frames > 1)?"s":"", can_handler.device, dropcnt[can_handler.socket]);
			if (log)
				WARNING(interface, "DROPCOUNT: dropped %d CAN frame%s on '%s' socket (total drops %d)\n",
					frames, (frames > 1)?"s":"", can_handler.device, dropcnt[can_handler.socket]);

			last_dropcnt[can_handler.socket] = dropcnt[can_handler.socket];
	}

	can_message->has_id = true;
	can_message->id = msg.id; // TODO make the parsing to extract id from data and only return left data into msg.msg_iov
	can_message->has_data = true;
	can_message->data = msg.msg_iov;
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
 */
static struct event *event_get(enum type type)
{

}

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
	struct event *event;
	struct json_object *json;

	if (get_type_for_req(req, &type))
	{
		event = event_get(type);
		if (event == NULL)
			afb_req_fail(req, "out-of-memory", NULL);
		else if (afb_req_subscribe(req, event->event) != 0)
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
