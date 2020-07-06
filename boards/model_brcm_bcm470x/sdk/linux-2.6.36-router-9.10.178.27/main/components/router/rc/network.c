/*
 * Network services
 *
 * Copyright (C) 2015, Broadcom Corporation. All Rights Reserved.
 * 
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 *
 * <<Broadcom-WL-IPTag/Open:>>
 *
 * $Id: network.c 554607 2015-05-06 02:59:30Z $
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <syslog.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if_arp.h>
#include <signal.h>

typedef u_int64_t u64;
typedef u_int32_t u32;
typedef u_int16_t u16;
typedef u_int8_t u8;

#include <linux/sockios.h>
#include <linux/types.h>
#include <linux/ethtool.h>

#include <bcmnvram.h>
#include <netconf.h>
#include <shutils.h>
#include <wlutils.h>
#include <nvparse.h>
#include <rc.h>
#include <bcmutils.h>
#include <etioctl.h>
#include <bcmparams.h>
#include <security_ipc.h>
#include <wlif_utils.h>
#include <dpsta_linux.h>

bool emf_enabled = FALSE;

static int
add_routes(char *prefix, char *var, char *ifname)
{
	char word[80], *next;
	char *ipaddr, *netmask, *gateway, *metric;
	char tmp[100];

	foreach(word, nvram_safe_get(strcat_r(prefix, var, tmp)), next) {
		dprintf("add %s\n", word);

		netmask = word;
		ipaddr = strsep(&netmask, ":");
		if (!ipaddr || !netmask)
			continue;
		gateway = netmask;
		netmask = strsep(&gateway, ":");
		if (!netmask || !gateway)
			continue;
		metric = gateway;
		gateway = strsep(&metric, ":");
		if (!gateway || !metric)
			continue;

		dprintf("add %s\n", ifname);

		route_add(ifname, atoi(metric) + 1, ipaddr, gateway, netmask);
	}

	return 0;
}

static int
del_routes(char *prefix, char *var, char *ifname)
{
	char word[80], *next;
	char *ipaddr, *netmask, *gateway, *metric;
	char tmp[100];

	foreach(word, nvram_safe_get(strcat_r(prefix, var, tmp)), next) {
		dprintf("add %s\n", word);

		netmask = word;
		ipaddr = strsep(&netmask, ":");
		if (!ipaddr || !netmask)
			continue;
		gateway = netmask;
		netmask = strsep(&gateway, ":");
		if (!netmask || !gateway)
			continue;
		metric = gateway;
		gateway = strsep(&metric, ":");
		if (!gateway || !metric)
			continue;

		dprintf("add %s\n", ifname);

		route_del(ifname, atoi(metric) + 1, ipaddr, gateway, netmask);
	}

	return 0;
}

static int
add_lan_routes(char *lan_ifname)
{
	return add_routes("lan_", "route", lan_ifname);
}

static int
del_lan_routes(char *lan_ifname)
{
	return del_routes("lan_", "route", lan_ifname);
}

/* Set initial QoS mode for all et interfaces that are up. */

static void
set_et_qos_mode(void)
{
	int i, s, qos;
	struct ifreq ifr;
	struct ethtool_drvinfo info;

	if ((s = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) < 0)
		return;

	qos = (strcmp(nvram_safe_get("wl_wme"), "off") != 0);

	for (i = 1; i <= DEV_NUMIFS; i ++) {
		ifr.ifr_ifindex = i;
		if (ioctl(s, SIOCGIFNAME, &ifr))
			continue;
		if (ioctl(s, SIOCGIFHWADDR, &ifr))
			continue;
		if (ifr.ifr_hwaddr.sa_family != ARPHRD_ETHER)
			continue;
		if (ioctl(s, SIOCGIFFLAGS, &ifr))
			continue;
		if (!(ifr.ifr_flags & IFF_UP))
			continue;
		/* Set QoS for et & bcm57xx devices */
		memset(&info, 0, sizeof(info));
		info.cmd = ETHTOOL_GDRVINFO;
		ifr.ifr_data = (caddr_t)&info;
		if (ioctl(s, SIOCETHTOOL, &ifr) < 0)
			continue;
		if ((strncmp(info.driver, "et", 2) != 0) &&
		    (strncmp(info.driver, "bcm57", 5) != 0))
			continue;
		ifr.ifr_data = (caddr_t)&qos;
		(void) ioctl(s, SIOCSETCQOS, &ifr);
	}

	close(s);
}

/*
 * Carry out a socket request including openning and closing the socket
 * Return -1 if failed to open socket (and perror); otherwise return
 * result of ioctl
 */
static int
soc_req(const char *name, int action, struct ifreq *ifr)
{
	int s;
	int rv = 0;

	if ((s = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) < 0) {
		perror("socket");
		return -1;
	}
	if (strlen(name) >= sizeof(ifr->ifr_name)) {
		fprintf(stderr, "soc_req: ifr name is too long\n");
		close(s);
		return -1;
	}
	strncpy(ifr->ifr_name, name, sizeof(ifr->ifr_name));
	rv = ioctl(s, action, ifr);
	close(s);

	return rv;
}

static int
wl_send_dif_event(const char *ifname, uint32 event)
{
	static int s = -1;
	int len, n;
	struct sockaddr_in to;
	char data[IFNAMSIZ + sizeof(uint32)];

	/* create a socket to receive dynamic i/f events */
	if (s < 0) {
		s = socket(AF_INET, SOCK_DGRAM, 0);
		if (s < 0) {
			perror("socket");
			return -1;
		}
	}

	/* Init the message contents to send to eapd. Specify the interface
	 * and the event that occured on the interface.
	 */
	strncpy(data, ifname, IFNAMSIZ);
	*(uint32 *)(data + IFNAMSIZ) = event;
	len = IFNAMSIZ + sizeof(uint32);

	/* send to eapd */
	to.sin_addr.s_addr = inet_addr(EAPD_WKSP_UDP_ADDR);
	to.sin_family = AF_INET;
	to.sin_port = htons(EAPD_WKSP_DIF_UDP_PORT);

	n = sendto(s, data, len, 0, (struct sockaddr *)&to,
	           sizeof(struct sockaddr_in));

	if (n != len) {
		perror("udp send failed\n");
		return -1;
	}

	dprintf("hotplug_net(): sent event %d\n", event);

	return n;
}
/* Check NVRam to see if "name" is explicitly enabled */
static inline int
wl_vif_enabled(const char *name, char *tmp)
{
	return (atoi(nvram_safe_get(strcat_r(name, "_bss_enabled", tmp))));
}

/* Set the HW address for interface "name" if present in NVRam */
static void
wl_vif_hwaddr_set(const char *name)
{
	int rc;
	char *ea;
	char hwaddr[20];
	struct ifreq ifr;
	int retry = 0;
	unsigned char comp_mac_address[ETHER_ADDR_LEN];
	snprintf(hwaddr, sizeof(hwaddr), "%s_hwaddr", name);
	ea = nvram_get(hwaddr);
	if (ea == NULL) {
		fprintf(stderr, "NET: No hw addr found for %s\n", name);
		return;
	}

	fprintf(stderr, "NET: Setting %s hw addr to %s\n", name, ea);
	ifr.ifr_hwaddr.sa_family = ARPHRD_ETHER;
	ether_atoe(ea, (unsigned char *)ifr.ifr_hwaddr.sa_data);
	ether_atoe(ea, comp_mac_address);
	if ((rc = soc_req(name, SIOCSIFHWADDR, &ifr)) < 0) {
		fprintf(stderr, "NET: Error setting hw for %s; returned %d\n", name, rc);
	}
	memset(&ifr, 0, sizeof(ifr));
	while (retry < 100) { /* maximum 100 millisecond waiting */
		usleep(1000); /* 1 ms sleep */
		if ((rc = soc_req(name, SIOCGIFHWADDR, &ifr)) < 0) {
			fprintf(stderr, "NET: Error Getting hw for %s; returned %d\n", name, rc);
		}
		if (memcmp(comp_mac_address, (unsigned char *)ifr.ifr_hwaddr.sa_data,
			ETHER_ADDR_LEN) == 0) {
			break;
		}
		retry++;
	}
	if (retry >= 100) {
		fprintf(stderr, "Unable to check if mac was set properly for  %s\n", name);
	}
}

#ifdef __CONFIG_EMF__
void
emf_mfdb_update(char *lan_ifname, char *lan_port_ifname, bool add)
{
	char word[256], *next;
	char *mgrp, *ifname;

	/* Add/Delete MFDB entries corresponding to new interface */
	foreach(word, nvram_safe_get("emf_entry"), next) {
		ifname = word;
		mgrp = strsep(&ifname, ":");

		if ((mgrp == 0) || (ifname == 0))
			continue;

		/* Add/Delete MFDB entry using the group addr and interface */
		if (strcmp(lan_port_ifname, ifname) == 0) {
			eval("emf", ((add) ? "add" : "del"),
			     "mfdb", lan_ifname, mgrp, ifname);
		}
	}

	return;
}

void
emf_uffp_update(char *lan_ifname, char *lan_port_ifname, bool add)
{
	char word[256], *next;
	char *ifname;

	/* Add/Delete UFFP entries corresponding to new interface */
	foreach(word, nvram_safe_get("emf_uffp_entry"), next) {
		ifname = word;

		/* Add/Delete UFFP entry for the interface */
		if (strcmp(lan_port_ifname, ifname) == 0) {
			eval("emf", ((add) ? "add" : "del"),
			     "uffp", lan_ifname, ifname);
		}
	}

	return;
}

void
emf_rtport_update(char *lan_ifname, char *lan_port_ifname, bool add)
{
	char word[256], *next;
	char *ifname;

	/* Add/Delete RTPORT entries corresponding to new interface */
	foreach(word, nvram_safe_get("emf_rtport_entry"), next) {
		ifname = word;

		/* Add/Delete RTPORT entry for the interface */
		if (strcmp(lan_port_ifname, ifname) == 0) {
			eval("emf", ((add) ? "add" : "del"),
			     "rtport", lan_ifname, ifname);
		}
	}

	return;
}

void
start_emf(char *lan_ifname)
{
	char word[256], *next;
	char *mgrp, *ifname;

	if (!nvram_match("emf_enable", "1"))
		return;

	/* Start EMF */
	eval("emf", "start", lan_ifname);

	/* Add the static MFDB entries */
	foreach(word, nvram_safe_get("emf_entry"), next) {
		ifname = word;
		mgrp = strsep(&ifname, ":");

		if ((mgrp == 0) || (ifname == 0))
			continue;

		/* Add MFDB entry using the group addr and interface */
		eval("emf", "add", "mfdb", lan_ifname, mgrp, ifname);
	}

	/* Add the UFFP entries */
	foreach(word, nvram_safe_get("emf_uffp_entry"), next) {
		ifname = word;

		/* Add UFFP entry for the interface */
		eval("emf", "add", "uffp", lan_ifname, ifname);
	}

	/* Add the RTPORT entries */
	foreach(word, nvram_safe_get("emf_rtport_entry"), next) {
		ifname = word;

		/* Add RTPORT entry for the interface */
		eval("emf", "add", "rtport", lan_ifname, ifname);
	}

	return;
}

void
load_emf(void)
{
	/* Load the EMF & IGMP Snooper modules */
	eval("insmod", "emf");
	eval("insmod", "igs");

	emf_enabled = TRUE;

	return;
}

void
unload_emf(void)
{
	if (!emf_enabled)
		return;

	/* Unload the EMF & IGMP Snooper modules */
	eval("rmmod", "igs");
	eval("rmmod", "emf");

	emf_enabled = FALSE;

	return;
}
#endif /* __CONFIG_EMF__ */

static int
dpsta_ioctl(char *name, void *buf, int len)
{
	struct ifreq ifr;
	int ret = 0;
	int s;

	/* open socket to kernel */
	if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("socket");
		return errno;
	}

	strncpy(ifr.ifr_name, name, IFNAMSIZ);
	ifr.ifr_name[sizeof(ifr.ifr_name) - 1] = '\0';
	ifr.ifr_data = (caddr_t)buf;
	if ((ret = ioctl(s, SIOCDEVPRIVATE, &ifr)) < 0)
		perror(ifr.ifr_name);

	/* cleanup */
	close(s);
	return ret;
}

static bool
l2bridge_no_ipaddr(const char *br_ifname)
{
	char l2bridge[NVRAM_MAX_PARAM_LEN];

	snprintf(l2bridge, sizeof(l2bridge), "%s_l2bridge_mode", br_ifname);

	/* For now, brX_l2bridge_mode only has 1 mode of On/Off for bridge IP address
	 * but it could be expanded to have other modes/flags in the future if needed
	 */
	return (nvram_match(l2bridge, "1") ? TRUE : FALSE);
}

static void
setup_lan(char *name, char *lan_ifname, char *hwaddr, int *dpsta, int ifidx)
{
	int s = 0;
	char tmp[100];
	char buf[255], *ptr;
	struct ifreq ifr;

	/* Bring up interface.Ignore any bogus/unknown
	 * Interfaces on the NVRAM list
	 */

	if (ifconfig(name, IFUP | IFF_ALLMULTI, NULL, NULL)) {
		perror("ifconfig");
		return;
	}
	else {
		/* Set the logical bridge address to that of the first interface */
		if ((s = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) < 0) {
			perror("socket");
			return;
		}
		strncpy(ifr.ifr_name, name, sizeof(ifr.ifr_name)-1);
		ifr.ifr_name[sizeof(ifr.ifr_name)-1] = '\0';
		if (ioctl(s, SIOCGIFHWADDR, &ifr) == 0) {
			if (memcmp(hwaddr, "\0\0\0\0\0\0", ETHER_ADDR_LEN) == 0) {
				memcpy(hwaddr, ifr.ifr_hwaddr.sa_data, ETHER_ADDR_LEN);
			}
		}
		close(s);

		/* if not a wl i/f then simply add it to the bridge */
		if (eval("wlconf", name, "up")) {
			if (eval("brctl", "addif", lan_ifname, name)) {
				perror("brctl");
				return;
			}
			else {
				snprintf(tmp, sizeof(tmp), "br%x_ifnames", ifidx);
				ptr = nvram_get(tmp);
				if (ptr)
					snprintf(buf, sizeof(buf), "%s %s", ptr, name);
				else
					snprintf(buf, sizeof(buf), "%s", name);
				nvram_set(tmp, buf);
			}
#ifdef __CONFIG_EMF__
			if (nvram_match("emf_enable", "1"))
				eval("emf", "add", "iface", lan_ifname, name);
#endif /* __CONFIG_EMF__ */
		}
		else {
			char mode[] = "wlXXXXXXXXXX_mode";
			int unit = -1;

			/* get the instance number of the wl i/f */
			wl_ioctl(name, WLC_GET_INSTANCE, &unit, sizeof(unit));

			snprintf(mode, sizeof(mode), "wl%d_mode", unit);

			/* WET specific configurations */
			if (nvram_match(mode, "wet")) {
				/* Receive all multicast frames in WET mode */
				ifconfig(name, IFUP | IFF_ALLMULTI, NULL, NULL);

				/* Enable host DHCP relay */
				if (nvram_match("lan_dhcp", "1"))
					wl_iovar_set(name, "wet_host_mac", ifr.ifr_hwaddr.sa_data,
					ETHER_ADDR_LEN);
			}
			/* Dont attach the main wl i/f in wds */
			if ((strncmp(name, "wl", 2) != 0) && (nvram_match(mode, "wds"))) {
				/* Save this interface name in unbridged_ifnames
				* This behaviour is consistent with BEARS release
				*/
				ptr = nvram_get("unbridged_ifnames");
				if (ptr)
					snprintf(buf, sizeof(buf), "%s %s", ptr, name);
				else
					snprintf(buf, sizeof(buf), "%s", name);
				nvram_set("unbridged_ifnames", buf);
				return;
			}

			/* Dont add main wl i/f when proxy sta is
			* enabled in both bands. Instead add the
			* dpsta interface.
			*/
			if (strstr(nvram_safe_get("dpsta_ifnames"), name)) {
				strcpy(name, !(*dpsta) ? "dpsta" : "");
				(*dpsta)++;

				/* Assign hw address */
				if ((s = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) >= 0) {
					strncpy(ifr.ifr_name, "dpsta", IFNAMSIZ);
					if (ioctl(s, SIOCGIFHWADDR, &ifr) == 0 &&
						memcmp(ifr.ifr_hwaddr.sa_data, "\0\0\0\0\0\0",
						ETHER_ADDR_LEN) == 0) {
							ifr.ifr_hwaddr.sa_family = ARPHRD_ETHER;
							memcpy(ifr.ifr_hwaddr.sa_data, hwaddr,
								ETHER_ADDR_LEN);
							if (ioctl(s, SIOCSIFHWADDR, &ifr)) {
								close(s);
								return;
							}
					}
					close(s);
				}
			}

			/* In 3GMAC mode, skip wl interfaces that avail of hw switching.
			 *
			 * Do not add these wl interfaces to the LAN bridge as they avail of
			 * HW switching. Misses in the HW switch's ARL will be forwarded via vlan1
			 * to br0 (i.e. via the network GMAC#2).
			 */
			if (nvram_match("gmac3_enable", "1") &&
			    find_in_list(nvram_get("fwd_wlandevs"), name))
				goto gmac3_no_swbr;

			eval("brctl", "addif", lan_ifname, name);
gmac3_no_swbr:

#ifdef __CONFIG_EMF__
			if (nvram_match("emf_enable", "1"))
				eval("emf", "add", "iface", lan_ifname, name);
#endif /* __CONFIG_EMF__ */

			snprintf(tmp, sizeof(tmp), "br%x_ifnames", ifidx);
			ptr = nvram_get(tmp);
			if (ptr)
				snprintf(buf, sizeof(buf), "%s %s", ptr, name);
			else {
				strncpy(buf, name, sizeof(buf));
				buf[sizeof(buf)-1] = '\0';
			}

			nvram_set(tmp, buf);
		}
	}
}
void
start_lan(void)
{
	char *lan_ifname = nvram_safe_get("lan_ifname");
	char br_ifname[80];
	char name[80], *next;
	char tmp[100];
	int i, s, dpsta = 0;
	struct ifreq ifr;
	char lan_stp[10];
	char *lan_ifnames;
	char lan_dhcp[10];
	char lan_ipaddr[15];
	char lan_netmask[15];
	char lan_hwaddr[15];
	char hwaddr[ETHER_ADDR_LEN];
	dpsta_enable_info_t info = { 0 };

	/* The NVRAM variable lan_ifnames contains all the available interfaces.
	 * This is used to build the unbridged interface list. Once the unbridged list
	 * is built lan_interfaces is rebuilt with only the interfaces in the bridge
	 *
	 * In 3GMAC mode, primary wl interfaces that avail of HW switching are
	 * excluded from lan_ifnames, and appear as LAN-like ports on the integrated
	 * switch, appearing to the Linux bridge as vlan1.
	 */

	dprintf("%s\n", lan_ifname);

	/* Create links */
	symlink("/sbin/rc", "/tmp/ldhclnt");


	nvram_unset("br0_ifname");
	nvram_unset("br1_ifname");
	nvram_unset("unbridged_ifnames");
	nvram_unset("br0_ifnames");
	nvram_unset("br1_ifnames");

#if defined(__CONFIG_EXTACS__) || defined(__CONFIG_WL_ACI__)
	nvram_unset("acs_ifnames");
#endif /* defined(_CONFIG_EXTACS__) || defined(__CONFIG_WL_ACI__ */
	/* If we're a travel router... then we need to make sure we get
	 * the primary wireless interface up before trying to attach slave
	 * interface(s) to the bridge
	 */
	if (nvram_match("ure_disable", "0") && nvram_match("router_disable", "0"))
	{
		eval("wlconf", nvram_get("wan0_ifname"), "up");
	}

	/* In 3GMAC mode, bring up the GMAC forwarding interfaces.
	 * Then bringup the primary wl interfaces that avail of hw switching.
	 */
	if (nvram_match("gmac3_enable", "1")) { /* __CONFIG_GMAC3__ */

		/* Bring up GMAC#0 and GMAC#1 forwarder(s) */
		foreach(name, nvram_safe_get("fwddevs"), next) {
			ifconfig(name, 0, NULL, NULL);
			ifconfig(name, IFUP | IFF_ALLMULTI | IFF_PROMISC, NULL, NULL);
		}
	}

	if (nvram_match("ses_bridge_disable", "1")) {
		/* dont bring up the bridge; only the individual interfaces */
		foreach(name, nvram_safe_get("lan_ifnames"), next) {
			/* Bring up interface */
			ifconfig(name, IFUP, NULL, NULL);
			/* if wl */
			eval("wlconf", name, "up");
		}
	} else {

	/* Bring up bridged interfaces */
	for (i = 0; i < MAX_NO_BRIDGE; i++) {
		if (!i) {
			lan_ifname = nvram_safe_get("lan_ifname");
			snprintf(lan_stp, sizeof(lan_stp), "lan_stp");
			snprintf(lan_dhcp, sizeof(lan_dhcp), "lan_dhcp");
			snprintf(lan_ipaddr, sizeof(lan_ipaddr), "lan_ipaddr");
			snprintf(lan_hwaddr, sizeof(lan_hwaddr), "lan_hwaddr");
			snprintf(lan_netmask, sizeof(lan_netmask), "lan_netmask");
			lan_ifnames = nvram_safe_get("lan_ifnames");
		}
		else {
			snprintf(tmp, sizeof(tmp), "lan%x_ifname", i);
			lan_ifname = nvram_safe_get(tmp);
			snprintf(lan_stp, sizeof(lan_stp), "lan%x_stp", i);
			snprintf(lan_dhcp, sizeof(lan_dhcp), "lan%x_dhcp", i);
			snprintf(lan_ipaddr, sizeof(lan_ipaddr), "lan%x_ipaddr", i);
			snprintf(lan_hwaddr, sizeof(lan_hwaddr), "lan%x_hwaddr", i);
			snprintf(lan_netmask, sizeof(lan_netmask), "lan%x_netmask", i);
			snprintf(tmp, sizeof(tmp), "lan%x_ifnames", i);
			lan_ifnames = nvram_safe_get(tmp);
		}
		if (strncmp(lan_ifname, "br", 2) == 0) {
			/* Set the bridge ifname in brX_ifname */
			snprintf(br_ifname, sizeof(br_ifname), "br%d_ifname", i);
			nvram_set(br_ifname, lan_ifname);

			eval("brctl", "addbr", lan_ifname);
			eval("brctl", "setfd", lan_ifname, "0");
			if (nvram_match(lan_stp, "0"))
				eval("brctl", "stp", lan_ifname, "off");
			else
				eval("brctl", "stp", lan_ifname, "on");
#ifdef __CONFIG_EMF__
			if (nvram_match("emf_enable", "1")) {
				eval("emf", "add", "bridge", lan_ifname);
				eval("igs", "add", "bridge", lan_ifname);
			}
#endif /* __CONFIG_EMF__ */
			memset(hwaddr, 0, sizeof(hwaddr));

			foreach(name, lan_ifnames, next) {
				if (strncmp(name, "wl", 2) == 0) {
					if (!(strcmp(nvram_safe_get("wl_mode"), "psr"))) {
						wl_vif_hwaddr_set(name);
						setup_lan(name, lan_ifname, hwaddr, &dpsta, i);
						continue;
					} else if (!wl_vif_enabled(name, tmp)) {
						continue; /* Ignore dsiabled WL VIF */
					}
					wl_vif_hwaddr_set(name);
				}

				setup_lan(name, lan_ifname, hwaddr, &dpsta, i);
			} /* foreach().... */

			if (memcmp(hwaddr, "\0\0\0\0\0\0", ETHER_ADDR_LEN) &&
			    (s = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) >= 0) {
				strncpy(ifr.ifr_name, lan_ifname, IFNAMSIZ);
				ifr.ifr_name[IFNAMSIZ-1] = '\0';
				ifr.ifr_hwaddr.sa_family = ARPHRD_ETHER;
				memcpy(ifr.ifr_hwaddr.sa_data, hwaddr, ETHER_ADDR_LEN);
				ioctl(s, SIOCSIFHWADDR, &ifr);
				close(s);
			}
		} /* if (strncmp(lan_ifname.... */
		/* specific non-bridged lan i/f */
		else if (strcmp(lan_ifname, "")) {
			/* Bring up interface */
			ifconfig(lan_ifname, IFUP, NULL, NULL);
			/* config wireless i/f */
			eval("wlconf", lan_ifname, "up");
		}
		else
			continue ; /* lanX_ifname is empty string , so donot do anything */

		/* Get current LAN hardware address */
		snprintf(tmp, sizeof(tmp), "br%x_ifnames", i);
		if ((strncmp(lan_ifname, "br", 2) != 0) || nvram_get(tmp)) {
			if ((s = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) >= 0) {
				char eabuf[32];
				strncpy(ifr.ifr_name, lan_ifname, IFNAMSIZ);
				ifr.ifr_name[IFNAMSIZ-1] = '\0';
				if (ioctl(s, SIOCGIFHWADDR, &ifr) == 0)
					nvram_set(lan_hwaddr,
					ether_etoa((unsigned char *)ifr.ifr_hwaddr.sa_data, eabuf));
				close(s);
			}
		} else
			nvram_unset(lan_hwaddr);

		if (l2bridge_no_ipaddr(lan_ifname)) {
			ifconfig(lan_ifname, IFUP, NULL, NULL);
		/* Launch DHCP client - AP only */
		}
		else if (nvram_match("router_disable", "1") && nvram_match(lan_dhcp, "1")) {
			char *dhcp_argv[] = {
				"udhcpc",
				"-i", lan_ifname,
				"-p", (sprintf(tmp, "/var/run/udhcpc-%s.pid", lan_ifname), tmp),
				"-s", "/tmp/ldhclnt",
				NULL
			};
			int pid;

			/* Start dhcp daemon */
			_eval(dhcp_argv, ">/dev/console", 0, &pid);
		}
		/* Handle static IP address - AP or Router */
		else {
			/* Bring up and configure LAN interface */
			ifconfig(lan_ifname, IFUP,
				nvram_safe_get(lan_ipaddr), nvram_safe_get(lan_netmask));
			/* We are done configuration */
			lan_up(lan_ifname);
		}

#ifdef __CONFIG_EMF__
		/* Start the EMF for this LAN */
		start_emf(lan_ifname);
#endif /* __CONFIG_EMF__ */
	} /* For loop */

	/* Configure dpsta module */
	if (dpsta) {
		int32 i = 0;

		/* Enable and set the policy to in-band and cross-band
		 * forwarding policy.
		 */
		info.enable = 1;
		info.policy = atoi(nvram_safe_get("dpsta_policy"));
		info.lan_uif = atoi(nvram_safe_get("dpsta_lan_uif"));
		foreach(name, nvram_safe_get("dpsta_ifnames"), next) {
			strcpy((char *)info.upstream_if[i], name);
			i++;
		}
		dpsta_ioctl("dpsta", &info, sizeof(dpsta_enable_info_t));

		/* Bring up dpsta interface */
		ifconfig("dpsta", IFUP, NULL, NULL);
	}

	} /* if (nvram_match("ses_bridge_disable", "1")) */

	/* Set initial QoS mode for LAN ports. */
	set_et_qos_mode();

	/* start syslogd if either log_ipaddr or log_ram_enable is set */
	if (nvram_invmatch("log_ipaddr", "") || nvram_match("log_ram_enable", "1")) {
#if !defined(__CONFIG_BUSYBOX__) || defined(BB_SYSLOGD)
		char *argv[] = {
			"syslogd",
			NULL,		/* -C */
			NULL, NULL,	/* -R host */
			NULL
		};
		int pid;
		int argc = 1;

		if (nvram_match("log_ram_enable", "1"))
			argv[argc++] = "-C";

		if (nvram_invmatch("log_ipaddr", "")) {
			argv[argc++] = "-R";
			argv[argc++] = nvram_get("log_ipaddr");
		}


		_eval(argv, NULL, 0, &pid);
#else /* Busybox configured w/o syslogd */
		cprintf("Busybox configured w/o syslogd\n");
#endif /* !defined(__CONFIG_BUSYBOX__) || defined(BB_SYSLOGD) */
	}

	dprintf("%s %s\n",
		nvram_safe_get("lan_ipaddr"),
		nvram_safe_get("lan_netmask"));

}

void
stop_lan(void)
{
	char *lan_ifname = nvram_safe_get("lan_ifname");
	char name[80], *next, signal[] = "XXXXXXXX";
	char br_prefix[20];
	char tmp[20];
	int i = 0;
	char* lan_ifnames;

	dprintf("%s\n", lan_ifname);

	/* Stop the syslogd daemon */
	eval("killall", "syslogd");
	/* release the DHCP address and kill the client */
	snprintf(signal, sizeof(signal), "-%d", SIGUSR2);
	eval("killall", signal, "udhcpc");
	eval("killall", "udhcpc");

	/* Remove static routes */
	del_lan_routes(lan_ifname);

	/* Bring down unbridged interfaces,if any */
	foreach(name, nvram_safe_get("unbridged_ifnames"), next) {
		eval("wlconf", name, "down");
		ifconfig(name, 0, NULL, NULL);
	}

	if (nvram_match("gmac3_enable", "1")) { /* __CONFIG_GMAC3__ */

		/* Bring down GMAC#0 and GMAC#1 forwarder(s) */
		foreach(name, nvram_safe_get("fwddevs"), next) {
			ifconfig(name, 0, NULL, NULL);
		}
	}

	for (i = 0; i < MAX_NO_BRIDGE; i++) {
		if (!i) {
			lan_ifname = nvram_safe_get("br0_ifname");
			snprintf(br_prefix, sizeof(br_prefix), "br0_ifnames");
		}
		else {
			snprintf(tmp, sizeof(tmp), "br%x_ifname", i);
			lan_ifname = nvram_safe_get(tmp);
			snprintf(br_prefix, sizeof(br_prefix), "br%x_ifnames", i);
		}
		if (!strcmp(lan_ifname, ""))
			continue;


		/* Bring down LAN interface */
		ifconfig(lan_ifname, 0, NULL, NULL);

		/* Bring down bridged interfaces */
		if (strncmp(lan_ifname, "br", 2) == 0) {
			lan_ifnames = nvram_safe_get(br_prefix);
			foreach(name, lan_ifnames, next) {
				if (!strcmp(name, "dpsta")) {
					char dp_uif[80], *dpnext;
					foreach(dp_uif, nvram_safe_get("dpsta_ifnames"),
					        dpnext) {
						eval("wlconf", dp_uif, "down");
						ifconfig(dp_uif, 0, NULL, NULL);
					}
				}
				eval("wlconf", name, "down");
				ifconfig(name, 0, NULL, NULL);

				/* List of primary WLAN interfaces that avail of HW switching. */
				/* In 3GMAC mode, each wl interfaces in "fwd_wlandevs" don't
				 * attach to the bridge.
				 */
				if (nvram_match("gmac3_enable", "1") &&
				    find_in_list(nvram_get("fwd_wlandevs"), name))
					goto gmac3_no_swbr;

				eval("brctl", "delif", lan_ifname, name);
gmac3_no_swbr:

#ifdef __CONFIG_EMF__
				/* Remove ifface from emf */
				if (nvram_match("emf_enable", "1"))
					eval("emf", "del", "iface", lan_ifname, name);
#endif /* __CONFIG_EMF__ */
			}
#ifdef __CONFIG_EMF__
			/* Stop the EMF for this LAN */
			eval("emf", "stop", lan_ifname);
			/* Remove Bridge from igs */
			eval("igs", "del", "bridge", lan_ifname);
			eval("emf", "del", "bridge", lan_ifname);
#endif /* __CONFIG_EMF__ */
			eval("brctl", "delbr", lan_ifname);
		}
		/* Bring down specific interface */
		else if (strcmp(lan_ifname, ""))
			eval("wlconf", lan_ifname, "down");
	}

	unlink("/tmp/ldhclnt");

	dprintf("done\n");
}

void
start_wl(void)
{
	int i;
	char *lan_ifname = nvram_safe_get("lan_ifname");
	char name[80], *next;
	char tmp[100];
	char *lan_ifnames;

	/* If we're a travel router... then we need to make sure we get
	 * the primary wireless interface up before trying to attach slave
	 * interface(s) to the bridge
	 */
	if (nvram_match("ure_disable", "0") && nvram_match("router_disable", "0")) {
		/* start wlireless */
		eval("wlconf", nvram_get("wan0_ifname"), "start");
	}

	if (nvram_match("ses_bridge_disable", "1")) {
		/* dont bring up the bridge; only the individual interfaces */
		foreach(name, nvram_safe_get("lan_ifnames"), next) {
			/* start wlireless */
			eval("wlconf", name, "start");
		}
	} else {

	/* Bring up bridged interfaces */
	for (i = 0; i < MAX_NO_BRIDGE; i++) {
		if (!i) {
			lan_ifname = nvram_safe_get("lan_ifname");
			lan_ifnames = nvram_safe_get("lan_ifnames");
		}
		else {
			snprintf(tmp, sizeof(tmp), "lan%x_ifname", i);
			lan_ifname = nvram_safe_get(tmp);
			snprintf(tmp, sizeof(tmp), "lan%x_ifnames", i);
			lan_ifnames = nvram_safe_get(tmp);
		}
		if (strncmp(lan_ifname, "br", 2) == 0) {
			foreach(name, lan_ifnames, next) {
				if (strncmp(name, "wl", 2) == 0) {
					if (!wl_vif_enabled(name, tmp)) {
						continue; /* Ignore disabled WL VIF */
					}
				}
				/* If a wl i/f, start it */
				eval("wlconf", name, "start");

			} /* foreach().... */
		} /* if (strncmp(lan_ifname.... */
		/* specific non-bridged lan i/f */
		else if (strcmp(lan_ifname, "")) {
			/* start wireless i/f */
			eval("wlconf", lan_ifname, "start");
		}
	} /* For loop */
	} /* if (nvram_match("ses_bridge_disable", "1")) */
}

#ifdef __CONFIG_NAT__
static int
wan_prefix(char *ifname, char *prefix)
{
	int unit;

	if ((unit = wan_ifunit(ifname)) < 0)
		return -1;

	sprintf(prefix, "wan%d_", unit);
	return 0;
}

static int
add_wan_routes(char *wan_ifname)
{
	char prefix[] = "wanXXXXXXXXXX_";

	/* Figure out nvram variable name prefix for this i/f */
	if (wan_prefix(wan_ifname, prefix) < 0)
		return -1;

	return add_routes(prefix, "route", wan_ifname);
}

static int
del_wan_routes(char *wan_ifname)
{
	char prefix[] = "wanXXXXXXXXXX_";

	/* Figure out nvram variable name prefix for this i/f */
	if (wan_prefix(wan_ifname, prefix) < 0)
		return -1;

	return del_routes(prefix, "route", wan_ifname);
}

static int
wan_valid(char *ifname)
{
	char name[80], *next;

	foreach(name, nvram_safe_get("wan_ifnames"), next)
		if (ifname && !strcmp(ifname, name))
			return 1;
	return 0;
}

void
start_wan(void)
{
	char *wan_ifname;
	char *wan_proto;
	int unit;
	char tmp[100], prefix[] = "wanXXXXXXXXXX_";
	char eabuf[32];
	int s;
	struct ifreq ifr;
	pid_t pid;

	/* check if we need to setup WAN */
	if (nvram_match("router_disable", "1"))
		return;

	/* do not bringup wan if in ses bridge disable mode */
	if (nvram_match("ses_bridge_disable", "1"))
		return;

	/* start connection independent firewall */
	start_firewall();

	/* Create links */
	if ((mkdir("/tmp/ppp", 0777) < 0) && (errno != EEXIST)) {
		perror("Could not create /tmp/ppp directory.");
		fprintf(stderr, "%s:%d: Aborting...\n", __FUNCTION__, __LINE__);
		abort();
	}
	symlink("/sbin/rc", "/tmp/ppp/ip-up");
	symlink("/sbin/rc", "/tmp/ppp/ip-down");

	symlink("/sbin/rc", "/tmp/udhcpc");

	/* Start each configured and enabled wan connection and its undelying i/f */
	for (unit = 0; unit < MAX_NVPARSE; unit ++) {
		snprintf(prefix, sizeof(prefix), "wan%d_", unit);

		/* make sure the connection exists and is enabled */
		wan_ifname = nvram_get(strcat_r(prefix, "ifname", tmp));
		if (!wan_ifname)
			continue;
		wan_proto = nvram_get(strcat_r(prefix, "proto", tmp));
		if (!wan_proto || !strcmp(wan_proto, "disabled"))
			continue;

		/* disable the connection if the i/f is not in wan_ifnames */
		if (!wan_valid(wan_ifname)) {
			nvram_set(strcat_r(prefix, "proto", tmp), "disabled");
			continue;
		}

		/* Checking for size of wan_ifname */
		if (strlen(wan_ifname) > (sizeof(ifr.ifr_name) - 1))
			continue;

		dprintf("%s %s\n", wan_ifname, wan_proto);

		/* Set i/f hardware address before bringing it up */
		if ((s = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) < 0)
			continue;
		strncpy(ifr.ifr_name, wan_ifname, sizeof(ifr.ifr_name));
		ifr.ifr_name[sizeof(ifr.ifr_name) - 1] = '\0';

		/* Configure i/f only once, specially for wl i/f shared by multiple connections */
		if (ioctl(s, SIOCGIFFLAGS, &ifr)) {
			close(s);
			continue;
		}

		if (!(ifr.ifr_flags & IFF_UP)) {
			/* Sync connection nvram address and i/f hardware address */
			memset(ifr.ifr_hwaddr.sa_data, 0, ETHER_ADDR_LEN);
			if (!nvram_invmatch(strcat_r(prefix, "hwaddr", tmp), "") ||
			    !ether_atoe(nvram_safe_get(strcat_r(prefix, "hwaddr", tmp)),
			    (unsigned char *)ifr.ifr_hwaddr.sa_data) ||
			    !memcmp(ifr.ifr_hwaddr.sa_data, "\0\0\0\0\0\0", ETHER_ADDR_LEN)) {
				if (ioctl(s, SIOCGIFHWADDR, &ifr)) {
					close(s);
					continue;
				}
				nvram_set(strcat_r(prefix, "hwaddr", tmp),
					ether_etoa((unsigned char *)ifr.ifr_hwaddr.sa_data,
					eabuf));
			} else {
				ifr.ifr_hwaddr.sa_family = ARPHRD_ETHER;
				if (ioctl(s, SIOCSIFHWADDR, &ifr)) {
					close(s);
					continue;
				}
			}

			/* Bring up i/f */
			ifconfig(wan_ifname, IFUP, NULL, NULL);

			/* do wireless specific config */
			if (nvram_match("ure_disable", "1")) {
				eval("wlconf", wan_ifname, "up");
				eval("wlconf", wan_ifname, "start");
			}
		}

		close(s);

		/* Set initial QoS mode again now that WAN port is ready. */
		set_et_qos_mode();

		/* Configure PPPoE connection. The PPPoE client will run
		* ip-up/ip-down scripts upon link's connect/disconnect.
		*/
		if (strcmp(wan_proto, "pppoe") == 0) {
			char *pppoe_argv[] = {
				"pppoecd",
				nvram_safe_get(strcat_r(prefix, "ifname", tmp)),
				"-u", nvram_safe_get(strcat_r(prefix, "pppoe_username", tmp)),
				"-p", nvram_safe_get(strcat_r(prefix, "pppoe_passwd", tmp)),
				"-r", nvram_safe_get(strcat_r(prefix, "pppoe_mru", tmp)),
				"-t", nvram_safe_get(strcat_r(prefix, "pppoe_mtu", tmp)),
				"-i", nvram_match(strcat_r(prefix, "pppoe_demand", tmp), "1") ?
				nvram_safe_get(strcat_r(prefix, "pppoe_idletime", tmp)) : "0",
				NULL, NULL,	/* pppoe_service */
				NULL, NULL,	/* pppoe_ac */
				NULL,		/* pppoe_keepalive */
				NULL, NULL,	/* ppp unit requested */
				NULL
			}, **arg;
			int timeout = 5;
			char pppunit[] = "XXXXXXXXXXXX";

			/* Add optional arguments */
			for (arg = pppoe_argv; *arg; arg++);
			if (nvram_invmatch(strcat_r(prefix, "pppoe_service", tmp), "")) {
				*arg++ = "-s";
				*arg++ = nvram_safe_get(strcat_r(prefix, "pppoe_service", tmp));
			}
			if (nvram_invmatch(strcat_r(prefix, "pppoe_ac", tmp), "")) {
				*arg++ = "-a";
				*arg++ = nvram_safe_get(strcat_r(prefix, "pppoe_ac", tmp));
			}
			if (nvram_match(strcat_r(prefix, "pppoe_demand", tmp), "1") ||
			nvram_match(strcat_r(prefix, "pppoe_keepalive", tmp), "1"))
				*arg++ = "-k";
			snprintf(pppunit, sizeof(pppunit), "%d", unit);
			*arg++ = "-U";
			*arg++ = pppunit;

			/* launch pppoe client daemon */
			_eval(pppoe_argv, NULL, 0, &pid);

			/* ppp interface name is referenced from this point on */
			wan_ifname = nvram_safe_get(strcat_r(prefix, "pppoe_ifname", tmp));

			/* Pretend that the WAN interface is up */
			if (nvram_match(strcat_r(prefix, "pppoe_demand", tmp), "1")) {
				/* Wait for pppx to be created */
				while (ifconfig(wan_ifname, IFUP, NULL, NULL) && timeout--)
					sleep(1);

				/* Retrieve IP info */
				if ((s = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) < 0)
					continue;
				strncpy(ifr.ifr_name, wan_ifname, IFNAMSIZ);

				/* Set temporary IP address */
				if (ioctl(s, SIOCGIFADDR, &ifr))
					perror(wan_ifname);
				nvram_set(strcat_r(prefix, "ipaddr", tmp),
				inet_ntoa(sin_addr(&ifr.ifr_addr)));
				nvram_set(strcat_r(prefix, "netmask", tmp), "255.255.255.255");

				/* Set temporary P-t-P address */
				if (ioctl(s, SIOCGIFDSTADDR, &ifr))
					perror(wan_ifname);
				nvram_set(strcat_r(prefix, "gateway", tmp),
				inet_ntoa(sin_addr(&ifr.ifr_dstaddr)));

				close(s);

				/* Preset routes so that traffic can be sent to proper pppx
				* even before the link is brought up.
				*/
				preset_wan_routes(wan_ifname);
			}
		}
		/* Configure DHCP connection. The DHCP client will run
		* 'udhcpc bound'/'udhcpc deconfig' upon finishing IP address
		* renew and release.
		*/
		else if (strcmp(wan_proto, "dhcp") == 0) {
			char *wan_hostname = nvram_safe_get(strcat_r(prefix, "hostname", tmp));
			char *dhcp_argv[] = { "udhcpc",
			"-i", wan_ifname,
			"-p", (sprintf(tmp, "/var/run/udhcpc%d.pid", unit), tmp),
			"-s", "/tmp/udhcpc",
			wan_hostname && *wan_hostname ? "-H" : NULL,
			wan_hostname && *wan_hostname ? wan_hostname : NULL,
			NULL
			};
			/* Start dhcp client */
			_eval(dhcp_argv, NULL, 0, &pid);
		}
		/* Configure static IP connection. */
		else if (strcmp(wan_proto, "static") == 0) {
			/* Assign static IP address to i/f */
			ifconfig(wan_ifname, IFUP,
				nvram_safe_get(strcat_r(prefix, "ipaddr", tmp)),
				nvram_safe_get(strcat_r(prefix, "netmask", tmp)));
			/* We are done configuration */
			wan_up(wan_ifname);
		}

		/* Start connection dependent firewall */
		start_firewall2(wan_ifname);

		dprintf("%s %s\n",
			nvram_safe_get(strcat_r(prefix, "ipaddr", tmp)),
			nvram_safe_get(strcat_r(prefix, "netmask", tmp)));
	}

	/* Report stats */
	if (nvram_invmatch("stats_server", "")) {
		char *stats_argv[] = { "stats", nvram_get("stats_server"), NULL };
		_eval(stats_argv, NULL, 5, NULL);
	}
}

void
stop_wan(void)
{
	char name[80], *next, signal[] = "XXXX";

#ifdef BCMQOS
		del_iQosRules();
#endif /* BCMQOS */
	eval("killall", "stats");
	eval("killall", "ntpclient");

	/* Shutdown and kill all possible tasks */
	eval("killall", "ip-up");
	eval("killall", "ip-down");
	snprintf(signal, sizeof(signal), "-%d", SIGHUP);
	eval("killall", signal, "pppoecd");
	eval("killall", "pppoecd");
	snprintf(signal, sizeof(signal), "-%d", SIGUSR2);
	eval("killall", signal, "udhcpc");
	eval("killall", "udhcpc");

	/* Bring down WAN interfaces */
	foreach(name, nvram_safe_get("wan_ifnames"), next)
		ifconfig(name, 0, "0.0.0.0", NULL);

	/* Remove dynamically created links */
	unlink("/tmp/udhcpc");

	unlink("/tmp/ppp/ip-up");
	unlink("/tmp/ppp/ip-down");
	rmdir("/tmp/ppp");

	dprintf("done\n");
}

static int
add_ns(char *wan_ifname)
{
	FILE *fp;
	char tmp[100], prefix[] = "wanXXXXXXXXXX_";
	char word[100], *next;
	char line[100];

	/* Figure out nvram variable name prefix for this i/f */
	if (wan_prefix(wan_ifname, prefix) < 0)
		return -1;

	/* Open resolv.conf to read */
	if (!(fp = fopen("/tmp/resolv.conf", "r+"))) {
		perror("/tmp/resolv.conf");
		return errno;
	}
	/* Append only those not in the original list */
	foreach(word, nvram_safe_get(strcat_r(prefix, "dns", tmp)), next) {
		fseek(fp, 0, SEEK_SET);
		while (fgets(line, sizeof(line), fp)) {
			char *token = strtok(line, " \t\n");

			if (!token || strcmp(token, "nameserver") != 0)
				continue;
			if (!(token = strtok(NULL, " \t\n")))
				continue;

			if (!strcmp(token, word))
				break;
		}
		if (feof(fp))
			fprintf(fp, "nameserver %s\n", word);
	}
	fclose(fp);

	/* notify dnsmasq */
	snprintf(tmp, sizeof(tmp), "-%d", SIGHUP);
	eval("killall", tmp, "dnsmasq");

	return 0;
}

static int
del_ns(char *wan_ifname)
{
	FILE *fp, *fp2;
	char tmp[100], prefix[] = "wanXXXXXXXXXX_";
	char word[100], *next;
	char line[100];

	/* Figure out nvram variable name prefix for this i/f */
	if (wan_prefix(wan_ifname, prefix) < 0)
		return -1;

	/* Open resolv.conf to read */
	if (!(fp = fopen("/tmp/resolv.conf", "r"))) {
		perror("fopen /tmp/resolv.conf");
		return errno;
	}
	/* Open resolv.tmp to save updated name server list */
	if (!(fp2 = fopen("/tmp/resolv.tmp", "w"))) {
		perror("fopen /tmp/resolv.tmp");
		fclose(fp);
		return errno;
	}
	/* Copy updated name servers */
	while (fgets(line, sizeof(line), fp)) {
		char *token = strtok(line, " \t\n");

		if (!token || strcmp(token, "nameserver") != 0)
			continue;
		if (!(token = strtok(NULL, " \t\n")))
			continue;

		foreach(word, nvram_safe_get(strcat_r(prefix, "dns", tmp)), next)
			if (!strcmp(word, token))
				break;
		if (!next)
			fprintf(fp2, "nameserver %s\n", token);
	}
	fclose(fp);
	fclose(fp2);
	/* Use updated file as resolv.conf */
	unlink("/tmp/resolv.conf");
	if (rename("/tmp/resolv.tmp", "/tmp/resolv.conf") != 0) {
		perror("rename fails /tmp/resolv.conf");
		return errno;
	}
	/* notify dnsmasq */
	snprintf(tmp, sizeof(tmp), "-%d", SIGHUP);
	eval("killall", tmp, "dnsmasq");

	return 0;
}

/*
*/
#ifdef __CONFIG_IPV6__
/* Start the 6to4 Tunneling interface.
*	Return > 0: number of interfaces processed by this routine.
*		==0: skipped since no action is required.
*		< 0: Error number
*/
static int
start_6to4(char *wan_ifname)
{
	int i, ret = 0;
	int siMode, siCount;
	unsigned short uw6to4ID;
	in_addr_t uiWANIP;
	char *pcLANIF, *pcWANIP, tmp[64], prefix[] = "wanXXXXXXXXXX_";

	/* Figure out nvram variable name prefix for this i/f */
	if (wan_prefix(wan_ifname, prefix) < 0)
		return 0;

	pcWANIP = nvram_safe_get(strcat_r(prefix, "ipaddr", tmp));
	uiWANIP = inet_network(pcWANIP);

	/* Check if the wan IP is private(RFC1918). 6to4 needs a global IP */
	if ((uiWANIP == 0) || (uiWANIP == -1) ||
		(uiWANIP & 0xffff0000) == 0xc0a80000 || /* 192.168.x.x */
		(uiWANIP & 0xfff00000) == 0xac100000 || /* 172.16.x.x */
		(uiWANIP & 0xff000000) == 0x0a000000) /* 10.x.x.x */
		return 0;

	/* Create 6to4 intrface and setup routing table */
	for (i = 0, siCount = 0; i < MAX_NO_BRIDGE; i++) {
		if (i == 0) {
			pcLANIF = nvram_safe_get("lan_ifname");
			siMode = atoi(nvram_safe_get("lan_ipv6_mode"));
			uw6to4ID = (unsigned short)atoi(nvram_safe_get("lan_ipv6_6to4id"));
		}
		else {
			snprintf(tmp, sizeof(tmp), "lan%x_ifname", i);
			pcLANIF = nvram_safe_get(tmp);
			snprintf(tmp, sizeof(tmp), "lan%x_ipv6_mode", i);
			siMode = atoi(nvram_safe_get(tmp));
			snprintf(tmp, sizeof(tmp), "lan%x_ipv6_6to4id", i);
			uw6to4ID = (unsigned short)atoi(nvram_safe_get(tmp));
		}

		if (siMode & IPV6_6TO4_ENABLED) {
			/* Add the 6to4 route. */
			snprintf(tmp, sizeof(tmp), "2002:%x:%x:%x::/64",
				(unsigned short)(uiWANIP>>16), (unsigned short)uiWANIP,	uw6to4ID);
			ret = eval("ip", "-6", "route", "add", tmp,
				"dev", pcLANIF, "metric", "1");
			siCount++;
		}
	}

	if (siCount == 0)
		return 0;

	/* Create 6to4 intrface and setup routing table */
	{
		char *pc6to4IF = "v6to4"; /* The 6to4 tunneling interface name */
		struct in_addr stWANIP;

		stWANIP.s_addr = htonl(uiWANIP);

		/* Create the tunneling interface */
		ret = eval("ip", "tunnel", "add", pc6to4IF, "mode", "sit",
			"ttl", "64", "remote", "any", "local", inet_ntoa(stWANIP));

		/* Bring the device up */
		ret = eval("ip", "link", "set", "dev", pc6to4IF, "up");

		/* Add 6to4 v4 anycast route to the global IPv6 network */
		ret = eval("ip", "-6", "route", "add", "2000::/3",
			"via", "::192.88.99.1", "dev", pc6to4IF, "metric", "1");
	}

#ifdef __CONFIG_RADVD__
	/* Restart radvd */
	{
		char acSignal[64];

		snprintf(acSignal, sizeof(acSignal), "-%d", SIGHUP);
		ret = eval("killall", acSignal, "radvd");
	}
#endif /* __CONFIG_RADVD__ */

#ifdef __CONFIG_NAT__
	/* Enable IPv6 protocol=41(0x29) on v4NAT */
	{
		char *pcWANIF;

		pcWANIF = nvram_match("wan_proto", "pppoe")?
			nvram_safe_get("wan_pppoe_ifname"): nvram_safe_get("wan_ifname");
		add_ipv6_filter(nvram_safe_get(pcWANIF));
	}
#endif /* __CONFIG_NAT__ */

	return siCount;
}
#endif /* __CONFIG_IPV6__ */
/*
*/

void
wan_up(char *wan_ifname)
{
	char tmp[100], prefix[] = "wanXXXXXXXXXX_";
	char *wan_proto;

	/* Figure out nvram variable name prefix for this i/f */
	if (wan_prefix(wan_ifname, prefix) < 0)
		return;

	wan_proto = nvram_safe_get(strcat_r(prefix, "proto", tmp));

	dprintf("%s %s\n", wan_ifname, wan_proto);

	/* Set default route to gateway if specified */
	if (nvram_match(strcat_r(prefix, "primary", tmp), "1"))
		route_add(wan_ifname, 0, "0.0.0.0",
			nvram_safe_get(strcat_r(prefix, "gateway", tmp)),
			"0.0.0.0");

	/* Install interface dependent static routes */
	add_wan_routes(wan_ifname);

	/* Add dns servers to resolv.conf */
	add_ns(wan_ifname);

	/* Sync time */
	start_ntpc();

#ifdef BCMQOS
	add_iQosRules(wan_ifname);
	start_iQos();
#endif /* BCMQOS */
/*
*/
#ifdef __CONFIG_IPV6__
	start_6to4(wan_ifname);
#endif /* __CONFIG_IPV6__ */
/*
*/

	dprintf("done\n");
}

void
wan_down(char *wan_ifname)
{
	char tmp[100], prefix[] = "wanXXXXXXXXXX_";
	char *wan_proto;

	/* Figure out nvram variable name prefix for this i/f */
	if (wan_prefix(wan_ifname, prefix) < 0)
		return;

	wan_proto = nvram_safe_get(strcat_r(prefix, "proto", tmp));

	printf("%s %s\n", wan_ifname, wan_proto);

	/* Remove default route to gateway if specified */
	if (nvram_match(strcat_r(prefix, "primary", tmp), "1"))
		route_del(wan_ifname, 0, "0.0.0.0",
			nvram_safe_get(strcat_r(prefix, "gateway", tmp)),
			"0.0.0.0");

	/* Remove interface dependent static routes */
	del_wan_routes(wan_ifname);

	/* Update resolv.conf */
	del_ns(wan_ifname);

	dprintf("done\n");
}
#endif	/* __CONFIG_NAT__ */

/* Enable WET DHCP relay for ethernet clients */
static int
enable_dhcprelay(char *ifname)
{
	char name[80], *next;

	dprintf("%s\n", ifname);

	/* WET interface is meaningful only in bridged environment */
	if (strncmp(ifname, "br", 2) == 0) {
		foreach(name, nvram_safe_get("lan_ifnames"), next) {
			char mode[] = "wlXXXXXXXXXX_mode";
			int unit;

			/* make sure the interface is indeed of wl */
			if (wl_probe(name))
				continue;

			/* get the instance number of the wl i/f */
			wl_ioctl(name, WLC_GET_INSTANCE, &unit, sizeof(unit));
			snprintf(mode, sizeof(mode), "wl%d_mode", unit);

			/* enable DHCP relay, there should be only one WET i/f */
			if (nvram_match(mode, "wet")) {
				uint32 ip;
				inet_aton(nvram_safe_get("lan_ipaddr"), (struct in_addr *)&ip);
				if (wl_iovar_setint(name, "wet_host_ipv4", ip))
					perror("wet_host_ipv4");
				break;
			}
		}
	}
	return 0;
}

void
lan_up(char *lan_ifname)
{
	/* Install default route to gateway - AP only */
	if (nvram_match("router_disable", "1") && nvram_invmatch("lan_gateway", ""))
		route_add(lan_ifname, 0, "0.0.0.0", nvram_safe_get("lan_gateway"), "0.0.0.0");

	/* Install interface dependent static routes */
	add_lan_routes(lan_ifname);

	/* Sync time - AP only */
	if (nvram_match("router_disable", "1"))
		start_ntpc();

	/* Enable WET DHCP relay if requested */
	if (atoi(nvram_safe_get("dhcp_relay")) == 1)
		enable_dhcprelay(lan_ifname);

	dprintf("done\n");
}

void
lan_down(char *lan_ifname)
{
	/* Remove default route to gateway - AP only */
	if (nvram_match("router_disable", "1") && nvram_invmatch("lan_gateway", ""))
		route_del(lan_ifname, 0, "0.0.0.0", nvram_safe_get("lan_gateway"), "0.0.0.0");

	/* Remove interface dependent static routes */
	del_lan_routes(lan_ifname);

	dprintf("done\n");
}

int
hotplug_net(void)
{
	char *lan_ifname = nvram_safe_get("lan_ifname");
	char *interface, *action;
	bool psta_if, dyn_if, add_event, remove_event;

	dprintf("hotplug_net(): start\n");

	if (!(interface = getenv("INTERFACE")) ||
	    !(action = getenv("ACTION")))
		return EINVAL;

	dprintf("hotplug_net(): interface %s action %s\n", interface, action);

#ifdef LINUX26
	add_event = !strcmp(action, "add");
#else
	add_event = !strcmp(action, "register");
#endif

#ifdef LINUX26
	remove_event = !strcmp(action, "remove");
#else
	remove_event = !strcmp(action, "unregister");
#endif

	psta_if = wl_wlif_is_psta(interface);
	dyn_if = !strncmp(interface, "wds", 3) || psta_if;

	if (!dyn_if && !remove_event)
		return 0;

	if (add_event) {
		/* Bring up the interface and add to the bridge */
		ifconfig(interface, IFUP, NULL, NULL);

#ifdef __CONFIG_EMF__
		if (nvram_match("emf_enable", "1")) {
			eval("emf", "add", "iface", lan_ifname, interface);
			emf_mfdb_update(lan_ifname, interface, TRUE);
			emf_uffp_update(lan_ifname, interface, TRUE);
			emf_rtport_update(lan_ifname, interface, TRUE);
		}
#endif /* __CONFIG_EMF__ */

		/* Indicate interface create event to eapd */
		if (psta_if) {
			dprintf("hotplug_net(): send dif event to %s\n", interface);
			wl_send_dif_event(interface, 0);
			return 0;
		}

		/* Bridge WDS interfaces */
		if (!strncmp(lan_ifname, "br", 2) &&
			eval("brctl", "addif", lan_ifname, interface, "wait")) {
			dprintf("hotplug_net():Adding interface %s\n", interface);
			return 0;
		}

		/* Inform driver to send up new WDS link event */
		if (wl_iovar_setint(interface, "wds_enable", 1)) {
			dprintf("%s set wds_enable failed\n", interface);
			return 0;
		}

		return 0;
	}

	if (remove_event) {
		/* Indicate interface delete event to eapd */
		wl_send_dif_event(interface, 1);

#ifdef __CONFIG_EMF__
		if (nvram_match("emf_enable", "1"))
			eval("emf", "del", "iface", lan_ifname, interface);
#endif /* __CONFIG_EMF__ */
	}

	return 0;
}

#ifdef __CONFIG_NAT__
int
wan_ifunit(char *wan_ifname)
{
	int unit;
	char tmp[100], prefix[] = "wanXXXXXXXXXX_";

	if ((unit = ppp_ifunit(wan_ifname)) >= 0)
		return unit;
	else {
		for (unit = 0; unit < MAX_NVPARSE; unit ++) {
			snprintf(prefix, sizeof(prefix), "wan%d_", unit);
			if (nvram_match(strcat_r(prefix, "ifname", tmp), wan_ifname) &&
			    (nvram_match(strcat_r(prefix, "proto", tmp), "dhcp") ||
			     nvram_match(strcat_r(prefix, "proto", tmp), "static")))
				return unit;
		}
	}
	return -1;
}

int
preset_wan_routes(char *wan_ifname)
{
	char tmp[100], prefix[] = "wanXXXXXXXXXX_";

	/* Figure out nvram variable name prefix for this i/f */
	if (wan_prefix(wan_ifname, prefix) < 0)
		return -1;

	/* Set default route to gateway if specified */
	if (nvram_match(strcat_r(prefix, "primary", tmp), "1"))
		route_add(wan_ifname, 0, "0.0.0.0", "0.0.0.0", "0.0.0.0");

	/* Install interface dependent static routes */
	add_wan_routes(wan_ifname);
	return 0;
}

int
wan_primary_ifunit(void)
{
	int unit;

	for (unit = 0; unit < MAX_NVPARSE; unit ++) {
		char tmp[100], prefix[] = "wanXXXXXXXXXX_";
		snprintf(prefix, sizeof(prefix), "wan%d_", unit);
		if (nvram_match(strcat_r(prefix, "primary", tmp), "1"))
			return unit;
	}

	return 0;
}
#endif	/* __CONFIG_NAT__ */
