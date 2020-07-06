/******************************************************************************
*
* Copyright (c) 2010 TP-LINK Technologies CO.,LTD.
* All rights reserved.
*
* FILE NAME  :   guiQuickSetup.h
* VERSION    :   1.0
* DESCRIPTION:   处理Quick Setup的流程.
*
* AUTHOR     :   Huangwenzhong <Huangwenzhong@tp-link.net>
* CREATE DATE:   11/19/2014
*
* HISTORY    :
* 01   11/19/2014  Huangwenzhong     Create.
*
******************************************************************************/
#ifndef __GUI_QUICK_SETUP_H__
#define __GUI_QUICK_SETUP_H__
#include "Common.h"
#include "GUI_QuickSetupProcessBar.h"

#define GUI_QS_TITLE_FONT					&GUI_FontF16DqW3
#define GUI_QS_SUB_TITLE_FONT				&GUI_FontF12DqW3
 
typedef enum
{	
	QS_STEP_NONE = -1,
	QS_STEP_START = 0,
	QS_STEP_TIME = 0,
	QS_STEP_WAN_TYPE,
	QS_STEP_DYN_IP,
	QS_STEP_STATIC_IP,
	QS_STEP_PPPOE,
	QS_STEP_L2TP,
	QS_STEP_PPTP,
	QS_STEP_WAN_MAC,	
	QS_STEP_WLAN,
	QS_STEP_SUMMARY,
	QS_STEP_TEST,
	QS_STEP_END = QS_STEP_TEST,
}QS_STEP_ID;

typedef enum 
{
	QS_STEP_TRUNK = 0,
	QS_STEP_BRANCH_WAN,
	QS_STEP_BRANCH_MAC,
}QS_STEP_LEVEL;


typedef struct _QS_STEP_ITEM
{
	struct _QS_STEP_ITEM *prev;
	struct _QS_STEP_ITEM *next;
	BOOL	flag;
	QS_STEP_LEVEL		level;
	QS_STEP_ID id;
	BOOL (*create)(WM_HWIN hWin, I32 x, I32 y);
}QS_STEP_ITEM;

typedef struct _QS_STEP_HEAD
{
	QS_STEP_ITEM *pStepList;
}QS_STEP_HEAD;


void GUI_QsInit();
BOOL GUI_QsAddStep(QS_STEP_ID id, QS_STEP_LEVEL level, BOOL flag);
BOOL GUI_QsFindNext(QS_STEP_ITEM **pStepData);
BOOL GUI_QsFindPrev(QS_STEP_ITEM **pStepData);
BOOL GUI_QsActiveBranchStep(QS_STEP_ID id);



#endif


