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

#include "WM.h"
#include "GUI.h"
#include "Common.h"
#include "GUI_QuickSetupProcessBar.h"

#define QS_PROCESS_BAR_PASSED_LINE_COLOR	(0x4dcb88)
#define QS_PROCESS_BAR_FUTURE_LINE_COLOR	(0xc2bfb3)
#define QS_PROCESS_BAR_LINE_HEIGHT			(2)
#define QS_PROCESS_BAR_LINE_WIDTH			(72)
#define QS_PROCESS_BAR_HEIGHT				(40)
#define QS_PROCESS_BAR_WIDTH				LCD_X_SIZE

#define QS_PROCESS_BAR_FUTURE_BMP 			&bmQS_BarFutureBmp
#define QS_PROCESS_BAR_NOW_BMP 				&bmQS_BarNowBmp
#define QS_PROCESS_BAR_PASSED_BMP 			&bmQS_BarPassedBmp
#define QS_PROCESS_BAR_FUTURE_BMP_WIDTH 	bmQS_BarFutureBmp.XSize
#define QS_PROCESS_BAR_NOW_BMP_WIDTH 		bmQS_BarNowBmp.XSize
#define QS_PROCESS_BAR_PASSED_BMP_WIDTH 	bmQS_BarPassedBmp.XSize


#define QS_PROCESS_BAR_BK_COLOR				(0xece9de)

typedef struct
{
	I32 totalStep;
	I32 curStep;
	I32 lineWidth;
}QS_PROCESS_BAR_DATA;

static void GUI_QsProcessBarCallback(WM_MESSAGE * pMsg)
{
	WM_HWIN hWin;

	GUI_RECT winRect;
	QS_PROCESS_BAR_DATA data;
	const GUI_BITMAP *pBitmap = NULL;

	I32 winXSize = 0, winYSize = 0;
	I32 barLen = 0;
	I32 xPos = 0, yPos = 0;
	I32 step = 1;
	GUI_COLOR color;

	hWin = pMsg->hWin;
	WM_GetWindowRectEx(hWin, &winRect);
	GUI_MoveRect(&winRect, -winRect.x0, -winRect.y0);
	WM_GetUserData(hWin, (void *)&data, sizeof(QS_PROCESS_BAR_DATA));
	
	winXSize = winRect.x1 - winRect.x0 + 1;
	winYSize = winRect.y1 - winRect.y0 + 1;
	
	switch (pMsg->MsgId) 
	{
	case WM_PAINT:
		/* draw background color */
		GUI_SetBkColor(QS_PROCESS_BAR_BK_COLOR);
		GUI_Clear();

		barLen = (data.totalStep - 1) * data.lineWidth
 				+ (data.curStep - 1) * QS_PROCESS_BAR_PASSED_BMP_WIDTH
 				+ QS_PROCESS_BAR_NOW_BMP_WIDTH
 				+ (data.totalStep - data.curStep) * QS_PROCESS_BAR_FUTURE_BMP_WIDTH
 				;

		xPos = (winXSize - barLen) / 2;

		/* draw pass line */
		for (step = 1; step <= data.totalStep; step++)
		{
			color = QS_PROCESS_BAR_FUTURE_LINE_COLOR;
			if (step < data.curStep)
			{
				pBitmap = QS_PROCESS_BAR_PASSED_BMP;
				color = QS_PROCESS_BAR_PASSED_LINE_COLOR;
			}
			else if (step > data.curStep)
			{
				pBitmap = QS_PROCESS_BAR_FUTURE_BMP;
			}
			else
			{
				pBitmap = QS_PROCESS_BAR_NOW_BMP;
			}

			/* draw circle */
			GUI_SetColor(color);
			yPos = (winYSize - pBitmap->YSize) / 2;
			GUI_DrawBitmap(pBitmap, xPos, yPos);
			xPos += pBitmap->XSize;

			/* draw line if need */
			if (step < data.totalStep)
			{
				yPos = (winYSize - QS_PROCESS_BAR_LINE_HEIGHT) / 2;
				GUI_DrawHLine(yPos, xPos, xPos + data.lineWidth - 1);
				xPos += data.lineWidth;
			}
		}
 		break;	
	default:
		WM_DefaultProc(pMsg);
		break;
	}
}


STATUS GUI_QsCreateProcessBar(WM_HWIN hParent, I32 x, I32 y, I32 totalStep, I32 curStep)
{
	WM_HWIN handle;
	QS_PROCESS_BAR_DATA data;
	if (!WM_IsWindow(hParent) || (totalStep <= 1) || (curStep < 1) || (curStep > totalStep))
	{
		return RET_FAIL;
	}
	handle = WM_CreateWindowAsChild(x, y, QS_PROCESS_BAR_WIDTH, QS_PROCESS_BAR_HEIGHT, hParent,
					WM_CF_SHOW | WM_CF_MEMDEV, GUI_QsProcessBarCallback, sizeof(QS_PROCESS_BAR_DATA));

	if (WM_IsWindow(handle))
	{
		data.curStep = curStep;
		data.totalStep = totalStep;
		data.lineWidth = QS_PROCESS_BAR_LINE_WIDTH;
		WM_SetUserData(handle, (const void *)&data, sizeof(QS_PROCESS_BAR_DATA));
		return RET_SUCCESS;
	}
	else
	{
		return RET_FAIL;
	}
}

