#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "main.h"
#include "Bsp.h"

#include "TP_Widget.h"

#include "TP_Bmp.h"
#include "TP_Button.h"
#include "TP_Menu.h"
#include "TP_Text.h"
#include "TP_ActionBar.h"
#include "TP_KeyBoard.h"

#include "EDIT.h"
#include "cmsis_os.h"

#include "GUI_QuickSetup.h"
#include "GUI_QuickSetupTime.h"


/* 下面开始是测试代码 */
static void Test_cbBkWindow(WM_MESSAGE * pMsg) 
{
	TP_MENU_Handle	hWin;
	static U8 pressed = 0; 
	static char showMsg[64];
	GUI_PID_STATE *pState = NULL;
    WM_PID_STATE_CHANGED_INFO *pPidChanged = NULL;
	GUI_RECT winRect;
	EDIT_Handle hEdit;
	static int xPos = 0, yPos = 0;
	int cont = 1;
	static int count = 0;

	hWin = pMsg->hWin;
  	WM_GetWindowRectEx(hWin, &winRect);
	GUI_MoveRect(&winRect, -winRect.x0, -winRect.y0);

	switch (pMsg->MsgId) 
	{
  	case WM_CREATE:
		if (cont == 1)
		{
			hEdit = EDIT_CreateAsChild(winRect.x0 + 10, winRect.y0 + 50, 
						100, 30, hWin, 10000, WM_CF_SHOW | WM_CF_MEMDEV, 20);
			EDIT_EnableBlink(hEdit, 300, 1);
			WM_SetFocus(hEdit);

			TP_KeyBoard_Create(hWin);
		}
		break;
	case WM_PAINT:
		if (cont == 1)
		{
			GUI_SetBkColor(GUI_WHITE);
			GUI_Clear();
			GUI_SetColor(GUI_BLACK);
			GUI_SetFont(&GUI_Font24_ASCII);
			GUI_SetTextMode(GUI_TEXTMODE_TRANS);
			memset(showMsg, 0, sizeof(showMsg));
			sprintf(showMsg, "pressed=%u,tsCount=%u,xpos=%d,ypos=%d", pressed, count, xPos, yPos);
			GUI_DispStringHCenterAt(showMsg, 240, 5);
		}
		break;

	case WM_TOUCH:
		//count++;
		pState = (GUI_PID_STATE *)pMsg->Data.p;
		if (pState && (pState->Pressed != pressed))
		{
			xPos = pState->x;
			yPos = pState->y;
			pressed = pState->Pressed;
			WM_InvalidateWindow(hWin);
		}		
		break;
	case WM_PID_STATE_CHANGED:
		pPidChanged = (WM_PID_STATE_CHANGED_INFO *)pMsg->Data.p;
		count++;
		
		if (pPidChanged)
		{
			
		}
		break;
  	default:
    	WM_DefaultProc(pMsg);
		break;
	}
}

static void Time_cbBkWindow(WM_MESSAGE * pMsg) 
{
	TP_MENU_Handle	hWin;

	GUI_PID_STATE *pState = NULL;
    WM_PID_STATE_CHANGED_INFO *pPidChanged = NULL;
	GUI_RECT winRect;

	static int count = 0;

	hWin = pMsg->hWin;
  	WM_GetWindowRectEx(hWin, &winRect);
	GUI_MoveRect(&winRect, -winRect.x0, -winRect.y0);

	switch (pMsg->MsgId) 
	{
	case WM_PAINT:
		GUI_SetBkColor(GUI_WHITE);
		GUI_Clear();

		break;

	case WM_TOUCH:
		//count++;
		pState = (GUI_PID_STATE *)pMsg->Data.p;
		if (pState)
		{
			WM_InvalidateWindow(hWin);
		}		
		break;
	case WM_PID_STATE_CHANGED:
		pPidChanged = (WM_PID_STATE_CHANGED_INFO *)pMsg->Data.p;
		count++;
		
		if (pPidChanged)
		{
			WM_InvalidateWindow(hWin);
		}
		break;
  	default:
    	WM_DefaultProc(pMsg);
		break;
	}
}


static void Start_Thread(void const *argument)
{
	while(1)
	{
		osDelay(100);
	}
}

static void GUI_Thread(void const *argument)
{  
    int cont = 0;
	WM_HWIN hTest;
	
	if (cont == 1)
	{
		WM_CreateWindow(0, 0, LCD_GetXSize(), LCD_GetYSize(),
                                    WM_CF_SHOW | WM_CF_MEMDEV, Test_cbBkWindow, 0);
	}
	else
	{
		hTest= WM_CreateWindow(0, 0, LCD_GetXSize(), LCD_GetYSize(),
                                    WM_CF_SHOW | WM_CF_MEMDEV, Time_cbBkWindow, 0);
		GUI_QsTimeCreate(hTest, 0, 0);
	}
 
	  
	while(1)
	{	
		GUI_Exec();       
        osDelay(50);	
	}
}

int main(void)
{
	BspInit();
	osThreadDef(t1, Start_Thread, osPriorityNormal, 0, 15 * configMINIMAL_STACK_SIZE);
	osThreadDef(t2, GUI_Thread, osPriorityHigh, 0, 2048);

	osThreadCreate (osThread(t1), NULL);
	osThreadCreate (osThread(t2), NULL);

	osKernelStart (NULL, NULL);

	for(;;);
}




#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line)
{
	while (1)
	{
	}
}
#endif


