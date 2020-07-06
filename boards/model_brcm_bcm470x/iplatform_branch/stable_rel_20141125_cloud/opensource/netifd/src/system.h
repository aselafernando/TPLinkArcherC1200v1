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
#ifndef __NETIFD_SYSTEM_H
#define __NETIFD_SYSTEM_H

#include <sys/time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "device.h"
#include "interface-ip.h"
#include "iprule.h"

enum tunnel_param {
	TUNNEL_ATTR_TYPE,
	TUNNEL_ATTR_REMOTE,
	TUNNEL_ATTR_LOCAL,
	TUNNEL_ATTR_TTL,
	TUNNEL_ATTR_6RD_PREFIX,
	TUNNEL_ATTR_6RD_RELAY_PREFIX,
	TUNNEL_ATTR_LINK,
	__TUNNEL_ATTR_MAX
};

const struct config_param_list tunnel_attr_list;

enum bridge_opt {
	/* stp and forward delay always set */
	BRIDGE_OPT_AGEING_TIME = (1 << 0),
	BRIDGE_OPT_HELLO_TIME  = (1 << 1),
	BRIDGE_OPT_MAX_AGE     = (1 << 2),
};

struct bridge_config {
	enum bridge_opt flags;
	bool stp;
	bool igmp_snoop;
	unsigned short priority;
	int forward_delay;

	int ageing_time;
	int hello_time;
	int max_age;
};

struct bonding_config {
	int mode;
	char xmit_hash_policy[16];
};

static inline int system_get_addr_family(unsigned int flags)
{
	if ((flags & DEVADDR_FAMILY) == DEVADDR_INET6)
		return AF_INET6;
	else
		return AF_INET;
}

static inline int system_get_addr_len(unsigned int flags)
{
	if ((flags & DEVADDR_FAMILY) == DEVADDR_INET6)
		return sizeof(struct in_addr);
	else
		return sizeof(struct in6_addr);
}

int system_init(void);

bool system_get_link_state(struct device *dev);
int system_bridge_addbr(struct device *bridge, struct bridge_config *cfg);
int system_bridge_delbr(struct device *bridge);
int system_bridge_addif(struct device *bridge, struct device *dev);
int system_bridge_delif(struct device *bridge, struct device *dev);

int system_bonding_addbonding(struct device *bonding, struct bonding_config *cfg);
int system_bonding_delbonding(struct device *bonding);
int system_bonding_addif(struct device *bonding, struct device *dev);
int system_bonding_delif(struct device *bonding, struct device *dev);

int system_vlan_add(struct device *dev, int id);
int system_vlan_del(struct device *dev);

void system_if_clear_state(struct device *dev);
int system_if_up(struct device *dev);
int system_if_down(struct device *dev);
int system_if_check(struct device *dev);
int system_if_dump_info(struct device *dev, struct blob_buf *b);
int system_if_dump_stats(struct device *dev, struct blob_buf *b);
struct device *system_if_get_parent(struct device *dev);
bool system_if_force_external(const char *ifname);
void system_if_apply_settings(struct device *dev, struct device_settings *s);

int system_if_get_addr(const char *ifname, uint32_t *ip);
int system_if_get_mask(const char *ifname, uint32_t *mask);
int system_if_get_macaddr(const char *ifname, uint8_t macaddr[6]);

int system_add_address(struct device *dev, struct device_addr *addr);
int system_del_address(struct device *dev, struct device_addr *addr);

int system_add_route(struct device *dev, struct device_route *route);
int system_del_route(struct device *dev, struct device_route *route);
int system_flush_routes(void);

bool system_resolve_rt_table(const char *name, unsigned int *id);

int system_del_ip_tunnel(const char *name);
int system_add_ip_tunnel(const char *name, struct blob_attr *attr);

int system_add_iprule(struct iprule *rule);
int system_del_iprule(struct iprule *rule);
int system_flush_iprules(void);

bool system_resolve_iprule_action(const char *action, unsigned int *id);

time_t system_get_rtime(void);
int system_get_localtime(struct tm *cur);

void system_fd_set_cloexec(int fd);

int system_update_ipv6_mtu(struct device *device, int mtu);

int system_exec_cmd(char *cmd);
int system_exec_fmt(char *fmt, ...);

#endif
