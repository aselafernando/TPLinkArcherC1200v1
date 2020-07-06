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
#include "TP_Switch.h"

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
    WM_HMEM hTest;
    int cont = 0;	
    if (cont == 0)
    {	
	hTest = WM_CreateWindow(0, 0, LCD_GetXSize(), LCD_GetYSize(), \
					WM_CF_SHOW | WM_CF_MEMDEV, Test_cbBkWindow, 0);
        TP_Switch_SetDefaultState(1);
        TP_Switch_Create(100, 100, 46, 30, hTest, 0 ,NULL, 0, 0);
        TP_Switch_SetDefaultState(0);
        TP_Switch_Create(200, 100, 46, 30, hTest, 0 ,NULL, 0, 0);
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