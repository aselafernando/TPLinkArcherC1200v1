/*
 * netifd - network interface daemon
 * Copyright (c) 2013 The Linux Foundation. All rights reserved.
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
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <errno.h>

#include "netifd.h"
#include "device.h"
#include "interface.h"
#include "system.h"

enum {
	BONDING_ATTR_IFNAME,
	BONDING_ATTR_SLAVES,
	BONDING_ATTR_MODE,
	BONDING_ATTR_XMITHASHPOL,
	__BONDING_ATTR_MAX
};

static const struct blobmsg_policy bonding_attrs[__BONDING_ATTR_MAX] = {
	[BONDING_ATTR_IFNAME] = { "ifname", BLOBMSG_TYPE_STRING },
	[BONDING_ATTR_SLAVES] = { "slaves", BLOBMSG_TYPE_ARRAY },
	[BONDING_ATTR_MODE] = { "mode", BLOBMSG_TYPE_INT32 },
	[BONDING_ATTR_XMITHASHPOL] = { "xmit_hash_policy", BLOBMSG_TYPE_STRING },
};

static const union config_param_info bonding_attr_info[__BONDING_ATTR_MAX] = {
	[BONDING_ATTR_SLAVES] = { .type = BLOBMSG_TYPE_STRING },
};

static const struct config_param_list bonding_attr_list = {
	.n_params = __BONDING_ATTR_MAX,
	.params = bonding_attrs,
	.info = bonding_attr_info,

	.n_next = 1,
	.next = { &device_attr_list },
};

static struct device *bonding_create(const char *name, struct blob_attr *attr);
static void bonding_config_init(struct device *dev);
static void bonding_free(struct device *dev);
static void bonding_dump_info(struct device *dev, struct blob_buf *b);
enum dev_change_type
bonding_reload(struct device *dev, struct blob_attr *attr);

const struct device_type bonding_device_type = {
	.name = "Bonding",
	.config_params = &bonding_attr_list,

	.create = bonding_create,
	.config_init = bonding_config_init,
	.reload = bonding_reload,
	.free = bonding_free,
	.dump_info = bonding_dump_info,
};

struct bonding_state {
	struct device dev;
	device_state_cb set_state;

	struct blob_attr *config_data;
	struct bonding_config config;
	struct blob_attr *ifnames;
	bool active;
	bool force_active;

	struct bonding_member *primary_port;
	struct vlist_tree members;
	int n_present;
};

struct bonding_member {
	struct vlist_node node;
	struct bonding_state *bst;
	struct device_user dev;
	bool present;
	char name[];
};

static void
bonding_reset_primary(struct bonding_state *bst)
{
	struct bonding_member *bm;

	if (!bst->primary_port &&
	    (bst->dev.settings.flags & DEV_OPT_MACADDR))
		return;

	bst->primary_port = NULL;
	bst->dev.settings.flags &= ~DEV_OPT_MACADDR;
	vlist_for_each_element(&bst->members, bm, node) {
		uint8_t *macaddr;

		if (!bm->present)
			continue;

		bst->primary_port = bm;
		if (bm->dev.dev->settings.flags & DEV_OPT_MACADDR)
			macaddr = bm->dev.dev->settings.macaddr;
		else
			macaddr = bm->dev.dev->orig_settings.macaddr;
		memcpy(bst->dev.settings.macaddr, macaddr, 6);
		bst->dev.settings.flags |= DEV_OPT_MACADDR;
		return;
	}
}

static int
bonding_disable_member(struct bonding_member *bm)
{
	struct bonding_state *bst = bm->bst;

	if (!bm->present)
		return 0;

	system_bonding_delif(&bst->dev, bm->dev.dev);
	device_release(&bm->dev);

	return 0;
}

static int
bonding_enable_member(struct bonding_member *bm)
{
	struct bonding_state *bst = bm->bst;
	int ret;

	if (!bm->present)
		return 0;

	ret = device_claim(&bm->dev);
	if (ret < 0)
		goto error;

	ret = system_bonding_addif(&bst->dev, bm->dev.dev);
	if (ret < 0) {
		D(DEVICE, "Bridge device %s could not be added\n", bm->dev.dev->ifname);
		goto error;
	}

	return 0;

error:
	bm->present = false;
	bst->n_present--;
	return ret;
}

static void
bonding_remove_member(struct bonding_member *bm)
{
	struct bonding_state *bst = bm->bst;

	if (!bm->present)
		return;

	if (bm == bst->primary_port);
		bonding_reset_primary(bst);

	if (bst->dev.active)
		bonding_disable_member(bm);

	bm->present = false;
	bm->bst->n_present--;

	bst->force_active = false;
	if (bst->n_present == 0)
		device_set_present(&bst->dev, false);
}

static void
bonding_free_member(struct bonding_member *bm)
{
	struct device *dev = bm->dev.dev;

	bonding_remove_member(bm);
	device_remove_user(&bm->dev);

	/*
	 * When reloading the config and moving a device from one bonding to
	 * another, the other bonding may have tried to claim this device
	 * before it was removed here.
	 * Ensure that claiming the device is retried by toggling its present
	 * state
	 */
	if (dev->present) {
		device_set_present(dev, false);
		device_set_present(dev, true);
	}

	free(bm);
}

static void
bonding_member_cb(struct device_user *dev, enum device_event ev)
{
	struct bonding_member *bm = container_of(dev, struct bonding_member, dev);
	struct bonding_state *bst = bm->bst;

	switch (ev) {
	case DEV_EVENT_ADD:
		assert(!bm->present);

		bm->present = true;
		bst->n_present++;

		if (bst->dev.active)
			bonding_enable_member(bm);
		else if (bst->n_present == 1)
			device_set_present(&bst->dev, true);

		break;
	case DEV_EVENT_REMOVE:
		if (dev->hotplug) {
			vlist_delete(&bst->members, &bm->node);
			return;
		}

		if (bm->present)
			bonding_remove_member(bm);

		break;
	default:
		return;
	}
}

static int
bonding_set_down(struct bonding_state *bst)
{
	struct bonding_member *bm;

	bst->set_state(&bst->dev, false);

	vlist_for_each_element(&bst->members, bm, node)
		bonding_disable_member(bm);

	system_bonding_delbonding(&bst->dev);

	return 0;
}

static int
bonding_set_up(struct bonding_state *bst)
{
	struct bonding_member *bm;
	int ret;

	if (!bst->force_active && !bst->n_present)
		return -ENOENT;

	ret = system_bonding_addbonding(&bst->dev, &bst->config);
	if (ret < 0)
		goto out;

	vlist_for_each_element(&bst->members, bm, node)
		bonding_enable_member(bm);

	if (!bst->force_active && !bst->n_present) {
		/* initialization of all member interfaces failed */
		system_bonding_delbonding(&bst->dev);
		device_set_present(&bst->dev, false);
		return -ENOENT;
	}

	bonding_reset_primary(bst);
	ret = bst->set_state(&bst->dev, true);
	if (ret < 0)
		bonding_set_down(bst);

out:
	return ret;
}

static int
bonding_set_state(struct device *dev, bool up)
{
	struct bonding_state *bst;

	bst = container_of(dev, struct bonding_state, dev);

	if (up)
		return bonding_set_up(bst);
	else
		return bonding_set_down(bst);
}

static struct bonding_member *
bonding_create_member(struct bonding_state *bst, struct device *dev, bool hotplug)
{
	struct bonding_member *bm;

	bm = calloc(1, sizeof(*bm) + strlen(dev->ifname) + 1);
	bm->bst = bst;
	bm->dev.cb = bonding_member_cb;
	bm->dev.hotplug = hotplug;
	strcpy(bm->name, dev->ifname);
	bm->dev.dev = dev;
	vlist_add(&bst->members, &bm->node, bm->name);
	if (hotplug)
		bm->node.version = -1;

	return bm;
}

static void
bonding_member_update(struct vlist_tree *tree, struct vlist_node *node_new,
		     struct vlist_node *node_old)
{
	struct bonding_member *bm;
	struct device *dev;

	if (node_new) {
		bm = container_of(node_new, struct bonding_member, node);

		if (node_old) {
			free(bm);
			return;
		}

		dev = bm->dev.dev;
		bm->dev.dev = NULL;
		device_add_user(&bm->dev, dev);
	}


	if (node_old) {
		bm = container_of(node_old, struct bonding_member, node);
		bonding_free_member(bm);
	}
}


static void
bonding_add_member(struct bonding_state *bst, const char *name)
{
	struct device *dev;

	dev = device_get(name, true);
	if (!dev)
		return;

	bonding_create_member(bst, dev, false);
}

static int
bonding_hotplug_add(struct device *dev, struct device *member)
{
	struct bonding_state *bst = container_of(dev, struct bonding_state, dev);

	bonding_create_member(bst, member, true);

	return 0;
}

static int
bonding_hotplug_del(struct device *dev, struct device *member)
{
	struct bonding_state *bst = container_of(dev, struct bonding_state, dev);
	struct bonding_member *bm;

	bm = vlist_find(&bst->members, member->ifname, bm, node);
	if (!bm)
		return UBUS_STATUS_NOT_FOUND;

	vlist_delete(&bst->members, &bm->node);
	return 0;
}

static int
bonding_hotplug_prepare(struct device *dev)
{
	struct bonding_state *bst;

	bst = container_of(dev, struct bonding_state, dev);
	bst->force_active = true;
	device_set_present(&bst->dev, true);

	return 0;
}

static const struct device_hotplug_ops bonding_ops = {
	.prepare = bonding_hotplug_prepare,
	.add = bonding_hotplug_add,
	.del = bonding_hotplug_del
};

static void
bonding_free(struct device *dev)
{
	struct bonding_state *bst;

	bst = container_of(dev, struct bonding_state, dev);
	vlist_flush_all(&bst->members);
	free(bst);
}

static void
bonding_dump_info(struct device *dev, struct blob_buf *b)
{
	struct bonding_state *bst;
	struct bonding_member *bm;
	void *list;

	bst = container_of(dev, struct bonding_state, dev);

	system_if_dump_info(dev, b);
	list = blobmsg_open_array(b, "bonding-members");

	vlist_for_each_element(&bst->members, bm, node)
		blobmsg_add_string(b, NULL, bm->dev.dev->ifname);

	blobmsg_close_array(b, list);
}

static void
bonding_config_init(struct device *dev)
{
	struct bonding_state *bst;
	struct blob_attr *cur;
	int rem;

	bst = container_of(dev, struct bonding_state, dev);

	if (!bst->ifnames)
		return;

	vlist_update(&bst->members);
	blobmsg_for_each_attr(cur, bst->ifnames, rem) {
		bonding_add_member(bst, blobmsg_data(cur));
	}
	vlist_flush(&bst->members);
}

static void
bonding_apply_settings(struct bonding_state *bst, struct blob_attr **tb)
{
	struct bonding_config *cfg = &bst->config;
	struct blob_attr *cur;

	/* defaults */
	cfg->mode = 0;

	if ((cur = tb[BONDING_ATTR_MODE]))
		cfg->mode = blobmsg_get_u32(cur);

	if ((cur = tb[BONDING_ATTR_XMITHASHPOL])) {
		memcpy(&cfg->xmit_hash_policy, blobmsg_get_string(cur),
		       sizeof(cfg->xmit_hash_policy));
	}
}

enum dev_change_type
bonding_reload(struct device *dev, struct blob_attr *attr)
{
	struct blob_attr *tb_dev[__DEV_ATTR_MAX];
	struct blob_attr *tb_br[__BONDING_ATTR_MAX];
	enum dev_change_type ret = DEV_CONFIG_APPLIED;
	unsigned long diff;
	struct bonding_state *bst;

	BUILD_BUG_ON(sizeof(diff) < __BONDING_ATTR_MAX / 8);
	BUILD_BUG_ON(sizeof(diff) < __DEV_ATTR_MAX / 8);

	bst = container_of(dev, struct bonding_state, dev);

	blobmsg_parse(device_attr_list.params, __DEV_ATTR_MAX, tb_dev,
		blob_data(attr), blob_len(attr));
	blobmsg_parse(bonding_attrs, __BONDING_ATTR_MAX, tb_br,
		blob_data(attr), blob_len(attr));

	bst->ifnames = tb_br[BONDING_ATTR_SLAVES];
	device_init_settings(dev, tb_dev);
	bonding_apply_settings(bst, tb_br);

	if (bst->config_data) {
		struct blob_attr *otb_dev[__DEV_ATTR_MAX];
		struct blob_attr *otb_br[__BONDING_ATTR_MAX];

		blobmsg_parse(device_attr_list.params, __DEV_ATTR_MAX, otb_dev,
			blob_data(bst->config_data), blob_len(bst->config_data));

		diff = 0;
		config_diff(tb_dev, otb_dev, &device_attr_list, &diff);
		if (diff & ~(1 << DEV_ATTR_IFNAME))
		    ret = DEV_CONFIG_RESTART;

		blobmsg_parse(bonding_attrs, __BONDING_ATTR_MAX, otb_br,
			blob_data(bst->config_data), blob_len(bst->config_data));

		diff = 0;
		config_diff(tb_br, otb_br, &bonding_attr_list, &diff);
		if (diff & ~(1 << BONDING_ATTR_IFNAME))
		    ret = DEV_CONFIG_RESTART;

		bonding_config_init(dev);
	}

	bst->config_data = attr;
	return ret;
}

static struct device *
bonding_create(const char *name, struct blob_attr *attr)
{
	struct bonding_state *bst;
	struct device *dev = NULL;

	bst = calloc(1, sizeof(*bst));
	if (!bst)
		return NULL;

	dev = &bst->dev;
	device_init(dev, &bonding_device_type, name);
	dev->config_pending = true;

	bst->set_state = dev->set_state;
	dev->set_state = bonding_set_state;

	dev->hotplug_ops = &bonding_ops;

	vlist_init(&bst->members, avl_strcmp, bonding_member_update);
	bst->members.keep_old = true;
	bonding_reload(dev, attr);

	return dev;
}
