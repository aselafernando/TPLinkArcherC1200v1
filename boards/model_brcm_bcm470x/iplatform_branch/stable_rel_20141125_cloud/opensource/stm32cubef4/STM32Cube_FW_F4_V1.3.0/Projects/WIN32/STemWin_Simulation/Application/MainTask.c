#include "TP_Global.h"

#include "TP_Bmp.h"
#include "TP_Button.h"
#include "TP_Menu.h"
#include "TP_Text.h"
#include "TP_ActionBar.h"


/* 下面开始是测试代码 */

static void Test_cbBkWindow(WM_MESSAGE * pMsg) {
	TP_MENU_Handle	hWin;
	TP_MENU_MSG *pMenuMsg = NULL;
	static char showMsg[64] = "TP_Menu - Sample";
//	int id;
	int NCode;
//	GUI_RECT rClient;

	hWin = pMsg->hWin;
  switch (pMsg->MsgId) {
	case WM_PAINT:
		GUI_SetBkColor(GUI_WHITE);
		GUI_Clear();
		GUI_SetColor(GUI_BLACK);
		GUI_SetFont(&GUI_Font24_ASCII);
		GUI_DispStringHCenterAt(showMsg, 160, 80);
	break;
	
	case WM_NOTIFY_PARENT:
		/* 这里不能通过WM_GetId来获取控件的ID，因为都是自定义控件
		* 自定义控件的ID都是设置在其对应的Obj对象中，没有设置到窗口中
		*/
		//id = WM_GetId(pMsg->hWinSrc);
		NCode = pMsg->Data.v;
		switch (NCode) {
		case WM_NOTIFICATION_CLICKED:
		//case WM_NOTIFICATION_RELEASED:
			/*
			* 想要在这里获取buttonID，那可能需要设置一些额外参数
			* 目前TP_ActionBar_Callback通过窗口坐标结合OBJ数据判断ID
			*/
			GUI_MessageBox("click", "message", 0);
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
	break;
  }
}

static void Child1_cbBkWindow(WM_MESSAGE * pMsg) 
{
	TP_MENU_Handle	hWin;

	static char showMsg[64] = "Child1";
	//int NCode;
	GUI_RECT winRect;

	hWin = pMsg->hWin;
	WM_GetWindowRectEx(hWin, &winRect);
	GUI_MoveRect(&winRect, -winRect.x0, -winRect.y0);
	switch (pMsg->MsgId) 
	{
	case WM_PAINT:
		GUI_SetBkColor(GUI_LIGHTRED);
		GUI_Clear();
		GUI_SetColor(GUI_BLACK);
		GUI_SetFont(&GUI_Font24_ASCII);
		GUI_DispStringInRect(showMsg, &winRect, GUI_TA_VCENTER | GUI_TA_HCENTER);
		break;
	default:
		WM_DefaultProc(pMsg);
		break;
	}
}

static void Child2_cbBkWindow(WM_MESSAGE * pMsg) 
{
	TP_MENU_Handle	hWin;

	static char showMsg[64] = "Child2";
	int NCode;
	GUI_RECT rect;

	hWin = pMsg->hWin;
	WM_GetWindowRectEx(hWin, &rect);
	GUI_MoveRect(&rect, -rect.x0, -rect.y0);
	switch (pMsg->MsgId) 
	{
	case WM_PAINT:
		GUI_SetBkColor(GUI_LIGHTYELLOW);
		GUI_Clear();
		GUI_SetColor(GUI_BLACK);
		GUI_SetFont(&GUI_Font24_ASCII);
		GUI_DispStringInRect(showMsg, &rect, GUI_TA_VCENTER | GUI_TA_HCENTER);
		break;
	default:
		WM_DefaultProc(pMsg);
		break;
	}
}


extern GUI_CONST_STORAGE GUI_BITMAP bmHomeAsUp;
extern GUI_CONST_STORAGE GUI_BITMAP bmWirelessHome;
extern GUI_CONST_STORAGE GUI_BITMAP bmtplinklogo;
extern GUI_CONST_STORAGE GUI_BITMAP bmMoreIcon;

/*********************************************************************
*
*       MainTask
*/
#if 1
void MainTask(void) {
	TP_AB_ICON homeAsUp= {
		TP_AB_UP_ID,
		{&bmHomeAsUp, &bmHomeAsUp},
	};
	TP_AB_ICON homeIcon = {
		TP_AB_HOME_ID,
		{&bmWirelessHome, &bmWirelessHome},
	};
	TP_AB_TITLE title = {
		TP_AB_TITLE_ID,
		"Wireless Setting",
	};
	TP_AB_BUTTON button0 = {
		TP_AB_BUTTON_ID_0,
		"button0",
		{NULL, NULL}
	};
	TP_AB_BUTTON button1 = {
		TP_AB_BUTTON_ID_1,
		"button1",
		{&bmtplinklogo, NULL}
	};
	TP_AB_BUTTON button2 = {
		TP_AB_BUTTON_ID_2,
		"button2",
		{&bmtplinklogo, NULL}
	};

	TP_AB_BUTTON button3 = {
		TP_AB_BUTTON_ID_3,
		"button3",
		{&bmtplinklogo, NULL}
	};

	TP_AB_BUTTON button4 = {
		TP_AB_BUTTON_ID_4,
		"button4",
		{&bmtplinklogo, NULL}
	};

	TP_AB_BUTTON button5 = {
		TP_AB_BUTTON_ID_4 + 1,
		"button5",
		{&bmtplinklogo, NULL}
	};

	TP_AB_BUTTON button6 = {
		TP_AB_BUTTON_ID_4 + 2,
		"button6",
		{&bmtplinklogo, NULL}
	};
	
	TP_ACTIONBAR_HANDLE hAb;
	WM_HMEM hTest, hChild1, hChild2;

	GUI_Init();
	if (1)
	{
		hTest = WM_CreateWindow(0, 0, LCD_GetXSize(), LCD_GetYSize(),
						WM_CF_SHOW | WM_CF_MEMDEV, Test_cbBkWindow, 0);
		hChild1 =  WM_CreateWindowAsChild(50,150, 150, 80, hTest, WM_CF_SHOW | WM_CF_MEMDEV, Child1_cbBkWindow, 0);
		hChild2 =  WM_CreateWindowAsChild(50,150, 150, 80, hTest, WM_CF_SHOW | WM_CF_MEMDEV, Child2_cbBkWindow, 0);
		WM_BringToTop(hTest);

		hAb = TP_ActionBar_Create(60, hTest, WM_CF_SHOW | WM_CF_MEMDEV, TP_AB_HANDLE_ID, NULL, 0);
		TP_ActionBar_SetUpIcon(hAb, homeAsUp);
		//TP_ActionBar_SetHomeIcon(hAb, homeIcon);
		TP_ActionBar_SetTitle(hAb, title);
		TP_ActionBar_AddButton(hAb, button0);
		TP_ActionBar_AddButton(hAb, button1);
		TP_ActionBar_AddButton(hAb, button2);
		TP_ActionBar_AddButton(hAb, button3);
		TP_ActionBar_AddButton(hAb, button4);
		TP_ActionBar_AddButton(hAb, button5);
		TP_ActionBar_AddButton(hAb, button6);
	}

	while (1) {
		//WM_BringToTop(hTest);
		GUI_Delay(100);
		WM_BringToTop(hChild1);
		GUI_Delay(5000);
		WM_BringToTop(hChild2);
		GUI_Delay(5000);
	}
}
#endif

