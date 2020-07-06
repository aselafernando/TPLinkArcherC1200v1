/*
 * chanim statistics for visualization tool
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
 * $Id: vis_wlcongestion.c 555336 2015-05-08 09:52:04Z $
 */
#ifdef WIN32
#include <windows.h>
#endif

#if !defined(TARGETOS_nucleus)
#define CLMDOWNLOAD
#endif


#if defined(__NetBSD__)
#include <typedefs.h>
#endif

/* Because IL_BIGENDIAN was removed there are few warnings that need
 * to be fixed. Windows was not compiled earlier with IL_BIGENDIAN.
 * Hence these warnings were not seen earlier.
 * For now ignore the following warnings
 */
#ifdef WIN32
#pragma warning(push)
#pragma warning(disable : 4244)
#pragma warning(disable : 4761)
#endif

#include <bcmendian.h>
#include "vis_common.h"
#if defined(WLPFN) && defined(linux)
#ifndef TARGETENV_android
#include <unistd.h>
#endif
#endif /* WLPFN */

#ifdef WLEXTLOG
#include <wlc_extlog_idstr.h>
#endif

#include <inttypes.h>
#include <errno.h>

#if defined SERDOWNLOAD || defined CLMDOWNLOAD
#include <sys/stat.h>
#include <trxhdr.h>
#ifdef SERDOWNLOAD
#include <usbrdl.h>
#endif
#include <stdio.h>
#include <errno.h>

#ifndef WIN32
#include <fcntl.h>
#endif /* WIN32 */
#endif /* SERDOWNLOAD || defined CLMDOWNLOAD */

#include "vis_wl.h"
#include "vis_wlcongestion.h"

#define ACS_CHANIM_BUF_LEN (2*1024)

extern long g_timestamp;

static int
wl_iovar_getbuf(char *ifname, char *iovar, void *param, int paramlen, void *bufptr, int buflen)
{
	int err;
	uint namelen;
	uint iolen;

	namelen = strlen(iovar) + 1;	 /* length of iovar name plus null */
	iolen = namelen + paramlen;

	/* check for overflow */
	if (iolen > buflen)
		return (BCME_BUFTOOSHORT);

	memcpy(bufptr, iovar, namelen);	/* copy iovar name including null */
	memcpy((int8*)bufptr + namelen, param, paramlen);

	err = wl_ioctl(ifname, WLC_GET_VAR, bufptr, buflen, FALSE);

	return (err);
}

/* Gets wifi, non wifi  interference */
congestion_list_t*
wl_get_chanim_stats(void *wl)
{
	int err;
	wl_chanim_stats_t *list;
	wl_chanim_stats_t param;
	chanim_stats_t *stats;
	int i;
	int count;
	int buflen = ACS_CHANIM_BUF_LEN;
	char *data_buf;
	congestion_list_t *congestionout = NULL;

	data_buf = (char*)malloc(sizeof(data_buf) * buflen);
	if (data_buf == NULL) {
		VIS_CHANIM("Failed to allocate data_buf of %d bytes\n", buflen);
		return NULL;
	}
	memset(data_buf, 0, buflen);

	param.buflen = htod32(buflen);
	param.count = htod32(WL_CHANIM_COUNT_ALL);

	if ((err = wl_iovar_getbuf(wl, "chanim_stats", &param, sizeof(wl_chanim_stats_t),
		data_buf, buflen)) < 0) {
		VIS_CHANIM("failed to get chanim results\n");
		free(data_buf);
		return NULL;
	}

	list = (wl_chanim_stats_t*)data_buf;

	list->buflen = dtoh32(list->buflen);
	list->version = dtoh32(list->version);
	list->count = dtoh32(list->count);

	if (list->buflen == 0) {
		list->version = 0;
		list->count = 0;
	} else if (list->version != WL_CHANIM_STATS_VERSION) {
		VIS_CHANIM("Sorry, your driver has wl_chanim_stats version %d "
			"but this program supports only version %d.\n",
				list->version, WL_CHANIM_STATS_VERSION);
		list->buflen = 0;
		list->count = 0;
	}

	count = dtoh32(list->count);
	VIS_CHANIM("Count is : %d\n", count);

	congestionout = (congestion_list_t*)malloc(sizeof(congestion_list_t) +
		(count * sizeof(congestion_t)));
	if (congestionout == NULL) {
		VIS_CHANIM("Failed to allocate congestionout buffer of size : %d\n", count);
		free(data_buf);
		return NULL;
	}
	congestionout->timestamp = g_timestamp;
	congestionout->length = 0;

	VIS_CHANIM("chanspec tx   inbss   obss   nocat   nopkt   doze     txop     "
		"goodtx  badtx   glitch   badplcp  knoise  idle  timestamp\n");

	for (i = 0; i < count; i++) {
		stats = &list->stats[i];

		congestionout->congest[congestionout->length].channel =
			CHSPEC_CHANNEL(stats->chanspec);

		congestionout->congest[congestionout->length].tx = stats->ccastats[0];
		congestionout->congest[congestionout->length].inbss = stats->ccastats[1];
		congestionout->congest[congestionout->length].obss = stats->ccastats[2];
		congestionout->congest[congestionout->length].nocat = stats->ccastats[3];
		congestionout->congest[congestionout->length].nopkt = stats->ccastats[4];
		congestionout->congest[congestionout->length].doze = stats->ccastats[5];
		congestionout->congest[congestionout->length].txop = stats->ccastats[6];
		congestionout->congest[congestionout->length].goodtx = stats->ccastats[7];
		congestionout->congest[congestionout->length].badtx = stats->ccastats[8];
		congestionout->congest[congestionout->length].glitchcnt = dtoh32(stats->glitchcnt);
		congestionout->congest[congestionout->length].badplcp = dtoh32(stats->badplcp);
		congestionout->congest[congestionout->length].knoise = stats->bgnoise;
		congestionout->congest[congestionout->length].chan_idle = stats->chan_idle;

		VIS_CHANIM("%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n",
			congestionout->congest[congestionout->length].channel,
			congestionout->congest[congestionout->length].tx,
			congestionout->congest[congestionout->length].inbss,
			congestionout->congest[congestionout->length].obss,
			congestionout->congest[congestionout->length].nocat,
			congestionout->congest[congestionout->length].nopkt,
			congestionout->congest[congestionout->length].doze,
			congestionout->congest[congestionout->length].txop,
			congestionout->congest[congestionout->length].goodtx,
			congestionout->congest[congestionout->length].badtx,
			congestionout->congest[congestionout->length].glitchcnt,
			congestionout->congest[congestionout->length].badplcp,
			congestionout->congest[congestionout->length].knoise,
			congestionout->congest[congestionout->length].chan_idle);

		congestionout->length++;
	}

	free(data_buf);
	return (congestionout);
}
