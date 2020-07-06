/******************************************************************************
*
* Copyright (c) 2010 TP-LINK Technologies CO.,LTD.
* All rights reserved.
*
* FILE NAME  :   UI_Common.h
* VERSION    :   1.0
* DESCRIPTION:   ����UI��صĹ��õ���Ϣ.
*
* AUTHOR     :   Huangwenzhong <Huangwenzhong@tp-link.net>
* CREATE DATE:   11/17/2014
*
* HISTORY    :
* 01   11/17/2014  Huangwenzhong     Create.
*
******************************************************************************/

#ifndef __UI_COMMON_H__
#define __UI_COMMON_H__

#include "TP_Type.h"
#include "GUI.h"

/* Ĭ������ */
extern GUI_CONST_STORAGE GUI_FONT GUI_FontF16DqW3;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontF12DqW3;

/* �ı����뷽ʽ */
/* ���Ϸ� */
#define TP_TA_LEFT_TOP           (GUI_TA_LEFT | GUI_TA_TOP)
/* ���Ϸ� */
#define TP_TA_HCENTER_TOP        (GUI_TA_HCENTER | GUI_TA_TOP)
/* ���Ϸ� */
#define TP_TA_RIGHT_TOP          (GUI_TA_RIGHT | GUI_TA_TOP)
/* ����� */
#define TP_TA_LEFT_VCENTER       (GUI_TA_LEFT | GUI_TA_VCENTER)
/* ���м� */
#define TP_TA_HCENTER_VCENTER    (GUI_TA_HCENTER | GUI_TA_VCENTER)
/* ���ұ� */
#define TP_TA_RIGHT_VCENTER      (GUI_TA_RIGHT | GUI_TA_VCENTER)
/* ���·� */
#define TP_TA_LEFT_BOTTOM        (GUI_TA_LEFT | GUI_TA_BOTTOM)
/* ���·� */
#define TP_TA_HCENTER_BOTTOM     (GUI_TA_HCENTER | GUI_TA_BOTTOM)
/* ���·� */
#define TP_TA_RIGHT_BOTTOM       (GUI_TA_RIGHT | GUI_TA_BOTTOM)

#define UI_ACTION_BAR_HEIGHT		(38)


#define TP_GUI_LOCK()       WM_LOCK()
#define TP_GUI_UNLOCK()     WM_UNLOCK()

#define TP_MAX_STR_LEN 255

/* ����0˵��������������һ�� */
I32 TP_CompareRect(GUI_RECT rect1, GUI_RECT rect2);

/* ��俹��ݵ�Բ�Ǿ��� */
void TP_AA_FillRoundedRect(I32 x0, I32 y0, I32 x1, I32 y1, I32 r);


#endif

