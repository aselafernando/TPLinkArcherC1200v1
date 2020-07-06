#include "Bsp.h"
#include "cmsis_os.h"
#include "main.h"
#include "TP_Dropdown.h"

#define _TP_DEBUG_
#undef _TP_DEBUG_

#ifdef _TP_DEBUG_

static void _cbBkWindow(WM_MESSAGE * pMsg)
{
	switch (pMsg->MsgId) {
	case WM_PAINT:
		GUI_SetBkColor(GUI_BLUE);
		GUI_Clear();
		break;

	default:
		WM_DefaultProc(pMsg);
	}
}

void MainTask()
{
	TP_DROPDOWN_Handle hWin;
	int xSize;
	int ySize;
	char sText[255];
	int i = 0;
	int itemNum = 8;

	/*
		注意在使用DROPDOWN控件的时候，必须先开启MOTION
	*/
	
	WM_MOTION_Enable(1);

	xSize = LCD_GetXSize();
	ySize = LCD_GetYSize();
	WM_SetSize(WM_HBKWIN, xSize, ySize);
	WM_SetCallback(WM_HBKWIN, _cbBkWindow);

	/*
		设置背景颜色
	*/
	//TP_DROPDOWN_SetDefaultBkColor(GUI_RED);

	/*
		设置头部、底部颜色以及高度
	*/
	//TP_DROPDOWN_SetDefaultHeadBkColor(GUI_RED);
	//TP_DROPDOWN_SetDefaultHeadHeight(10);
	//TP_DROPDOWN_SetDefaultFooterBkColor(GUI_GREEN);
	//TP_DROPDOWN_SetDefaultFooterHeight(20);

	/*
		设置字体以及字体颜色
	*/
	//TP_DROPDOWN_SetDefaultFont(const GUI_FONT * pFont);
	//TP_DROPDOWN_SetDefaultFontColor(GUI_RED);

	/*
		设置条目区域背景颜色
	*/
	//TP_DROPDOWN_SetDefaultItemBkColor(GUI_BLUE);

	/*
		设置条目的高度、宽度
	*/
	//TP_DROPDOWN_SetDefaultItemHeight(30);
	//TP_DROPDOWN_SetDefaultItemWidth(400);

	/*
		设置条目左边空白区域的宽度
	*/
	//TP_DROPDOWN_SetDefaultLeftBlankWidth(5);
	
	/*
		设置条目各个边距值
		字符显示的起始位置是左边空白的宽度+BODER_LEFT
		图片显示的结束位置是条目的宽度-BODER_RIGHT
	*/
	//TP_DROPDOWN_SetDefaultBorderSize(TP_BORDER_LEFT, 80);

	/*
		设置滚动条的宽度、高度、颜色
	*/
	//TP_DROPDOWN_SetDefaultScrollWidth(3);
	//TP_DROPDOWN_SetDefaultScorllHeight(80);
	//TP_DROPDOWN_SetDefaultScorllColor(GUI_BLUE);

	hWin = TP_DROPDOWN_Create(0, 20, 480, 100, WM_HBKWIN, WM_CF_SHOW, NULL, 0, 0);

	/*
		添加条目，第二个参数值是左边显示的字符串，第三个参数是条目id
	*/
	for (i = 0; i < itemNum; i++)
	{
		memset(sText, 0, 255);
		sprintf(sText, "item%d", i);
		TP_DROPDOWN_AddItem(hWin, sText, i);
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

