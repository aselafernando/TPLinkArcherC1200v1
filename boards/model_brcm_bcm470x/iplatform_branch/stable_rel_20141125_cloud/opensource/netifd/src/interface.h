/*
 * netifd - network interface daemon
 * Copyright (C) 2012 Felix Fietkau <nbd@openwrt.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
#ifndef __NETIFD_INTERFACE_H
#define __NETIFD_INTERFACE_H

#include "device.h"
#include "config.h"

struct interface;
struct interface_proto_state;

enum interface_event {
	IFEV_DOWN,
	IFEV_UP,
	IFEV_FREE,
	IFEV_RELOAD,
};

enum interface_state {
	IFS_SETUP,
	IFS_UP,
	IFS_TEARDOWN,
	IFS_DOWN,
};

enum interface_config_state {
	IFC_NORMAL,
	IFC_RELOAD,
	IFC_REMOVE
};

enum interface_conn_mode {
    IFCM_INVALID = 0,
    IFCM_AUTO,
    IFCM_MANUAL,
    IFCM_DEMAND,
    IFCM_TIMEBASED,
    IFCM_MAX
};

struct interface_error {
	struct list_head list;

	const char *subsystem;
	const char *code;
	const char *data[];
};

struct interface_user {
	struct list_head list;
	struct interface *iface;
	void (*cb)(struct interface_user *dep, struct interface *iface, enum interface_event ev);
};

struct interface_ip_settings {
	struct interface *iface;
	bool enabled;
	bool no_defaultroute;
	bool no_dns;

	uint8_t assignment_length;
	int32_t assignment_hint;

	struct vlist_tree addr;
	struct vlist_tree route;
	struct vlist_tree prefix;

	struct vlist_simple_tree dns_servers;
	struct vlist_simple_tree dns_search;
};

struct interface_data {
	struct avl_node node;
	struct blob_attr data[];
};

struct interface_conn_time {
    time_t idle;     /* max idle second for demand or manual         */
    time_t start;    /* valid start second of daytime for time based */
    time_t end;      /* valid end second of daytime for time based   */
};

/*
 * interface configuration
 */
struct interface {
	struct vlist_node node;
	struct list_head hotplug_list;
	enum interface_event hotplug_ev;

	char name[IFNAMSIZ];
	const char *ifname;

	bool available;
	bool autostart;
    bool connectable;
	bool config_autostart;
	bool device_config;
	bool link_state;

	time_t start_time;
	enum interface_state state;
	enum interface_config_state config_state;

    enum interface_conn_mode conn_mode;
    struct interface_conn_time conn_time;

	struct list_head users;

	const char *parent_ifname;
	struct interface_user parent_iface;

	/* main interface that the interface is bound to */
	struct device_user main_dev;

	/* interface that layer 3 communication will go through */
	struct device_user l3_dev;

	struct blob_attr *config;

	/* primary protocol state */
	const struct proto_handler *proto_handler;
	struct interface_proto_state *proto;

	struct interface_ip_settings proto_ip;
	struct interface_ip_settings config_ip;
	struct vlist_tree host_routes;

	int metric;

	/* errors/warnings while trying to bring up the interface */
	struct list_head errors;

	/* extra data provided by protocol handlers or modules */
	struct avl_tree data;

	struct uloop_timeout remove_timer;
	struct ubus_object ubus;
};


extern struct vlist_tree interfaces;
extern const struct config_param_list interface_attr_list;

void interface_init(struct interface *iface, const char *name,
		    struct blob_attr *config);

void interface_add(struct interface *iface, struct blob_attr *config);
bool interface_add_alias(struct interface *iface, struct blob_attr *config);

void interface_set_proto_state(struct interface *iface, struct interface_proto_state *state);

void interface_set_available(struct interface *iface, bool new_state);
int interface_set_up(struct interface *iface);
int interface_set_down(struct interface *iface);
void __interface_set_down(struct interface *iface, bool force);

void interface_set_main_dev(struct interface *iface, struct device *dev);
void interface_set_l3_dev(struct interface *iface, struct device *dev);

void interface_add_user(struct interface_user *dep, struct interface *iface);
void interface_remove_user(struct interface_user *dep);

int interface_add_link(struct interface *iface, struct device *dev);
int interface_remove_link(struct interface *iface, struct device *dev);

void interface_add_error(struct interface *iface, const char *subsystem,
			 const char *code, const char **data, int n_data);

int interface_add_data(struct interface *iface, const struct blob_attr *data);

void interface_update_start(struct interface *iface);
void interface_update_complete(struct interface *iface);

void interface_start_pending(void);

#endif
