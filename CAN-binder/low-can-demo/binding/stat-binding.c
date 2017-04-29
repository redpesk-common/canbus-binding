/*
 * Copyright (C) 2016 "IoT.bzh"
 * Author José Bollo <jose.bollo@iot.bzh>
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
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <fcntl.h>
#include <math.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/timerfd.h>

#include <json-c/json.h>

#include <systemd/sd-event.h>

#include <afb/afb-binding.h>
#include <afb/afb-service-itf.h>

/*
 * the interface to afb-daemon
 */
const struct afb_binding_interface *afbitf;

#define DEFAULT_PERIOD   1   /* 1 second */

struct event;

const char *cpu_fields[]= {
	"user",
	"nice",
	"system",
	"idle",
	"iowait",
	"irq",
	"softirq",
	"steal",
	"guest",
	"guest_nice"
};

#define CPU_FIELD_COUNT (sizeof cpu_fields / sizeof *cpu_fields)

struct status {
	uint64_t cpu[CPU_FIELD_COUNT];
};

static int fd_proc;
static int fd_timer;
static struct sd_event_source *source = NULL;

static struct status older;
static struct status newer;
static struct status diff;
static struct afb_event event;

/***************************************************************************************/
/***************************************************************************************/
/**                                                                                   **/
/**                                                                                   **/
/**       SECTION: BINDING VERBS IMPLEMENTATION                                       **/
/**                                                                                   **/
/**                                                                                   **/
/***************************************************************************************/
/***************************************************************************************/

static int read_status(int fd, struct status *s)
{
	int n;
	ssize_t sz;
	off_t off;
	char buffer[8192];
	long long unsigned x[CPU_FIELD_COUNT];

	off = lseek(fd, 0, SEEK_SET);
	if (off == (off_t)-1)
		return -1;

	sz = read(fd, buffer, sizeof buffer);
	if (sz == -1)
		return -1;

	n = sscanf(buffer, "cpu %Lu %Lu %Lu %Lu %Lu %Lu %Lu %Lu %Lu %Lu", &x[0], &x[1], &x[2], &x[3], &x[4], &x[5], &x[6], &x[7], &x[8], &x[9]);
	if (n != CPU_FIELD_COUNT)
		return -1;
	while(n) {
		n--;
		s->cpu[n] = x[n];
	}
	return 0;
}

static void stop()
{
	sd_event_source_unref(source);
	afb_event_drop(event);
	close(fd_timer);
	close(fd_proc);
	fd_timer = -1;
	fd_proc = -1;
	event.itf = NULL;
	source = NULL;
}

static int emit(sd_event_source *src, int fd, uint32_t revents, void *userdata)
{
	int f, rc, p;
	struct json_object *obj;
	uint64_t u, s, i;

	read(fd, &i, sizeof i);

	memcpy(&older, &newer, sizeof older);
	rc = read_status(fd_proc, &newer);
	for(f = 0; f < (int)CPU_FIELD_COUNT ; f++)
		diff.cpu[f] = newer.cpu[f] - older.cpu[f];
	
	u = 0;
	s = 0;
	i = 0;
	u += diff.cpu[0]; // "user",
	u += diff.cpu[1]; // "nice",
	s += diff.cpu[2]; // "system",
	i += diff.cpu[3]; // "idle",
	i += diff.cpu[4]; // "iowait",
	s += diff.cpu[5]; // "irq",
	s += diff.cpu[6]; // "softirq",
	i += diff.cpu[7]; // "steal",
	u += diff.cpu[8]; // "guest",
	u += diff.cpu[9]; // "guest_nice"
	p = (int)(unsigned)((100 * (u + s)) / (u + s + i));

	obj = json_object_new_int(p);
	if (afb_event_push(event, obj) == 0)
		stop();
	return 0;
}

static int start()
{
	int fdp, fdt, rc;
	struct itimerspec ts;
	struct sd_event_source *src;

	fdp = open("/proc/stat", O_RDONLY|O_CLOEXEC);
	if (fdp >= 0) {
		rc = read_status(fdp, &newer);
		if (rc >= 0) {
			fdt = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK|TFD_CLOEXEC);
			if (fdt >= 0) {
				ts.it_interval.tv_sec = 1;
				ts.it_value.tv_sec = 1;
				ts.it_interval.tv_nsec = 0;
				ts.it_value.tv_nsec = 0;
				rc = timerfd_settime(fdt, 0, &ts, NULL);
				if (rc >= 0) {
					event = afb_daemon_make_event(afbitf->daemon, "stat");
					if (afb_event_is_valid(event)) {
						rc = sd_event_add_io(afb_daemon_get_event_loop(afbitf->daemon), &src, fdt, EPOLLIN, emit, NULL);
						if (rc >= 0) {
							fd_proc = fdp;
							fd_timer = fdt;
							source = src;
							return 0;
						}
						afb_event_drop(event);
					}
				}
				close(fdt);
			}
		}
		close(fdp);
	}
	return -1;
}

static int ensure_started()
{
	return source == NULL ? start() : 0;
}

/***************************************************************************************/
/***************************************************************************************/
/**                                                                                   **/
/**                                                                                   **/
/**       SECTION: BINDING VERBS IMPLEMENTATION                                       **/
/**                                                                                   **/
/**                                                                                   **/
/***************************************************************************************/
/***************************************************************************************/

/*
 * subscribe to notification of stat
 */
static void subscribe(struct afb_req req)
{
	int rc;

	rc = ensure_started();
	if (rc < 0) {
		afb_req_fail(req, "failed", "Can't start");
	} else if (afb_req_subscribe(req, event) != 0) {
		afb_req_fail_f(req, "failed", "afb_req_subscribe returned an error: %m");
	} else {
		afb_req_success(req, NULL, NULL);
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
	if (afb_event_is_valid(event))
		afb_req_unsubscribe(req, event);
	afb_req_success(req, NULL, NULL);
}

/*
 * array of the verbs exported to afb-daemon
 */
static const struct afb_verb_desc_v1 binding_verbs[] = {
  /* VERB'S NAME            SESSION MANAGEMENT          FUNCTION TO CALL         SHORT DESCRIPTION */
  { .name= "subscribe",    .session= AFB_SESSION_NONE, .callback= subscribe,    .info= "subscribe to notification of statistics" },
  { .name= "unsubscribe",  .session= AFB_SESSION_NONE, .callback= unsubscribe,  .info= "unsubscribe a previous subscription" },
  { .name= NULL } /* marker for end of the array */
};

/*
 * description of the binding for afb-daemon
 */
static const struct afb_binding binding_description =
{
  /* description conforms to VERSION 1 */
  .type= AFB_BINDING_VERSION_1,
  .v1= {			/* fills the v1 field of the union when AFB_BINDING_VERSION_1 */
    .prefix= "stat",		/* the API name (or binding name or prefix) */
    .info= "Get system statistics",	/* short description of of the binding */
    .verbs = binding_verbs	/* the array describing the verbs of the API */
  }
};

/*
 * activation function for registering the binding called by afb-daemon
 */
const struct afb_binding *afbBindingV1Register(const struct afb_binding_interface *itf)
{
	afbitf = itf;			/* records the interface for accessing afb-daemon */
	return &binding_description;	/* returns the description of the binding */
}

