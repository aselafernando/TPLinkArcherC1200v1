/*! Copyright(c) 2008-2014 Shenzhen TP-LINK Technologies Co.Ltd.
 *
 *\file     detect_online.c
 *\brief	Only apply to ipv4 temporarily.
 *\details
 *
 *\author   Teng Fei<tengfei@tp-link.net>
 *\version  1.0.0
 *\date     19Nov14
 *
 *\warning
 *
 *\history \arg 19Nov14, Teng Fei, create the file.
 */
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>		/* fcntl() */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip_icmp.h>

#include "netifd.h"
#include "interface.h"
#include "interface-ip.h"
#include "proto.h"
#include "connect.h"
#include "config.h"

#include <arpa/inet.h>

/*
**	Detection states.
**	Do NOT modify values of following vars unless you understand it!
##	=======================================================
##	Actions [State1 -> State2]	Contitions...
##	---------------------------------------------------------------------
1.	a [* -> 6]	Non automatic connection OR non connected OR an error happened.
2.	b [0 -> 4]	Get info of connected status,  initialize some resources, try to begin 
				to prepare for the online detection.
3.	c [4 -> 1]	If the current attempt isn't the last attempt, will begin to start online
				detection.
4.	d [4 -> 5]	If you have already completed the last attempt, enter Offine state to
				terminate the "bad" connection.
5.	e [1 -> 2]	It has sent a request package of some protocol, waiting for response
				from the peer to detect the link state.
6.	f [1 -> 4]	The current attempt way cannot start, enter the next attempt.
7.	g [2 -> 2]	Timeout to receive a response from the peer, continue to do it again!
8.	h [2 -> 3]	Has received a GOOD response from the peer, enter Online state, 
				implies current connection is well.
9.	i [2 -> 4] 	Received a bad response from the peer, or received none after all 
				attempts, try to begin to prepare for the next time online detection.
10.	j [3 -> 0]	After sleeping, enter the next round of online detection.
11.	k [5 -> 0]	After sleeping, enter the next round of online detection.
12.	l [6 -> 0] 	Current connection state has changed to connected from disconnected
				begin to enter a new round of online detection.
##	=======================================================
*/
#define	D_INITIAL		0	/* initial state */
#define	D_STARTED		1	/* begin to do online-detect */
#define	D_DETECTING	2	/* waiting for result */
#define	D_ONLINE		3	/* connected */
#define	D_RETRY		4	/* retry to detect */
#define	D_OFFLINE		5	/* disconnected from ISP */
#define	D_OVER			6	/* idle, nothing to do */
#define	D_MAX			7	/* number of states */

/* ways for online-detect */
#define	DETECT_WAY_DNS	1
#define	DETECT_WAY_GW	2
#define	DETECT_WAY_CNT	4

#define	DETECT_DNS_MAX	2
#define	DETECT_DNS_PORT	0x35
#define	DETECT_ICMP_ECHO	0x8  /* Echo Request */

/* invalid socket fd implies to close current socket. */
#define	DETECT_SOCK_INVALID	-1


/*  Online Detect Finite State Machine. */
struct detect_fsm
{
	unsigned	state;
	unsigned	timeunit;
	unsigned	times;
	unsigned	times_lmt;

	void (*cb)(struct detect_fsm*, unsigned*);
};

/* args */
struct detect_data
{
	int	af;
	union if_addr addr;
};

struct detect_ways
{
	unsigned	type;
	struct detect_data args;
	bool	done;
	
	int (*cb)(struct detect_data*, bool);
};


static void detect_initial(struct detect_fsm*, unsigned*);
static void detect_started(struct detect_fsm*, unsigned*);
static void detect_detecting(struct detect_fsm*, unsigned*);
static void detect_online(struct detect_fsm*, unsigned*);
static void detect_retry(struct detect_fsm*, unsigned*);
static void detect_offline(struct detect_fsm*, unsigned*);
static void detect_over(struct detect_fsm*, unsigned*);

/*
**	Detection handlers.
**	Do NOT modify values of "state" member in "struct detect_fsm" unless you understand it!
*/
static struct detect_fsm detect_hdlrs[D_MAX] =
{
	{
		.state = D_INITIAL,
		.timeunit = 0x1,
		.times = 0x0,
		.times_lmt = 0x1,
		.cb = detect_initial,
	},
	{
		.state = D_STARTED,
		.timeunit = 0x0,
		.times = 0x1,
		.times_lmt = 0x1,
		.cb = detect_started,
	},
	{
		.state = D_DETECTING,
		.timeunit = 0x1,
		.times = 0x1,
		.times_lmt = 0x3,
		.cb = detect_detecting,
	},
	{
		.state = D_ONLINE,
		.timeunit = 0x3,
		.times = 0x1,
		.times_lmt = 0x8,
		.cb = detect_online,
	},
	{
		.state = D_RETRY,
		.timeunit = 0x0,
		.times = 0x0,
		.times_lmt = 0x2,
		.cb = detect_retry,
	},
	{
		.state = D_OFFLINE,
		.timeunit = 0x3,
		.times = 0x1,
		.times_lmt = 0x8,
		.cb = detect_offline,
	},
	{
		.state = D_OVER,
		.timeunit = 0x15,
		.times = 0x0,
		.times_lmt = 0x1,
		.cb = detect_over,
	},
};

static struct detect_ways detect_way[DETECT_WAY_CNT];


/* Timer to do online-detect. */
static struct uloop_timeout detect_timer;

static struct interface *detect_if = NULL;


/*
**	Get && Set index of Arrary detect_way.
*/
static int detect_gset_way_idx(const int newidx, bool set)
{
	static int detect_way_idx = 0;

	/* Set */
	if (set)
	{
		detect_way_idx = newidx;
	}

	return detect_way_idx;
}


/*
**	1. Get current socket fd if "set" false.
**	2. Record new socket fd and close current socket if newsock equals DETECT_SOCK_INVALID.
*/
static int detect_gset_sock(const int newsock, bool set)
{
	/* Socket fd for online-detect */
	static int detect_sockfd = DETECT_SOCK_INVALID;

	/* Just get current socket fd  OR nothing to do */
	if (!set || detect_sockfd == newsock)
	{
		return detect_sockfd;
	}

	/* Record new socket fd */
	if (detect_sockfd >= 0)
	{
		close(detect_sockfd);
	}

	detect_sockfd = newsock;
	return detect_sockfd;
}


static int 
detect_be_over(void)
{
	struct interface *iface = NULL; /*, *phy_if = NULL*/
	int ret = 0x2;

	vlist_for_each_element(&interfaces, iface, node) 
	{
		if (strcmp(iface->name, CONNECT_IFACE_WAN) &&
			strcmp(iface->name, CONNECT_IFACE_INTERNET))
		{
			continue;
		}
		if (iface->conn_mode != IFCM_AUTO)
		{
			continue;
		}

		if (!iface->proto_handler || !strcmp(iface->proto_handler->name, "static"))
		{
			continue;
		}
		
		if (iface->state == IFS_UP)
		{
			detect_if = iface;
			return 0;
		}
		return 1;
	}

	return ret;
}


static int in_cksum(unsigned short *buf, int sz)
{
	int nleft = sz;
	int sum = 0;
	unsigned short *w = buf;
	unsigned short ans = 0;

	while (nleft > 1) {
		sum += *w++;
		nleft -= 2;
	}

	if (nleft == 1) {
		*(unsigned char *) (&ans) = *(unsigned char *) w;
		sum += ans;
	}

	sum = (sum >> 16) + (sum & 0xFFFF);
	sum += (sum >> 16);
	ans = ~sum;
	return ans;
}


static int 
detect_dns_query(struct detect_data* data, bool send)
{
	int sockfd = -1;
	struct sockaddr_in cli_addr;
	struct sockaddr_in serv_addr;

	int nbytes;
	char buf[128];
	char rbuf[256];
	char *ptr;
	short one, n = 0;

	if (!send)
	{
		goto recv_step;
	}

	memset(&serv_addr, 0, sizeof(serv_addr));	
	serv_addr.sin_family = data->af;
	serv_addr.sin_addr = data->addr.in;
	serv_addr.sin_port = htons(DETECT_DNS_PORT); /* 53 */
	if ((sockfd = socket(data->af, SOCK_DGRAM, 0)) < 0)
	{
		return -1;
	}
	memset(&cli_addr, 0, sizeof(cli_addr));
	cli_addr.sin_family = data->af;
	cli_addr.sin_addr.s_addr = htonl(INADDR_ANY);	
	cli_addr.sin_port = htons(0);
	if (bind(sockfd, (struct sockaddr *)&cli_addr, sizeof(cli_addr)) <0)
	{
		close(sockfd);
		return -2;
	}	
	
	ptr = buf;
	*((u_short *)ptr) = htons(1234);	/* id */
	ptr += 2;
	*((u_short *)ptr) = htons(0x0100);	/* flag */
	ptr += 2;
	*((u_short *)ptr) = htons(1);		/* number of questions */
	ptr += 2;
	*((u_short *)ptr) = htons(0);		/* number of answers RR */
	ptr += 2;
	*((u_short *)ptr) = htons(0);		/* number of Authority RR */
	ptr += 2;
	*((u_short *)ptr) = htons(0);		/* number of Additional RR */
	ptr += 2;

	memcpy(ptr,"\001a\014root-servers\003net\000", 20);
	ptr += 20;
	one = htons(1);
	memcpy(ptr, &one, 2);		/* query type = A */
	ptr += 2;
	memcpy(ptr, &one, 2);		/* query class = 1 (IP addr) */
	ptr += 2;

	nbytes = 36;
	if (sendto(sockfd, buf, nbytes, 0, (struct sockaddr *)&serv_addr, 
		sizeof(struct sockaddr_in)) != nbytes)
	{
		/* perror("sendto"); */
		close(sockfd);
		return -3;
	}

	if (fcntl(sockfd, F_SETFL, O_NONBLOCK) < 0)
	{
		/* perror("fcntl"); */
		close(sockfd);
		return -4;
	}

	detect_gset_sock(sockfd, true);
	return 0;

recv_step:

	sockfd = detect_gset_sock(sockfd, false);

	if (sockfd < 0)
	{
		return -3;
	}

	n = recvfrom(sockfd, rbuf, sizeof(rbuf), 0, (struct sockaddr *)0, (socklen_t *)0);
	if (n == -1 && (errno == EAGAIN /*|| *perrno == EAGAIN*/))
	{
		return -1;
	}
	if (n >= 0)
	{
		/* good answer */
		rbuf[n] = 0;
		if (htons(*(short *)&rbuf[6]) >= 1) /* good we got an answer */
			return 0;
	}

	return -2;
}


static int 
detect_gw_query(struct detect_data* data, bool send)
{
	int sockfd = -1;
	struct sockaddr_in pingaddr;
	ssize_t ret = 0;
	//char cmd[0x100];
	char sndPacket[192];
	struct icmp *sndPkt = NULL;

	struct icmp *rcvPkt = NULL;
	char rbuf[192];
	short n = 0;
	static short detect_icmp_id = 0x0;

	if (!send)
	{
		goto recv_step;
	}

	sockfd = socket(AF_INET, SOCK_RAW, 0x1);	/* 0x1 means ICMP */
	if (sockfd < 0)
	{
		return -1;
	}

	memset(&pingaddr, 0x0, sizeof(struct sockaddr_in));
	pingaddr.sin_family = AF_INET;
	pingaddr.sin_addr = data->addr.in;

	if (fcntl(sockfd, F_SETFL, O_NONBLOCK) < 0)
        {
                /* perror("fcntl"); */
                close(sockfd);
                return -3;
        }
	
	detect_icmp_id &= 0x0fff;
	detect_icmp_id++;
	
	sndPkt = (struct icmp *)sndPacket;
	memset(sndPkt, 0x0, sizeof(sndPacket));
	sndPkt->icmp_type = DETECT_ICMP_ECHO;
	sndPkt->icmp_id = detect_icmp_id;
	sndPkt->icmp_cksum = in_cksum((unsigned short *)sndPkt, sizeof(sndPacket));
	ret = sendto(sockfd, sndPkt, sizeof(sndPacket)/*0x40*//*DEFDATALEN + ICMP_MINLEN*/, 0x0, 
		(struct sockaddr *)&pingaddr, sizeof(struct sockaddr_in));
	if (ret < 0)
	{
		close(sockfd);
		return -2;
	}

	//char buf[0x80];
	//inet_ntop(AF_INET, &data->addr, buf, 0x80);
	////tp_prtf("gw: %s sockfd%d", buf, sockfd);
	detect_gset_sock(sockfd, true);
	return 0;

recv_step:

	sockfd = detect_gset_sock(sockfd, false);

	if (sockfd < 0)
	{
		return -3;
	}

	n = recvfrom(sockfd, rbuf, sizeof(rbuf), 0, (struct sockaddr *)0, (socklen_t *)0);
	if (n == -1 && (errno == EAGAIN /*|| *perrno == EAGAIN*/))
	{
		return -1;
	}
	if (n >= 76)
	{
		struct iphdr *iphdr = (struct iphdr *)rbuf;			/* ip + icmp */
		rcvPkt = (struct icmp *)(rbuf + (iphdr->ihl << 2));	/* skip ip hdr */			
		if (rcvPkt->icmp_type == ICMP_ECHOREPLY && rcvPkt->icmp_id == detect_icmp_id)
	{
		return 0;
	}
		return -4;
	}
	
	return -2;
}


/*
##	=======================================================
##	Actions [State1 -> State2]	Contitions...
##	---------------------------------------------------------------------
1.	a [0 -> 6]	Non automatic connection OR non connected OR an error happened.
2.	b [0 -> 4]	Get info of connected status,  initialize some resources, try to begin 
				to prepare for the online detection.
10.	j [3 -> 0]	After sleeping, enter the next round of online detection.
11.	k [5 -> 0]	After sleeping, enter the next round of online detection.
12.	l [6 -> 0] 	Current connection state has changed to connected from disconnected
				begin to enter a new round of online detection.
##	=======================================================
*/
static void
detect_initial(struct detect_fsm* fsm, unsigned* phase)
{
	struct interface *iface = detect_if;
	struct interface_ip_settings *ip = NULL;
	struct dns_server *dns = NULL;
	struct device_route *route = NULL;
	struct detect_ways *way = NULL;
	int i = 0, j = 0;
	// char cmd[0x100];
	char buf[0x80];

	if (!iface || iface->state != IFS_UP)
	{
		*phase = D_OVER;
		return;
	}

	ip = &iface->proto_ip;
	if (ip->no_dns)
	{
		ip = &iface->config_ip;
		if (ip->no_dns)
		{
			*phase = D_OVER;
			return;
		}
	}

	i = 0;
	vlist_simple_for_each_element(&ip->dns_servers, dns, node) 
	{
		if (dns->af != AF_INET || dns->addr.in.s_addr == 0)
		{
			continue;
		}
		way = detect_way + i;
		way->type = DETECT_WAY_DNS;
		way->args.af = dns->af;
		way->args.addr = dns->addr;
		way->done = false;
		way->cb = detect_dns_query;
		
		inet_ntop(dns->af, &dns->addr, buf, 0x80);
		i++;
		//tp_prtf("dns server%d: %s", i, buf);
		if (i == DETECT_DNS_MAX)
		{
			break;
		}
	}

	for (ip = &iface->proto_ip, j = 0; j < 0x2; ip = &iface->config_ip, ++j)
	{
		vlist_for_each_element(&ip->route, route, node) 
		{	
			if (!route->enabled)	
				continue;
			
			if ((route->flags & DEVADDR_FAMILY) != DEVADDR_INET4)
				continue;

			if (route->nexthop.in.s_addr == 0)
				continue;

			if (i >= DETECT_WAY_CNT)
			{
				break;
			}

			way = detect_way + i;
			way->type = DETECT_WAY_GW;
			way->args.af = AF_INET;
			way->args.addr = route->nexthop;
			way->done = false;
			way->cb = detect_gw_query;
			inet_ntop(AF_INET, &route->nexthop, buf, 0x80);
			//tp_prtf("gw%d: %s", i, buf);
			i++;
			break;
		}

		if (i >= DETECT_WAY_CNT)
		{
			break;
		}
	}
	
	detect_hdlrs[D_RETRY].times = 0;
	detect_hdlrs[D_RETRY].times_lmt = i;

	fsm->times++;
	if (fsm->times > fsm->times_lmt)
	{
		fsm->times = fsm->times_lmt;
	}

	*phase = D_RETRY;
}


/*
##	=======================================================
##	Actions [State1 -> State2]	Contitions...
##	---------------------------------------------------------------------
1.	a [1 -> 6]	Non automatic connection OR non connected OR an error happened.
3.	c [4 -> 1]	If the current attempt isn't the last attempt, will begin to start online
				detection.
5.	e [1 -> 2]	It has sent a request package of some protocol, waiting for response
				from the peer to detect the link state.
6.	f [1 -> 4]	The current attempt way cannot start, enter the next attempt.
##	=======================================================
*/
static void
detect_started(struct detect_fsm* fsm, unsigned* phase)
{
	int i = detect_gset_way_idx(0, false);
	struct detect_ways *way = NULL;

	way = detect_way + i;
	if (!way || way->done || i >= DETECT_WAY_CNT)
	{
		*phase = D_RETRY;
		return;
	}

	way->done = true;

	if (!way->cb || way->cb(&way->args, true))
	{
		*phase = D_OVER;
		return;
	}
	
	detect_hdlrs[D_DETECTING].times = 0x1;
	*phase = D_DETECTING;
}


/*
##	=======================================================
##	Actions [State1 -> State2]	Contitions...
##	---------------------------------------------------------------------
1.	a [2 -> 6]	Non automatic connection OR non connected OR an error happened.
5.	e [1 -> 2]	It has sent a request package of some protocol, waiting for response
				from the peer to detect the link state.
7.	g [2 -> 2]	Timeout to receive a response from the peer, continue to do it again!
8.	h [2 -> 3]	Has received a GOOD response from the peer, enter Online state, 
				implies current connection is well.
9.	i [2 -> 4] 	Received a bad response from the peer, or received none after all 
				attempts, try to begin to prepare for the next time online detection.
##	=======================================================
*/
static void
detect_detecting(struct detect_fsm* fsm, unsigned* phase)
{
	int ret = 0;
	int i = detect_gset_way_idx(0, false);
	struct detect_ways *way = NULL;
	//char cmd[0x100];

	way = detect_way + i;
	if (!way || i >= DETECT_WAY_CNT)
	{
		*phase = D_RETRY;
		goto end;
	}

	if (!way->cb)
	{
		*phase = D_OVER;
		goto end;
	}

	ret = way->cb(&way->args, false);
	//tp_prtf("detecting: ret = %d", ret);
	
	fsm->times++;
	switch (ret)
	{
	case 0:
		/* msg received */
		*phase = D_ONLINE;
		break;
	case -1:
	case -2:
	case -4:
		if (fsm->times >= fsm->times_lmt || ret != -1)
		{
			/* timeout */
			fsm->times = 0x1;
			*phase = D_RETRY;
			break;
		}
		*phase = D_DETECTING;
		return;
	case -3:
		*phase = D_OVER;
		break;
	default:
		*phase = D_OVER;
	}

end:
	detect_gset_sock(DETECT_SOCK_INVALID, true);
}


/*
##	=======================================================
##	Actions [State1 -> State2]	Contitions...
##	---------------------------------------------------------------------
8.	h [2 -> 3]	Has received a GOOD response from the peer, enter Online state, 
				implies current connection is well.
10.	j [3 -> 0]	After sleeping, enter the next round of online detection.
##	=======================================================
*/
static void
detect_online(struct detect_fsm* fsm, unsigned* phase)
{
	fsm->times++;
	if (fsm->times > fsm->times_lmt)
	{
		fsm->times = fsm->times_lmt;
	}

	detect_hdlrs[D_OFFLINE].times = 0x1;
	*phase = D_INITIAL;
}


/*
##	=======================================================
##	Actions [State1 -> State2]	Contitions...
##	---------------------------------------------------------------------
2.	b [0 -> 4]	Get info of connected status,  initialize some resources, try to begin 
				to prepare for the online detection.
3.	c [4 -> 1]	If the current attempt isn't the last attempt, will begin to start online
				detection.
4.	d [4 -> 5]	If you have already completed the last attempt, enter Offine state to
				terminate the "bad" connection.
6.	f [1 -> 4]	The current attempt way cannot start, enter the next attempt.
9.	i [2 -> 4] 	Received a bad response from the peer, or received none after all 
				attempts, try to begin to prepare for the next time online detection.
##	=======================================================
*/
static void
detect_retry(struct detect_fsm* fsm, unsigned* phase)
{
	detect_gset_sock(DETECT_SOCK_INVALID, true);
	
	detect_gset_way_idx(fsm->times, true);
	
	fsm->times++;
	if (fsm->times > fsm->times_lmt)
	{
		fsm->times = 0;
		*phase = D_OFFLINE;
		return;
	}

	*phase = D_STARTED;
}


/*
##	=======================================================
##	Actions [State1 -> State2]	Contitions...
##	---------------------------------------------------------------------
4.	d [4 -> 5]	If you have already completed the last attempt, enter Offine state to
				terminate the "bad" connection.
11.	k [5 -> 0]	After sleeping, enter the next round of online detection.
##	=======================================================
*/
static void
detect_offline(struct detect_fsm* fsm, unsigned* phase)
{
	fsm->times++;
	if (fsm->times > fsm->times_lmt)
	{
		fsm->times = fsm->times_lmt;
	}

	detect_hdlrs[D_ONLINE].times = 0x1;
	*phase = D_INITIAL;
}


/*
##	=======================================================
##	Actions [State1 -> State2]	Contitions...
##	---------------------------------------------------------------------
1.	a [* -> 6]	Non automatic connection OR non connected OR an error happened.
12.	l [6 -> 0] 	Current connection state has changed to connected from disconnected
				begin to enter a new round of online detection.
##	=======================================================
*/
static void
detect_over(struct detect_fsm* fsm, unsigned* phase)
{
	detect_gset_sock(DETECT_SOCK_INVALID, true);

	fsm->times++;
	if (fsm->times > fsm->times_lmt)
	{
		fsm->times = fsm->times_lmt;
	}

	*phase = D_OVER;
}


static void 
detect_timeout(struct uloop_timeout *timeout)
{
	static unsigned detect_phase = D_INITIAL;
	struct detect_fsm* hdlr = NULL;
	int period = 0x1, res = 0;
	// char cmd[0x100];

	res = detect_be_over();
	if (res)
	{
		// tp_prtf("res %d", res);
		detect_phase = D_OVER;
		if (!access("/tmp/detect_exit", F_OK))
		{
			// tp_prtf("exit!");
			return;
		}
	}
	else if (detect_phase == D_OVER)
	{
		// tp_prtf(" back to inital!");
		detect_phase = D_INITIAL;
	}

	hdlr = &detect_hdlrs[detect_phase];
	do
	{
		// tp_prtf("phase: %d, times: %d, timelmt: %d", detect_phase, hdlr->times, hdlr->times_lmt);
		if (hdlr->cb)
		{
			/* "detect_phase" will be moved to another value in most cases. */
			(void)hdlr->cb(hdlr, &detect_phase);
		}
		
		hdlr = &detect_hdlrs[detect_phase];
		if (detect_phase == D_OFFLINE && !detect_be_over() && detect_if)
		{
			// tp_prtf("Disconnecting ....");
			/* Terminate the connection */
			connect_ifdown(detect_if);
		}
		
		if (hdlr->times && hdlr->timeunit)
		{
			period = hdlr->timeunit * (1 << (hdlr->times - 1));
			break;
		}
	}while (true);
	
	(void)uloop_timeout_set(timeout, period * 1000);
}


int 
detect_init_exit(bool init)
{
	static bool detect_inited = false;
	int ret = 0;

	/* exit case */
	if (!init)
	{
		if (detect_inited)
		{
			uloop_timeout_cancel(&detect_timer);
			detect_inited = false;
		}
		return ret;
	}

	/* init case */
	if (!detect_inited)
	{
		detect_timer.cb = detect_timeout;
		ret = uloop_timeout_set(&detect_timer, 6500);

		detect_inited = true;
	}
	return ret;
}

