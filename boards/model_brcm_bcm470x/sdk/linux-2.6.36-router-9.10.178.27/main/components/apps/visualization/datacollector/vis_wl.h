/*
 * Site survey statistics header for visualization tool
 *
 * Copyright (C) 2015, Broadcom Corporation
 * All Rights Reserved.
 * 
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
 * the contents of this file may not be disclosed to third parties, copied
 * or duplicated in any form, in whole or in part, without the prior
 * written permission of Broadcom Corporation.
 *
 *
 * <<Broadcom-WL-IPTag/Proprietary:>>
 *
 * $Id: vis_wl.h 555336 2015-05-08 09:52:04Z $
 */
#ifndef _vis_wl_h_
#define _vis_wl_h_

#include "vis_struct.h"

#define IOCTL_ERROR  -2		/* Error code for Ioctl failure */

extern const char *g_wlu_av0;

extern void syserr(char *s);

/* check driver version */
extern int wl_check(void *wl);

extern int wl_ioctl(void *wl, int cmd, void *buf, int len, bool set);

extern int wl_scan(void *wl);

extern networks_list_t* wl_dump_networks(void *wl);

extern int wl_is_AP(void *wl, dut_info_t *dut_info);

extern int wl_get_bss_info(void *wl, dut_info_t *dut_info);

extern int wl_get_mac(void *wl, char *strmac);

extern int wl_is_up(void *wl, int *isup);

extern char *wl_ether_etoa(const struct ether_addr *n);

#endif /* _vis_wl_h_ */
