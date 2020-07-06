/******************************************************************************
*
* Copyright (c) 2010 TP-LINK Technologies CO.,LTD.
* All rights reserved.
*
* FILE NAME  :   GUI_QuickSetupProcessBar.h
* VERSION    :   1.0
* DESCRIPTION:   绘制进度条.
*
* AUTHOR     :   Huangwenzhong <Huangwenzhong@tp-link.net>
* CREATE DATE:   11/24/2014
*
* HISTORY    :
* 01   11/24/2014  Huangwenzhong     Create.
*
******************************************************************************/

#ifndef __GUI_QUICK_SETUP_PROCESS_BAR_H__
#define __GUI_QUICK_SETUP_PROCESS_BAR_H__

#include "Common.h"

#define QS_PROCESS_BAR_DEFAULT_STEP_NUM		(6)

STATUS GUI_QsCreateProcessBar(WM_HWIN hParent, I32 x, I32 y, I32 totalStep, I32 curStep);


#endif/* __GUI_QUICK_SETUP_PROCESS_BAR_H__ */
