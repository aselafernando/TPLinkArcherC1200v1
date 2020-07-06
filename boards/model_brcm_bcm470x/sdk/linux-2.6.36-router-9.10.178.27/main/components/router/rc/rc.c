/*
 * Router rc control script
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
 * $Id: rc.c 584192 2015-09-04 10:52:25Z $
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <syslog.h>
#include <signal.h>
#include <fcntl.h> /* for open */
#include <string.h>
#include <sys/klog.h>
#include <sys/types.h>
#include <sys/mount.h>
#include <sys/reboot.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <sys/time.h>
#include <sys/utsname.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/utsname.h> /* for uname */
#include <net/if_arp.h>
#include <dirent.h>

#include <epivers.h>
#include <router_version.h>
#include <mtd.h>
#include <shutils.h>
#include <rc.h>
#include <netconf.h>
#include <nvparse.h>
#include <bcmdevs.h>
#include <bcmparams.h>
#include <bcmnvram.h>
#include <wlutils.h>
#include <ezc.h>
#include <pmon.h>
#include <bcmconfig.h>
#include <confmtd_utils.h>

#if defined(__CONFIG_WAPI__) || defined(__CONFIG_WAPI_IAS__)
#include <wapi_path.h>
#endif
#if defined(__CONFIG_CIFS__)
#include <cifs_path.h>
#endif

#include <etutils.h>

#ifdef __CONFIG_NAT__
static void auto_bridge(void);
#endif	/* __CONFIG_NAT__ */

#ifdef __CONFIG_EMF__
extern void load_emf(void);
#endif /* __CONFIG_EMF__ */

#ifdef __CONFIG_DHDAP__
#define MAX_FW_PATH	512
#endif /* __CONFIG_DHDAP__ */

static void restore_defaults(void);
static void sysinit(void);
static void rc_signal(int sig);

extern struct nvram_tuple router_defaults[];

#define RESTORE_DEFAULTS() \
	(!nvram_match("restore_defaults", "0") || nvram_invmatch("os_name", "linux"))

#ifdef LINUX_2_6_36
static int
coma_uevent(void)
{
	char *modalias = NULL;
	char lan_ifname[32], *lan_ifnames, *next;

	modalias = getenv("MODALIAS");
	if (!strcmp(modalias, "platform:coma_dev")) {

		/* down WiFi adapter */
		lan_ifnames = nvram_safe_get("lan_ifnames");
		foreach(lan_ifname, lan_ifnames, next) {
			if (!strncmp(lan_ifname, "eth", 3)) {
				eval("wl", "-i", lan_ifname, "down");
			}
		}

		system("echo \"2\" > /proc/bcm947xx/coma");
	}
	return 0;
}
#endif /* LINUX_2_6_36 */

static int
build_ifnames(char *type, char *names, int *size)
{
	char name[32], *next;
	int len = 0;
	int s;

	/* open a raw scoket for ioctl */
	if ((s = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) < 0)
		return -1;

	/*
	 * go thru all device names (wl<N> il<N> et<N> vlan<N>) and interfaces to
	 * build an interface name list in which each i/f name coresponds to a device
	 * name in device name list. Interface/device name matching rule is device
	 * type dependant:
	 *
	 *	wl:	by unit # provided by the driver, for example, if eth1 is wireless
	 *		i/f and its unit # is 0, then it will be in the i/f name list if
	 *		wl0 is in the device name list.
	 *	il/et:	by mac address, for example, if et0's mac address is identical to
	 *		that of eth2's, then eth2 will be in the i/f name list if et0 is
	 *		in the device name list.
	 *	vlan:	by name, for example, vlan0 will be in the i/f name list if vlan0
	 *		is in the device name list.
	 */
	foreach(name, type, next) {
		struct ifreq ifr;
		int i, unit;
		char var[32], *mac;
		unsigned char ea[ETHER_ADDR_LEN];

		/* vlan: add it to interface name list */
		if (!strncmp(name, "vlan", 4)) {
			/* append interface name to list */
			len += snprintf(&names[len], *size - len, "%s ", name);
			continue;
		}

		/* others: proceed only when rules are met */
		for (i = 1; i <= DEV_NUMIFS; i ++) {
			/* ignore i/f that is not ethernet */
			ifr.ifr_ifindex = i;
			if (ioctl(s, SIOCGIFNAME, &ifr))
				continue;
			if (ioctl(s, SIOCGIFHWADDR, &ifr))
				continue;
			if (ifr.ifr_hwaddr.sa_family != ARPHRD_ETHER)
				continue;
			if (!strncmp(ifr.ifr_name, "vlan", 4))
				continue;

			/* wl: use unit # to identify wl */
			if (!strncmp(name, "wl", 2)) {
				if (wl_probe(ifr.ifr_name) ||
				    wl_ioctl(ifr.ifr_name, WLC_GET_INSTANCE, &unit, sizeof(unit)) ||
				    unit != atoi(&name[2]))
					continue;
			}
			/* et/il: use mac addr to identify et/il */
			else if (!strncmp(name, "et", 2) || !strncmp(name, "il", 2)) {
				snprintf(var, sizeof(var), "%smacaddr", name);
				if (!(mac = nvram_get(var)) || !ether_atoe(mac, ea) ||
				    bcmp(ea, ifr.ifr_hwaddr.sa_data, ETHER_ADDR_LEN))
					continue;
			}
			/* mac address: compare value */
			else if (ether_atoe(name, ea) &&
				!bcmp(ea, ifr.ifr_hwaddr.sa_data, ETHER_ADDR_LEN))
				;
			/* others: ignore */
			else
				continue;

			/* append interface name to list */
			len += snprintf(&names[len], *size - len, "%s ", ifr.ifr_name);
		}
	}

	close(s);

	*size = len;
	return 0;
}

#ifdef __CONFIG_WPS__
static void
wps_restore_defaults(void)
{
	/* cleanly up nvram for WPS */
	nvram_unset("wps_config_state");
	nvram_unset("wps_device_pin");
	nvram_unset("wps_proc_status");
	nvram_unset("wps_sta_pin");
	nvram_unset("wps_restart");
	nvram_unset("wps_config_method");
}
#endif /* __CONFIG_WPS__ */

static void
ses_cleanup(void)
{
	/* well known event to cleanly initialize state machine */
	nvram_set("ses_event", "2");

	/* Delete lethal dynamically generated variables */
	nvram_unset("ses_bridge_disable");
}

static void
ses_restore_defaults(void)
{
	char tmp[100], prefix[] = "wlXXXXXXXXXX_ses_";
	int i, j;
	int len = 0;

	/* Delete dynamically generated variables */
	for (i = 0; i < MAX_NVPARSE; i++) {
		sprintf(prefix, "wl%d_ses_", i);
		nvram_unset(strcat_r(prefix, "ssid", tmp));
		nvram_unset(strcat_r(prefix, "closed", tmp));
		nvram_unset(strcat_r(prefix, "wpa_psk", tmp));
		nvram_unset(strcat_r(prefix, "auth", tmp));
		nvram_unset(strcat_r(prefix, "wep", tmp));
		nvram_unset(strcat_r(prefix, "auth_mode", tmp));
		nvram_unset(strcat_r(prefix, "crypto", tmp));
		nvram_unset(strcat_r(prefix, "akm", tmp));
		nvram_unset(strcat_r(prefix, "wds", tmp));
		nvram_unset(strcat_r(prefix, "wds_timeout", tmp));
	}

	/* Delete dynamically generated variables */
	for (i = 0; i < DEV_NUMIFS; i++) {
		sprintf(prefix, "wl%d_wds", i);
		len = strlen(prefix);
		for (j = 0; j < MAX_NVPARSE; j++) {
			sprintf(&prefix[len], "%d", j);
			nvram_unset(prefix);
		}
	}

	nvram_unset("ses_fsm_current_states");
	nvram_unset("ses_fsm_last_status");
}
static void
virtual_radio_restore_defaults(void)
{
	char tmp[100], prefix[] = "wlXXXXXXXXXXXXXXXXXXXXXXXXXXXX_mssid_";
	int i, j;

	nvram_unset("unbridged_ifnames");
	nvram_unset("ure_disable");

	/* Delete dynamically generated variables */
	for (i = 0; i < MAX_NVPARSE; i++) {
		sprintf(prefix, "wl%d_", i);
		nvram_unset(strcat_r(prefix, "vifs", tmp));
		nvram_unset(strcat_r(prefix, "ssid", tmp));
		nvram_unset(strcat_r(prefix, "guest", tmp));
		nvram_unset(strcat_r(prefix, "ure", tmp));
		nvram_unset(strcat_r(prefix, "ipconfig_index", tmp));
		nvram_unset(strcat_r(prefix, "nas_dbg", tmp));
		sprintf(prefix, "lan%d_", i);
		nvram_unset(strcat_r(prefix, "ifname", tmp));
		nvram_unset(strcat_r(prefix, "ifnames", tmp));
		nvram_unset(strcat_r(prefix, "gateway", tmp));
		nvram_unset(strcat_r(prefix, "proto", tmp));
		nvram_unset(strcat_r(prefix, "ipaddr", tmp));
		nvram_unset(strcat_r(prefix, "netmask", tmp));
		nvram_unset(strcat_r(prefix, "lease", tmp));
		nvram_unset(strcat_r(prefix, "stp", tmp));
		nvram_unset(strcat_r(prefix, "hwaddr", tmp));
		sprintf(prefix, "dhcp%d_", i);
		nvram_unset(strcat_r(prefix, "start", tmp));
		nvram_unset(strcat_r(prefix, "end", tmp));

		/* clear virtual versions */
		for (j = 0; j < 16; j++) {
			sprintf(prefix, "wl%d.%d_", i, j);
			nvram_unset(strcat_r(prefix, "ssid", tmp));
			nvram_unset(strcat_r(prefix, "ipconfig_index", tmp));
			nvram_unset(strcat_r(prefix, "guest", tmp));
			nvram_unset(strcat_r(prefix, "closed", tmp));
			nvram_unset(strcat_r(prefix, "wpa_psk", tmp));
			nvram_unset(strcat_r(prefix, "auth", tmp));
			nvram_unset(strcat_r(prefix, "wep", tmp));
			nvram_unset(strcat_r(prefix, "auth_mode", tmp));
			nvram_unset(strcat_r(prefix, "crypto", tmp));
			nvram_unset(strcat_r(prefix, "akm", tmp));
			nvram_unset(strcat_r(prefix, "hwaddr", tmp));
			nvram_unset(strcat_r(prefix, "bss_enabled", tmp));
			nvram_unset(strcat_r(prefix, "bss_maxassoc", tmp));
			nvram_unset(strcat_r(prefix, "wme_bss_disable", tmp));
			nvram_unset(strcat_r(prefix, "ifname", tmp));
			nvram_unset(strcat_r(prefix, "unit", tmp));
			nvram_unset(strcat_r(prefix, "ap_isolate", tmp));
			nvram_unset(strcat_r(prefix, "macmode", tmp));
			nvram_unset(strcat_r(prefix, "maclist", tmp));
			nvram_unset(strcat_r(prefix, "maxassoc", tmp));
			nvram_unset(strcat_r(prefix, "mode", tmp));
			nvram_unset(strcat_r(prefix, "radio", tmp));
			nvram_unset(strcat_r(prefix, "radius_ipaddr", tmp));
			nvram_unset(strcat_r(prefix, "radius_port", tmp));
			nvram_unset(strcat_r(prefix, "radius_key", tmp));
			nvram_unset(strcat_r(prefix, "key", tmp));
			nvram_unset(strcat_r(prefix, "key1", tmp));
			nvram_unset(strcat_r(prefix, "key2", tmp));
			nvram_unset(strcat_r(prefix, "key3", tmp));
			nvram_unset(strcat_r(prefix, "key4", tmp));
			nvram_unset(strcat_r(prefix, "wpa_gtk_rekey", tmp));
			nvram_unset(strcat_r(prefix, "nas_dbg", tmp));
			nvram_unset(strcat_r(prefix, "probresp_mf", tmp));

			nvram_unset(strcat_r(prefix, "bss_opmode_cap_reqd", tmp));
			nvram_unset(strcat_r(prefix, "mcast_regen_bss_enable", tmp));
			nvram_unset(strcat_r(prefix, "wmf_bss_enable", tmp));
			nvram_unset(strcat_r(prefix, "preauth", tmp));
			nvram_unset(strcat_r(prefix, "dwds", tmp));
			nvram_unset(strcat_r(prefix, "acs_dfsr_deferred", tmp));
			nvram_unset(strcat_r(prefix, "wet_tunnel", tmp));
			nvram_unset(strcat_r(prefix, "bridge", tmp));
			nvram_unset(strcat_r(prefix, "mfp", tmp));
			nvram_unset(strcat_r(prefix, "acs_dfsr_activity", tmp));
			nvram_unset(strcat_r(prefix, "acs_dfsr_immediate", tmp));
			nvram_unset(strcat_r(prefix, "wme", tmp));
			nvram_unset(strcat_r(prefix, "net_reauth", tmp));
			nvram_unset(strcat_r(prefix, "sta_retry_time", tmp));
			nvram_unset(strcat_r(prefix, "infra", tmp));

#ifdef __CONFIG_HSPOT__
			nvram_unset(strcat_r(prefix, "hsflag", tmp));
			nvram_unset(strcat_r(prefix, "hs2cap", tmp));
			nvram_unset(strcat_r(prefix, "opercls", tmp));
			nvram_unset(strcat_r(prefix, "anonai", tmp));
			nvram_unset(strcat_r(prefix, "wanmetrics", tmp));
			nvram_unset(strcat_r(prefix, "oplist", tmp));
			nvram_unset(strcat_r(prefix, "homeqlist", tmp));
			nvram_unset(strcat_r(prefix, "osu_ssid", tmp));
			nvram_unset(strcat_r(prefix, "osu_frndname", tmp));
			nvram_unset(strcat_r(prefix, "osu_uri", tmp));
			nvram_unset(strcat_r(prefix, "osu_nai", tmp));
			nvram_unset(strcat_r(prefix, "osu_method", tmp));
			nvram_unset(strcat_r(prefix, "osu_icons", tmp));
			nvram_unset(strcat_r(prefix, "osu_servdesc", tmp));
			nvram_unset(strcat_r(prefix, "concaplist", tmp));
			nvram_unset(strcat_r(prefix, "qosmapie", tmp));
			nvram_unset(strcat_r(prefix, "gascbdel", tmp));
			nvram_unset(strcat_r(prefix, "iwnettype", tmp));
			nvram_unset(strcat_r(prefix, "hessid", tmp));
			nvram_unset(strcat_r(prefix, "ipv4addr", tmp));
			nvram_unset(strcat_r(prefix, "ipv6addr", tmp));
			nvram_unset(strcat_r(prefix, "netauthlist", tmp));
			nvram_unset(strcat_r(prefix, "venuegrp", tmp));
			nvram_unset(strcat_r(prefix, "venuetype", tmp));
			nvram_unset(strcat_r(prefix, "venuelist", tmp));
			nvram_unset(strcat_r(prefix, "ouilist", tmp));
			nvram_unset(strcat_r(prefix, "3gpplist", tmp));
			nvram_unset(strcat_r(prefix, "domainlist", tmp));
			nvram_unset(strcat_r(prefix, "realmlist", tmp));
#endif /* __CONFIG_HSPOT__ */
		}
	}
}

static void
set_psr_vars(int unit)
{
	int wl_unit = unit;
	int max_no_vifs = 0;
	int i;
	char *name;
	char *next = NULL;
	char cap[WLC_IOCTL_SMLEN];
	char caps[WLC_IOCTL_MEDLEN];
	char nv_param[NVRAM_MAX_PARAM_LEN];
	char nv_value[NVRAM_MAX_VALUE_LEN];
	char nv_interface[NVRAM_MAX_PARAM_LEN];

	/* Set the mode */
	sprintf(nv_param, "wl%d_mode", wl_unit);
	nvram_set(nv_param, "psr");
	/* Set the second radio as AP */
	sprintf(nv_param, "wl%d_mode", (wl_unit + 1));
	nvram_set(nv_param, "ap");

	nvram_set("wl_nband", "1");
	nvram_set("wl_phytype", "v");

	/* Get the number of VIFS */
	sprintf(nv_interface, "wl%d_ifname", wl_unit);
	name = nvram_safe_get(nv_interface);
	if (wl_iovar_get(name, "cap", (void *)caps, sizeof(caps))) {
		return;
	}

	sprintf(nv_interface, "wl%d_vifs", wl_unit);
	sprintf(nv_value, "wl%d.1", wl_unit);
	nvram_set(nv_interface, nv_value);

	foreach(cap, caps, next) {
		if (!strcmp(cap, "mbss16"))
			max_no_vifs = 16;
		else if (!strcmp(cap, "mbss4"))
			max_no_vifs = 4;
	}

	/* Enable the primary VIF */
	sprintf(nv_param, "wl%d.1_bss_enabled", wl_unit);
	nvram_set(nv_param, "1");

	/* Disable all VIFS wlX.2 onwards */
	for (i = 2; i < max_no_vifs; i++) {
		sprintf(nv_param, "wl%d.%d_bss_enabled", wl_unit, i);
		nvram_set(nv_param, "0");
	}

	/* Commit nvram variables */
	nvram_commit();
}

#ifdef __CONFIG_URE__
static void
ure_restore_defaults(int unit)
{
	char tmp[100], prefix[] = "wlXXXXXXXXXX_";
	struct nvram_tuple *t;

	sprintf(prefix, "wl%d.1_", unit);

	for (t = router_defaults; t->name; t++) {
		if (!strncmp(t->name, "wl_", 3)) {
			strcat_r(prefix, &t->name[3], tmp);
			nvram_unset(tmp);
		}
	}

	sprintf(prefix, "wl%d_ure", unit);
	nvram_unset(prefix);
}

static void
set_ure_vars(int unit)
{
	int wl_unit = unit;
	int travel_router;
	char prefix[] = "wlXXXXXXXXXX_";
	struct nvram_tuple *t;
	char nv_param[NVRAM_MAX_PARAM_LEN];
	char nv_value[NVRAM_MAX_VALUE_LEN];
	char nv_interface[NVRAM_MAX_PARAM_LEN];
	char os_interface[NVRAM_MAX_PARAM_LEN];
	int os_interface_size = sizeof(os_interface);
	char *temp = NULL;
	char interface_list[NVRAM_MAX_VALUE_LEN];
	int interface_list_size = sizeof(interface_list);
	char *wan0_ifname = "wan0_ifname";
	char *lan_ifnames = "lan_ifnames";
	char *wan_ifnames = "wan_ifnames";

	sprintf(prefix, "wl%d.1_", wl_unit);

	/* Clone default wl nvram settings to wl0.1 */
	for (t = router_defaults; t->name; t++) {
		if (!strncmp(t->name, "wl_", 3)) {
			strcat_r(prefix, &t->name[3], nv_param);
			nvram_set(nv_param, t->value);
		}
	}

	/* Overwrite some specific nvram settings */
	sprintf(nv_param, "wl%d_ure", wl_unit);
	nvram_set(nv_param, "1");

	sprintf(nv_param, "wl%d_vifs", wl_unit);
	sprintf(nv_value, "wl%d.1", wl_unit);
	nvram_set(nv_param, nv_value);

	sprintf(nv_param, "wl%d.1_unit", wl_unit);
	sprintf(nv_value, "%d.1", wl_unit);
	nvram_set(nv_param, nv_value);

	nvram_set("wl_ampdu_rr_rtylimit_tid", "3 3 3 3 3 3 3 3");
	nvram_set("wl_ampdu_rtylimit_tid", "7 7 7 7 7 7 7 7");
	sprintf(nv_param, "wl%d_ampdu_rr_rtylimit_tid", wl_unit);
	nvram_set(nv_param, "3 3 3 3 3 3 3 3");
	sprintf(nv_param, "wl%d_ampdu_rtylimit_tid", wl_unit);
	nvram_set(nv_param, "7 7 7 7 7 7 7 7");

	if (nvram_match("router_disable", "1"))
		travel_router = 0;
	else
		travel_router = 1;

	/* Set the wl modes for the primary wireless adapter
	 * and it's virtual interface
	 */
	sprintf(nv_param, "wl%d_mode", wl_unit);
	if (travel_router == 1) {
		nvram_set(nv_param, "sta");
		nvram_set("wl_mode", "sta");
	}
	else {
		nvram_set(nv_param, "wet");
		nvram_set("wl_mode", "wet");
	}

	sprintf(nv_param, "wl%d.1_mode", wl_unit);
	nvram_set(nv_param, "ap");

	if (travel_router == 1) {
		/* For URE with routing (Travel Router) we're using the STA part
		 * of our URE enabled radio as our WAN connection. So, we need to
		 * remove this interface from the list of bridged lan interfaces
		 * and set it up as the WAN device.
		 */
		temp = nvram_safe_get(lan_ifnames);
		strncpy(interface_list, temp, interface_list_size);

		/* Leverage build_ifnames() to get OS-dependent interface name.
		 * One white space is appended after build_ifnames(); need to
		 * remove it.
		 */
		sprintf(nv_interface, "wl%d", wl_unit);
		memset(os_interface, 0, os_interface_size);
		build_ifnames(nv_interface, os_interface, &os_interface_size);
		if (strlen(os_interface) > 1) {
			os_interface[strlen(os_interface) - 1] = '\0';
		}
		remove_from_list(os_interface, interface_list, interface_list_size);
		nvram_set(lan_ifnames, interface_list);

		/* Now remove the existing WAN interface from "wan_ifnames" */
		temp = nvram_safe_get(wan_ifnames);
		strncpy(interface_list, temp, interface_list_size);

		temp = nvram_safe_get(wan0_ifname);
		if (strlen(temp) != 0) {
			/* Stash this interface name in an nvram variable in case
			 * we need to restore this interface as the WAN interface
			 * when URE is disabled.
			 */
			nvram_set("old_wan0_ifname", temp);
			remove_from_list(temp, interface_list, interface_list_size);
		}

		/* Set the new WAN interface as the pimary WAN interface and add to
		 * the list wan_ifnames.
		 */
		nvram_set(wan0_ifname, os_interface);
		add_to_list(os_interface, interface_list, interface_list_size);
		nvram_set(wan_ifnames, interface_list);

		/* Now add the AP to the list of bridged lan interfaces */
		temp = nvram_safe_get(lan_ifnames);
		strncpy(interface_list, temp, interface_list_size);
		sprintf(nv_interface, "wl%d.1", wl_unit);

		/* Virtual interfaces that appear in NVRAM lists are ALWAYS stored
		 * as the NVRAM_FORM so we can add to list without translating.
		 */
		add_to_list(nv_interface, interface_list, interface_list_size);
		nvram_set(lan_ifnames, interface_list);
	}
	else {
		/* For URE without routing (Range Extender) we're using the STA
		 * as a WET device to connect to the "upstream" AP. We need to
		 * add our virtual interface(AP) to the bridged lan.
		 */
		temp = nvram_safe_get(lan_ifnames);
		strncpy(interface_list, temp, interface_list_size);

		sprintf(nv_interface, "wl%d.1", wl_unit);
		add_to_list(nv_interface, interface_list, interface_list_size);
		nvram_set(lan_ifnames, interface_list);
	}

	/* Make lan1_ifname, lan1_ifnames empty so that br1 is not created in URE mode. */
	nvram_set("lan1_ifname", "");
	nvram_set("lan1_ifnames", "");

	if (nvram_match("wl0_ure_mode", "wre")) {
		/* By default, wl0 bss is disabled in WRE mode */
		nvram_set("wl_bss_enabled", "0");
		nvram_set("wl0_bss_enabled", "0");
	}
}
#endif /* __CONFIG_URE__ */

static void
ses_cl_cleanup(void)
{
	/* well known event to cleanly initialize state machine */
	nvram_set("ses_cl_event", "0");
}

#ifdef __CONFIG_NAT__
static void
auto_bridge(void)
{

	struct nvram_tuple generic[] = {
		{ "lan_ifname", "br0", 0 },
		{ "lan_ifnames", "eth0 eth2 eth3 eth4", 0 },
		{ "wan_ifname", "eth1", 0 },
		{ "wan_ifnames", "eth1", 0 },
		{ 0, 0, 0 }
	};
#ifdef __CONFIG_VLAN__
	struct nvram_tuple vlan[] = {
		{ "lan_ifname", "br0", 0 },
		{ "lan_ifnames", "vlan0 eth1 eth2 eth3", 0 },
		{ "wan_ifname", "vlan1", 0 },
		{ "wan_ifnames", "vlan1", 0 },
		{ 0, 0, 0 }
	};
#endif	/* __CONFIG_VLAN__ */
	struct nvram_tuple dyna[] = {
		{ "lan_ifname", "br0", 0 },
		{ "lan_ifnames", "", 0 },
		{ "wan_ifname", "", 0 },
		{ "wan_ifnames", "", 0 },
		{ 0, 0, 0 }
	};
	struct nvram_tuple generic_auto_bridge[] = {
		{ "lan_ifname", "br0", 0 },
		{ "lan_ifnames", "eth0 eth1 eth2 eth3 eth4", 0 },
		{ "wan_ifname", "", 0 },
		{ "wan_ifnames", "", 0 },
		{ 0, 0, 0 }
	};
#ifdef __CONFIG_VLAN__
	struct nvram_tuple vlan_auto_bridge[] = {
		{ "lan_ifname", "br0", 0 },
		{ "lan_ifnames", "vlan0 vlan1 eth1 eth2 eth3", 0 },
		{ "wan_ifname", "", 0 },
		{ "wan_ifnames", "", 0 },
		{ 0, 0, 0 }
	};
#endif	/* __CONFIG_VLAN__ */

	struct nvram_tuple dyna_auto_bridge[] = {
		{ "lan_ifname", "br0", 0 },
		{ "lan_ifnames", "", 0 },
		{ "wan_ifname", "", 0 },
		{ "wan_ifnames", "", 0 },
		{ 0, 0, 0 }
	};

	struct nvram_tuple *linux_overrides;
	struct nvram_tuple *t, *u;
	int auto_bridge = 0, i;
#ifdef __CONFIG_VLAN__
	uint boardflags;
#endif	/* __CONFIG_VLAN_ */
	char *landevs, *wandevs;
	char lan_ifnames[128], wan_ifnames[128];
	char dyna_auto_ifnames[128];
	char wan_ifname[32], *next;
	int len;
	int ap = 0;

	printf(" INFO : enter function auto_bridge()\n");

	if (!strcmp(nvram_safe_get("auto_bridge_action"), "1")) {
		auto_bridge = 1;
		cprintf("INFO: Start auto bridge...\n");
	} else {
		nvram_set("router_disable_auto", "0");
		cprintf("INFO: Start non auto_bridge...\n");
	}

	/* Delete dynamically generated variables */
	if (auto_bridge) {
		char tmp[100], prefix[] = "wlXXXXXXXXXX_";
		for (i = 0; i < MAX_NVPARSE; i++) {

			del_filter_client(i);
			del_forward_port(i);
#if !defined(AUTOFW_PORT_DEPRECATED)
			del_autofw_port(i);
#endif

			snprintf(prefix, sizeof(prefix), "wan%d_", i);
			for (t = router_defaults; t->name; t ++) {
				if (!strncmp(t->name, "wan_", 4))
					nvram_unset(strcat_r(prefix, &t->name[4], tmp));
			}
		}
	}

	/*
	 * Build bridged i/f name list and wan i/f name list from lan device name list
	 * and wan device name list. Both lan device list "landevs" and wan device list
	 * "wandevs" must exist in order to preceed.
	 */
	if ((landevs = nvram_get("landevs")) && (wandevs = nvram_get("wandevs"))) {
		/* build bridged i/f list based on nvram variable "landevs" */
		len = sizeof(lan_ifnames);
		if (!build_ifnames(landevs, lan_ifnames, &len) && len)
			dyna[1].value = lan_ifnames;
		else
			goto canned_config;
		/* build wan i/f list based on nvram variable "wandevs" */
		len = sizeof(wan_ifnames);
		if (!build_ifnames(wandevs, wan_ifnames, &len) && len) {
			dyna[3].value = wan_ifnames;
			foreach(wan_ifname, wan_ifnames, next) {
				dyna[2].value = wan_ifname;
				break;
			}
		}
		else
			ap = 1;

		if (auto_bridge)
		{
			printf("INFO: lan_ifnames=%s\n", lan_ifnames);
			printf("INFO: wan_ifnames=%s\n", wan_ifnames);
			sprintf(dyna_auto_ifnames, "%s %s", lan_ifnames, wan_ifnames);
			printf("INFO: dyna_auto_ifnames=%s\n", dyna_auto_ifnames);
			dyna_auto_bridge[1].value = dyna_auto_ifnames;
			linux_overrides = dyna_auto_bridge;
			printf("INFO: linux_overrides=dyna_auto_bridge \n");
		}
		else
		{
			linux_overrides = dyna;
			printf("INFO: linux_overrides=dyna \n");
		}

	}
	/* override lan i/f name list and wan i/f name list with default values */
	else {
canned_config:
#ifdef __CONFIG_VLAN__
		boardflags = strtoul(nvram_safe_get("boardflags"), NULL, 0);
		if (boardflags & BFL_ENETVLAN) {
			if (auto_bridge)
			{
				linux_overrides = vlan_auto_bridge;
				printf("INFO: linux_overrides=vlan_auto_bridge \n");
			}
			else
			{
				linux_overrides = vlan;
				printf("INFO: linux_overrides=vlan \n");
			}
		} else {
#endif	/* __CONFIG_VLAN__ */
			if (auto_bridge)
			{
				linux_overrides = generic_auto_bridge;
				printf("INFO: linux_overrides=generic_auto_bridge \n");
			}
			else
			{
				linux_overrides = generic;
				printf("INFO: linux_overrides=generic \n");
			}
#ifdef __CONFIG_VLAN__
		}
#endif	/* __CONFIG_VLAN__ */
	}

		for (u = linux_overrides; u && u->name; u++) {
			nvram_set(u->name, u->value);
			printf("INFO: action nvram_set %s, %s\n", u->name, u->value);
			}

	/* Force to AP */
	if (ap)
		nvram_set("router_disable", "1");

	if (auto_bridge) {
		printf("INFO: reset auto_bridge flag.\n");
		nvram_set("auto_bridge_action", "0");
	}

	nvram_commit();
	cprintf("auto_bridge done\n");
}

#endif	/* __CONFIG_NAT__ */


static void
upgrade_defaults(void)
{
	char temp[100];
	int i;
	bool bss_enabled = TRUE;
	char *val;

	/* Check whether upgrade is required or not
	 * If lan1_ifnames is not found in NVRAM , upgrade is required.
	 */
	if (!nvram_get("lan1_ifnames") && !RESTORE_DEFAULTS()) {
		cprintf("NVRAM upgrade required.  Starting.\n");

		if (nvram_match("ure_disable", "1")) {
			nvram_set("lan1_ifname", "br1");
			nvram_set("lan1_ifnames", "wl0.1 wl0.2 wl0.3 wl1.1 wl1.2 wl1.3");
		}
		else {
			nvram_set("lan1_ifname", "");
			nvram_set("lan1_ifnames", "");
			for (i = 0; i < 2; i++) {
				snprintf(temp, sizeof(temp), "wl%d_ure", i);
				if (nvram_match(temp, "1")) {
					snprintf(temp, sizeof(temp), "wl%d.1_bss_enabled", i);
					nvram_set(temp, "1");
				}
				else {
					bss_enabled = FALSE;
					snprintf(temp, sizeof(temp), "wl%d.1_bss_enabled", i);
					nvram_set(temp, "0");
				}
			}
		}
		if (nvram_get("lan1_ipaddr")) {
			nvram_set("lan1_gateway", nvram_get("lan1_ipaddr"));
		}

		for (i = 0; i < 2; i++) {
			snprintf(temp, sizeof(temp), "wl%d_bss_enabled", i);
			nvram_set(temp, "1");
			snprintf(temp, sizeof(temp), "wl%d.1_guest", i);
			if (nvram_match(temp, "1")) {
				nvram_unset(temp);
				if (bss_enabled) {
					snprintf(temp, sizeof(temp), "wl%d.1_bss_enabled", i);
					nvram_set(temp, "1");
				}
			}

			snprintf(temp, sizeof(temp), "wl%d.1_net_reauth", i);
			val = nvram_get(temp);
			if (!val || (*val == 0))
				nvram_set(temp, nvram_default_get(temp));

			snprintf(temp, sizeof(temp), "wl%d.1_wpa_gtk_rekey", i);
			val = nvram_get(temp);
			if (!val || (*val == 0))
				nvram_set(temp, nvram_default_get(temp));
		}

		nvram_commit();

		cprintf("NVRAM upgrade complete.\n");
	}
}

#ifdef LINUX_2_6_36
/* Override the "0 5u" to "0 5" to backward compatible with old image */
static void
fa_override_vlan2ports()
{
	char port[] = "XXXX", *nvalue;
	char *next, *cur, *ports, *u;
	int len;

	ports = nvram_get("vlan2ports");
	nvalue = malloc(strlen(ports) + 2);
	if (!nvalue) {
		cprintf("Memory allocate failed!\n");
		return;
	}
	memset(nvalue, 0, strlen(ports) + 2);

	/* search last port include 'u' */
	for (cur = ports; cur; cur = next) {
		/* tokenize the port list */
		while (*cur == ' ')
			cur ++;
		next = strstr(cur, " ");
		len = next ? next - cur : strlen(cur);
		if (!len)
			break;
		if (len > sizeof(port) - 1)
			len = sizeof(port) - 1;
		strncpy(port, cur, len);
		port[len] = 0;

		/* prepare new value */
		if ((u = strchr(port, 'u')))
			*u = '\0';
		strcat(nvalue, port);
		strcat(nvalue, " ");
	}

	/* Remove last " " */
	len = strlen(nvalue);
	if (len) {
		nvalue[len-1] = '\0';
		nvram_set("vlan2ports", nvalue);
	}
	free(nvalue);
}

static void
fa_nvram_adjust()
{
	FILE *fp;
	int fa_mode;
	bool reboot = FALSE;

	if (RESTORE_DEFAULTS())
		return;

	fa_mode = atoi(nvram_safe_get("ctf_fa_mode"));
	switch (fa_mode) {
		case CTF_FA_BYPASS:
		case CTF_FA_NORMAL:
			break;
		default:
			fa_mode = CTF_FA_DISABLED;
			break;
	}

	if ((fp = fopen("/proc/fa", "r"))) {
		/* FA is capable */
		fclose(fp);

		if (FA_ON(fa_mode)) {
			char wan_ifnames[128];
			char wan_ifname[32], *next;
			int len, ret;

			cprintf("\nFA on.\n");

			/* Set et2macaddr, et2phyaddr as same as et0macaddr, et0phyaddr */
			if (!nvram_get("vlan2ports") || !nvram_get("wandevs"))  {
				cprintf("Insufficient envram, cannot do FA override\n");
				return;
			}

			/* adjusted */
			if (!strcmp(nvram_get("wandevs"), "vlan2") &&
			    !strchr(nvram_get("vlan2ports"), 'u'))
			    return;

			/* The vlan2ports will be change to "0 8u" dynamically by
			 * robo_fa_imp_port_upd. Keep nvram vlan2ports unchange.
			 */
			fa_override_vlan2ports();

			/* Override wandevs to "vlan2" */
			nvram_set("wandevs", "vlan2");
			/* build wan i/f list based on def nvram variable "wandevs" */
			len = sizeof(wan_ifnames);
			ret = build_ifnames("vlan2", wan_ifnames, &len);
			if (!ret && len) {
				/* automatically configure wan0_ too */
				nvram_set("wan_ifnames", wan_ifnames);
				nvram_set("wan0_ifnames", wan_ifnames);
				foreach(wan_ifname, wan_ifnames, next) {
					nvram_set("wan_ifname", wan_ifname);
					nvram_set("wan0_ifname", wan_ifname);
					break;
				}
			}
			cprintf("Override FA nvram...\n");
			reboot = TRUE;
		}
		else {
			cprintf("\nFA off.\n");
		}
	}
	else {
		/* FA is not capable */
		if (FA_ON(fa_mode)) {
			nvram_unset("ctf_fa_mode");
			cprintf("FA not supported...\n");
			reboot = TRUE;
		}
	}

	if (reboot) {
		nvram_commit();
		cprintf("FA nvram overridden, rebooting...\n");
		kill(1, SIGTERM);
	}
}

#define GMAC3_ENVRAM_BACKUP(name)				\
do {								\
	char *var, bvar[NVRAM_MAX_PARAM_LEN];			\
	if ((var = nvram_get(name)) != NULL) {			\
		snprintf(bvar, sizeof(bvar), "old_%s", name);	\
		nvram_set(bvar, var);				\
	}							\
} while (0)

/* Override GAMC3 nvram */
static void
gmac3_override_nvram()
{
	char var[32], *lists, *next;
	char newlists[NVRAM_MAX_PARAM_LEN];

	/* back up old embedded nvram */
	GMAC3_ENVRAM_BACKUP("et0macaddr");
	GMAC3_ENVRAM_BACKUP("et1macaddr");
	GMAC3_ENVRAM_BACKUP("et2macaddr");
	GMAC3_ENVRAM_BACKUP("et0mdcport");
	GMAC3_ENVRAM_BACKUP("et1mdcport");
	GMAC3_ENVRAM_BACKUP("et2mdcport");
	GMAC3_ENVRAM_BACKUP("et0phyaddr");
	GMAC3_ENVRAM_BACKUP("et1phyaddr");
	GMAC3_ENVRAM_BACKUP("et2phyaddr");
	GMAC3_ENVRAM_BACKUP("vlan1ports");
	GMAC3_ENVRAM_BACKUP("vlan2ports");
	GMAC3_ENVRAM_BACKUP("vlan1hwname");
	GMAC3_ENVRAM_BACKUP("vlan2hwname");
	GMAC3_ENVRAM_BACKUP("wandevs");

	/* change mac, mdcport, phyaddr */
	nvram_set("et2macaddr", nvram_get("et0macaddr"));
	nvram_set("et2mdcport", nvram_get("et0mdcport"));
	nvram_set("et2phyaddr", nvram_get("et0phyaddr"));
	nvram_set("et1mdcport", nvram_get("et0mdcport"));
	nvram_set("et1phyaddr", nvram_get("et0phyaddr"));
	nvram_set("et0macaddr", "00:00:00:00:00:00");
	nvram_set("et1macaddr", "00:00:00:00:00:00");

	/* change vlan ports */
	if (!(lists = nvram_get("vlan1ports"))) {
		cprintf("Default vlan1ports is not specified, override GMAC3 defaults...\n");
		nvram_set("vlan1ports", "1 2 3 4 5 7 8*");
	} else {
		strncpy(newlists, lists, sizeof(newlists));
		newlists[sizeof(newlists)-1] = '\0';

		/* search first port include '*' or 'u' and remove it */
		foreach(var, lists, next) {
			if (strchr(var, '*') || strchr(var, 'u')) {
				remove_from_list(var, newlists, sizeof(newlists));
				break;
			}
		}

		/* add port 5, 7 and 8* */
		add_to_list("5", newlists, sizeof(newlists));
		add_to_list("7", newlists, sizeof(newlists));
		add_to_list("8*", newlists, sizeof(newlists));
		nvram_set("vlan1ports", newlists);
	}

	if (!(lists = nvram_get("vlan2ports"))) {
		cprintf("Default vlan2ports is not specified, override GMAC3 defaults...\n");
		nvram_set("vlan2ports", "0 8u");
	} else {
		strncpy(newlists, lists, sizeof(newlists));
		newlists[sizeof(newlists)-1] = '\0';

		/* search first port include '*' or 'u' and remove it */
		foreach(var, lists, next) {
			if (strchr(var, '*') || strchr(var, 'u')) {
				remove_from_list(var, newlists, sizeof(newlists));
				break;
			}
		}

		/* add port 8u */
		add_to_list("8u", newlists, sizeof(newlists));
		nvram_set("vlan2ports", newlists);
	}

	/* change wandevs vlan hw name */
	nvram_set("wandevs", "et2");
	nvram_set("vlan1hwname", "et2");
	nvram_set("vlan2hwname", "et2");

	/* landevs should be have wl1 wl2 */

	/* set fwd_wlandevs from lan_ifnames */
	if (!(lists = nvram_get("lan_ifnames"))) {
		/* should not be happened */
		cprintf("lan_ifnames is not exist, override GMAC3 defaults...\n");
		nvram_set("fwd_wlandevs", "eth1 eth2 eth3");
	} else {
		strncpy(newlists, lists, sizeof(newlists));
		newlists[sizeof(newlists)-1] = '\0';

		/* remove ifname if it's not a wireless interrface */
		foreach(var, lists, next) {
			if (wl_probe(var)) {
				remove_from_list(var, newlists, sizeof(newlists));
				continue;
			}
		}
		nvram_set("fwd_wlandevs", newlists);
	}

	/* set fwddevs */
	nvram_set("fwddevs", "fwd0 fwd1");
}

#define GMAC3_ENVRAM_RESTORE(name)				\
do {								\
	char *var, bvar[NVRAM_MAX_PARAM_LEN];			\
	snprintf(bvar, sizeof(bvar), "old_%s", name);		\
	if ((var = nvram_get(bvar))) {				\
		nvram_set(name, var);				\
		nvram_unset(bvar);				\
	}							\
	else {							\
		nvram_unset(name);				\
	}							\
} while (0)

static void
gmac3_restore_nvram()
{
	/* back up old embedded nvram */
	GMAC3_ENVRAM_RESTORE("et0macaddr");
	GMAC3_ENVRAM_RESTORE("et1macaddr");
	GMAC3_ENVRAM_RESTORE("et2macaddr");
	GMAC3_ENVRAM_RESTORE("et0mdcport");
	GMAC3_ENVRAM_RESTORE("et1mdcport");
	GMAC3_ENVRAM_RESTORE("et2mdcport");
	GMAC3_ENVRAM_RESTORE("et0phyaddr");
	GMAC3_ENVRAM_RESTORE("et1phyaddr");
	GMAC3_ENVRAM_RESTORE("et2phyaddr");
	GMAC3_ENVRAM_RESTORE("vlan1ports");
	GMAC3_ENVRAM_RESTORE("vlan2ports");
	GMAC3_ENVRAM_RESTORE("vlan1hwname");
	GMAC3_ENVRAM_RESTORE("vlan2hwname");
	GMAC3_ENVRAM_RESTORE("wandevs");

	nvram_unset("fwd_wlandevs");
	nvram_unset("fwddevs");
}

static void
gmac3_restore_defaults()
{
	/* nvram variables will be changed when gmac3_enable */
	if (!strcmp(nvram_safe_get("wandevs"), "et2") &&
	    nvram_get("fwd_wlandevs") && nvram_get("fwddevs")) {
		gmac3_restore_nvram();

		/* Handle the case of user disable GMAC3 and do restore defaults. */
		if (nvram_invmatch("gmac3_enable", "1"))
			nvram_set("gmac3_reboot", "1");
	}
}

static void
gmac3_nvram_adjust()
{
	int fa_mode;
	bool reboot = FALSE;
	bool gmac3_enable, gmac3_configured = FALSE;

	/* gmac3_enable nvram control everything */
	gmac3_enable = nvram_match("gmac3_enable", "1") ? TRUE : FALSE;

	/* nvram variables will be changed when gmac3_enable */
	if (!strcmp(nvram_safe_get("wandevs"), "et2") &&
	    nvram_get("fwd_wlandevs") &&
	    nvram_get("fwddevs"))
		gmac3_configured = TRUE;

	fa_mode = atoi(nvram_safe_get("ctf_fa_mode"));
	if (fa_mode == CTF_FA_NORMAL || fa_mode == CTF_FA_BYPASS) {
		cprintf("GMAC3 based forwarding not compatible with ETFA - AuX port...\n");
		return;
	}

	if (et_capable(NULL, "gmac3")) {
		if (gmac3_enable) {
			cprintf("\nGMAC3 on.\n");

			if (gmac3_configured)
				return;

			/* The vlan2ports will be change to "0 8u" dynamically by
			 * robo_fa_imp_port_upd. Keep nvram vlan2ports unchange.
			 */
			gmac3_override_nvram();

			cprintf("Override GMAC3 nvram...\n");
			reboot = TRUE;
		} else {
			cprintf("\nGMAC3 off.\n");
			if (gmac3_configured) {
				gmac3_restore_nvram();
				reboot = TRUE;
			}
		}
	} else {
		cprintf("GMAC3 not supported...\n");

		if (gmac3_enable) {
			nvram_unset("gmac3_enable");
			reboot = TRUE;
		}

		/* GMAC3 is not capable */
		if (gmac3_configured) {
			gmac3_restore_nvram();
			reboot = TRUE;
		}
	}

	if (nvram_get("gmac3_reboot")) {
		nvram_unset("gmac3_reboot");
		reboot = TRUE;
	}

	if (reboot) {
		nvram_commit();
		cprintf("GMAC3 nvram overridden, rebooting...\n");
		kill(1, SIGTERM);
	}
}
#endif /* LINUX_2_6_36 */

#ifdef __CONFIG_FAILSAFE_UPGRADE_SUPPORT__
static void
failsafe_nvram_adjust(void)
{
	FILE *fp;
	char dev[PATH_MAX];
	bool found = FALSE, need_commit = FALSE;
	int i, partialboots;

	partialboots = atoi(nvram_safe_get(PARTIALBOOTS));
	if (partialboots != 0) {
		nvram_set(PARTIALBOOTS, "0");
		need_commit = TRUE;
	}

	if (nvram_get(BOOTPARTITION) != NULL) {
		/* get mtdblock device */
		if (!(fp = fopen("/proc/mtd", "r"))) {
			cprintf("Can't open /proc/mtd\n");
		} else {
			while (fgets(dev, sizeof(dev), fp)) {
				if (sscanf(dev, "mtd%d:", &i) && strstr(dev, LINUX_SECOND)) {
					found = TRUE;
					break;
				}
			}
			fclose(fp);

			/* The BOOTPARTITON was set, but linux2 partition can't be created.
			 * So unset BOOTPARTITION.
			 */
			if (found == FALSE) {
				cprintf("Unset bootpartition due to no linux2 partition found.\n");
				nvram_unset(BOOTPARTITION);
				need_commit = TRUE;
			}
		}
	}

	if (need_commit == TRUE)
		nvram_commit();
}
#endif /* __CONFIG_FAILSAFE_UPGRADE_SUPPORT__ */

static void
restore_defaults(void)
{
	struct nvram_tuple generic[] = {
		{ "lan_ifname", "br0", 0 },
		{ "lan_ifnames", "eth0 eth2 eth3 eth4", 0 },
		{ "wan_ifname", "eth1", 0 },
		{ "wan_ifnames", "eth1", 0 },
		{ "lan1_ifname", "br1", 0 },
		{ "lan1_ifnames", "wl0.1 wl0.2 wl0.3 wl1.1 wl1.2 wl1.3", 0 },
		{ 0, 0, 0 }
	};
#ifdef __CONFIG_VLAN__
	struct nvram_tuple vlan[] = {
		{ "lan_ifname", "br0", 0 },
		{ "lan_ifnames", "vlan0 eth1 eth2 eth3", 0 },
		{ "wan_ifname", "vlan1", 0 },
		{ "wan_ifnames", "vlan1", 0 },
		{ "lan1_ifname", "br1", 0 },
		{ "lan1_ifnames", "wl0.1 wl0.2 wl0.3 wl1.1 wl1.2 wl1.3", 0 },
		{ 0, 0, 0 }
	};
#endif	/* __CONFIG_VLAN__ */
	struct nvram_tuple dyna[] = {
		{ "lan_ifname", "br0", 0 },
		{ "lan_ifnames", "", 0 },
		{ "wan_ifname", "", 0 },
		{ "wan_ifnames", "", 0 },
		{ "lan1_ifname", "br1", 0 },
		{ "lan1_ifnames", "wl0.1 wl0.2 wl0.3 wl1.1 wl1.2 wl1.3", 0 },
		{ 0, 0, 0 }
	};

	struct nvram_tuple sta_mode[] = {
		{ "lan_ifname", "", 0 },
		{ "lan_ifnames", "", 0 },
		{ "wan_ifname", "", 0 },
		{ "wan_ifnames", "", 0 },
		{ "lan1_ifname", "", 0 },
		{ "lan1_ifnames", "wl0.1 wl0.2 wl0.3 wl1.1 wl1.2 wl1.3", 0 },
		{ 0, 0, 0 }
	};

	struct nvram_tuple psta_mode[] = {
		{ "lan_ifname", "br0", 0 },
		{ "lan_ifnames", "", 0 },
		{ "wan_ifname", "", 0 },
		{ "wan_ifnames", "", 0 },
		{ "wl_mode", "psta", 0 },
		{ "lan_dhcp", "0", 0},
		{ "router_disable", "0", 0 },
		{ 0, 0, 0 }
	};

	struct nvram_tuple psr_mode[] = {
		{ "lan_ifname", "br0", 0 },
		{ "lan_ifnames", "", 0 },
		{ "wan_ifname", "", 0 },
		{ "wan_ifnames", "", 0 },
		{ "wl_mode", "psr", 0 },
		{ "lan_dhcp", "0", 0 },
		{ "router_disable", "0", 0 },
		{ 0, 0, 0 }
	};

	struct nvram_tuple *linux_overrides = generic;
	struct nvram_tuple *t, *u;
	int restore_defaults, i;
#ifdef __CONFIG_VLAN__
	uint boardflags;
#endif	/* __CONFIG_VLAN_ */
	char *landevs, *wandevs;
	char lan_ifnames[128], wan_ifnames[128];
	char wan_ifname[32], *next;
	char lan_ifname[32];
	char lan1_ifname[32];
	int len;
	int ap = 0;
	int j;

	/* Restore defaults if told to or OS has changed */
	restore_defaults = RESTORE_DEFAULTS();

	if (restore_defaults) {
		cprintf("Restoring defaults...");
	}

	/* Delete dynamically generated variables */
	if (restore_defaults) {
		char tmp[100], prefix[] = "wlXXXXXXXXXX_";
		for (i = 0; i < MAX_NVPARSE; i++) {
#ifdef __CONFIG_NAT__
			del_filter_client(i);
			del_forward_port(i);
#if !defined(AUTOFW_PORT_DEPRECATED)
			del_autofw_port(i);
#endif
#endif	/* __CONFIG_NAT__ */
			snprintf(prefix, sizeof(prefix), "wl%d_", i);
			for (t = router_defaults; t->name; t ++) {
				if (!strncmp(t->name, "wl_", 3))
					nvram_unset(strcat_r(prefix, &t->name[3], tmp));
			}
#ifdef __CONFIG_NAT__
			snprintf(prefix, sizeof(prefix), "wan%d_", i);
			for (t = router_defaults; t->name; t ++) {
				if (!strncmp(t->name, "wan_", 4))
					nvram_unset(strcat_r(prefix, &t->name[4], tmp));
			}
#endif	/* __CONFIG_NAT__ */

			/* Delete dynamically generated NVRAMs for Traffic Mgmt & DWM */
			snprintf(prefix, sizeof(prefix), "wl%d_", i);
			for (j = 0; j < MAX_NUM_TRF_MGMT_RULES; j++) {
				del_trf_mgmt_port(prefix, j);
			}
			for (j = 0; j < MAX_NUM_TRF_MGMT_DWM_RULES; j++) {
				del_trf_mgmt_dwm(prefix, j);
			}

#ifdef __CONFIG_DHDAP__
			/* Delete dynamically generated NVRAMs for DHDAP */
			snprintf(tmp, sizeof(tmp), "wl%d_cfg_maxassoc", i);
			nvram_unset(tmp);
#endif
		}
		ses_restore_defaults();
#ifdef __CONFIG_WPS__
		wps_restore_defaults();
#endif /* __CONFIG_WPS__ */
#ifdef __CONFIG_WAPI_IAS__
		nvram_unset("as_mode");
#endif /* __CONFIG_WAPI_IAS__ */
		virtual_radio_restore_defaults();
#ifdef __CONFIG_URE__
		if (nvram_match("wl0_ure_mode", "wre") ||
		    nvram_match("wl0_ure_mode", "ure")) {
			ure_restore_defaults(0);
		}
#endif /* __CONFIG_URE__ */
#ifdef LINUX_2_6_36
		/* Delete dynamically generated variables */
		gmac3_restore_defaults();
#endif
	}

	/*
	 * Build bridged i/f name list and wan i/f name list from lan device name list
	 * and wan device name list. Both lan device list "landevs" and wan device list
	 * "wandevs" must exist in order to preceed.
	 */

	if (!strcmp(nvram_safe_get("devicemode"), "sta")) {
		int val;
		int lan_if = 0;
		if ((landevs = nvram_get("landevs"))) {
			/* Build the OS interface names */
			len = sizeof(lan_ifnames);
			if (!build_ifnames(landevs, lan_ifnames, &len) && len)
				sta_mode[1].value = lan_ifnames;
			else
				goto canned_config;
			foreach(lan_ifname, lan_ifnames, next) {
				/* Looking for primary wl interface */
				if (!wl_ioctl(lan_ifname, WLC_GET_MAGIC, &val, sizeof(val))) {
					sta_mode[0].value = lan_ifname;
					break;
				}
			}
			foreach(lan1_ifname, lan_ifnames, next) {
				/* Looking for secondary wl interface if any */
				if (!wl_ioctl(lan1_ifname, WLC_GET_MAGIC, &val, sizeof(val))) {
					/* Skip to the next WLAN interface if there is one */
					if (lan_if) {
						sta_mode[4].value = lan1_ifname;
						break;
					}
					lan_if++;
				}
			}
			linux_overrides = sta_mode;
		} else
			goto canned_config;
	} else if (!strcmp(nvram_safe_get("devicemode"), "psta")) {
		/* Get the lan and wan if names to configure */
		/* Note: In the event of misconfiguration, we still need to reach the router
		 * via HTTP or Telnet. Hence, check for landevs "or" wandevs.
		 */
		if ((landevs = nvram_get("landevs"))) {
			/* build bridged i/f list based on nvram variable "landevs" */
			len = sizeof(lan_ifnames);
			if (!build_ifnames(landevs, lan_ifnames, &len) && len)
				psta_mode[1].value = lan_ifnames;
			else
				goto canned_config;
		}

		/* build wan i/f list based on nvram variable "wandevs" */
		if ((wandevs = nvram_get("wandevs"))) {
			len = sizeof(wan_ifnames);
			if (!build_ifnames(wandevs, wan_ifnames, &len) && len) {
				psta_mode[3].value = wan_ifnames;
				foreach(wan_ifname, wan_ifnames, next) {
					psta_mode[2].value = wan_ifname;
					break;
				}
			}
		}
		if (landevs || wandevs) {
			linux_overrides = psta_mode;
		} else
			goto canned_config;
	} else if (!strcmp(nvram_safe_get("devicemode"), "psr")) {
		/* Get the lan and wan ifnames to configure */
		/* Note: In the event of misconfiguration, we still need to reach the router
		 * via HTTP or Telnet. Hence, check for landevs "or" wandevs.
		 */
		if ((landevs = nvram_get("landevs"))) {
			/* build bridged i/f list based on nvram variable "landevs" */
			len = sizeof(lan_ifnames);
			if (!build_ifnames(landevs, lan_ifnames, &len) && len) {
				char nv_interface[NVRAM_MAX_PARAM_LEN];
				int wl_unit = 0;
				sprintf(nv_interface, "wl%d.1", wl_unit);
				add_to_list(nv_interface, lan_ifnames, sizeof(lan_ifnames));
				psr_mode[1].value = lan_ifnames;
			} else
				goto canned_config;
		}

		if ((wandevs = nvram_get("wandevs"))) {
			/* build wan i/f list based on nvram variable "wandevs" */
			len = sizeof(wan_ifnames);
			if (!build_ifnames(wandevs, wan_ifnames, &len) && len) {
				psr_mode[3].value = wan_ifnames;
				foreach(wan_ifname, wan_ifnames, next) {
					psr_mode[2].value = wan_ifname;
					break;
				}
			}
		} else
			ap = 1;
		if (landevs || wandevs) {
			linux_overrides = psr_mode;
		} else
			goto canned_config;
	} else if ((landevs = nvram_get("landevs")) && (wandevs = nvram_get("wandevs"))) {
		/* build bridged i/f list based on nvram variable "landevs" */
		len = sizeof(lan_ifnames);
		if (!build_ifnames(landevs, lan_ifnames, &len) && len)
			dyna[1].value = lan_ifnames;
		else
			goto canned_config;

		/* build wan i/f list based on nvram variable "wandevs" */
		len = sizeof(wan_ifnames);
		if (!build_ifnames(wandevs, wan_ifnames, &len) && len) {
			dyna[3].value = wan_ifnames;
			foreach(wan_ifname, wan_ifnames, next) {
				dyna[2].value = wan_ifname;
				break;
			}
		}
		else
			ap = 1;
		linux_overrides = dyna;
	}
	/* override lan i/f name list and wan i/f name list with default values */
	else {
canned_config:
#ifdef __CONFIG_VLAN__
		boardflags = strtoul(nvram_safe_get("boardflags"), NULL, 0);
		if (boardflags & BFL_ENETVLAN)
			linux_overrides = vlan;
		else
#endif	/* __CONFIG_VLAN__ */
			linux_overrides = generic;
	}

	/* Check if nvram version is set, but old */
	if (nvram_get("nvram_version")) {
		int old_ver, new_ver;

		old_ver = atoi(nvram_get("nvram_version"));
		new_ver = atoi(NVRAM_SOFTWARE_VERSION);
		if (old_ver < new_ver) {
			cprintf("NVRAM: Updating from %d to %d\n", old_ver, new_ver);
			nvram_set("nvram_version", NVRAM_SOFTWARE_VERSION);
		}
	}

	/* Restore defaults */
	for (t = router_defaults; t->name; t++) {
		if (restore_defaults || !nvram_get(t->name)) {
			for (u = linux_overrides; u && u->name; u++) {
				if (!strcmp(t->name, u->name)) {
					nvram_set(u->name, u->value);
					break;
				}
			}
			if (!u || !u->name) {
				nvram_set(t->name, nvram_default_get(t->name));
			}
		}
	}

	/* Force to AP */
	if (ap)
		nvram_set("router_disable", "1");

	/* Always set OS defaults */
	nvram_set("os_name", "linux");
	nvram_set("os_version", ROUTER_VERSION_STR);
	nvram_set("os_date", __DATE__);
	/* Always set WL driver version! */
	nvram_set("wl_version", EPI_VERSION_STR);

	nvram_set("is_modified", "0");
	nvram_set("ezc_version", EZC_VERSION_STR);

	if (restore_defaults) {
		FILE *fp;
		char memdata[256] = {0};
		uint memsize = 0;
		char pktq_thresh[8] = {0};
		char et_pktq_thresh[8] = {0};

		if ((fp = fopen("/proc/meminfo", "r")) != NULL) {
			/* get memory count in MemTotal = %d */
			while (fgets(memdata, 255, fp) != NULL) {
				if (strstr(memdata, "MemTotal") != NULL) {
					sscanf(memdata, "MemTotal:        %d kB", &memsize);
					break;
				}
			}
			fclose(fp);
		}

		if (memsize <= 32768) {
			/* Set to 512 as long as onboard memory <= 32M */
			sprintf(pktq_thresh, "512");
			sprintf(et_pktq_thresh, "512");
		}
		else if (memsize <= 65536) {
			/* We still have to set the thresh to prevent oom killer */
			sprintf(pktq_thresh, "1024");
			sprintf(et_pktq_thresh, "1536");
		}
		else {
			sprintf(pktq_thresh, "1024");
			/* Adjust the et thresh to 3300 as long as memory > 64M.
			 * The thresh value is based on benchmark test.
			 */
			sprintf(et_pktq_thresh, "3300");
		}

		nvram_set("wl_txq_thresh", pktq_thresh);
		nvram_set("et_txq_thresh", et_pktq_thresh);
#if defined(__CONFIG_USBAP__)
		nvram_set("wl_rpcq_rxthresh", pktq_thresh);
#endif
	}

#ifdef __CONFIG_URE__
	if (restore_defaults) {
		if (nvram_match("wl0_ure_mode", "ure") ||
		    nvram_match("wl0_ure_mode", "wre")) {
			nvram_set("ure_disable", "0");
			nvram_set("router_disable", "1");
			/* Set ure related nvram settings */
			set_ure_vars(0);
		}
	}
#endif /* __CONFIG_URE__ */

	if (restore_defaults) {
		/* We readjust once we start the wl driver */
		if (nvram_match("devicemode", "psr")) {
			nvram_set("wl0_mode", "ap");
			nvram_set("wl1_mode", "ap");
		}
	}

	/* Commit values */
	if (restore_defaults) {
		nvram_commit();
		cprintf("done\n");
	}
}

#ifdef __CONFIG_NAT__
static void
set_wan0_vars(void)
{
	int unit;
	char tmp[100], prefix[] = "wanXXXXXXXXXX_";

	/* check if there are any connections configured */
	for (unit = 0; unit < MAX_NVPARSE; unit ++) {
		snprintf(prefix, sizeof(prefix), "wan%d_", unit);
		if (nvram_get(strcat_r(prefix, "unit", tmp)))
			break;
	}
	/* automatically configure wan0_ if no connections found */
	if (unit >= MAX_NVPARSE) {
		struct nvram_tuple *t;
		char *v;

		/* Write through to wan0_ variable set */
		snprintf(prefix, sizeof(prefix), "wan%d_", 0);
		for (t = router_defaults; t->name; t ++) {
			if (!strncmp(t->name, "wan_", 4)) {
				if (nvram_get(strcat_r(prefix, &t->name[4], tmp)))
					continue;
				v = nvram_get(t->name);
				nvram_set(tmp, v ? v : t->value);
			}
		}
		nvram_set(strcat_r(prefix, "unit", tmp), "0");
		nvram_set(strcat_r(prefix, "desc", tmp), "Default Connection");
		nvram_set(strcat_r(prefix, "primary", tmp), "1");
	}
}
#endif	/* __CONFIG_NAT__ */

#if defined(LINUX26)

#define FS_JFFS2		0x1
#define FS_YAFFS2		0x2
#define FS_JFFS2_ENAB(x)	(x & FS_JFFS2)
#define FS_YAFFS2_ENAB(x)	(x & FS_YAFFS2)

#define JFFS2_MAGIC		0x1985

static int
nand_fs_check(uint32 *flag)
{
	FILE *fp;
	char buf[NAME_MAX];

	if (!(fp = fopen("/proc/filesystems", "r")))
		return -1;

	while (fgets(buf, sizeof(buf), fp) != NULL) {
		if (strstr(buf, "jffs2")) {
			*flag |= FS_JFFS2;
		} else if (strstr(buf, "yaffs2")) {
			*flag |= FS_YAFFS2;
		}
	}

	fclose(fp);

	return 0;
}

static int
read_jffs2_magic(uint32 flag, uint16 *magic)
{
	if (FS_YAFFS2_ENAB(flag)) {
		int fd;
		int readbytes;

		/* get oob free areas data */
		if ((fd = open("/proc/brcmnand", O_RDONLY)) < 0)
			return -1;

		readbytes = read(fd, (char*)magic, sizeof(*magic));

		close(fd);

		if (readbytes != sizeof(*magic)) {
			fprintf(stderr, "Read brcmnand oob failed, "
				"want %d but got %d bytes\n", sizeof(*magic), readbytes);
			return -1;
		}
	} else {
		*magic = JFFS2_MAGIC;
	}

	return 0;
}

static int
nand_fs_mtd_mount(void)
{
	FILE *fp;
	char *dpath = NULL, dev[PATH_MAX];
	int i, ret = -1;
	char *jpath = "/tmp/media/nand";
	uint32 fs_flag = 0;
	uint16 magic = 0;

	/* get mtdblock device */
	if (!(fp = fopen("/proc/mtd", "r")))
		return -1;

	while (fgets(dev, sizeof(dev), fp)) {
		if (sscanf(dev, "mtd%d:", &i) && strstr(dev, "brcmnand")) {
			snprintf(dev, sizeof(dev), "/dev/mtdblock%d", i);
			dpath = dev;
			break;
		}
	}
	fclose(fp);

	/* check if we have brcmnand partition */
	if (dpath == NULL)
		return -1;

	/* create mount directory */
	if (mkdir(jpath, 0777) != 0 && errno != EEXIST)
		return -1;

	/* check filesystem support for nand */
	if ((nand_fs_check(&fs_flag) != 0) || fs_flag == 0)
		return -1;

	if (read_jffs2_magic(fs_flag, &magic) != 0)
		return -1;

	if ((magic == JFFS2_MAGIC) && FS_JFFS2_ENAB(fs_flag)) {
		if (mount(dpath, jpath, "jffs2", 0, NULL)) {
			fprintf(stderr, "Mount nflash MTD jffs2 partition %s to %s failed\n",
				dpath, jpath);
			goto erase_and_mount;
		}

		return 0;
	}

	if (FS_YAFFS2_ENAB(fs_flag)) {
		if (mount(dpath, jpath, "yaffs2", 0, NULL)) {
			fprintf(stderr, "Mount nflash MTD yaffs2 partition %s to %s failed\n",
				dpath, jpath);
			goto erase_and_mount;
		}

		/* Force creation of YAFFS2 checkpoint (YAFFS2 run-time state snapshot) */
		sync();

		return 0;
	}

erase_and_mount:
	if ((ret = mtd_erase("brcmnand"))) {
		fprintf(stderr, "Erase nflash MTD partition %s failed %d\n", dpath, ret);
		return ret;
	}

	ret = -1;

	if (FS_YAFFS2_ENAB(fs_flag)) {
		if ((ret = mount(dpath, jpath, "yaffs2", 0, NULL)) != 0) {
			fprintf(stderr, "Mount nflash MTD yaffs2 partition %s to %s failed\n",
				dpath, jpath);
		} else {
			/* Force creation of YAFFS2 checkpoint (YAFFS2 run-time state snapshot) */
			sync();
		}
	}

	if (FS_JFFS2_ENAB(fs_flag) && ret != 0) {
		if ((ret = mount(dpath, jpath, "jffs2", 0, NULL)) != 0) {
			fprintf(stderr, "Mount nflash MTD jffs2 partition %s to %s failed\n",
				dpath, jpath);
		}
	}

	return ret;
}
#endif /* LINUX26 */

static int noconsole = 0;

static void
sysinit(void)
{
	char buf[PATH_MAX];
	struct utsname name;
	struct stat tmp_stat;
	time_t tm = 0;
	char *loglevel;

	struct utsname unamebuf;
	char *lx_rel;

	/* Use uname() to get the system's hostname */
	uname(&unamebuf);
	lx_rel = unamebuf.release;

	if (memcmp(lx_rel, "2.6", 3) == 0) {
		int fd;
		if ((fd = open("/dev/console", O_RDWR)) < 0) {
			if (memcmp(lx_rel, "2.6.36", 6) == 0) {
				mount("devfs", "/dev", "devtmpfs", MS_MGC_VAL, NULL);
			}
			else {
				mount("devfs", "/dev", "tmpfs", MS_MGC_VAL, NULL);
				if (mknod("/dev/console", S_IRWXU|S_IFCHR, makedev(5, 1)) < 0 &&
					errno != EEXIST) {
					perror("filesystem node /dev/console not created");
				}
			}
		}
		else {
			close(fd);
		}
	}

	/* /proc */
	mount("proc", "/proc", "proc", MS_MGC_VAL, NULL);
#ifdef LINUX26
	mount("sysfs", "/sys", "sysfs", MS_MGC_VAL, NULL);
#endif /* LINUX26 */

	/* /tmp */
	mount("ramfs", "/tmp", "ramfs", MS_MGC_VAL, NULL);

	/* /var */
	if (mkdir("/tmp/var", 0777) < 0 && errno != EEXIST) perror("/tmp/var not created");
	if (mkdir("/var/lock", 0777) < 0 && errno != EEXIST) perror("/var/lock not created");
	if (mkdir("/var/log", 0777) < 0 && errno != EEXIST) perror("/var/log not created");
	if (mkdir("/var/run", 0777) < 0 && errno != EEXIST) perror("/var/run not created");
	if (mkdir("/var/tmp", 0777) < 0 && errno != EEXIST) perror("/var/tmp not created");
	if (mkdir("/tmp/media", 0777) < 0 && errno != EEXIST) perror("/tmp/media not created");

#ifdef __CONFIG_HSPOT__
	if (mkdir(RAMFS_CONFMTD_DIR, 0777) < 0 && errno != EEXIST) {
		perror("/tmp/confmtd not created.");
	}
	if (mkdir(RAMFS_CONFMTD_DIR"/hspot", 0777) < 0 && errno != EEXIST) {
		perror("/tmp/confmtd/hspot not created.");
	}
#endif /* __CONFIG_HSPOT__ */

#ifdef __CONFIG_DHDAP__
	if (mkdir(RAMFS_CONFMTD_DIR, 0777) < 0 && errno != EEXIST) {
		perror("/tmp/confmtd not created.");
	}
	if (mkdir(RAMFS_CONFMTD_DIR"/crash_logs", 0777) < 0 && errno != EEXIST) {
		perror("/tmp/confmtd/crash_logs not created.");
	}
#endif /* __CONFIG_DHDAP__ */

#ifdef BCM_DRSDBD
	if (mkdir(RAMFS_CONFMTD_DIR, 0777) < 0 && errno != EEXIST) {
		perror("/tmp/confmtd not created.");
	}
	if (mkdir(RAMFS_CONFMTD_DIR"/drsdbd", 0777) < 0 && errno != EEXIST) {
		perror("/tmp/confmtd/drsdbd not created.");
	}
#endif /* BCM_DRSDBD */

#if defined(LINUX26)
	nand_fs_mtd_mount();
#endif /* LINUX26 */

	confmtd_restore();

#ifdef __CONFIG_UTELNETD__
	/* If kernel enable unix908 pty then we have to make following things. */
	if (mkdir("/dev/pts", 0777) < 0 && errno != EEXIST) perror("/dev/pts not created");
	if (mount("devpts", "/dev/pts", "devpts", MS_MGC_VAL, NULL) == 0) {
		/* pty master */
		if (mknod("/dev/ptmx", S_IRWXU|S_IFCHR, makedev(5, 2)) < 0 && errno != EEXIST)
			perror("filesystem node /dev/ptmx not created");
	} else {
		rmdir("/dev/pts");
	}
#endif	/* LINUX2636 && __CONFIG_UTELNETD__ */

#ifdef __CONFIG_SAMBA__
	/* Add Samba Stuff */
	if (mkdir("/tmp/samba", 0777) < 0 && errno != EEXIST) {
		perror("/tmp/samba not created");
	}
	if (mkdir("/tmp/samba/lib", 0777) < 0 && errno != EEXIST) {
		perror("/tmp/samba/lib not created");
	}
	if (mkdir("/tmp/samba/private", 0777) < 0 && errno != EEXIST) {
		perror("/tmp/samba/private not created");
	}
	if (mkdir("/tmp/samba/var", 0777) < 0 && errno != EEXIST) {
		perror("/tmp/samba/var not created");
	}
	if (mkdir("/tmp/samba/var/locks", 0777) < 0 && errno != EEXIST) {
		perror("/tmp/samba/var/locks not created");
	}
#if defined(LINUX_2_6_36)
	/* To improve SAMBA upload performance */
	reclaim_mem_earlier();
#endif /* LINUX_2_6_36 */
#endif /* __CONFIG_SAMBA__ */

#ifdef BCMQOS
	if (mkdir("/tmp/qos", 0777) < 0 && errno != EEXIST) perror("/tmp/qos not created");
#endif
	/* Setup console */
	if (console_init())
		noconsole = 1;

#ifdef LINUX26
	if (mkdir("/dev/shm", 0777) < 0 && errno != EEXIST) perror("/dev/shm not created");
	eval("/sbin/hotplug2", "--coldplug");
#endif /* LINUX26 */

	if ((loglevel = nvram_get("console_loglevel")))
		klogctl(8, NULL, atoi(loglevel));
	else
		klogctl(8, NULL, 1);

	/* Modules */
	uname(&name);
	snprintf(buf, sizeof(buf), "/lib/modules/%s", name.release);
	if (stat("/proc/modules", &tmp_stat) == 0 &&
	    stat(buf, &tmp_stat) == 0) {
		char module[80], *modules, *next;
		/* Load ctf */
		if (!nvram_match("ctf_disable", "1"))
			eval("insmod", "ctf");

#if defined(__CONFIG_WAPI__) || defined(__CONFIG_WAPI_IAS__)
		if (stat(WAPI_DIR, &tmp_stat) != 0) {
			if (mkdir(WAPI_DIR, 0777) < 0 && errno != EEXIST) {
				perror("WAPI_DIR not created");
			}
			if (mkdir(WAPI_WAI_DIR, 0777) < 0 && errno != EEXIST) {
				perror("WAPI_WAI_DIR not created");
			}
			if (mkdir(WAPI_AS_DIR, 0777) < 0 && errno != EEXIST) {
				perror("WAPI_AS_DIR not created");
			}
		}
#endif /* __CONFIG_WAPI__ || __CONFIG_WAPI_IAS__ */
#if defined(__CONFIG_CIFS__)
		if (stat(CIFS_DIR, &tmp_stat) != 0) {
			if (mkdir(CIFS_DIR, 0777) < 0 && errno != EEXIST) {
				perror("CIFS_DIR not created");
			}
			eval("cp", "/usr/sbin/cs_cfg.txt", CIFS_DIR);
			eval("cp", "/usr/sbin/cm_cfg.txt", CIFS_DIR);
			eval("cp", "/usr/sbin/pwd_list.txt", CIFS_DIR);
		}
#endif /* __CONFIG_CIFS__ */

/* #ifdef BCMVISTAROUTER */
#ifdef __CONFIG_IPV6__
		eval("insmod", "ipv6");
#endif /* __CONFIG_IPV6__ */
/* #endif */

#ifdef __CONFIG_EMF__
		/* Load the EMF & IGMP Snooper modules */
		load_emf();
#endif /*  __CONFIG_EMF__ */

		/* Load kernel modules. Make sure dpsta is loaded before wl
		 * due to symbol dependency.
		 */
#ifdef __CONFIG_DHDAP__
		modules = nvram_get("kernel_mods") ? : "et bcm57xx dpsta dhd wl";
#else
		modules = nvram_get("kernel_mods") ? : "et bcm57xx dpsta wl";
#endif /* __CONFIG_DHDAP__ */

		foreach(module, modules, next) {
#ifdef __CONFIG_DHDAP__
			/* For DHD, additional module params have to be passed. */
			if ((strcmp(module, "dhd") == 0) || (strcmp(module, "wl") == 0)) {
				int	i = 0, maxwl_eth = 0, maxunit = -1;
				int	unit = -1;
				char ifname[16] = {0};
				char instance_base[128];

				/* Search for existing wl devices and the max unit number used */
				for (i = 1; i <= DEV_NUMIFS; i++) {
					snprintf(ifname, sizeof(ifname), "eth%d", i);
					if (!wl_probe(ifname)) {
						if (!wl_ioctl(ifname, WLC_GET_INSTANCE, &unit,
							sizeof(unit))) {
							maxwl_eth = i;
							maxunit = (unit > maxunit) ? unit : maxunit;
						}
					}
				}
				snprintf(instance_base, sizeof(instance_base), "instance_base=%d",
					maxunit + 1);

				eval("insmod", module, instance_base);
			} else
#endif /* __CONFIG_DHDAP__ */
				eval("insmod", module);
		}

		hotplug_usb_init();

#ifdef __CONFIG_USBAP__
		eval("insmod", "usbcore");
		eval("insmod", "usb-storage");
		{
			char	insmod_arg[128];
			int	i = 0, maxwl_eth = 0, maxunit = -1;
			char	ifname[16] = {0};
			int	unit = -1;
			char arg1[20] = {0};
			char arg2[20] = {0};
			char arg3[20] = {0};
			char arg4[20] = {0};
			char arg5[20] = {0};
			char arg6[20] = {0};
			char arg7[20] = {0};
			const int wl_wait = 3;	/* max wait time for wl_high to up */

			/* Save QTD cache params in nvram */
			sprintf(arg1, "log2_irq_thresh=%d", atoi(nvram_safe_get("ehciirqt")));
			sprintf(arg2, "qtdc_pid=%d", atoi(nvram_safe_get("qtdc_pid")));
			sprintf(arg3, "qtdc_vid=%d", atoi(nvram_safe_get("qtdc_vid")));
			sprintf(arg4, "qtdc0_ep=%d", atoi(nvram_safe_get("qtdc0_ep")));
			sprintf(arg5, "qtdc0_sz=%d", atoi(nvram_safe_get("qtdc0_sz")));
			sprintf(arg6, "qtdc1_ep=%d", atoi(nvram_safe_get("qtdc1_ep")));
			sprintf(arg7, "qtdc1_sz=%d", atoi(nvram_safe_get("qtdc1_sz")));

			eval("insmod", "ehci-hcd", arg1, arg2, arg3, arg4, arg5,
				arg6, arg7);

			/* Search for existing PCI wl devices and the max unit number used.
			 * Note that PCI driver has to be loaded before USB hotplug event.
			 * This is enforced in rc.c
			 */
			for (i = 1; i <= DEV_NUMIFS; i++) {
				sprintf(ifname, "eth%d", i);
				if (!wl_probe(ifname)) {
					if (!wl_ioctl(ifname, WLC_GET_INSTANCE, &unit,
						sizeof(unit))) {
						maxwl_eth = i;
						maxunit = (unit > maxunit) ? unit : maxunit;
					}
				}
			}

			/* Set instance base (starting unit number) for USB device */
			sprintf(insmod_arg, "instance_base=%d", maxunit + 1);
			eval("insmod", "wl_high", insmod_arg);

			/* Hold until the USB/HSIC interface is up (up to wl_wait sec) */
			sprintf(ifname, "eth%d", maxwl_eth + 1);
			i = wl_wait;
			while (wl_probe(ifname) && i--) {
				sleep(1);
			}
			if (!wl_ioctl(ifname, WLC_GET_INSTANCE, &unit, sizeof(unit)))
				cprintf("wl%d is up in %d sec\n", unit, wl_wait - i);
			else
				cprintf("wl%d not up in %d sec\n", unit, wl_wait);
		}
#ifdef LINUX26
		mount("usbdeffs", "/proc/bus/usb", "usbfs", MS_MGC_VAL, NULL);
#else
		mount("none", "/proc/bus/usb", "usbdevfs", MS_MGC_VAL, NULL);
#endif /* LINUX26 */
#endif /* __CONFIG_USBAP__ */

#ifdef __CONFIG_WCN__
		modules = "scsi_mod sd_mod usbcore usb-ohci usb-storage fat vfat msdos";
		foreach(module, modules, next)
			eval("insmod", module);
#endif

#ifdef __CONFIG_SOUND__
		modules = "soundcore snd snd-timer snd-page-alloc snd-pcm snd-pcm-oss "
		        "snd-soc-core i2c-core i2c-algo-bit i2c-gpio snd-soc-bcm947xx-i2s "
		        "snd-soc-bcm947xx-pcm snd-soc-wm8750 snd-soc-wm8955 snd-soc-bcm947xx";
		foreach(module, modules, next)
			eval("insmod", module);
		mknod("/dev/dsp", S_IRWXU|S_IFCHR, makedev(14, 3));
		if (mkdir("/dev/snd", 0777) < 0 && errno != EEXIST) perror("/dev/snd not created");
		mknod("/dev/snd/controlC0", S_IRWXU|S_IFCHR, makedev(116, 0));
		mknod("/dev/snd/pcmC0D0c", S_IRWXU|S_IFCHR, makedev(116, 24));
		mknod("/dev/snd/pcmC0D0p", S_IRWXU|S_IFCHR, makedev(116, 16));
		mknod("/dev/snd/timer", S_IRWXU|S_IFCHR, makedev(116, 33));
#endif

#ifdef LINUX_2_6_36
		/* To combat hotplug event lost because it could possibly happen before
		 * Rootfs is mounted or rc (preinit) is invoked during kernel boot-up with
		 * USB device attached.
		 */
		{
			FILE *fp = NULL;
			char buf[128];
			uint chipid = 0;

			/* Get PlatSoc CHIP info */
			if ((fp = fopen("/proc/bcm_chipinfo", "r")) != NULL) {
				while (fgets(buf, sizeof(buf), fp) != NULL) {
					if (strstr(buf, "ChipID") != NULL) {
						sscanf(buf, "ChipID: 0x%x", &chipid);
						break;
					}
				}
				fclose(fp);
			}

			/* 53573/53574/47189 only supports USB2.0 */
			modules = nvram_get("hci_mods") ? :
				((BCM53573_CHIP(chipid)) ?
				"ehci-hcd ohci-hcd" : "xhci-hcd ehci-hcd ohci-hcd");
		}

		foreach(module, modules, next)
			eval("insmod", module);
#endif /* LINUX_2_6_36 */

#if defined(LINUX_2_6_36) && defined(__CONFIG_TREND_IQOS__)
		{
			struct stat file_stat;

			if (stat("/usr/sbin/qos.conf", &file_stat) == 0) {
				if (mkdir("/tmp/trend", 0777) < 0 && errno != EEXIST)
					perror("/tmp/trend not created");
				else {
					eval("cp", "/usr/sbin/tdts_rule_agent", "/tmp/trend");
					eval("cp", "/usr/sbin/setup.sh", "/tmp/trend");
					eval("cp", "/usr/sbin/rule.trf", "/tmp/trend");
					eval("cp", "/usr/sbin/app_patrol_all.conf", "/tmp/trend");
					eval("cp", "/usr/sbin/app_patrol_clear.conf", "/tmp/trend");
					eval("cp", "/usr/sbin/app_patrol.conf", "/tmp/trend");
					eval("cp", "/usr/sbin/sample.bin", "/tmp/trend");
					eval("cp", "/usr/sbin/qos.sh", "/tmp/trend");
					eval("cp", "/usr/sbin/moni.sh", "/tmp/trend");
					eval("cp", "/usr/sbin/patrol_tq.conf", "/tmp/trend");
				}
			}
		}
#endif /* LINUX_2_6_36 && __CONFIG_TREND_IQOS__ */
	}

	if (memcmp(lx_rel, "2.6.36", 6) == 0) {
		int fd;
		if ((fd = open("/proc/irq/163/smp_affinity", O_RDWR)) >= 0) {
			close(fd);

			if (nvram_match("gmac3_enable", "1")) {
				char *fwd_cpumap;

				/* Place network interface vlan1/eth0 on CPU hosting 5G upper */
				fwd_cpumap = nvram_get("fwd_cpumap");

				if (fwd_cpumap == NULL) {
					/* BCM4709acdcrh: Network interface GMAC on Core#0
					 *    [5G+2G:163 on Core#0] and [5G:169 on Core#1].
					 *    Bind et2:vlan1:eth0:181 to Core#0
					 *    Note, USB3 xhci_hcd's irq#112 binds Core#1
					 *    bind eth0:181 to Core#1 impacts USB3 performance
					 */
					system("echo 1 > /proc/irq/181/smp_affinity");

				} else {

					char cpumap[32], *next;

					foreach(cpumap, fwd_cpumap, next) {
						char mode, chan;
						int band, irq, cpu;

						/* Format: mode:chan:band#:irq#:cpu# */
						if (sscanf(cpumap, "%c:%c:%d:%d:%d",
						           &mode, &chan, &band, &irq, &cpu) != 5) {
							break;
						}
						if (cpu > 1) {
							break;
						}
						/* Find the single 5G upper */
						if ((chan == 'u') || (chan == 'U')) {
							char command[128];
							snprintf(command, sizeof(command),
							    "echo %d > /proc/irq/181/smp_affinity",
							    1 << cpu);
							system(command);
							break;
						}
					}
				}

			} else { /* ! gmac3_enable */

				if (!nvram_match("txworkq", "1")) {
					system("echo 2 > /proc/irq/163/smp_affinity");
					system("echo 2 > /proc/irq/169/smp_affinity");
				}
			}

			system("echo 2 > /proc/irq/112/smp_affinity");
		}
	}
	/* Set a sane date */
	stime(&tm);

	dprintf("done\n");
}

/* States */
enum {
	RESTART,
	STOP,
	START,
	TIMER,
	IDLE
};
static int state = START;
static int signalled = -1;

/* Signal handling */
static void
rc_signal(int sig)
{
	if (sig == SIGHUP) {
		dprintf("signalling RESTART\n");
		signalled = RESTART;
	}
	else if (sig == SIGUSR2) {
		dprintf("signalling START\n");
		signalled = START;
	}
	else if (sig == SIGINT) {
		dprintf("signalling STOP\n");
		signalled = STOP;
	}
	else if (sig == SIGALRM) {
		dprintf("signalling TIMER\n");
		signalled = TIMER;
	}
}

/* Get the timezone from NVRAM and set the timezone in the kernel
 * and export the TZ variable
 */
static void
set_timezone(void)
{

	/* Export TZ variable for the time libraries to
	 * use.
	 */
	setenv("TZ", nvram_get("time_zone"), 1);

#if defined(__CONFIG_WAPI__) || defined(__CONFIG_WAPI_IAS__)
#ifndef	RC_BUILDTIME
#define	RC_BUILDTIME	1252636574
#endif
	{
		struct timeval tv = {RC_BUILDTIME, 0};
		time_t now;
		time(&now);
		if (now < RC_BUILDTIME)
			settimeofday(&tv, NULL);
	}
#endif /* __CONFIG_WAPI__ || __CONFIG_WAPI_IAS__ */
}

/* Timer procedure.Gets time from the NTP servers once every timer interval
 * Interval specified by the NVRAM variable timer_interval
 */
int
do_timer(void)
{
	int interval = atoi(nvram_safe_get("timer_interval"));

	dprintf("%d\n", interval);

	if (interval == 0)
		return 0;

	/* Report stats */
	if (nvram_invmatch("stats_server", "")) {
		char *stats_argv[] = { "stats", nvram_get("stats_server"), NULL };
		_eval(stats_argv, NULL, 5, NULL);
	}

	/* Sync time */
	start_ntpc();

	alarm(interval);

	return 0;
}

/* Main loop */
static void
main_loop(void)
{
	int defaults;
#ifdef CAPI_AP
	static bool start_aput = TRUE;
#endif
	sigset_t sigset;
	pid_t shell_pid = 0;
#ifdef __CONFIG_VLAN__
	uint boardflags;
#endif

	defaults = RESTORE_DEFAULTS();

	/* Basic initialization */
	sysinit();

	/* Setup signal handlers */
	signal_init();
	signal(SIGHUP, rc_signal);
	signal(SIGUSR2, rc_signal);
	signal(SIGINT, rc_signal);
	signal(SIGALRM, rc_signal);
	sigemptyset(&sigset);

	/* JIRA SWWLAN-29748 */
	if (nvram_match("rc_interrupt", "on")) {
		/* Give user a chance to run a shell before bringing up the rest of the system */
		if (!noconsole)
			run_shell(1, 0);
	}

	/* Get boardflags to see if VLAN is supported */
#ifdef __CONFIG_VLAN__
	boardflags = strtoul(nvram_safe_get("boardflags"), NULL, 0);
#endif	/* __CONFIG_VLAN__ */

	/* Add loopback */
	config_loopback();

	/* Convert deprecated variables */
	convert_deprecated();

	/* ses cleanup of variables left half way through */
	ses_cleanup();

	/* ses_cl cleanup of variables left half way through */
	ses_cl_cleanup();

	/* Upgrade NVRAM variables to MBSS mode */
	upgrade_defaults();

	/* Restore defaults if necessary */
	restore_defaults();

#ifdef LINUX_2_6_36
	/* Ajuest FA NVRAM variables */
	fa_nvram_adjust();

	/* Ajuest GMAC3 NVRAM variables */
	gmac3_nvram_adjust();
#endif

#ifdef __CONFIG_NAT__
	/* Auto Bridge if neccessary */
	if (!strcmp(nvram_safe_get("auto_bridge"), "1"))
	{
		auto_bridge();
	}
	/* Setup wan0 variables if necessary */
	set_wan0_vars();
#endif	/* __CONFIG_NAT__ */

#if defined(WLTEST) && defined(RWL_SOCKET)
	/* Shorten TCP timeouts to prevent system from running slow with rwl */
	system("echo \"10 10 10 10 3 3 10 10 10 10\">/proc/sys/net/ipv4/ip_conntrack_tcp_timeouts");
#endif /* WL_TEST && RWL_SOCKET */

#ifdef __CONFIG_FAILSAFE_UPGRADE_SUPPORT__
	failsafe_nvram_adjust();
#endif

	/* Loop forever */
	for (;;) {
		switch (state) {
		case RESTART:
			dprintf("RESTART\n");
			/* Fall through */
		case STOP:
			dprintf("STOP\n");
			pmon_init();
			stop_services();
#ifdef __CONFIG_NAT__
			stop_wan();
#endif	/* __CONFIG_NAT__ */
			stop_lan();
#ifdef __CONFIG_VLAN__
			if (boardflags & BFL_ENETVLAN)
				stop_vlan();
#endif	/* __CONFIG_VLAN__ */
#ifdef __CONFIG_SOUND__
#ifdef __CONFIG_SALSA__
#ifdef __CONFIG_AMIXER__
			stop_amixer();
#endif /* __CONFIG_AMIXER__ */
#endif /* __CONFIG_SALSA__ */
#ifdef __CONFIG_AIRPLAY__
			stop_airplay();
#endif /* __CONFIG_AIRPLAY__ */
#ifdef __CONFIG_ALSACTL__
			stop_alsactl();
#endif /* __CONFIG_ALSACTL__ */
#endif /* __CONFIG_SOUND__ */
#ifdef __CONFIG_MDNSRESPONDER__
			stop_mdns();
#endif /* __CONFIG_MDNSRESPONDER__ */
			if (state == STOP) {
				state = IDLE;
				break;
			}
			/* Fall through */
		case START:
			dprintf("START\n");
			pmon_init();
			{ /* Set log level on restart */
				char *loglevel;
				int loglev = 8;

				if ((loglevel = nvram_get("console_loglevel"))) {
					loglev = atoi(loglevel);
				}
				klogctl(8, NULL, loglev);
				if (loglev < 7) {
					printf("WARNING: console log level set to %d\n", loglev);
				}
			}

			set_timezone();
#ifdef __CONFIG_VLAN__
			if (boardflags & BFL_ENETVLAN)
				start_vlan();
#endif	/* __CONFIG_VLAN__ */
			start_lan();
			start_services();
#ifdef __CONFIG_NAT__
			start_wan();
#endif	/* __CONFIG_NAT__ */
			start_wl();

			if (defaults) {
				if (!(strcmp(nvram_safe_get("devicemode"), "psr"))) {
					set_psr_vars(0);
				}
			}

			if (strcmp(nvram_safe_get("wl_mode"), "psr") != 0) {
				/* If we are not in PSTA Repeater mode start the sound config */
#ifdef __CONFIG_SOUND__
#ifdef __CONFIG_ALSACTL__
				start_alsactl();
#endif /* __CONFIG_ALSACTL__ */
#ifdef __CONFIG_SALSA__
#ifdef __CONFIG_AMIXER__
				/* Start the Mixer */
				start_amixer();
#endif /* __CONFIG_AMIXER__ */
#endif /* __CONFIG_SALSA__ */
#endif /* __CONFIG_SOUND__ */

#ifdef __CONFIG_MDNSRESPONDER__
				/* Start Airplay */
				start_mdns();
				sleep(3);
#endif /* __CONFIG_MDNSRESPONDER__ */
#ifdef __CONFIG_AIRPLAY__
				start_airplay();
#endif /* __CONFIG_AIRPLAY__ */
			}
			/* Fall through */
		case TIMER:
			dprintf("TIMER\n");
			do_timer();
			/* Fall through */
		case IDLE:
			dprintf("IDLE\n");
			state = IDLE;
#ifdef CAPI_AP
			if (start_aput == TRUE) {
				system("/usr/sbin/wfa_aput_all&");
				start_aput = FALSE;
			}
#endif /* CAPI_AP */
			/* Wait for user input or state change */
			while (signalled == -1) {
				if (!noconsole && (!shell_pid || kill(shell_pid, 0) != 0))
					shell_pid = run_shell(0, 1);
				else {

					sigsuspend(&sigset);
				}
#ifdef LINUX26
				system("echo 1 > /proc/sys/vm/drop_caches");
#ifdef USBAP
				system("echo 4096 > /proc/sys/vm/min_free_kbytes");
#endif
#elif defined(__CONFIG_SHRINK_MEMORY__)
				eval("cat", "/proc/shrinkmem");
#endif	/* LINUX26 */
			}
			state = signalled;
			signalled = -1;
			break;
		default:
			dprintf("UNKNOWN\n");
			return;
		}
	}
}

int
main(int argc, char **argv)
{
#ifdef LINUX26
	char *init_alias = "preinit";
#else
	char *init_alias = "init";
#endif
	char *base = strrchr(argv[0], '/');

	base = base ? base + 1 : argv[0];

	/* init */
	if (strstr(base, init_alias)) {
		main_loop();
		return 0;
	}

	/* Set TZ for all rc programs */
	setenv("TZ", nvram_safe_get("time_zone"), 1);

	/* rc [stop|start|restart ] */
	if (strstr(base, "rc")) {
		if (argv[1]) {
			if (strncmp(argv[1], "start", 5) == 0)
				return kill(1, SIGUSR2);
			else if (strncmp(argv[1], "stop", 4) == 0)
				return kill(1, SIGINT);
			else if (strncmp(argv[1], "restart", 7) == 0)
				return kill(1, SIGHUP);
		} else {
			fprintf(stderr, "usage: rc [start|stop|restart]\n");
			return EINVAL;
		}
	}

#ifdef __CONFIG_NAT__
	/* ppp */
	else if (strstr(base, "ip-up"))
		return ipup_main(argc, argv);
	else if (strstr(base, "ip-down"))
		return ipdown_main(argc, argv);

	/* udhcpc [ deconfig bound renew ] */
	else if (strstr(base, "udhcpc"))
		return udhcpc_wan(argc, argv);
#endif	/* __CONFIG_NAT__ */

	/* ldhclnt [ deconfig bound renew ] */
	else if (strstr(base, "ldhclnt"))
		return udhcpc_lan(argc, argv);

	/* stats [ url ] */
	else if (strstr(base, "stats"))
		return http_stats(argv[1] ? : nvram_safe_get("stats_server"));

	/* erase [device] */
	else if (strstr(base, "erase")) {
		if (argv[1] && ((!strcmp(argv[1], "boot")) ||
			(!strcmp(argv[1], "linux")) ||
			(!strcmp(argv[1], "linux2")) ||
			(!strcmp(argv[1], "rootfs")) ||
			(!strcmp(argv[1], "rootfs2")) ||
			(!strcmp(argv[1], "brcmnand")) ||
			(!strcmp(argv[1], "confmtd")) ||
			(!strcmp(argv[1], "nvram")))) {

			return mtd_erase(argv[1]);
		} else {
			fprintf(stderr, "usage: erase [device]\n");
			return EINVAL;
		}
	}


	/* write [path] [device] */
	else if (strstr(base, "write")) {
		if (argc >= 3)
			return mtd_write(argv[1], argv[2]);
		else {
			fprintf(stderr, "usage: write [path] [device]\n");
			return EINVAL;
		}
	}

	/* hotplug [event] */
	else if (strstr(base, "hotplug")) {
		if (argc >= 2) {
			if (!strcmp(argv[1], "net"))
				return hotplug_net();
			else if (!strcmp(argv[1], "usb"))
				return hotplug_usb();
			else if (!strcmp(argv[1], "block"))
				return hotplug_block();
#if defined(LINUX_2_6_36)
			else if (!strcmp(argv[1], "platform"))
				return coma_uevent();
#endif /* LINUX_2_6_36 */
		} else {
			fprintf(stderr, "usage: hotplug [event]\n");
			return EINVAL;
		}
	}

	return EINVAL;
}
