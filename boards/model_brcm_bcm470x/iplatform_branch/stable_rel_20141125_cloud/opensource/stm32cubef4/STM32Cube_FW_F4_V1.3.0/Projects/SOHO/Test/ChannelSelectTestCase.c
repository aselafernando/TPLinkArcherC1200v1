#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "main.h"
#include "Bsp.h"

#include "TP_Widget.h"
#include "TP_ChannelSelect.h"

#include "TP_Bmp.h"
#include "TP_Button.h"
#include "TP_Menu.h"
#include "TP_Text.h"
#include "TP_ActionBar.h"
#include "TP_KeyBoard.h"
#include "TP_Indicator.h"
#include "TP_Switch.h"

#include "EDIT.h"
#include "cmsis_os.h"

#define DELAY {GUI_Exec();  osDelay(50);}
#define DELAY_VALUE 1500
#define TP_CHANNELSELECT_DEFAULT_TEXT_FONT         &GUI_Font24_ASCII

/* 下面开始是测试代码 */
static void Test_cbBkWindow(WM_MESSAGE * pMsg) {
	WM_HMEM hWin;
	static char showMsg[64] = "TP_ChannelSelect - Sample";

	int NCode;
    int times = 0;

	hWin = pMsg->hWin;
    switch (pMsg->MsgId) 
    {
	    case WM_PAINT:
	    GUI_SetBkColor(GUI_LIGHTGRAY);
	    GUI_Clear();
	    GUI_SetColor(GUI_RED);
	    GUI_SetFont(&GUI_Font24_ASCII);
	    GUI_DispStringHCenterAt(showMsg, 160, 150);
	    break;
	
	    case WM_NOTIFY_PARENT:
		    NCode = pMsg->Data.v;
		    switch (NCode) {
		    case WM_NOTIFICATION_CLICKED:
			    break;
		    case WM_NOTIFICATION_RELEASED:
			    break;
		    case WM_NOTIFICATION_VALUE_CHANGED:
                times ++;
			    break;
		    }
		    break;
#if 1
	    case WM_SET_FOCUS:
		    if (pMsg->Data.v)
		    {
			    pMsg->Data.v = 0;
		    }
		    WM_InvalidateWindow(hWin);
		    break;
#endif
        default:
            WM_DefaultProc(pMsg);
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
    TP_CHANNELSELECT_Handle hChannelSelect;
    TP_CHANNELSELECT_Handle hChannelSelectEx;

    WM_HMEM hTest;
  

    int cont = 0;	
    if (cont == 0)
    {	
	hTest = WM_CreateWindow(0, 0, LCD_GetXSize(), LCD_GetYSize(), \
					WM_CF_SHOW | WM_CF_MEMDEV , Test_cbBkWindow, 0);

        /*创建控件*/
        hChannelSelect   = TP_ChannelSelect_Create(30, 60, 201, 35, hTest, 0 ,NULL, 0, 0); 
        hChannelSelectEx = TP_ChannelSelect_Create(260, 60, 201, 35, hTest, 0 ,NULL, 0, 0); 
        /*设置左边文字*/
        TP_ChannelSelect_SetText(hChannelSelect, 1, "2.4GHz");
        /*设置右边文字*/
        TP_ChannelSelect_SetText(hChannelSelect, 2, "5GHz");
        TP_ChannelSelect_SetText(hChannelSelectEx, 1, "CHAN-1");
        TP_ChannelSelect_SetText(hChannelSelectEx, 2, "CHAN-2");
    }
    while (1) 
    {
        GUI_Delay(100);
        GUI_Delay(1000);
        /*设置选中背景以及边框颜色*/
        TP_ChannelSelect_SetColorIndex(hChannelSelect, 0, GUI_BLACK);
        WM_InvalidateWindow(hChannelSelect);
        GUI_Delay(1000);
        /*设置未选中文字颜色*/
        TP_ChannelSelect_SetColorIndex(hChannelSelect, 1, GUI_CYAN);
        WM_InvalidateWindow(hChannelSelect);
        GUI_Delay(1000);
        /*设置选中文字颜色*/
        TP_ChannelSelect_SetColorIndex(hChannelSelect, 2, GUI_YELLOW);
        WM_InvalidateWindow(hChannelSelect);
        GUI_Delay(1000);
        /*设置文字字体*/
        TP_ChannelSelect_SetFont(hChannelSelect, GUI_DEFAULT_FONT);
        WM_InvalidateWindow(hChannelSelect);
        GUI_Delay(1000);
        TP_ChannelSelect_SetText(hChannelSelect, 1, "Pad");
        TP_ChannelSelect_SetText(hChannelSelect, 2, "Pad");
        WM_InvalidateWindow(hChannelSelect);
        GUI_Delay(1000);

        TP_ChannelSelect_SetFont(hChannelSelect, TP_CHANNELSELECT_DEFAULT_TEXT_FONT);
        WM_InvalidateWindow(hChannelSelect);
        GUI_Delay(1000);
        /*设置当前选中项1或2*/
        TP_ChannelSelect_SetState(hChannelSelect, 1);
        WM_InvalidateWindow(hChannelSelect);
        GUI_Delay(1000);
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