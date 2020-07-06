/*
 * Linux Visualization Data Collector client socket header
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
 * $Id: vis_socketclient.h 555336 2015-05-08 09:52:04Z $
 */
#ifndef _VIS_SOCK_CLIENT_H_
#define _VIS_SOCK_CLIENT_H_

#include "vis_sock_util.h"

extern int connect_to_server(uint32 nport, char *straddrs);

#endif /* _VIS_SOCK_CLIENT_H_ */
