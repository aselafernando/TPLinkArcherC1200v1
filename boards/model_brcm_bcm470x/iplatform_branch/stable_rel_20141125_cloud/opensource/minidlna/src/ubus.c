/*
 * =====================================================================================
 *
 *       Filename:  ubus.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/11/2014 09:02:03 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Zhu Xianfeng<zhuxianfeng@tp-link.net>
 *   Organization:  
 *
 * =====================================================================================
 */
#if !defined(__X86__)

#include <stdlib.h>
#include <stdio.h>

#include <libubox/uloop.h>
#include <libubox/blob.h>
#include <libubox/blobmsg.h>
#include <libubus.h>

#define DEBUG(fmt, args...) \
do \
{  \
    printf("%s():%d " fmt, __FUNCTION__, __LINE__, ##args); \
} while (0)

enum 
{
    GNET_GET_TYPE,
    GNET_GET_MAX,
};

struct gnet_resp
{
    int parsed;
    int guest_net;
};

static int l_gnet_init;
static struct gnet_resp l_gnet_resp;
static struct ubus_context *l_ubus_ctx;
static struct blob_buf l_blob_buf;

static struct blobmsg_policy l_resp_policy[GNET_GET_MAX] = 
{
    {
        .name = "guest",
        .type = BLOBMSG_TYPE_INT8,
    },
};

static void 
gnet_data_cb(struct ubus_request *req, int type, struct blob_attr *msg)
{
    struct blob_attr *tb[GNET_GET_MAX];
    int ret = 0;

    l_gnet_resp.parsed = 0;

    if (!msg)
    {
        return;
    }

    ret = blobmsg_parse(l_resp_policy, GNET_GET_MAX, tb, blob_data(msg), blob_len(msg));
    if (ret < 0)
    {
        DEBUG("Response MSG parsed failed\n");
        return;
    }

    l_gnet_resp.guest_net = blobmsg_get_u8(tb[GNET_GET_TYPE]);
    l_gnet_resp.parsed = 1;
}

int gnet_is_guest(int ipaddr, int *guest)
{
    struct blob_buf *buf = &l_blob_buf;
    uint32_t id;
    int ret;

    ret = ubus_lookup_id(l_ubus_ctx, "client_mgmt", &id);
    if (ret != 0)
    {
        DEBUG("Failed to look up \"client_mgmt\" object\n");
        return -1;
    }

    blob_buf_init(buf, 0);
    blobmsg_add_u32(buf, "request_type", 1);
    blobmsg_add_u32(buf, "ipv4_addr", ipaddr);

    ret = ubus_invoke(l_ubus_ctx, id, "get", buf->head, gnet_data_cb, NULL, 3000);
    if (ret != UBUS_STATUS_OK)
    {
        DEBUG("ubus_invoke return %d\n", ret);
        return -1;
    }

    if (!l_gnet_resp.parsed)
    {
        return -1;
    }

    *guest = l_gnet_resp.guest_net;
    return 0;
}

int gnet_ubus_init()
{
    char *ubus_sock_path = NULL;

    if (l_gnet_init)
    {
        return 0;
    }

    uloop_init();
    l_ubus_ctx = ubus_connect(ubus_sock_path);
    if (!l_ubus_ctx)
    {
        DEBUG("Failed to connect to ubus\n");
        return -1;
    }

    ubus_add_uloop(l_ubus_ctx);

    l_gnet_init = 1;
    return 0;
}

void gnet_ubus_exit()
{
    if (!l_gnet_init)
    {
        return;
    }

    if (l_ubus_ctx != NULL)
    {
        ubus_free(l_ubus_ctx);
    }

    uloop_done();
    l_gnet_init = 0;
}

#else

int gnet_is_guest(int ipaddr, int *guest)
{
    return 0;
}

int gnet_ubus_init()
{
    return 0;
}

void gnet_ubus_exit()
{
    return;
}

#endif
