/******************************************************************************
*
* Copyright (c) 2010 TP-LINK Technologies CO.,LTD.
* All rights reserved.
*
* FILE NAME  :   homeAction.h
* VERSION    :   1.0
* DESCRIPTION:   用于处理Home键的响应.
*
* AUTHOR     :   Huangwenzhong <Huangwenzhong@tp-link.net>
* CREATE DATE:   11/14/2014
*
* HISTORY    :
* 01   11/14/2014  Huangwenzhong     Create.
*
******************************************************************************/

#ifndef _HOME_ACTION_H__
#define _HOME_ACTION_H__
#include "GUI.h"
#include "TP_Type.h"

void homeActionInit();
STATUS homeActionPushApp(I32 appId);
STATUS homeActionPopApp(I32 appId);

#endif

