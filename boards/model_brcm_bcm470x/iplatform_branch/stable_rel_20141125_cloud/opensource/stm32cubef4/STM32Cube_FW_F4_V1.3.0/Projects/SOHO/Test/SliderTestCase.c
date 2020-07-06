#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "main.h"
#include "Bsp.h"

#include "TP_Widget.h"
#include "TP_Slider.h"

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

/* 下面开始是测试代码 */
static void Test_cbBkWindow(WM_MESSAGE * pMsg) {
	WM_HMEM hWin;

	int NCode;
        int times = 0;

	hWin = pMsg->hWin;
    switch (pMsg->MsgId) 
    {
	    case WM_PAINT:
                    GUI_Clear();
                    GUI_SetTextMode(GUI_TEXTMODE_TRANS);
                    GUI_SetColor(GUI_WHITE);
                    GUI_SetFont(&GUI_Font24_ASCII);
                    GUI_DispDecAt( TP_Slider_GetValue(WM_GetFirstChild(hWin)),400, 100, 3);
                    break;
	
	    case WM_NOTIFY_PARENT:
		    NCode = pMsg->Data.v;
                    switch (NCode) {
                                case WM_NOTIFICATION_CLICKED:
                                      break;
                                case WM_NOTIFICATION_RELEASED:
                                      break;
                                case WM_NOTIFICATION_VALUE_CHANGED:
                                      WM_InvalidateWindow(hWin);
                                      times ++;
                                      break;
                            }
                            break;
#if 0
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
    WM_HMEM hTest;
    TP_SLIDER_Handle hSlider;

    int cont = 0;	
    if (cont == 0)
    {	
	hTest = WM_CreateWindow(0, 0, LCD_GetXSize(), LCD_GetYSize(), \
					WM_CF_SHOW | WM_CF_MEMDEV , Test_cbBkWindow, 0);
        /*控件的实际长度、宽度已经由UI组确定了，所以窗口尺寸理论上只要大于160,20即可*/
        /*但是窗口的大小直接决定了用户操作的有效区域，所以建议具体调试决定*/
        hSlider =  TP_Slider_Create(30, 220, 200, 40, hTest, 0 ,NULL, 0, 0); 
        /*设置滚动条初始显示的值*/
        /*滚动条值0-100*/
        TP_Slider_SetValue(hSlider, 20);
    }
    while (1) 
    {
        GUI_Delay(100);
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