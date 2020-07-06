/*! Copyright(c) 2008-2014 Shenzhen TP-LINK Technologies Co.Ltd.
 *
 *\file     connect.c
 *\brief        
 *\details  
 *
 *\author   Zhu Xianfeng<zhuxianfeng@tp-link.net>
 *\version  1.0.0
 *\date     29May14
 *
 *\warning  
 *
 *\history \arg 29May14, Zhu Xianfeng, create the file.
 */
#include <string.h>
#include <time.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "netifd.h"
#include "interface.h"
#include "proto.h"
#include "system.h"
#include "connect.h"

#define IPT_STAT_BASE_CTL       (128)
#define IPT_STAT_SET_NET        (IPT_STAT_BASE_CTL)
#define IPT_STAT_DEL_ALL        (IPT_STAT_BASE_CTL + 5)
#define IPT_STAT_GET_WAN_STAT   (IPT_STAT_BASE_CTL + 1)

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x)           (sizeof(x)/sizeof(x[0]))
#endif

#define CONNECT_MACADDR_LEN     	(6)
#define CONNECT_TIMER_PERIOD		(1000)
#define dprintf(fmt, args...)   	D(INTERFACE, fmt, ##args)

struct ipt_net
{
    uint32_t ip;
    uint32_t mask;
};

struct ipt_stat
{
    uint32_t rcv_pkt;
    uint32_t rcv_byte;
    uint32_t snd_pkt;
    uint32_t snd_byte;
};

struct conn_handler
{
    enum interface_conn_mode mode;
    void (*process)(struct interface *iface);
};

static int connect_ipt_outgone();
static void connect_auto(struct interface *iface);
static void connect_manual(struct interface *iface);
static void connect_demand(struct interface *iface);
static void connect_timebased(struct interface *iface);
static void connect_dft_route(int add);

static int initialized;
static int tmp_ip_set;
static char tmp_ip_str[16];
static time_t last_in_sec;         /* lan -> wan had outgoing pkts last second    */
static time_t last_ex_sec = 0;         /* lan -> wan hadn't outgoing pkts last second */
static struct ipt_stat last_stat;
static struct uloop_timeout timeout;
static struct conn_handler conn_handlers[] =
{
    {
        .mode = IFCM_AUTO,
        .process = connect_auto,
    },
    {
        .mode = IFCM_MANUAL,
        .process = connect_manual,
    },
    {
        .mode = IFCM_DEMAND,
        .process = connect_demand,
    },
    {
        .mode = IFCM_TIMEBASED,
        .process = connect_timebased,
    },
};

static void
connect_ifup(struct interface *iface)
{
    struct interface *parent = NULL;

    if (!strcmp(iface->name, CONNECT_IFACE_INTERNET) ||
	!strcmp(iface->name, CONNECT_IFACE_INTERNETV6))
    {
        parent = vlist_find(&interfaces, CONNECT_IFACE_WAN, iface, node);

        if (parent && parent->state == IFS_DOWN)
        {
            /* dprintf("interface(%p): %s, available: %d, connectable: %d, state: %d, conn_mode: %d\n",
                    iface, iface->name, iface->available, 
                    iface->connectable, iface->state, iface->conn_mode); */
            (void)interface_set_up(parent);
        }

        if (iface->state == IFS_DOWN && iface->connectable)
        {
            dprintf("interface(%s) set up\n", iface->name);
            (void)interface_set_up(iface);
        }
    }
    else 
    {
        /* wan or wanv6 */
        if (iface->state == IFS_DOWN)
        {
            (void)interface_set_up(iface);
        }
    }
}

void
connect_ifdown(struct interface *iface)
{
#if 1
    struct interface *parent = NULL;
    bool pppoe = false;

    if (!strcmp(iface->name, CONNECT_IFACE_INTERNET) ||
	!strcmp(iface->name, CONNECT_IFACE_INTERNETV6))
    {
        parent = vlist_find(&interfaces, CONNECT_IFACE_WAN, iface, node);

        /* Sequential action: 
         *   1. Down L2TP/PPTP/PPPoE;
         *   2. Down DHCP;
         * Make sure both PPP Termination and DHCP Release frame was transmitted.
         */

	(void)interface_set_down(iface);

        if (iface->proto_handler && strcmp(iface->proto_handler->name, "pppoe") == 0)
        {
            pppoe = true;
        }

        if (iface->state == IFS_DOWN && parent && !pppoe)
        {
		(void)interface_set_down(parent);
        }
    }
    else 
    {
        /* wan or wanv6 */
        (void)interface_set_down(iface);
    }
#else
    (void)interface_set_down(iface);
#endif
}


static bool 
connect_state_is_down(struct interface *iface)
{
	struct interface *parent = NULL;
	
	if (iface->state == IFS_UP) 
	{
		return false;
	}

	if ((!strcmp(iface->name, CONNECT_IFACE_INTERNET) ||
		!strcmp(iface->name, CONNECT_IFACE_INTERNETV6)) &&
		!(iface->proto_handler && strcmp(iface->proto_handler->name, "pppoe") == 0))
	{
        	parent = vlist_find(&interfaces, CONNECT_IFACE_WAN, iface, node);

		if (parent && parent->state == IFS_UP)
		{
			return false;
		}
	}

	return true;
}

static void 
connect_auto(struct interface *iface)
{
#if	0
     bool dhcp = false;
    struct interface *internet = NULL;
	
    internet = vlist_find(&interfaces, CONNECT_IFACE_INTERNET, internet, node);
    if (!internet && strcmp(iface->name, CONNECT_IFACE_WAN) == 0 &&
        iface->proto_handler && strcmp(iface->proto_handler->name, "dhcp") == 0)
    {
        dhcp = true;
    }


    /* device unplugged */
    if (dhcp && !iface->link_state)
    {
        if (iface->state == IFS_UP)
        {
            (void)interface_set_down(iface);
        }
        
        return;
    }
#endif

    if (iface->state != IFS_UP) 
    {
        connect_ifup(iface);
    }
}

static void 
connect_manual(struct interface *iface)
{
	time_t now = 0;
	int ret = 0;

	dprintf("interface: %s, state: %d, idle: %ld\n", 
		iface->name, iface->state, iface->conn_time.idle);

	if (iface->conn_time.idle == 0)
	{
		return;
	}

	if (connect_state_is_down(iface)) 
	{
		last_ex_sec = 0;
		return;
	}

	ret = connect_ipt_outgone();
	dprintf("lan -> wan had outgoing pkts: %s\n", ret ? "yes" : "no");
	now = time(NULL);
	if (ret)
	{
        /* lan -> wan had outgoing pkts */
		last_ex_sec = now;
		return;
	}

	if (!last_ex_sec)
	{
		last_ex_sec = now;
	}

	if ((now - last_ex_sec) >= iface->conn_time.idle)
	{
		connect_ifdown(iface);
		dprintf("interface(%s) set down\n", iface->name);
	}

	return; 
}

static void 
connect_demand(struct interface *iface)
{
    time_t now;
    int ret = 0;

    if (!connect_state_is_down(iface))
    {
        connect_dft_route(0);

        if (iface->conn_time.idle == 0)
        {
            return;
        }

        ret = connect_ipt_outgone();
        now = time(NULL);
        if (ret)
        {
            last_in_sec = now;
            return;
        }

        if (last_in_sec == 0)
        {
            last_in_sec = now;
        }

        if ((now - last_in_sec) >= iface->conn_time.idle)
        {
            connect_ifdown(iface);
        }
    }
    else 
    {
        if (iface->state == IFS_DOWN)
        {
            connect_dft_route(1);
        }

        ret = connect_ipt_outgone();
        if (!ret)
        {
            return;
        }

        last_in_sec = time(NULL);
        connect_dft_route(0);
        connect_ifup(iface);
    }

    return;
}

static void 
connect_timebased(struct interface *iface)
{
    struct tm cur;
    time_t cur_sec;
    int valid = 0;
    int ret = 0;

    ret = system_get_localtime(&cur);
    if (ret != 0)
    {
        return;
    }

    cur_sec = cur.tm_hour * 60 * 60 + cur.tm_min * 60 + cur.tm_sec;
    if (cur_sec > iface->conn_time.start && cur_sec < iface->conn_time.end)
    {
        valid = 1;
    }

    if (!connect_state_is_down(iface) && !valid)
    {
        connect_ifdown(iface);
    }
    else if (connect_state_is_down(iface) && valid)
    {
        connect_ifup(iface);
    }
}

static int 
connect_process()
{
    struct interface *iface, *phy_if = NULL;
    struct interface *internet = NULL;
    int i = 0;
    bool link = true;

    internet = vlist_find(&interfaces, CONNECT_IFACE_INTERNET, internet, node);
    if (internet != NULL && internet->conn_mode != IFCM_DEMAND)
    {
        connect_dft_route(0);
    }

    vlist_for_each_element(&interfaces, iface, node) 
    {
        if (strcmp(iface->name, CONNECT_IFACE_WAN) != 0 &&
            strcmp(iface->name, CONNECT_IFACE_WANV6) != 0 &&
            strcmp(iface->name, CONNECT_IFACE_INTERNET) != 0)
        {
            continue;
        }

        /* dprintf("interface(%p): %s, available: %d, connectable: %d, state: %d, conn_mode: %d\n",
                iface, iface->name, iface->available, 
                iface->connectable, iface->state, iface->conn_mode); */

	/*
	** provide successful connection support for
 	** l2tp/pppoe/pptp + dhcp/staticip case.
	*/
#if	0
        if (!iface->available)
        {
            continue;
        }
#endif
	 if ((!strcmp(iface->name, CONNECT_IFACE_INTERNET) ||
		!strcmp(iface->name, CONNECT_IFACE_INTERNETV6)) &&
		!iface->main_dev.dev)
	 {
		phy_if = vlist_find(&interfaces, CONNECT_IFACE_WAN, iface, node);
	 }
	 if (!phy_if)
	 {
		phy_if = iface;
	 }

	if (phy_if->main_dev.dev)
	{
		link = system_get_link_state(phy_if->main_dev.dev);
		device_set_link(phy_if->main_dev.dev, link);
	}

        if ((!iface->connectable || !link) &&
		(iface->proto_handler && strcmp(iface->proto_handler->name, "static")))
        {
		if (iface->state == IFS_SETUP || iface->state == IFS_UP)
            		connect_ifdown(iface);

		last_in_sec = last_ex_sec = 0;
		continue;
        }

        for (i = 0; i < ARRAY_SIZE(conn_handlers); i++)
        {
            if (iface->conn_mode == conn_handlers[i].mode)
            {
                conn_handlers[i].process(iface);
                break;
            }

            /* Invalid connect mode, ignore */
        }
    }

    return 0;
}

static void 
connect_timeout(struct uloop_timeout *timeout)
{
    (void)connect_process();
    (void)uloop_timeout_set(timeout, CONNECT_TIMER_PERIOD);
}

#if 0
static int
connect_dev_hasip(const char *ifname)
{
    uint32_t ip = 0;
    int ret = 0;

    ret = system_if_get_addr(ifname, &ip);
    if (ret != 0 || ip == 0)
    {
        return 0;
    }

    return 1;
}
#endif

static void
connect_dft_route(int add)
{
    const char *dftgw = "1.0.0.1";
    const char *wan_ifname;
    const char *lan_ifname;
    const char *lan_ipaddr;
    uint8_t macaddr[CONNECT_MACADDR_LEN];
    int ret = 0;

    wan_ifname = config_option_str(CONNECT_IFACE_WAN, "ifname");
    if (!wan_ifname) 
    {
        dprintf("interface %s: invalid option ifname\n", CONNECT_IFACE_WAN);
        return;
    }

    if (add)
    {
#if 0
        if (tmp_ip_set || connect_dev_hasip(wan_ifname))
        {
            /* tmp ip addr for wan's device is set already, or it has ip */
            return;
        }
#endif

        lan_ifname = config_option_str(CONNECT_IFACE_LAN, "ifname");
        if (!lan_ifname)
        {
            dprintf("interface %s: invalid option ifname\n", CONNECT_IFACE_LAN);
            return;
        }

        lan_ipaddr = config_option_str(CONNECT_IFACE_LAN, "ipaddr");
        if (!lan_ipaddr)
        {
            dprintf("interface %s: invalid option ipaddr\n", CONNECT_IFNAME_LAN);
            return;
        }

        ret = system_if_get_macaddr(lan_ifname, macaddr);
        if (ret != 0)
        {
            dprintf("ifname %s: can't get macaddr\n", lan_ifname);
            return;
        }

        /* system_exec_fmt("ifconfig %s 0.0.0.0", wan_ifname); */
        memset(tmp_ip_str, 0, sizeof(tmp_ip_str));
        if ((macaddr[CONNECT_MACADDR_LEN - 1] != 0x01) &&
            (macaddr[CONNECT_MACADDR_LEN - 1] != 0xFF))
        {
            snprintf(tmp_ip_str, sizeof(tmp_ip_str), "1.0.%d.%d", 
                     macaddr[CONNECT_MACADDR_LEN - 2], 
                     macaddr[CONNECT_MACADDR_LEN - 1]);
        }
        else
        {
            snprintf(tmp_ip_str, sizeof(tmp_ip_str), "1.0.%d.10", 
                     macaddr[CONNECT_MACADDR_LEN - 2]);
        }

        /* system_exec_fmt("ifconfig %s %s", wan_ifname, tmp_ip_str); */
		system_exec_fmt("ip addr add %s/8 dev %s", tmp_ip_str, wan_ifname);
        system_exec_fmt("ip route replace default via %s dev %s", dftgw, wan_ifname);
        system_exec_fmt("ifconfig %s up", wan_ifname);
        system_exec_fmt("nat del dns && nat add dns { %s %s }", lan_ipaddr, dftgw);
        tmp_ip_set = 1;
    }
    else 
    {
        if (tmp_ip_set)
        {
            /* delete tmp ip and default route */
            system_exec_fmt("ip addr del %s/8 dev %s", tmp_ip_str, wan_ifname);
            /* dnsproxy will execute nat del dns */
            /* system_exec_cmd("nat del dns"); */
            tmp_ip_set = 0;
        }
    }
}

static int 
connect_ipt_stat(struct ipt_stat *stat)
{
    socklen_t len = sizeof(struct ipt_stat);
    int sock = -1;
    int ret = 0;

    sock = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    if (sock < 0)
    {
        dprintf("create socket failed\n");
        ret = -1;
        goto out;
    }

    ret = getsockopt(sock, IPPROTO_IP, IPT_STAT_GET_WAN_STAT, stat, &len);
    if (ret != 0)
    {
        dprintf("setsockopt failed to get stats\n");
        goto out;
    }
    else if (len != sizeof(struct ipt_stat))
    {
        dprintf("len %d != %d\n", len, sizeof(struct ipt_stat));
        ret = -1;
        goto out;
    }

    ret = 0;

out:
    if (sock > -1)
    {
        close(sock);
    }
    return ret;
}

static int
connect_ipt_outgone()
{
    struct ipt_stat ipts;
    int ret = 0;

    ret = connect_ipt_stat(&ipts);
    if (ret != 0)
    {
        return 0;
    }

    dprintf("old/new send pkts: %d, %d\n", last_stat.snd_pkt, ipts.snd_pkt);
    if (ipts.snd_pkt != 0 && ipts.snd_pkt > last_stat.snd_pkt)
    {
        ret = 1;
    }
    memcpy(&last_stat, &ipts, sizeof(struct ipt_stat));

    return ret;
}

static int
connect_cfg_ip4addr(const char *secname, const char *name, uint32_t *ip4addr)
{
    const char *val = NULL;
    struct in_addr addr;
    int ret = 0;

    val = config_option_str(secname, name);
    if (!val) 
    {
        dprintf("interface lan doesn't exist ipaddr\n");
        ret = -1;
        goto out;
    }

    ret = inet_aton(val, &addr);
    if (ret == 0)
    {
        ret = -1;
        dprintf("Invalid lan ipaddr\n");
        goto out;
    }

    *ip4addr = ntohl(addr.s_addr);
    ret = 0;

out:
    return ret;
}

static int 
connect_ipt_init()
{
    struct ipt_net ipnet;
    int sock = -1;
    int ret = 0;

#if 0
    ret = system_if_get_addr(CONNECT_IFNAME_LAN, &ipnet.ip);
    if (ret != 0)
    {
        dprintf("system_if_get_addr(lan) failed\n");
        goto out;
    }

    ret = system_if_get_mask(CONNECT_IFNAME_LAN, &ipnet.mask);
    if (ret != 0)
    {
        dprintf("system_if_get_mask(lan) failed\n");
        goto out;
    }

#else
    ret = connect_cfg_ip4addr(CONNECT_IFACE_LAN, "ipaddr", &ipnet.ip);
    if (ret != 0)
    {
        goto out;
    }

    ret = connect_cfg_ip4addr(CONNECT_IFACE_LAN, "netmask", &ipnet.mask);
    if (ret != 0)
    {
        goto out;
    }
#endif

    sock = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    if (sock < 0) 
    {
        dprintf("create socket failed\n");
        ret = -1;
        goto out;
    }

    ret = setsockopt(sock, IPPROTO_IP, IPT_STAT_DEL_ALL, NULL, 0);
    if (ret != 0)
    {
        dprintf("setsockopt stats del all failed");
        goto out;
    }

    ret = setsockopt(sock, IPPROTO_IP, IPT_STAT_SET_NET, &ipnet, sizeof(ipnet));
    if (ret != 0)
    {
        dprintf("setsockopt stats set net failed");
        goto out;
    }

    ret = 0;

out:
    if (sock > -1)
    {
        close(sock);
    }
    return ret;
}

void connect_dump_info(struct blob_buf *b)
{
    blobmsg_add_u32(b, "timeout remaining", uloop_timeout_remaining(&timeout));
}

int connect_init()
{
    int ret = 0;

    if (initialized)
    {
        return ret;
    }

    ret = connect_ipt_init();
    if (ret != 0)
    {
        dprintf("connect_ipt_init failed\n");
        return ret;
    }

    timeout.cb = connect_timeout;
    ret = uloop_timeout_set(&timeout, CONNECT_TIMER_PERIOD);

   detect_init_exit(true);

    initialized = 1;
    return ret;
}

void connect_exit()
{
    if (initialized)
    {
	detect_init_exit(false);
        uloop_timeout_cancel(&timeout);
        initialized = 0;
    }
}
