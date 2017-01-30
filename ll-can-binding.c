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

#include "ll-can-binding.h"

/*************************************************************************/
/*************************************************************************/
/**									**/
/**									**/
/**	   SECTION: UTILITY FUNCTIONS					**/
/**									**/
/**									**/
/*************************************************************************/
/*************************************************************************/

/*
 * Retry a function 3 times
 *
 * param int function(): function that return an int wihtout any parameter
 *
 * return : 0 if ok, -1 if failed
 *
 */
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

/*
 * Test that socket is really opened
 *
 * param
 *
 * return : 0 or positive int if ok, negative value if failed
 *
 */
static int socket_test()
{
	if (can_handler.socket < 0)
	{
		return -1;
	}
	return 0;
}

/*
 * Browse chained list and return the one with specified id
 *
 * param uint32_t id : can arbitration identifier
 *
 * return can_event
 */
static can_event *get_event_of_id(uint32_t id)
{
	can_event *current;

	/* create and return if lists not exists */
	if (!can_events_list)
	{
		can_events_list = (can_event*)calloc(1, sizeof(can_event));
		can_events_list->id = id;
		return can_events_list;
	}

	/* search for id */
	current = can_events_list;
	while(current)
	{
		if (current->id == id)
			return current;
		if (!current->next)
		{
			current->next = (can_event*)calloc(1, sizeof(can_event));
			current->next->id = id;
			return current->next;
		}
		current = current->next;
	}

	return NULL;
}

/*
 * Take an id and return it into a char array
 */
static char* create_name(uint32_t id)
{
	char name[32];
	size_t nchar;

	nchar = (size_t)sprintf(name, "can_%u", id);
	if (nchar > 0)
	{
		char *result = (char*)malloc(nchar + 1);
		memcpy(result, name, nchar);
		result[nchar] = 0;
		return result;
	}

	return NULL;
}

/*
 * Create json object that will be pushed through event_loop to any subscriber
 *
 *  param : openxc_CanMessage structure complete with data to put into json
 *  object.
 *
 *  return : json object
 *
 *
 */
static json_object* create_json_from_openxc_CanMessage(event *event)
{
	struct json_object *json;
	openxc_CanMessage can_message;

	/*
	 * TODO: process the openxc_CanMessage struct. Should be a call to a
	 * decoder function relative to that msg
	 */
	can_message = event->can_message;

	json = json_object_new_object();
	json_object_object_add(json, "name", json_object_new_string(event->name));

	return json;
}

/*************************************************************************/
/*************************************************************************/
/**									**/
/**									**/
/**	   SECTION: HANDLE CAN DEVICE					**/
/**									**/
/**									**/
/*************************************************************************/
/*************************************************************************/
/*
 * open the can socket
 */
static int open_can_dev()
{
	const int canfd_on = 1;
	struct ifreq ifr;
	struct timeval timeout = {1,0};

	DEBUG(interface, "open_can_dev: CAN Handler socket : %d", can_handler.socket);
	if (can_handler.socket >= 0)
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
		if (setsockopt(can_handler.socket, SOL_CAN_RAW, CAN_RAW_FD_FRAMES, &canfd_on, sizeof(canfd_on)) < 0)
		{
			NOTICE(interface, "open_can_dev: Can not switch into CAN Extended frame format.");
			can_handler.is_fdmode_on = false;
		} else {
			can_handler.is_fdmode_on = true;
		}

		/* Attempts to open a socket to CAN bus */
		strcpy(ifr.ifr_name, can_handler.device);
		if(ioctl(can_handler.socket, SIOCGIFINDEX, &ifr) < 0)
			ERROR(interface, "open_can_dev: ioctl failed");
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
				return 0;
			}
		}
		close(can_handler.socket);
		can_handler.socket = -1;
	}
	return -1;
}

/*
 * TODO : test that socket is really opened
 */
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

	/* Test that socket is really opened */
	if ( socket_test < 0)
	{
		if (retry(open_can_dev) < 0)
		{
			ERROR(interface, "read_can: Socket unavailable");
			return -1;
		}
	}

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
		return -2;
	}

	/* CAN frame integrity check */
	if ((size_t)nbytes == CAN_MTU)
		maxdlen = CAN_MAX_DLEN;
	else if ((size_t)nbytes == CANFD_MTU)
		maxdlen = CANFD_MAX_DLEN;
	else
	{
		ERROR(interface, "read_can: CAN frame incomplete");
		return -3;
	}

	if (parse_can_frame(can_message, &canfd_frame, maxdlen))
	{
		ERROR(interface, "read_can: Can't parse the can frame. ID: %i, DLC: %i, DATA: %X", 
		      canfd_frame.can_id, canfd_frame.len, canfd_frame.data);
		return -4;
	}

	return 0;
}

/*
 * Parse the CAN frame data payload as a CAN packet
 * TODO: parse as an OpenXC Can Message. Don't translate as ASCII and put bytes
 * directly into openxc_CanMessage
 */
static int parse_can_frame(openxc_CanMessage *can_message, struct canfd_frame *canfd_frame, int maxdlen)
{
	int i, len;
	//size_t n_msg;

	len = (canfd_frame->len > maxdlen) ? maxdlen : canfd_frame->len;

	can_message->has_id = true;
	if (canfd_frame->can_id & CAN_ERR_FLAG)
		can_message->id = canfd_frame->can_id & (CAN_ERR_MASK|CAN_ERR_FLAG);
	else if (canfd_frame->can_id & CAN_EFF_FLAG)
	{
		can_message->has_frame_format = true;
		can_message->frame_format = openxc_CanMessage_FrameFormat_EXTENDED;
		can_message->id = canfd_frame->can_id & CAN_EFF_MASK;
	} else
	{
		can_message->has_frame_format = true;
		can_message->frame_format = openxc_CanMessage_FrameFormat_STANDARD;
		can_message->id = canfd_frame->can_id & CAN_SFF_MASK;
	}

	/* Don't know what to do with that for now as we haven't
	 * len fields in openxc_CanMessage struct

	 * standard CAN frames may have RTR enabled. There are no ERR frames with RTR
	if (maxdlen == CAN_MAX_DLEN && canfd_frame->can_id & CAN_RTR_FLAG)
	{
		/* print a given CAN 2.0B DLC if it's not zero
		if (canfd_frame->len && canfd_frame->len <= CAN_MAX_DLC)
			buf[offset++] = hex_asc_upper[canfd_frame->len & 0xF];

		buf[offset] = 0;
		return NULL;
	}
	*/

	/* Doesn't handle real canfd_frame for now
	if (maxdlen == CANFD_MAX_DLEN)
	{
		/* add CAN FD specific escape char and flags
		canfd_frame->flags & 0xF;
	} */

	if (sizeof(canfd_frame->data) <= sizeof(can_message->data.bytes))
	{
		for (i = 0; i < len; i++)
			can_message->data.bytes[i] = canfd_frame->data[i];
		return 0;
	} else if (sizeof(canfd_frame->data) <= CAN_MAX_DLEN)
	{
		ERROR(interface, "parse_can_frame: can_frame data too long to be stored into openxc_CanMessage data field");
		return -1;
		/* TODO create as many as needed openxc_CanMessage into an array to store all data from canfd_frame
		n_msg = CAN_MAX_DLEN / sizeof(canfd_frame->data.bytes);
		for (i = 0; i < len; i++)
			can_message->data.bytes[i] = canfd_frame->data[i]; */
	} else
	{
		ERROR(interface, "parse_can_frame: can_frame is really too long here. Size of data greater than canfd maximum 64bytes size. Is it a CAN message ?");
		return -2;
	}

	return NULL;
}

/*************************************************************************/
/*************************************************************************/
/**									**/
/**									**/
/**       SECTION: MANAGING EVENTS					**/
/**									**/
/**									**/
/*************************************************************************/
/*************************************************************************/
/*
 * called on an event on the CAN bus
 */
static int on_event(sd_event_source *s, int fd, uint32_t revents, void *userdata)
{
	openxc_CanMessage can_message;

	can_message = openxc_CanMessage_init_default;

	/* read available data */
	if ((revents & EPOLLIN) != 0)
	{
		read_can(&can_message);
		send_event();
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
 */
static event *get_event(uint32_t id, enum type type)
{
	event *event;
	can_event *list;

	/* find the can list by id */
	list = get_event_of_id(id);

	/* make the new event */
	event = (can_event*)calloc(1, sizeof(can_event));
	event->next = event;
	list->events = event;
	event->name = create_name(id);
	event->afb_event = afb_daemon_make_event(interface->daemon, event->name);

	return event;
}

/*
 * Send all events
 */
static void send_event()
{
	can_event *current;
	event *events;
	json_object *object;

	/* Browse can_events */
	current = can_events_list;
	while(current)
	{
		/* Browse event for each can_events no matter what the id */
		events = current->events;
		while(events)
		{
			object = create_json_from_openxc_CanMessage(events);
			afb_event_push(events->afb_event, object);
			events = events->next;
		}
		current = current->next;
	}
}

/*
 * Get the event loop running.
 * Will trigger on_event function on EPOLLIN event on socket
 *
 * Return 0 or positive value on success. Else negative value for failure.
 */
static int connect_to_event_loop()
{
	sd_event *event_loop;
	sd_event_source *source;
	int rc;

	if (can_handler.socket < 0)
	{
		return can_handler.socket;
	}

	event_loop = afb_daemon_get_event_loop(interface->daemon);
	rc = sd_event_add_io(event_loop, &source, can_handler.socket, EPOLLIN, on_event, NULL);
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

/*************************************************************************/
/*************************************************************************/
/**									**/
/**									**/
/**	   SECTION: BINDING VERBS IMPLEMENTATION			**/
/**									**/
/**									**/
/*************************************************************************/
/*************************************************************************/
/*
 * Returns the type corresponding to the given name
 */
static enum type type_of_name(const char *name)
{
	enum type result;
	if (name == NULL)
		return type_DEFAULT;
	for (result = 0 ; (size_t)result < type_size; result++)
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
	event *event;
	uint32_t id;
	struct json_object *json;

	if (get_type_for_req(req, &type))
	{
		id = (uint32_t)atoi(afb_req_value(req, "id"));
		event = get_event(id, type);
		if (event == NULL)
			afb_req_fail(req, "out-of-memory", NULL);
		else if (afb_req_subscribe(req, event->afb_event) != 0)
			afb_req_fail_f(req, "failed", "afb_req_subscribe returned an error: %m");
		else
		{
			/* TODO : build json openXC message to send. I guess */
			json = json_object_new_object();
			json_object_object_add(json, "name", json_object_new_string(event->name));
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
	event *event;

	id = afb_req_value(req, "id");
	if (id == NULL)
		afb_req_fail(req, "missing-id", NULL);
	else
	{
		event = get_event_of_id(atoi(id));
		if (event == NULL)
			afb_req_fail(req, "bad-id", NULL);
		else
		{
			afb_req_unsubscribe(req, event->afb_event);
			afb_req_success(req, NULL, NULL);
		}
	}
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
	/* Open CAN socket */
	retry(open_can_dev);
	return connect_to_event_loop();
}
