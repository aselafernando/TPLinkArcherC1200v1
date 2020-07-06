/*
 * Copyright (C) 2011-2012 Felix Fietkau <nbd@openwrt.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 2.1
 * as published by the Free Software Foundation
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include <libubox/blob.h>
#include <libubox/blobmsg.h>

#include "libubus.h"
#include "libubus-internal.h"
#include "ubusmsg.h"

const char *__ubus_strerror[__UBUS_STATUS_LAST] = {
	[UBUS_STATUS_OK] = "Success",
	[UBUS_STATUS_INVALID_COMMAND] = "Invalid command",
	[UBUS_STATUS_INVALID_ARGUMENT] = "Invalid argument",
	[UBUS_STATUS_METHOD_NOT_FOUND] = "Method not found",
	[UBUS_STATUS_NOT_FOUND] = "Not found",
	[UBUS_STATUS_NO_DATA] = "No response",
	[UBUS_STATUS_PERMISSION_DENIED] = "Permission denied",
	[UBUS_STATUS_TIMEOUT] = "Request timed out",
	[UBUS_STATUS_NOT_SUPPORTED] = "Operation not supported",
	[UBUS_STATUS_UNKNOWN_ERROR] = "Unknown error",
	[UBUS_STATUS_CONNECTION_FAILED] = "Connection failed",
};

struct blob_buf b __hidden = {};

struct ubus_pending_msg {
	struct list_head list;
	struct ubus_msghdr hdr;
};

static int ubus_cmp_id(const void *k1, const void *k2, void *ptr)
{
	const uint32_t *id1 = k1, *id2 = k2;

	if (*id1 < *id2)
		return -1;
	else
		return *id1 > *id2;
}

const char *ubus_strerror(int error)
{
	static char err[32];

	if (error < 0 || error >= __UBUS_STATUS_LAST)
		goto out;

	if (!__ubus_strerror[error])
		goto out;

	return __ubus_strerror[error];

out:
	sprintf(err, "Unknown error: %d", error);
	return err;
}

static void
ubus_queue_msg(struct ubus_context *ctx, struct ubus_msghdr *hdr)
{
	struct ubus_pending_msg *pending;

	pending = calloc(1, sizeof(*pending) + blob_raw_len(hdr->data));
	if (!pending)
		return;

	memcpy(&pending->hdr, hdr, sizeof(*hdr) + blob_raw_len(hdr->data));
	list_add(&pending->list, &ctx->pending);
}

void __hidden
ubus_process_msg(struct ubus_context *ctx, struct ubus_msghdr *hdr)
{

	switch(hdr->type) {
	case UBUS_MSG_STATUS:
	case UBUS_MSG_DATA:
		ubus_process_req_msg(ctx, hdr);
		break;

	case UBUS_MSG_INVOKE:
	case UBUS_MSG_UNSUBSCRIBE:
	case UBUS_MSG_NOTIFY:
		if (ctx->stack_depth > 2) {
			ubus_queue_msg(ctx, hdr);
			break;
		}

		ubus_process_obj_msg(ctx, hdr);
		break;
	}
}

void __hidden ubus_process_pending_msg(struct ubus_context *ctx)
{
	struct ubus_pending_msg *pending;

	while (!list_empty(&ctx->pending)) {
		pending = list_first_entry(&ctx->pending, struct ubus_pending_msg, list);
		list_del(&pending->list);
		ubus_process_msg(ctx, &pending->hdr);
		free(pending);
		if (ctx->stack_depth > 2)
			break;
	}
}

struct ubus_lookup_request {
	struct ubus_request req;
	ubus_lookup_handler_t cb;
};

static void ubus_lookup_cb(struct ubus_request *ureq, int type, struct blob_attr *msg)
{
	struct ubus_lookup_request *req;
	struct ubus_object_data obj;
	struct blob_attr **attr;

	req = container_of(ureq, struct ubus_lookup_request, req);
	attr = ubus_parse_msg(msg);

	if (!attr[UBUS_ATTR_OBJID] || !attr[UBUS_ATTR_OBJPATH] ||
	    !attr[UBUS_ATTR_OBJTYPE])
		return;

	memset(&obj, 0, sizeof(obj));
	obj.id = blob_get_u32(attr[UBUS_ATTR_OBJID]);
	obj.path = blob_data(attr[UBUS_ATTR_OBJPATH]);
	obj.type_id = blob_get_u32(attr[UBUS_ATTR_OBJTYPE]);
	obj.signature = attr[UBUS_ATTR_SIGNATURE];
	req->cb(ureq->ctx, &obj, ureq->priv);
}

int ubus_lookup(struct ubus_context *ctx, const char *path,
		ubus_lookup_handler_t cb, void *priv)
{
	struct ubus_lookup_request lookup;

	blob_buf_init(&b, 0);
	if (path)
		blob_put_string(&b, UBUS_ATTR_OBJPATH, path);

	if (ubus_start_request(ctx, &lookup.req, b.head, UBUS_MSG_LOOKUP, 0) < 0)
		return UBUS_STATUS_INVALID_ARGUMENT;

	lookup.req.raw_data_cb = ubus_lookup_cb;
	lookup.req.priv = priv;
	lookup.cb = cb;
	return ubus_complete_request(ctx, &lookup.req, 0);
}

static void ubus_lookup_id_cb(struct ubus_request *req, int type, struct blob_attr *msg)
{
	struct blob_attr **attr;
	uint32_t *id = req->priv;

	attr = ubus_parse_msg(msg);

	if (!attr[UBUS_ATTR_OBJID])
		return;

	*id = blob_get_u32(attr[UBUS_ATTR_OBJID]);
}

int ubus_lookup_id(struct ubus_context *ctx, const char *path, uint32_t *id)
{
	struct ubus_request req;

	blob_buf_init(&b, 0);
	if (path)
		blob_put_string(&b, UBUS_ATTR_OBJPATH, path);

	if (ubus_start_request(ctx, &req, b.head, UBUS_MSG_LOOKUP, 0) < 0)
		return UBUS_STATUS_INVALID_ARGUMENT;

	req.raw_data_cb = ubus_lookup_id_cb;
	req.priv = id;

	return ubus_complete_request(ctx, &req, 0);
}

static int ubus_event_cb(struct ubus_context *ctx, struct ubus_object *obj,
			 struct ubus_request_data *req,
			 const char *method, struct blob_attr *msg)
{
	struct ubus_event_handler *ev;

	ev = container_of(obj, struct ubus_event_handler, obj);
	ev->cb(ctx, ev, method, msg);
	return 0;
}

static const struct ubus_method event_method = {
	.name = NULL,
	.handler = ubus_event_cb,
};

int ubus_register_event_handler(struct ubus_context *ctx,
				struct ubus_event_handler *ev,
				const char *pattern)
{
	struct ubus_object *obj = &ev->obj;
	struct blob_buf b2;
	int ret;

	if (!obj->id) {
		obj->methods = &event_method;
		obj->n_methods = 1;

		if (!!obj->name ^ !!obj->type)
			return UBUS_STATUS_INVALID_ARGUMENT;

		ret = ubus_add_object(ctx, obj);
		if (ret)
			return ret;
	}

	/* use a second buffer, ubus_invoke() overwrites the primary one */
	memset(&b2, 0, sizeof(b2));
	blob_buf_init(&b2, 0);
	blobmsg_add_u32(&b2, "object", obj->id);
	if (pattern)
		blobmsg_add_string(&b2, "pattern", pattern);

	return ubus_invoke(ctx, UBUS_SYSTEM_OBJECT_EVENT, "register", b2.head,
			  NULL, NULL, 0);
}

int ubus_send_event(struct ubus_context *ctx, const char *id,
		    struct blob_attr *data)
{
	struct ubus_request req;
	void *s;

	blob_buf_init(&b, 0);
	blob_put_int32(&b, UBUS_ATTR_OBJID, UBUS_SYSTEM_OBJECT_EVENT);
	blob_put_string(&b, UBUS_ATTR_METHOD, "send");
	s = blob_nest_start(&b, UBUS_ATTR_DATA);
	blobmsg_add_string(&b, "id", id);
	blobmsg_add_field(&b, BLOBMSG_TYPE_TABLE, "data", blob_data(data), blob_len(data));
	blob_nest_end(&b, s);

	if (ubus_start_request(ctx, &req, b.head, UBUS_MSG_INVOKE, UBUS_SYSTEM_OBJECT_EVENT) < 0)
		return UBUS_STATUS_INVALID_ARGUMENT;

	return ubus_complete_request(ctx, &req, 0);
}

static void ubus_default_connection_lost(struct ubus_context *ctx)
{
	if (ctx->sock.registered)
		uloop_end();
}

struct ubus_context *ubus_connect(const char *path)
{
	struct ubus_context *ctx;

	ctx = calloc(1, sizeof(*ctx));
	if (!ctx)
		return NULL;

	ctx->sock.fd = -1;
	ctx->sock.cb = ubus_handle_data;
	ctx->connection_lost = ubus_default_connection_lost;

	INIT_LIST_HEAD(&ctx->requests);
	INIT_LIST_HEAD(&ctx->pending);
	avl_init(&ctx->objects, ubus_cmp_id, false, NULL);
	if (ubus_reconnect(ctx, path)) {
		free(ctx);
		ctx = NULL;
	}

	return ctx;
}

void ubus_free(struct ubus_context *ctx)
{
	blob_buf_free(&b);
	close(ctx->sock.fd);
	free(ctx);
}
