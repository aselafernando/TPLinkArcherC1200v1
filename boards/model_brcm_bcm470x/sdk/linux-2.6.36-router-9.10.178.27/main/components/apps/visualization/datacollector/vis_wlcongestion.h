/*
 * Linux Visualization Data Collector chanim stats header
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
 * $Id: vis_wlcongestion.h 555336 2015-05-08 09:52:04Z $
 */
#ifndef _vis_wlcongestion_h_
#define _vis_wlcongestion_h_

#include "vis_struct.h"

extern congestion_list_t *wl_get_chanim_stats(void *wl);

#endif /* _vis_wlcongestion_h_ */
