#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "main.h"
#include "Bsp.h"

#include "TP_Widget.h"
#include "TP_Toast.h"

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
/* ���濪ʼ�ǲ��Դ��� */
static void Test_cbBkWindow(WM_MESSAGE * pMsg) {
	WM_HMEM hWin;
	static char showMsg[64] = "TP_Indicator - Sample";

	int NCode;

    
	hWin = pMsg->hWin;
    switch (pMsg->MsgId) 
    {
	case WM_PAINT:
        GUI_SetColor(GUI_WHITE);
        GUI_SetBkColor(GUI_LIGHTBLUE);
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
    TP_TOAST_Handle hToast;
    WM_HMEM hTest;

    int cont = 0;	
    if (cont == 0)
    {	
	hTest = WM_CreateWindow(0, 0, LCD_GetXSize(), LCD_GetYSize(), \
					WM_CF_SHOW | WM_CF_MEMDEV, Test_cbBkWindow, 0);

      
     }
 
     while (1)
     {
        /*�����ؼ�����ʱ��ʱ���Զ�ɾ��*/
        hToast = TP_Toast_Create(50,100,200,100, hTest, 0 ,NULL, NULL, 100, 0);
        /*����������ɫ*/
        TP_Toast_SetColorIndex(hToast, 1, GUI_RED);
        /*���ñ�������ɫ*/
        TP_Toast_SetColorIndex(hToast,0,GUI_YELLOW);
        /*���ñ������͸��ֵ*/
        TP_Toast_SetToastAlpha (hToast, 0, 200);
        /*������Ϣ�����ʱ��*/
        TP_Toast_SetLastTime(hToast, 2000);
        /*������Ϣ�򵭳�ʱ��*/
        TP_Toast_SetFadeTime(hToast, 1000);
        /*�����ı�*/
        TP_Toast_SetText(hToast, "Hello!");
        GUI_Delay(4000);
#if 0
        hToast = TP_Toast_Create(50,100,200,100, hTest, 0 ,NULL, NULL, 100, 0);
        TP_Toast_SetLastTime(hToast, 8000);

        TP_Toast_SetText(hToast, "This is help message--1!");   
        /*�������ֶ��뷽ʽ*/
        TP_Toast_SetTextAlign(hToast, GUI_TA_HCENTER|GUI_TA_VCENTER);
        WM_InvalidateWindow(hToast);
        GUI_Delay(2000);

        
        TP_Toast_SetText(hToast, "This is help message!");   
        TP_Toast_SetTextAlign(hToast, GUI_TA_LEFT|GUI_TA_BOTTOM);
        WM_InvalidateWindow(hToast);
        GUI_Delay(2000);

        TP_Toast_SetText(hToast, "Hello!");
        TP_Toast_SetFadeTime(hToast, 500);
        TP_Toast_SetColorIndex(hToast,0,GUI_YELLOW);
        TP_Toast_SetTextAlign(hToast, GUI_TA_HCENTER|GUI_TA_VCENTER);
        WM_InvalidateWindow(hToast);
        GUI_Delay(2000);

        TP_Toast_SetFont(hToast, GUI_DEFAULT_FONT);
        TP_Toast_SetText(hToast, "Hello Jack KALUACK!");
        TP_Toast_SetColorIndex(hToast,1,GUI_GREEN);
        TP_Toast_SetFadeTime(hToast, 1000);
        /*���ñ�����yuan�ǰ뾶*/
        TP_Toast_SetRadius(hToast,20);
        WM_InvalidateWindow(hToast);
        GUI_Delay(5000);
#endif
               
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