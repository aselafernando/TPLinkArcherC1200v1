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
          ����Ĭ�����壬ע�⣬Edit��������ʹ�õȿ�����
        */
        
	TP_EDIT_SetDefaultFont(&GUI_FontF16ASCIICourier);
        /*
          ����Ĭ�ϱ�����ɫ����������� or �н����
        */
	//TP_EDIT_SetDefaultBkColor(TP_EDIT_BK_NORMAL, GUI_WHITE);
	//TP_EDIT_SetDefaultBkColor(TP_EDIT_BK_FOCUS, GUI_WHITE);
        
        /*
          ����Ĭ�ϱ߿���ɫ
        */
	//TP_EDIT_SetDefaultBorderColor(GUI_WHITE);
        
        /*
          ����������ɫ
        */
        TP_EDIT_SetDefaultTextColor(0, GUI_RED);

		/*
			�����������������ַ�����
		*/
		TP_EDIT_SetDefaultMaxInputCharNum(25);
        
        
        /*
          Edit create API, �����֪���α��xsize��ysize,����ʹ��0��0����ʹ��0��0
          ��ʱ���α괰�ڵ�xsize �� ysize��ʹ��Ĭ���α�ͼƬ��size
        */
	hWin = TP_EDIT_Create(20, 20, 240, 42, WM_HBKWIN, WM_CF_SHOW, NULL, 0, 0, 0, 0);
	//TP_EDIT_SetBorderColor(hWin, GUI_WHITE);
	//TP_EDIT_SetBkColor(hWin, TP_EDIT_BK_NORMAL, GUI_WHITE);
	//TP_EDIT_SetBkColor(hWin, TP_EDIT_BK_FOCUS, GUI_WHITE);

	/*
		��ȡEDIT������ı�
	*/
	TP_EDIT_GetText(hWin, aText, 20);
	WM_InvalidateWindow(WM_HBKWIN);
	
	/*
		���ù����˸
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

