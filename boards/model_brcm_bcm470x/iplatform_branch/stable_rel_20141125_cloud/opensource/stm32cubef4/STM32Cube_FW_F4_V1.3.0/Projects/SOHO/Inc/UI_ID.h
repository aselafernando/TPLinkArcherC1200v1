/******************************************************************************
*
* Copyright (c) 2010 TP-LINK Technologies CO.,LTD.
* All rights reserved.
*
* FILE NAME  :   UI_ID.h
* VERSION    :   1.0
* DESCRIPTION:   统一管理所有UI相关的ID.
*
* AUTHOR     :   Huangwenzhong <Huangwenzhong@tp-link.net>
* CREATE DATE:   11/17/2014
*
* HISTORY    :
* 01   11/17/2014  Huangwenzhong     Create.
*
******************************************************************************/
#ifndef _UI_ID_H__
#define _UI_ID_H__
#include "GUI.h"

/* For ActionBar */
#define TP_AB_BASE_ID	GUI_ID_USER
#define TP_AB_HANDLE_ID	(TP_AB_BASE_ID + 0x01)
#define TP_AB_UP_ID		(TP_AB_BASE_ID + 0x02)
#define TP_AB_HOME_ID	(TP_AB_BASE_ID + 0x03)
#define TP_AB_TITLE_ID	(TP_AB_BASE_ID + 0x04)

#define TP_AB_BUTTON_ID_0 (TP_AB_BASE_ID + 0x10)
#define TP_AB_BUTTON_ID_1 (TP_AB_BASE_ID + 0x11)
#define TP_AB_BUTTON_ID_2 (TP_AB_BASE_ID + 0x12)
#define TP_AB_BUTTON_ID_3 (TP_AB_BASE_ID + 0x13)
#define TP_AB_BUTTON_ID_4 (TP_AB_BASE_ID + 0x14)

/* For Keyboard */
#define TP_KB_BASE_ID		(GUI_ID_USER + 32)
#define TP_KB_LOWER_ID		(TP_KB_BASE_ID)
#define TP_KB_UPPER_ID		(TP_KB_BASE_ID + 40)
#define TP_KB_NUM_ID		(TP_KB_BASE_ID + 80)
#define TP_KB_CHAR_ID		(TP_KB_BASE_ID + 120)

/* for Timer */
#define TP_TIMER_ID0 (GUI_ID_USER + 0x99)

#endif
