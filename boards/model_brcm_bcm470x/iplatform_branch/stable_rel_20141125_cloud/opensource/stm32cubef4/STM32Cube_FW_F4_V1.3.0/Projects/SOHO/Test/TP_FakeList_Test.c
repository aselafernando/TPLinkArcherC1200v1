#include "Bsp.h"
#include "cmsis_os.h"
#include "main.h"
#include "TP_FakeList.h"

#include "TP_Button.h"

#define _TP_DEBUG_
#undef _TP_DEBUG_

extern GUI_CONST_STORAGE GUI_FONT GUI_FontF16ASCIICourier;
extern GUI_CONST_STORAGE GUI_BITMAP bmDP_TickBmp;
extern GUI_CONST_STORAGE GUI_BITMAP bmFAKELIST_RightArrowBmp;
extern GUI_CONST_STORAGE GUI_BITMAP bmFAKELIST_SwitchOnBmp;

#ifdef _TP_DEBUG_
#define TP_BUTTON_DG_0 (WM_USER + 0x59)

static void _cbItem0(WM_MESSAGE * pMsg)
{
	WM_HWIN hWin;
	int id;
	int code;

	hWin = pMsg->hWin;
	switch (pMsg->MsgId)
	{
	case WM_PAINT:
		GUI_SetBkColor(GUI_BLUE);
		GUI_Clear();
		TP_Button_Create(20, 20, 80, 40, hWin, WM_CF_SHOW, NULL, 0, TP_BUTTON_DG_0);
		break;

	case WM_NOTIFY_PARENT:
		code = pMsg->Data.v;
		id = WM_GetId(pMsg->hWinSrc);
		switch (code)
		{
		case WM_NOTIFICATION_RELEASED:
			switch (id)
			{
			case TP_BUTTON_DG_0:
				WM_DeleteWindow(hWin);
				break;
			}
			break;
		}
		break;

	case WM_NOTIFICATION_CHILD_DELETED:
		break;

	default:
		WM_DefaultProc(pMsg);
	}
}

static void _Click0(WM_HWIN hWin)
{
	WM_CreateWindow(0, 0, LCD_GetXSize(), LCD_GetYSize(), WM_CF_SHOW, _cbItem0, 0);
}
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
	TP_FAKELIST_Handle hWin;
	int xSize;
	int ySize;

	/*
		注意在使用FAKELIST控件的时候，必须先开启MOTION
	*/
	
	WM_MOTION_Enable(1);

	xSize = LCD_GetXSize();
	ySize = LCD_GetYSize();
	WM_SetSize(WM_HBKWIN, xSize, ySize);
	WM_SetCallback(WM_HBKWIN, _cbBkWindow);

	/*
		设置默认字体、字体颜色
	*/
	TP_FAKELIST_SetDefaultFont(&GUI_FontF16ASCIICourier);
	TP_FAKELIST_SetDefaultFontColor(GUI_RED);

	/*
		设置默认头部、底部区域高度
	*/
	TP_FAKELIST_SetDefaultFooterHeight(40);
	TP_FAKELIST_SetDefaultHeadHeight(40);

	/*
		设置FAKELIST整个控件的背景颜色
	*/
	//TP_FAKELIST_SetBkColor(GUI_BLUE);

	/*
		设置条目背景颜色、头部和底部、内容中间区域的背景颜色
		请注意内容这4个颜色会覆盖掉FAKELIST设置的整体的背景颜色
	*/
	//TP_FAKELIST_SetDefaultBodyItemBkColor(GUI_BLUE);
	//TP_FAKELIST_SetDefaultHeadBkColor(GUI_RED);
	//TP_FAKELIST_SetDefaultFooterBkColor(GUI_RED);
	//TP_FAKELIST_SetDefaultBodyBkColor(GUI_RED);

	/*
		设置条目的高度、宽度，注意宽度不要超过body的宽度
	*/
	//TP_FAKELIST_SetDefaultItemHeight(50);
	//TP_FAKELIST_SetDefaultItemWidth(300);

	/*
		设置右侧滚动条的颜色、高度、宽度
	*/
	TP_FAKELIST_SetDefaultScrollColor(GUI_RED);
	//TP_FAKELIST_SetDefaultScrollHeight(30);
	//TP_FAKELIST_SetDefaultScrollWidth(5);
	
	/*
		设置body左边留白宽度，注意必须大于要设置的图片宽度
	*/
	//TP_FAKELIST_SetDefaultBodyLeftBlankWidth(20);
	
	/*
		FAKELIST Create API
	*/
	hWin = TP_FAKELIST_Create(20, 20, 400, 200, WM_HBKWIN, WM_CF_SHOW, NULL, 0, 0);

	/*
		添加条目，注意其中第三个和第六个参数，留做后续扩展使用，目前都设为0
		sTextL, pBitmapL, sTextR, pBitmapR 分别为左边的图片、文字，右边的图片、文字
		func为其响应动作，一般为一个窗口, item0中的_Click0为一个示例
	*/
	TP_FAKELIST_AddItem(hWin, 0, 0, "item0", 0, 0, "right0", 0, &_Click0);
	TP_FAKELIST_AddItem(hWin, 0, 0, "item1", 0, 0, "right1", &bmFAKELIST_RightArrowBmp, NULL);
	TP_FAKELIST_AddItem(hWin, 0, 0, "item2", 0, 0, NULL, &bmFAKELIST_SwitchOnBmp, NULL);
	TP_FAKELIST_AddItem(hWin, 0, 0, "item3", 0, 0, NULL, &bmFAKELIST_SwitchOnBmp, NULL);
	TP_FAKELIST_AddItem(hWin, 0, 0, "item4", 0, 0, NULL, &bmFAKELIST_SwitchOnBmp, NULL);
	TP_FAKELIST_AddItem(hWin, 0, 0, "item5", 0, 0, NULL, &bmFAKELIST_SwitchOnBmp, NULL);
	TP_FAKELIST_AddItem(hWin, 0, 0, "item6", &bmFAKELIST_RightArrowBmp, 0, NULL, &bmFAKELIST_SwitchOnBmp, NULL);

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
