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
#include "TP_Indicator.h"

#include "EDIT.h"
#include "cmsis_os.h"

#define DELAY {GUI_Exec();  osDelay(50);}
#define DELAY_VALUE 1500
/* 下面开始是测试代码 */
static void Test_cbBkWindow(WM_MESSAGE * pMsg) {
	WM_HMEM hWin;
	static char showMsg[64] = "TP_Indicator - Sample";

	int NCode;

    
	hWin = pMsg->hWin;
    switch (pMsg->MsgId) 
    {
	    case WM_PAINT:
                    GUI_SetColor(GUI_WHITE);
                    GUI_SetBkColor(GUI_BLACK);
                    GUI_Clear();
                    GUI_SetFont(&GUI_Font24_ASCII);
                    GUI_SetTextMode(GUI_TEXTMODE_TRANS);
                    GUI_AA_SetFactor(4);
                    GUI_DispStringHCenterAt(showMsg, 240, 200);   
                    break;
	
	    case WM_NOTIFY_PARENT:
		    NCode = pMsg->Data.v;
		    switch (NCode) {
		    case WM_NOTIFICATION_CLICKED:
			    break;
		    case WM_NOTIFICATION_RELEASED:
			    break;
		    case WM_NOTIFICATION_VALUE_CHANGED:
			    break;
		    }
		    break;

	    case WM_SET_FOCUS:
		    if (pMsg->Data.v)
		    {
			    pMsg->Data.v = 0;
		    }
		    WM_InvalidateWindow(hWin);
		    break;
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

    TP_INDICATOR_Handle hIndicatorEx;

    WM_HMEM hTest;

    int cont = 0;	
    if (cont == 0)
    {	
	hTest = WM_CreateWindow(0, 0, LCD_GetXSize(), LCD_GetYSize(), \
					WM_CF_SHOW | WM_CF_MEMDEV, Test_cbBkWindow, 0);

        /*创建控件*/
        hIndicatorEx = TP_Indicator_Create(205,230,70,30, hTest, 0 ,NULL, 0, 0);
     }
       
     while(1)
     {	

        /*设置背景框透明，一般全透明*/
        TP_Indicator_SetIndicatorAlpha(hIndicatorEx,2,255);

        /*设置当前指示的页面序号,从1开始*/
        TP_Indicator_SetPage(hIndicatorEx,1);
        GUI_Delay(DELAY_VALUE);
        TP_Indicator_SetPage(hIndicatorEx,2);
        GUI_Delay(DELAY_VALUE);
        TP_Indicator_SetPage(hIndicatorEx,3);
        GUI_Delay(DELAY_VALUE);
        
        
        TP_Indicator_SetIndicatorAlpha(hIndicatorEx,2,0);
        TP_Indicator_SetPage(hIndicatorEx,1);
        GUI_Delay(DELAY_VALUE);
        TP_Indicator_SetPage(hIndicatorEx,2);
        /*设置背景框办透明*/
        TP_Indicator_SetIndicatorAlpha(hIndicatorEx,2,100);
        GUI_Delay(DELAY_VALUE);
        TP_Indicator_SetPage(hIndicatorEx,3);
        /*设置背景框不透明*/
        TP_Indicator_SetIndicatorAlpha(hIndicatorEx,2,255);
        GUI_Delay(DELAY_VALUE);

        TP_Indicator_SetIndicatorAlpha(hIndicatorEx,2,0);
        /*设置背景框颜色*/
        TP_Indicator_SetColorIndex(hIndicatorEx,2,GUI_RED);
        /*设置选中（高亮）页面指示器的颜色*/
        TP_Indicator_SetColorIndex(hIndicatorEx,0,GUI_GREEN);
         /*设置未选中页面指示器的颜色*/
        TP_Indicator_SetColorIndex(hIndicatorEx,1,GUI_BLUE);


         /*设置指示器的间距（边缘之间的距离）*/
        TP_Indicator_SetSpace(hIndicatorEx,18);
        TP_Indicator_SetPage(hIndicatorEx,1);
        GUI_Delay(DELAY_VALUE);
        TP_Indicator_SetPage(hIndicatorEx,2);
        GUI_Delay(DELAY_VALUE);
        TP_Indicator_SetPage(hIndicatorEx,3);
        GUI_Delay(DELAY_VALUE);

         /*设置指示器的直径*/
        TP_Indicator_SetSize(hIndicatorEx,10);
        TP_Indicator_SetPage(hIndicatorEx,1);
        GUI_Delay(DELAY_VALUE);
        TP_Indicator_SetPage(hIndicatorEx,2);
        GUI_Delay(DELAY_VALUE);
        TP_Indicator_SetPage(hIndicatorEx,3);
        GUI_Delay(DELAY_VALUE);

         /*设置指示器的个数，即页面总数*/
        TP_Indicator_SetNum(hIndicatorEx,2);
        TP_Indicator_SetPage(hIndicatorEx,1);
        GUI_Delay(DELAY_VALUE);
        TP_Indicator_SetPage(hIndicatorEx,2);
        GUI_Delay(DELAY_VALUE);
        TP_Indicator_SetPage(hIndicatorEx,3);
        GUI_Delay(DELAY_VALUE);
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