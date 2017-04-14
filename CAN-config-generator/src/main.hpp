/*
 * Copyright (C) 2015, 20"IoT.bzh"
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

struct afb_config_list {
		struct afb_config_list *next;
		char *value;
};

// main config structure
struct afb_config {
		char *console;          // console device name (can be a file or a tty)
		char *rootdir;          // base dir for files
		char *roothttp;         // directory for http files
		char *rootbase;         // Angular HTML5 base URL
		char *rootapi;          // Base URL for REST APIs
		char *workdir;          // where to run the program
		char *uploaddir;        // where to store transient files
		char *token;            // initial authentication token [default NULL no session]

		struct afb_config_list *aliases;
		struct afb_config_list *dbus_clients;
		struct afb_config_list *dbus_servers;
		struct afb_config_list *ws_clients;
		struct afb_config_list *ws_servers;
		struct afb_config_list *so_bindings;
		struct afb_config_list *ldpaths;

		char **exec;

		int httpdPort;
		int background;         // run in backround mode
		int cacheTimeout;
		int apiTimeout;
		int cntxTimeout;        // Client Session Context timeout
		int nbSessionMax;       // max count of sessions
		int mode;               // mode of listening
		int tracereq;
		int noHttpd;
};

extern struct afb_config *parse_arguments(int argc, char **argv);