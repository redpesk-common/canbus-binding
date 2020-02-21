/*
 * Copyright (C) 2018 "IoT.bzh"
 * Author "Aymeric Aillet" <aymeric.aillet@iot.bzh>
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

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <poll.h>
#include <pthread.h>
#include <sys/types.h>
#include <ifaddrs.h>
#define __USE_MISC 1
#include <net/if.h>
#include <linux/can.h>

#include "wrap-json.h"

#include "ctl-plugin.h"

#define CAN_ETHERNET_PLUGIN_NAME "can-ethernet"

int IsInterfaceAvailableAndUP (char *ith_name);
void *PassthroughThread(void *arg);
struct can_frame encode(int nbytes, unsigned char *buffer);
unsigned char* decode (struct can_frame frame);
int start(afb_api_t apiHandle, char *uidToStart);
int freethread(char *uidToFree);

struct IpCanConnection {
    char *uid;
    char *ip;
    char *vcan;
    int listen_port;
    int send_to_port;
    int udpsock;
    int cansock;
    pthread_t *thread;
    int timeout_count;
    int threadkill;
    int cancellation_pipe[2];
};

struct IpCanConnectionT {
    struct IpCanConnection *ipCanConnectiontable;
    int size;
};

struct IpCanConnectionT *pluginData = NULL;

CTLP_CAPI_REGISTER(CAN_ETHERNET_PLUGIN_NAME);

CTLP_ONLOAD(plugin, callbacks)
{
    int i, paramsIsArray = 0, paramsLength, *autostart, err;
    json_object *paramsJ, *currentParamsJ;

    AFB_API_DEBUG(plugin->api, "Plugin %s is loading ...", CAN_ETHERNET_PLUGIN_NAME);

    if(!plugin->paramsJ) {
        AFB_API_ERROR(plugin->api, "%s: Empty plugin parameter json", CAN_ETHERNET_PLUGIN_NAME);
            return -1;
    }

    switch(json_object_get_type(plugin->paramsJ)) {
        case json_type_array:
            paramsIsArray = 1;
            paramsLength = json_object_array_length(plugin->paramsJ);
            break;

        case json_type_object:
            paramsLength = 1;
            break;

        default:
            AFB_API_ERROR(plugin->api,
                          "Unrecognized plugin parameter json type, should be object/array type (is '%s')",
                          json_object_get_string(plugin->paramsJ));
            return -1;
    }

    paramsJ = plugin->paramsJ;

    pluginData = calloc(1, sizeof(struct IpCanConnectionT));
    if(!pluginData) {
        AFB_API_ERROR(plugin->api,
                          "Global ipCan structure allocation error");
        return -1;
    }

    pluginData->size = paramsLength;
    pluginData->ipCanConnectiontable = calloc(paramsLength, sizeof(struct IpCanConnection));
    if(!pluginData->ipCanConnectiontable) {
         AFB_API_ERROR(plugin->api,
                          "ipCanConnection table allocation error");
        return -1;
    }

    autostart = calloc(paramsLength, sizeof(int));

    for(i = 0; i < paramsLength; i++) {
        if(paramsIsArray)
            currentParamsJ = json_object_array_get_idx(paramsJ, i);
        else
            currentParamsJ = paramsJ;

        if(!json_object_is_type(currentParamsJ, json_type_object)) {
            AFB_API_ERROR(plugin->api,
                          "Unrecognized plugin element json type, should be object type (is '%s')",
                          json_object_get_string(currentParamsJ));
            free(pluginData);
            return -1;
        }

        int err = wrap_json_unpack(currentParamsJ,
                               "{s:s, s:s, s:i, s:i, s:s, s:i, s:i !}",
                               "uid", &pluginData->ipCanConnectiontable[i].uid,
                               "ip", &pluginData->ipCanConnectiontable[i].ip,
                               "listen_port", &pluginData->ipCanConnectiontable[i].listen_port,
                               "send_to_port", &pluginData->ipCanConnectiontable[i].send_to_port,
                               "vcan", &pluginData->ipCanConnectiontable[i].vcan,
                               "timeout_count", &pluginData->ipCanConnectiontable[i].timeout_count,
                               "autostart", &autostart[i]);
        if(err) {
            AFB_API_ERROR(plugin->api,
                          "Invalid json object, should contain [uid]|[ip]|[listen_port]|[send_to_port]|[vcan] (is '%s')",
                          json_object_get_string(currentParamsJ));
            return -1;
        }

        if(!IsInterfaceAvailableAndUP(pluginData->ipCanConnectiontable[i].vcan)) {
            AFB_API_ERROR(plugin->api,
                          "Interface %s is not available or DOWN",
                          pluginData->ipCanConnectiontable[i].vcan);
            return -1;
        }
    }

    AFB_API_INFO(plugin->api, "Plugin loaded correctly");

    for (i = 0; i<paramsLength; i++) {
        if (autostart[i]) {
            err = start(plugin, pluginData->ipCanConnectiontable[i].uid);
            if(err) {
                AFB_API_ERROR(plugin->api, "AUTOSTART : Error %i when tried to start %s", err, pluginData->ipCanConnectiontable[i].uid);
                return -1;
            }
            AFB_API_INFO(plugin->api, "AUTOSTART : Thread %s started correctly", pluginData->ipCanConnectiontable[i].uid);
        }
    }

    return 0;
}

CTLP_CAPI(CElist, source, argsJ, eventJ)
{
    int i, err;

    json_object *responseArrayJ = NULL, *connectionJ;

    if(!pluginData || !pluginData->size || !pluginData->ipCanConnectiontable) {
        afb_req_fail_f(source->request, "empty_data", "Current plugin data structure is empty, cannot answer the request");
        AFB_REQ_ERROR(source->request, "Current plugin data structure is empty, cannot answer the request");
        return -1;
    }

    responseArrayJ = json_object_new_array();

    for(i = 0; i < pluginData->size; i++) {
        err = wrap_json_pack(&connectionJ,
                             "{s:s, s:s, s:s, s:i, s:i, s:b}",
                             "uid", pluginData->ipCanConnectiontable[i].uid,
                             "ip", pluginData->ipCanConnectiontable[i].ip,
                             "vcan", pluginData->ipCanConnectiontable[i].vcan,
                             "listen_port", pluginData->ipCanConnectiontable[i].listen_port,
                             "send_to_port", pluginData->ipCanConnectiontable[i].send_to_port,
                             "running", pluginData->ipCanConnectiontable[i].thread ? 1 : 0);
        if(err) {
            afb_req_fail_f(source->request, "no_answer", "Generation of the answer failed");
            AFB_REQ_ERROR(source->request, "Generation of the answer failed");
            json_object_put(responseArrayJ);
            return -1;
        }

        json_object_array_add(responseArrayJ, connectionJ);
    }

    afb_req_success(source->request, responseArrayJ, "List of bidirectional communication way");

    return 0;
}

CTLP_CAPI(CEstart, source, argsJ, eventJ)
{
    int err;

    char *uidToStart = NULL;

    afb_api_t apiHandle = afb_req_get_api(source->request);

    if(!eventJ) {
        afb_req_fail_f(source->request, "empty_param", "START : Empty json parameter");
        AFB_API_ERROR(apiHandle, "START : Empty json parameter");
        return -1;
    }

    err = wrap_json_unpack(eventJ, "{s:s !}", "uid", &uidToStart);
    if(err) {
        afb_req_fail_f(source->request, "invalid_json", "START : Error unpacking json parameter");
        AFB_API_ERROR(apiHandle, "START : Error unpacking json parameter (%s)", json_object_get_string(eventJ));
        return -1;
    }

    err = start(apiHandle, uidToStart);
    if(err) {
        if(err == -1) {
            afb_req_fail_f(source->request, "already_running", "START : Thread %s is already running", uidToStart);
            return -1;
            //would like to use afb_req_success here but cause a seg fault ...
        }
        else if (err == -2) {
            afb_req_fail_f(source->request, "creation_error", "START : Thread creation error for uid %s", uidToStart);
            return -2;
        }
        else if (err == -3) {
            afb_req_fail_f(source->request, "uid_not_found", "START : UID %s not found", uidToStart);
            return -3;
        }
    }

    afb_req_success_f(source->request, NULL, "START : Thread for uid %s well started", uidToStart);

    return 0;
}

CTLP_CAPI(CEstop, source, argsJ, eventJ)
{
    int err;
    char dummy = 0;
    char *uidToStop = NULL;

    if(!eventJ) {
        afb_req_fail_f(source->request, "empty_param", "STOP : Empty json parameter");
        AFB_REQ_ERROR(source->request, "STOP : Empty json parameter");
        return -1;
    }

    err = wrap_json_unpack(eventJ, "{s:s !}", "uid", &uidToStop);
    if(err) {
        afb_req_fail_f(source->request, "invalid_json", "STOP : Error unpacking json parameter");
        AFB_REQ_ERROR(source->request, "STOP : Error unpacking json parameter (%s)", json_object_get_string(eventJ));
        return -1;
    }

    for (int i = 0; i < pluginData->size; i++) {
        if (! strcasecmp(pluginData->ipCanConnectiontable[i].uid, uidToStop)) {
            if (!pluginData->ipCanConnectiontable[i].thread) {
                afb_req_fail_f(source->request, "not_started", "STOP : Thread for uid %s is not running", uidToStop);
                AFB_REQ_ERROR(source->request, "STOP : Thread is not running");
                return -1;
            }

            pluginData->ipCanConnectiontable[i].threadkill = 1;
            write(pluginData->ipCanConnectiontable[i].cancellation_pipe[1], &dummy, sizeof(dummy));

            while(pluginData->ipCanConnectiontable[i].threadkill) {
                usleep(250);
            }

            freethread(pluginData->ipCanConnectiontable[i].uid);

            afb_req_success_f(source->request, NULL, "STOP : Thread for uid %s properly stoped", uidToStop);
            return 0;
        }
    }
    afb_req_fail_f(source->request, "not_found", "STOP : UID not found");
    AFB_REQ_ERROR(source->request, "STOP : UID not found");
    return -1;
}

CTLP_CAPI(CEstatus, source, argsJ, eventJ)
{
///callback for the event infos
}

int IsInterfaceAvailableAndUP (char *ith_name){
    struct ifaddrs *ifap, *ifa;

    getifaddrs (&ifap);
    for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
        if(!strcmp(ifa->ifa_name, ith_name) ) {
            if (ifa->ifa_flags & IFF_UP){
                freeifaddrs(ifap);
                return 1;
            }
        }
    }
    freeifaddrs(ifap);
    return 0;
}

void *PassthroughThread(void *arg) {
    struct IpCanConnection *MyIpCanConnection = (struct IpCanConnection *) arg;

    struct sockaddr_in my_addr, client_addr;
    struct sockaddr_can can_addr;
    struct ifreq ifr;

    int nbytes, err, timeout=500, timeout_count=0;
    struct can_frame frame;
	unsigned char *buffer;
    buffer = calloc(1,13);

    // Creating and managing CAN socket file descriptor
    MyIpCanConnection->cansock = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (MyIpCanConnection->cansock < 0) {
		perror("CAN socket creation failed");
        // TODO : print error
        return -1;
    }
    strcpy(ifr.ifr_name, MyIpCanConnection->vcan);
    ioctl(MyIpCanConnection->cansock, SIOCGIFINDEX, &ifr);
    can_addr.can_family = AF_CAN;
    can_addr.can_ifindex = ifr.ifr_ifindex;
    int res = bind(MyIpCanConnection->cansock, (struct sockaddr *)&can_addr, sizeof(can_addr));
    if (res < 0){
       perror("CAN socket binding failed");
       //TODO : print error
       return -1;
    }

	// Creating UDP socket file descriptor
    MyIpCanConnection->udpsock = socket(AF_INET, SOCK_DGRAM, 0);
	if (MyIpCanConnection->udpsock < 0) {
		perror("UDP socket creation failed");
        // TODO : print error
        return -1;
	}

    //preparing my sockaddr
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons((uint16_t)MyIpCanConnection->listen_port);
	my_addr.sin_addr.s_addr = INADDR_ANY;
    res = bind(MyIpCanConnection->udpsock,(struct sockaddr*)&my_addr,sizeof(my_addr));
    if (res < 0){
        perror("UDP socket binding failed");
        //TODO : print error
        return -1;
    }

    // Preparing client sockaddr
    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons((uint16_t)MyIpCanConnection->send_to_port);
    client_addr.sin_addr.s_addr = inet_addr(MyIpCanConnection->ip);

	int len = sizeof(client_addr);

    //managing poll settings
    struct pollfd ufds[3];
    ufds[0].fd = MyIpCanConnection->udpsock;
    ufds[0].events = POLLIN;
    ufds[1].fd = MyIpCanConnection->cansock;
    ufds[1].events = POLLIN;
    ufds[2].fd = MyIpCanConnection->cancellation_pipe[0];
    ufds[2].events = POLLIN;

    while (!MyIpCanConnection->threadkill)
    {
        err = poll(ufds,3,timeout);
        if(err) {
            timeout = 500;
            timeout_count = 0;
            if (ufds[0].revents == POLLIN) {
                nbytes = recvfrom(MyIpCanConnection->udpsock, buffer, 1024, MSG_WAITALL, (struct sockaddr *) &client_addr,&len);
                if(nbytes != 13) {
                    perror("Error receiving udp frame");
                    continue;
                }

                if(strncmp (inet_ntoa(client_addr.sin_addr), MyIpCanConnection->ip,16)) {
                    perror("Error, UDP frame not comming from given IP address");
                    continue;
                }

                frame = encode(nbytes,buffer);
                nbytes = write(MyIpCanConnection->cansock, &frame, sizeof(struct can_frame));

                if(nbytes == 16)
                    printf("Encoded UDP frame well written to %s\n",MyIpCanConnection->vcan);
                else
                    perror("Error sending can frame");
            }
            else if (ufds[1].revents == POLLIN) {
                nbytes = read(MyIpCanConnection->cansock,&frame,sizeof(struct can_frame));
                if (nbytes != 16) {
                    perror("Error receiving can frame");
                    continue;
                }

                buffer = decode(frame);
                nbytes = sendto(MyIpCanConnection->udpsock,buffer,13 ,0 ,(struct sockaddr *) &client_addr, len);

                if(nbytes == 13) {
                    printf("Decoded CAN frame well written to %s:%d\n",MyIpCanConnection->ip,MyIpCanConnection->send_to_port);
                }
                else {
                    perror("Error sending udp frame");
                }
            }
        }
        else {
            if(err == 0) {
                timeout_count += 1;
                //printf("Poll timeout, timeout_count = %d/%d\n",timeout_count,MyIpCanConnection->timeout_count);
                if(timeout_count == MyIpCanConnection->timeout_count) {
                    //check if we enter the if at the right moment (do a -1 ?)
                    MyIpCanConnection->threadkill = 2;
                    //printf("Timeout count reached setted count, the thread is being kill\n");
                }
                else{
                    timeout *= 2;
                    //printf("new timeout_time = %d\n",timeout);
                }
            }
            if(err < 0) {
                perror("Error polling");
            }
        }
    }
    //printf("kill signal acquired in thread %s, killing thread\n", MyIpCanConnection->uid);

    free(buffer);

    if(MyIpCanConnection->threadkill == 2) {
        freethread(MyIpCanConnection->uid);
    }

    MyIpCanConnection->threadkill = 0;
}

struct can_frame encode(int nbytes, unsigned char *buffer) {

    unsigned char id[4], payload[8], is_extended, length;

    buffer[nbytes] = '\0';

    for (int i=0;i<nbytes;i++){
        if (i == 0) {
            length = (buffer[i] & 0x0F);
            is_extended = (buffer[i] & 0xF0) >> 4;
        }
        if (0<i && i<5) id[i-1] = buffer[i];
        if (4<i) payload[i-5] = buffer[i];
    }

    struct can_frame frame;
    for (int j=0;j<(int)length;j++) {
        frame.data[j] = payload[j];
    }
    frame.can_id = (id[0]<<8*3)+(id[1]<<8*2)+(id[2]<<8*1)+(id[3]);
    if (is_extended == 10){
        frame.can_id |= 1 << 31;
    }
    frame.can_dlc = (int)length;

    return frame;
}

unsigned char* decode (struct can_frame frame) {

    unsigned char* buffer;
    buffer = (unsigned char*) calloc(13*sizeof(unsigned char),1);

    if(frame.can_id >> 31) {
        buffer[0] = 0xA0;
        frame.can_id ^= 1UL << 31;
    }
    else {
        buffer[0] = 0x20;
    }
    buffer[0] |= frame.can_dlc;

    for(int i=0;i<4;i++){
        buffer[4-i] = ((frame.can_id>>(8*i)) & 0xFF);
    }

    for (int i=0; i<frame.can_dlc; i++){
        buffer[5+i] = frame.data[i];
    }

    return buffer;
}

int start(afb_api_t apiHandle, char *uidToStart) {
    int i;

    for (i = 0; i < pluginData->size; i++) {
        if (! strcasecmp(pluginData->ipCanConnectiontable[i].uid, uidToStart)) {
            if (pluginData->ipCanConnectiontable[i].thread) {
                AFB_API_NOTICE(apiHandle, "START : Thread is already running");
                return -1;
            }

            pluginData->ipCanConnectiontable[i].thread = calloc(1, sizeof(pthread_t));

            int err = pipe(pluginData->ipCanConnectiontable[i].cancellation_pipe);
            if(err == -1) {
                printf("Error pipping\n");
            }

            //TODO : declare both fd

            if (pthread_create(pluginData->ipCanConnectiontable[i].thread, NULL, PassthroughThread, &pluginData->ipCanConnectiontable[i])) {
                //TODO : close fd
                AFB_API_ERROR(apiHandle, "START : Thread creation error");
                return -2;
            }
            return 0;
        }
    }
    AFB_API_ERROR(apiHandle, "START : UID not found");
    return -3;
}

int freethread(char *uidToFree) {
    int i;
    for (i=0; i < pluginData->size; i++) {
        if(!strcasecmp(pluginData->ipCanConnectiontable[i].uid, uidToFree)) {
            if(pluginData->ipCanConnectiontable[i].thread) {
                free(pluginData->ipCanConnectiontable[i].thread);
                pluginData->ipCanConnectiontable[i].thread = NULL;
                close(pluginData->ipCanConnectiontable[i].cancellation_pipe[0]);
                close(pluginData->ipCanConnectiontable[i].cancellation_pipe[1]);
                close(pluginData->ipCanConnectiontable[i].udpsock);
                close(pluginData->ipCanConnectiontable[i].cansock);
                return 0;
            }
            //thread is not running
            return -1;
        }
    }
    //uid not found
    return -2;
}
