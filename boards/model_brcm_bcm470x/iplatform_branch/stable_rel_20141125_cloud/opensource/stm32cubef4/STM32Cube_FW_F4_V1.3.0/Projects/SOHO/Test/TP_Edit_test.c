#include "Bsp.h"
#include "cmsis_os.h"
#include "main.h"
#include "TP_Edit.h"

#define _TP_DEBUG_
#undef _TP_DEBUG_

extern GUI_CONST_STORAGE GUI_FONT GUI_FontF16ASCIICourier;

char aText[255];

#ifdef _TP_DEBUG_
static void _cbBkWindow(WM_MESSAGE * pMsg)
{
	switch (pMsg->MsgId) {
	case WM_PAINT:
		GUI_SetBkColor(GUI_WHITE);
		GUI_Clear();
		GUI_SetColor(GUI_BLUE);
		GUI_DispStringAt(aText, 20, 80);
		break;

	default:
		WM_DefaultProc(pMsg);
	}
}

void MainTask()
{
	TP_EDIT_Handle hWin;

	WM_SetCallback(WM_HBKWIN, _cbBkWindow);
        
        /*
          设置默认字体，注意，Edit输入框必须使用等宽字体
        */
        
	TP_EDIT_SetDefaultFont(&GUI_FontF16ASCIICourier);
        /*
          设置默认背景颜色，正常情况下 or 有焦点的
        */
	//TP_EDIT_SetDefaultBkColor(TP_EDIT_BK_NORMAL, GUI_WHITE);
	//TP_EDIT_SetDefaultBkColor(TP_EDIT_BK_FOCUS, GUI_WHITE);
        
        /*
          设置默认边框颜色
        */
	//TP_EDIT_SetDefaultBorderColor(GUI_WHITE);
        
        /*
          设置字体颜色
        */
        TP_EDIT_SetDefaultTextColor(0, GUI_RED);

		/*
			设置允许输入的最大字符个数
		*/
		TP_EDIT_SetDefaultMaxInputCharNum(25);
        
        
        /*
          Edit create API, 如果不知道游标的xsize和ysize,可以使用0，0，在使用0，0
          的时候，游标窗口的xsize 和 ysize则使用默认游标图片的size
        */
	hWin = TP_EDIT_Create(20, 20, 240, 42, WM_HBKWIN, WM_CF_SHOW, NULL, 0, 0, 0, 0);
	//TP_EDIT_SetBorderColor(hWin, GUI_WHITE);
	//TP_EDIT_SetBkColor(hWin, TP_EDIT_BK_NORMAL, GUI_WHITE);
	//TP_EDIT_SetBkColor(hWin, TP_EDIT_BK_FOCUS, GUI_WHITE);

	/*
		获取EDIT输入框文本
	*/
	TP_EDIT_GetText(hWin, aText, 20);
	WM_InvalidateWindow(WM_HBKWIN);
	
	/*
		设置光标闪烁
	*/
	//TP_EDIT_EnableBlink(hWin, 1000, 1);

	//GUI_Delay(10000);
	//TP_EDIT_EnableBlink(hWin, 1000, 0);
	//TP_EDIT_SetTextColor(hWin, TP_EDIT_FONT_ENABLED, GUI_RED);
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
    MainTask();

    while(1)
    {
        GUI_Exec();
        osDelay(50);
    }
}

void main(void)
{
    BspInit();
    osThreadDef(t1, Start_Thread, osPriorityNormal, 0, 15 * configMINIMAL_STACK_SIZE);
    osThreadDef(t2, GUI_Thread, osPriorityHigh, 0, 2048);

    osThreadCreate (osThread(t1), NULL);
    osThreadCreate (osThread(t2), NULL);

    osKernelStart (NULL, NULL);

    for(;;);
}
#endif

