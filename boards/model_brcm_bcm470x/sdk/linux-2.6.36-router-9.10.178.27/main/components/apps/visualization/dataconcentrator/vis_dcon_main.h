/*
 * Linux Visualization Data Concentrator main header
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
 * $Id: vis_dcon_main.h 555336 2015-05-08 09:52:04Z $
 */

#ifndef _VIS_DCON_MAIN_H_
#define _VIS_DCON_MAIN_H_

#include "vis_struct.h"

int vis_debug_level; /* Indicates the level of debug message to be printed on the console */

configs_t g_configs;

alldut_settings_t *g_alldutsettings;

#endif /* _VIS_DCON_MAIN_H_ */
