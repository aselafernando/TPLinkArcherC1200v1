/******************************************************************************
*
* Copyright (c) 2010 TP-LINK Technologies CO.,LTD.
* All rights reserved.
*
* FILE NAME  :   guiQuickSetup.h
* VERSION    :   1.0
* DESCRIPTION:   QuickSetup中时间和地区设置的界面.
*
* AUTHOR     :   Huangwenzhong <Huangwenzhong@tp-link.net>
* CREATE DATE:   11/19/2014
*
* HISTORY    :
* 01   11/19/2014  Huangwenzhong     Create.
*
******************************************************************************/
#include <string.h>
#include "Common.h"
#include "GUI_QuickSetupTime.h"
#include "GUI_QuickSetup.h"
#include "TP_ActionBar.h"
#include "TP_Button.h"
#include "TP_Bmp.h"
#include "TP_Text.h"
 
#define GUI_QS_TIME_ACTION_BAR_ID			(1000)
#define GUI_QS_TIME_ACTION_BAR_BACK_ID		(1001)
#define GUI_QS_TIME_ACTION_BAR_TITLE_ID		(1002)
#define GUI_QS_TIME_ACTION_BAR_SUB_TITLE_ID	(1003)
//#define GUI_QS_TIME_ACTION_BAR_TITLE_ID		(1004)
#define GUI_QS_TIME_ACTION_BAR_NEXT_ID		(1005)

static void GUI_QsTimeCallback(WM_MESSAGE * pMsg)
{
	WM_HWIN hWin;
	CHAR showMsg[64];
	GUI_RECT winRect;

	I32 winXSize, winYSize;
	TP_ACTIONBAR_HANDLE hActionBar;
	TP_ACTIONBAR_MSG *pAbMsg;

	static I32 id = 0;
	WM_PID_STATE_CHANGED_INFO * pPidState = NULL;
	
	TP_AB_ICON backIcon = 
	{
		GUI_QS_TIME_ACTION_BAR_BACK_ID,
		{&bmBackBmp, &bmBackBmp},
	};

	TP_AB_TITLE subTitle = 
	{
		GUI_QS_TIME_ACTION_BAR_SUB_TITLE_ID,
		STR_QS_REGION_TIME_ZONE,
		GUI_QS_SUB_TITLE_FONT
	};

	TP_AB_TITLE title = 
	{
		GUI_QS_TIME_ACTION_BAR_TITLE_ID,
		STR_QS_TITLE" - ",
		GUI_QS_TITLE_FONT
	};

	TP_AB_BUTTON nextButton = 
	{
		GUI_QS_TIME_ACTION_BAR_NEXT_ID,
		"",
		{&bmNextBmp, &bmNextBmp},
		NULL,
	};

	hWin = pMsg->hWin;
	WM_GetWindowRectEx(hWin, &winRect);
	GUI_MoveRect(&winRect, -winRect.x0, -winRect.y0);
	winXSize = winRect.x1 - winRect.x0 + 1;
	winYSize = winRect.y1 - winRect.y0 + 1;
	
	switch (pMsg->MsgId) 
	{
	case WM_CREATE:
		
		hActionBar = TP_ActionBar_Create(winRect.x0, winRect.y0, winXSize, UI_ACTION_BAR_HEIGHT, 
							hWin, WM_CF_MEMDEV | WM_CF_SHOW, GUI_QS_TIME_ACTION_BAR_ID, NULL, 0);
		TP_ActionBar_SetUpIcon(hActionBar, backIcon);
		TP_ActionBar_SetSubTitle(hActionBar, subTitle);
		TP_ActionBar_SetTitle(hActionBar, title);
		TP_ActionBar_AddButton(hActionBar, nextButton);

		GUI_QsCreateProcessBar(hWin, winRect.x0, winRect.y0 + UI_ACTION_BAR_HEIGHT,
											QS_PROCESS_BAR_DEFAULT_STEP_NUM, 1);

		break;
	case WM_PAINT:
		GUI_SetBkColor(GUI_WHITE);
		GUI_Clear();
	 	GUI_SetColor(GUI_BLACK);
        GUI_SetTextMode(GUI_TEXTMODE_TRANS);
        GUI_SetFont(GUI_DEFAULT_FONT);
		sprintf(showMsg, "button id %d is clicked", id);
		GUI_DispStringInRect(showMsg, &winRect, GUI_TA_HCENTER | GUI_TA_VCENTER);
		break;

	case WM_PID_STATE_CHANGED:
        pPidState = (WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p);
        if (pPidState)
        {
            id++;
			WM_InvalidateWindow(hWin);
        }
        break;

	case WM_TP_ACTION_BAR:
		pAbMsg = (TP_ACTIONBAR_MSG *)pMsg->Data.p;

		if (pAbMsg && pAbMsg->MsgType == TP_AB_BUTTON_ON_RELEASED)
		{
			id = pAbMsg->id;
			WM_InvalidateWindow(hWin);
		}
		break;
	
	default:
		WM_DefaultProc(pMsg);
		break;
	}
}


BOOL GUI_QsTimeCreate(WM_HWIN hWin, I32 x, I32 y)
{
	WM_CreateWindowAsChild(x, y, LCD_X_SIZE, LCD_Y_SIZE, hWin, 
				WM_CF_SHOW | WM_CF_MEMDEV, GUI_QsTimeCallback, 0);

	return TRUE;
}

