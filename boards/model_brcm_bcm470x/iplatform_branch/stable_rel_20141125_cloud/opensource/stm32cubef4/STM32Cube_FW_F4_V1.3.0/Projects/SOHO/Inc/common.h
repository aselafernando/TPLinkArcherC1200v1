/******************************************************************************
*
* Copyright (c) 2010 TP-LINK Technologies CO.,LTD.
* All rights reserved.
*
* FILE NAME  :   common.h
* VERSION    :   1.0
* DESCRIPTION:   定义整个应用程序共用的资源.
*
* AUTHOR     :   Huangwenzhong <Huangwenzhong@tp-link.net>
* CREATE DATE:   11/17/2014
*
* HISTORY    :
* 01   11/17/2014  Huangwenzhong     Create.
*
******************************************************************************/

#ifndef __COMMON_H__
#define __COMMON_H__
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "WM.h"
#include "GUI.h"
#include "UI_Common.h"
#include "UI_ID.h"
#include "UI_Image.h"
#include "TP_Type.h"
#include "Language.h"


#define TP_MALLOC	malloc
#define TP_MEMSET	memset
#define TP_MEMCPY	GUI_MEMCPY
#define TP_FREE		free

#define LCD_X_SIZE	LCD_GetXSize()
#define LCD_Y_SIZE	LCD_GetYSize()

#endif

