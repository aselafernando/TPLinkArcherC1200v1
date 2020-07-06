#include <string.h> // Definition of NULL
#include <stdlib.h> // malloc() and free()

#include "TP_Global.h"
#include "TP_CheckBox.h"

//#include "CHECKBOX.H"

#define TP_CHECKBOX_X_OFFSET			(20)
#define TP_CHECKBOX_DEFAULT_PEN_SIZE	(3)
#define TP_CHECKBOX_DEFAULT_SIZE		(20)
//#define CHECKBOX_DEFAULT_Y_SIZE		(15)

#define TP_CHECKBOX_DEFAULT_ID	(0)

#define TP_CHECKBOX_DEFAULT_BOX_BK_COLOR	(GUI_WHITE)
#define TP_CHECKBOX_DEFAULT_BOX_COLOR		(GUI_BLACK)

#define TP_CHECKBOX_DEFAULT_TEXT_BK_NORMAL_COLOR	(GUI_WHITE)
#define TP_CHECKBOX_DEFAULT_TEXT_BK_CLICKED_COLOR	(GUI_LIGHTGRAY)
#define TP_CHECKBOX_DEFAULT_TEXT_BK_DISABLE_COLOR	(GUI_GRAY)

#define TP_CHECKBOX_DEFAULT_TEXT_COLOR				(GUI_BLACK)
#define TP_CHECKBOX_DEFAULT_HOOK_COLOR				(GUI_GREEN)

#define TP_CHECKBOX_DEFAULT_FONT					(GUI_DEFAULT_FONT)

#define TP_CHECKBOX_DEFAULT_TEXT_ALIGN_STYLE		(GUI_TA_LEFT | GUI_TA_VCENTER)
#define TP_CHECKBOX_DEFAULT_BOX_ALIGN_STYLE		(TP_CHECKBOX_ALIGN_H_LEFT)


/*********************************************************************
*
* 复选框的默认属性值
*/

static TP_CHECKBOX_Obj TP_CheckBox_Default = 
{
	TP_CHECKBOX_DEFAULT_ID,
	TP_CHECKBOX_DEFAULT_BOX_BK_COLOR,	/*框的背景颜色*/
	TP_CHECKBOX_DEFAULT_BOX_COLOR,		/*框的颜色*/
	{									/*文本的背景颜色*/
		TP_CHECKBOX_DEFAULT_TEXT_BK_NORMAL_COLOR,
		TP_CHECKBOX_DEFAULT_TEXT_BK_CLICKED_COLOR,
		TP_CHECKBOX_DEFAULT_TEXT_BK_DISABLE_COLOR
	},
	TP_CHECKBOX_DEFAULT_TEXT_COLOR,		/*文本的颜色*/
	TP_CHECKBOX_DEFAULT_HOOK_COLOR,		/*钩子的颜色*/
	NULL,								/*要显示的文本*/
	(const GUI_FONT*)TP_CHECKBOX_DEFAULT_FONT,		/*文本的字体*/
	10,									/*方框与文本框之间的距离*/
	TP_CHECKBOX_STATE_UNCHECKED,		/*当前复选框的状态*/
	0,
	TP_CHECKBOX_DEFAULT_SIZE,			/*框的宽度*/
	//CHECKBOX_DEFAULT_Y_SIZE,			/*框的高度*/
	//GUI_RED,	/* 按下时方框的颜色 */
	0,
	TP_CHECKBOX_DEFAULT_TEXT_ALIGN_STYLE,	/* 文本的对齐方式 */
	TP_CHECKBOX_DEFAULT_BOX_ALIGN_STYLE,	/* 方框的对齐方式 */
};

const GUI_FONT* TP_CheckBox_GetDefaultFont(void)
{
	return TP_CheckBox_Default.pFont;
}

const GUI_FONT* TP_CheckBox_SetDefaultFont(const GUI_FONT* pFont)
{
	const GUI_FONT* pPreFont = TP_CheckBox_Default.pFont;

	TP_CheckBox_Default.pFont = (GUI_FONT*)pFont;
	return pPreFont;
}

GUI_COLOR TP_CheckBox_GetDefaultTextBkColor(int index)
{
	if (index < TP_CHECKBOX_TB_CI_NORMAL
		|| index > TP_CHECKBOX_TB_CI_DISABLED)
	{
		return GUI_INVALID_COLOR;
	}
	return TP_CheckBox_Default.textBkColor[index];
}

void TP_CheckBox_SetDefaultTextBkColor(GUI_COLOR color, int index)
{
	if (index < TP_CHECKBOX_TB_CI_NORMAL
		|| index > TP_CHECKBOX_TB_CI_DISABLED)
	{
		return;
	}
	TP_CheckBox_Default.textBkColor[index] = color;
}


int	TP_CheckBox_GetDefaultSpacing(void)
{
	return TP_CheckBox_Default.space;
}

int	TP_CheckBox_SetDefaultSpacing(int space)
{
	TP_CheckBox_Default.space = space;

	return RET_SUCCESS;
}


int	TP_CheckBox_GetDefaultTextAlign(void)
{
	return TP_CheckBox_Default.textAlign;
}


int	TP_CheckBox_SetDefaultTextAlign(int align)
{
	TP_CheckBox_Default.textAlign = align;

	return RET_SUCCESS;
}

GUI_COLOR TP_CheckBox_GetDefaultTextColor(void)
{
	return TP_CheckBox_Default.textColor;
}

int TP_CheckBox_SetDefaultTextColor(GUI_COLOR color)
{
	TP_CheckBox_Default.textColor = color;
	return RET_SUCCESS;
}

GUI_COLOR TP_CheckBox_GetTextBkColor(TP_CHECKBOX_Handle hObj, int index)
{
	TP_CHECKBOX_Obj obj;

	if (index < TP_CHECKBOX_TB_CI_NORMAL
			|| index > TP_CHECKBOX_TB_CI_DISABLED)
	{
		return GUI_INVALID_COLOR;
	}
	
	WM_GetUserData(hObj, &obj, sizeof(obj));

	return obj.textBkColor[index];
}

void  TP_CheckBox_SetTextBkColor(TP_CHECKBOX_Handle hObj, GUI_COLOR color,int index)
{
	TP_CHECKBOX_Obj obj;

	if (index < TP_CHECKBOX_TB_CI_NORMAL
				|| index > TP_CHECKBOX_TB_CI_DISABLED)
	{
		return;
	}

	WM_GetUserData(hObj, &obj, sizeof(obj));
	obj.textBkColor[index] = color;
	WM_SetUserData(hObj, &obj, sizeof(obj));
}

GUI_COLOR TP_CheckBox_GetTextColor(TP_CHECKBOX_Handle hObj)
{
	TP_CHECKBOX_Obj obj;
	WM_GetUserData(hObj, &obj, sizeof(obj));

	return obj.textColor;
}


GUI_COLOR TP_CheckBox_SetTextColor(TP_CHECKBOX_Handle hObj, GUI_COLOR color)
{
	TP_CHECKBOX_Obj obj;
	GUI_COLOR preColor;

	WM_GetUserData(hObj, &obj, sizeof(obj));
	preColor = obj.textColor;
	obj.textColor = color;
	WM_SetUserData(hObj, &obj, sizeof(obj));

	return preColor;
}


GUI_COLOR TP_CheckBox_GetBoxBkColor(TP_CHECKBOX_Handle hObj)
{
	TP_CHECKBOX_Obj obj;
	WM_GetUserData(hObj, &obj, sizeof(obj));

	return obj.boxBkColor;
}

GUI_COLOR TP_CheckBox_SetBoxBkColor(TP_CHECKBOX_Handle hObj, GUI_COLOR color)
{
	TP_CHECKBOX_Obj obj;
	GUI_COLOR preColor;

	WM_GetUserData(hObj, &obj, sizeof(obj));
	preColor = obj.boxBkColor;
	obj.boxBkColor = color;
	WM_SetUserData(hObj, &obj, sizeof(obj));

	return preColor;
}

TP_CHECKBOX_STATE TP_CheckBox_GetState(TP_CHECKBOX_Handle hObj)
{
	TP_CHECKBOX_Obj obj;
	WM_GetUserData(hObj, &obj, sizeof(obj));

	return obj.state;
}

void TP_CheckBox_SetState (TP_CHECKBOX_Handle hObj, TP_CHECKBOX_STATE state)
{
	TP_CHECKBOX_Obj obj;

	WM_GetUserData(hObj, &obj, sizeof(obj));
	obj.state = state;
	WM_SetUserData(hObj, &obj, sizeof(obj));
}

int	TP_CheckBox_GetText(TP_CHECKBOX_Handle hObj, char * pBuffer, unsigned int maxLen)
{
	TP_CHECKBOX_Obj obj;
	int len = 0;
	
	WM_GetUserData(hObj, &obj, sizeof(obj));
	if ((pBuffer == NULL) || (maxLen <= 0) || (obj.pText == NULL))
	{
		return RET_FAIL;
	}

	if (strlen(obj.pText) > maxLen)
	{
		len = maxLen;
	}
	else
	{
		len = strlen(obj.pText);
	}

	TP_MEMCPY(pBuffer, obj.pText, len);

	return RET_SUCCESS;
}

int TP_CheckBox_SetText(TP_CHECKBOX_Handle hObj, const char* pText)
{
	TP_CHECKBOX_Obj obj;
	int len = 0;
	
	WM_GetUserData(hObj, &obj, sizeof(obj));
	if (pText == NULL)
	{
		return RET_FAIL;
	}
	len = strlen(pText) + 1;
	
	if (obj.pText)
	{
		TP_FREE(obj.pText);
	}

	obj.pText = TP_MALLOC(len);

	if (obj.pText)
	{
		TP_MEMSET(obj.pText, 0, len);
		TP_MEMCPY(obj.pText, pText, len - 1);
	}

	WM_SetUserData(hObj, &obj, sizeof(obj));
	return RET_SUCCESS;
}

const GUI_FONT *  TP_CheckBox_GetFont(TP_CHECKBOX_Handle hObj, const GUI_FONT * pFont)
{
	TP_CHECKBOX_Obj obj;

	WM_GetUserData(hObj, &obj, sizeof(obj));

	return obj.pFont;
}

const GUI_FONT *  TP_CheckBox_SetFont(TP_CHECKBOX_Handle hObj, const GUI_FONT * pFont)
{
	TP_CHECKBOX_Obj obj;
	const GUI_FONT* pPreFont = NULL;

	WM_GetUserData(hObj, &obj, sizeof(obj));
	pPreFont = obj.pFont;
	obj.pFont = (GUI_FONT *)pFont;
	WM_SetUserData(hObj, &obj, sizeof(obj));
	return pPreFont;
}

int TP_CheckBox_GetSpacing(TP_CHECKBOX_Handle hObj)
{
	TP_CHECKBOX_Obj obj;

	WM_GetUserData(hObj, &obj, sizeof(obj));

	return obj.space;
}


void TP_CheckBox_SetSpacing(TP_CHECKBOX_Handle hObj, int space)
{
	TP_CHECKBOX_Obj obj;

	WM_GetUserData(hObj, &obj, sizeof(obj));
	obj.space = space;
	WM_SetUserData(hObj, &obj, sizeof(obj));
}

int TP_CheckBox_GetTextAlign(TP_CHECKBOX_Handle hObj)
{
	TP_CHECKBOX_Obj obj;

	WM_GetUserData(hObj, &obj, sizeof(obj));

	return obj.textAlign;
}


void TP_CheckBox_SetTextAlign(TP_CHECKBOX_Handle hObj, int align)
{
	TP_CHECKBOX_Obj obj;

	WM_GetUserData(hObj, &obj, sizeof(obj));
	obj.textAlign = align;
	WM_SetUserData(hObj, &obj, sizeof(obj));
}

void TP_CheckBox_Callback(WM_MESSAGE * pMsg)
{
	TP_CHECKBOX_Handle	hWin;
	GUI_PID_STATE	*pState;
	TP_CHECKBOX_Obj	obj;
	GUI_RECT	WinRect, tmpRect;
	CHAR *pText = NULL;
	U8	Pressed;
	WM_PID_STATE_CHANGED_INFO *pPidState;	
	int len, height;
	//int width;
	GUI_COLOR boxColor, textBkColor, boxBkColor;

	hWin = pMsg->hWin;
	WM_GetWindowRectEx(hWin, &WinRect);
	GUI_MoveRect(&WinRect, -WinRect.x0, -WinRect.y0);
	WM_GetUserData(hWin, &obj, sizeof(TP_CHECKBOX_Obj));
	pText = obj.pText;
	
	switch (pMsg->MsgId) {
	case WM_PAINT:
		boxColor = obj.boxColor;

		if (WM_IsEnabled(hWin))
		{
			if (obj.pressed)
			{
				textBkColor = obj.textBkColor[TP_CHECKBOX_TB_CI_CLICKED];
				boxBkColor = textBkColor;
			}
			else
			{
				textBkColor = obj.textBkColor[TP_CHECKBOX_TB_CI_NORMAL];
				boxBkColor = obj.boxBkColor;
			}
		}
		else
		{
			textBkColor = obj.textBkColor[TP_CHECKBOX_TB_CI_DISABLED];
			boxBkColor = textBkColor;
		}

		GUI_SetColor(textBkColor);/* 显示背景颜色 */
		GUI_FillRectEx(&WinRect);

		height = (WinRect.y1 - WinRect.y0);
		//width = (WinRect.x1 - WinRect.x0);
		len = obj.size;

		tmpRect = WinRect;

		if (obj.boxAlign == TP_CHECKBOX_ALIGN_H_LEFT)
		{
			tmpRect.x0 += TP_CHECKBOX_X_OFFSET;
			tmpRect.y0 += (height - len) / 2;
			tmpRect.x1 = tmpRect.x0 + len;
			tmpRect.y1 = tmpRect.y0 + len;
		}
		else if (obj.boxAlign == TP_CHECKBOX_ALIGN_H_RIGHT)
		{
			tmpRect.x0 = tmpRect.x1 - TP_CHECKBOX_X_OFFSET - len;
			tmpRect.y0 += (height - len) / 2;
			tmpRect.x1 = tmpRect.x0 + len;
			tmpRect.y1 = tmpRect.y0 + len;
		}

		GUI_SetColor(boxBkColor);
		GUI_FillRectEx(&tmpRect);/* 填充方框的颜色 */

		GUI_SetColor(boxColor);		
		GUI_DrawRectEx(&tmpRect);/* 画框 */

		/* 如果复选框的状态选上了，则画勾 */
		if (obj.state == TP_CHECKBOX_STATE_CHECKED)
		{
			GUI_SetColor(obj.hookColor);
			GUI_SetPenSize(TP_CHECKBOX_DEFAULT_PEN_SIZE);
			GUI_DrawLine(tmpRect.x0 + 4, tmpRect.y0 + len / 2,
							tmpRect.x0 + len / 2, tmpRect.y1 - 4);

			GUI_DrawLine(tmpRect.x0 + len / 2, tmpRect.y1 - 4,
							tmpRect.x1 - 4, tmpRect.y0 + 4);
		}

		if (pText)
		{
			tmpRect = WinRect;

			if (obj.boxAlign == TP_CHECKBOX_ALIGN_H_LEFT)
			{
				tmpRect.x0 += TP_CHECKBOX_X_OFFSET + len + obj.space;
				tmpRect.x1 -= TP_CHECKBOX_X_OFFSET;
			}
			else if (obj.boxAlign == TP_CHECKBOX_ALIGN_H_RIGHT)
			{
				tmpRect.x1 -= TP_CHECKBOX_X_OFFSET + len + obj.space;
				tmpRect.x0 += TP_CHECKBOX_X_OFFSET;
			}

			GUI_SetColor(obj.textColor);
			GUI_SetTextMode(GUI_TM_TRANS);
			GUI_SetFont(obj.pFont);
			GUI_DispStringInRect((const char*)pText, &tmpRect, obj.textAlign);
		}
		break;

		case WM_TOUCH:
		pState = (GUI_PID_STATE *)pMsg->Data.p;
		if (pState)
		{
			Pressed = pState->Pressed;
		}
		else
		{
			Pressed = 0;
		}

		if (obj.pressed != Pressed)
		{
			obj.pressed = Pressed;
			WM_SetUserData(hWin, &obj, sizeof(TP_CHECKBOX_Obj));
			if (Pressed)
			{
				WM_SetFocus(hWin);
			}
			WM_InvalidateWindow(hWin);

			if (Pressed)
			{
				/* 告诉父窗口自己被点击 */
				WM_NotifyParent(hWin, WM_NOTIFICATION_CLICKED);
			}
			else
			{
				/* 告诉父窗口自己的点击已被释放 */
				WM_NotifyParent(hWin, WM_NOTIFICATION_RELEASED);
			}
		}
		break;
	case WM_SET_FOCUS:
		if (pMsg->Data.v)
		{
		  pMsg->Data.v = 0;
		} 
		WM_InvalidateWindow(hWin);
		break;

	case WM_PID_STATE_CHANGED:
		pPidState = (WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p);
		if (WM_IsEnabled(hWin))
		{
			if ((pPidState->StatePrev == WM_PID_STATE_PRESSED)
				&& (pPidState->State == WM_PID_STATE_UNPRESSED))
			{
				obj.state = (obj.state == TP_CHECKBOX_STATE_CHECKED)?
					(TP_CHECKBOX_STATE_UNCHECKED) : (TP_CHECKBOX_STATE_CHECKED);
				WM_SetUserData(hWin, &obj, sizeof(TP_CHECKBOX_Obj));
				WM_InvalidateWindow(hWin);

				/* 告诉父窗口自己的状态发生了变化 */
				WM_NotifyParent(hWin, WM_NOTIFICATION_VALUE_CHANGED);
			}
		}
		break;
	case WM_GET_ID:
		pMsg->Data.v = obj.id;
		break;
	case WM_SET_ID:
		obj.id = pMsg->Data.v;
		WM_SetUserData(hWin, &obj, sizeof(TP_CHECKBOX_Obj));
		break;
	case WM_DELETE:
		if (obj.pText)
		{
		TP_FREE(obj.pText);
		WM_SetUserData(hWin, &obj, sizeof(TP_CHECKBOX_Obj));
		}
		
		break;
	default:
		WM_DefaultProc(pMsg);
		break;
	}
}

int TP_CheckBox_GetUserData(TP_CHECKBOX_Handle hWin, void * pDest, int sizeOfBuffer)
{
	TP_CHECKBOX_Obj	 obj;
	int			 numBytes;
	U8		   * pExtraBytes = NULL;

	if ((pDest == NULL) || (sizeOfBuffer <= 0))
	{
		return 0;
	}
	
	WM_GetUserData(hWin, &obj, sizeof(TP_CHECKBOX_Obj));
	pExtraBytes = (U8 *)TP_MALLOC(sizeof(TP_CHECKBOX_Obj) + obj.numExtraBytes);
	if (pExtraBytes) 
	{
		WM_GetUserData(hWin, pExtraBytes, sizeof(TP_CHECKBOX_Obj) + obj.numExtraBytes);
		
		if (sizeOfBuffer >= obj.numExtraBytes)
		{
			numBytes = obj.numExtraBytes;
		}
		else
		{
			numBytes = sizeOfBuffer;
		}
		
		TP_MEMCPY(pDest, pExtraBytes + sizeof(TP_CHECKBOX_Obj), numBytes);
		TP_FREE(pExtraBytes);
		return numBytes;
	}
	return 0;
}


int TP_CheckBox_SetUserData(TP_CHECKBOX_Handle hWin, void * pSrc, int sizeOfBuffer)
{
	TP_CHECKBOX_Obj	 obj;
	int			 numBytes;
	U8		   * pExtraBytes = NULL;

	if ((pSrc == NULL) || (sizeOfBuffer <= 0))
	{
		return 1;
	}

	WM_GetUserData(hWin, &obj, sizeof(TP_CHECKBOX_Obj));
	pExtraBytes = (U8 *)TP_MALLOC(sizeof(TP_CHECKBOX_Obj) + obj.numExtraBytes);
	if (pExtraBytes)
	{
		WM_GetUserData(hWin, pExtraBytes, sizeof(TP_CHECKBOX_Obj) + obj.numExtraBytes);

		if (sizeOfBuffer >= obj.numExtraBytes)
		{
		  numBytes = obj.numExtraBytes;
		}
		else
		{
		  numBytes = sizeOfBuffer;
		}
		TP_MEMSET(pExtraBytes + sizeof(TP_CHECKBOX_Obj), 0, obj.numExtraBytes);
		TP_MEMCPY(pExtraBytes + sizeof(TP_CHECKBOX_Obj), pSrc, numBytes);
		WM_SetUserData(hWin, pExtraBytes, sizeof(TP_CHECKBOX_Obj) + obj.numExtraBytes);
		TP_FREE(pExtraBytes);
		return 0;
	}

	return 1;
}

TP_CHECKBOX_Handle TP_CheckBox_Create(int x0, int y0, int xSize, int ySize,
								WM_HWIN hWinParent, U32 style, const char *pText,
								WM_CALLBACK * pfCallback, int numExtraBytes, int id)
{
	TP_CHECKBOX_Handle	handle;
	TP_CHECKBOX_Obj		obj;
	WM_CALLBACK     	*pCb;

	if (pfCallback)
	{
		pCb = pfCallback;
	}
	else
	{
		pCb = TP_CheckBox_Callback;
	}

	obj	= TP_CheckBox_Default;
	
	obj.numExtraBytes = numExtraBytes;
	obj.id = id;
	if (pText)
	{
		obj.pText = TP_MALLOC(strlen(pText) + 1);
		TP_MEMSET(obj.pText, 0, strlen(pText) + 1);
		TP_MEMCPY(obj.pText, pText, strlen(pText));
	}
	
	handle = WM_CreateWindowAsChild(x0, y0, xSize, ySize, hWinParent, 
		style, pCb, sizeof(TP_CHECKBOX_Obj) + numExtraBytes);
	WM_SetUserData(handle, &obj, sizeof(TP_CHECKBOX_Obj));
	return handle;
	
}

#if  0
/* 下面开始是测试代码 */

static void createDemoCheckbox()
{
	CHECKBOX_Handle hDemoCheckbox;
	char acExtraBytes[] = "CheckBox";

	hDemoCheckbox = CHECKBOX_CreateEx(10, 100, 150, 50,  
    					WM_HBKWIN, WM_CF_SHOW, 0, GUI_ID_CHECK0);
	CHECKBOX_SetText(hDemoCheckbox, acExtraBytes);
	CHECKBOX_SetText(hDemoCheckbox, "TP-LINK");
	//CHECKBOX_SetDefaultAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
}

static void Test_cbBkWindow(WM_MESSAGE * pMsg) {
	WM_HMEM hWin;
	static char showMsg[64] = "TP_Check - Sample";

	int NCode;

	hWin = pMsg->hWin;
  switch (pMsg->MsgId) {
	case WM_PAINT:
	GUI_SetBkColor(GUI_YELLOW);
	GUI_Clear();
	GUI_SetColor(GUI_BLACK);
	GUI_SetFont(&GUI_Font24_ASCII);
	GUI_DispStringHCenterAt(showMsg, 160, 150);
	break;
	
	case WM_NOTIFY_PARENT:
		NCode = pMsg->Data.v;
		switch (NCode) {
		case WM_NOTIFICATION_CLICKED:
			GUI_MessageBox("click", "message", GUI_MESSAGEBOX_CF_MOVEABLE);
			break;
		case WM_NOTIFICATION_RELEASED:
			GUI_MessageBox("release", "message", GUI_MESSAGEBOX_CF_MOVEABLE);
			break;
		case WM_NOTIFICATION_VALUE_CHANGED:
			GUI_MessageBox("change", "message", GUI_MESSAGEBOX_CF_MOVEABLE);
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


/*********************************************************************
*
*       MainTask
*/
void MainTask(void) {
	TP_CHECKBOX_Handle hCheckbox;
	char acExtraBytes[] = "Build By TP-LINK";
	WM_HMEM hTest;

	GUI_Init();
	GUI_SetDefaultFont(GUI_DEFAULT_FONT);
	
	hTest = WM_CreateWindow(0, 0, LCD_GetXSize(), LCD_GetYSize(),
					WM_CF_SHOW | WM_CF_MEMDEV, Test_cbBkWindow, 0);
	hCheckbox = TP_CheckBox_Create(0, 100, LCD_GetXSize(), 50, hTest, WM_CF_SHOW, NULL, NULL, 0, 100);
	TP_CheckBox_SetText(hCheckbox, acExtraBytes);
	
#if 0
	TP_CheckBox_SetTextBkColor(hCheckbox, GUI_RED, TP_CHECKBOX_TB_CI_CLICKED);
	TP_CheckBox_SetTextBkColor(hCheckbox, GUI_BLACK, TP_CHECKBOX_TB_CI_NORMAL);
	TP_CheckBox_SetTextBkColor(hCheckbox, GUI_GRAY, TP_CHECKBOX_TB_CI_DISABLED);
	TP_CheckBox_SetTextColor(hCheckbox, GUI_WHITE);
	TP_CheckBox_SetBoxBkColor(hCheckbox, GUI_GRAY);
	TP_CheckBox_SetState(hCheckbox, TP_CHECKBOX_STATE_CHECKED);
	TP_CheckBox_SetText(hCheckbox, "TP-LINK");
	TP_CheckBox_SetSpacing(hCheckbox, 50);
	TP_CheckBox_SetTextAlign(hCheckbox, GUI_TA_BOTTOM | GUI_TA_HCENTER);
#endif

	//createDemoCheckbox();

	while (1) {
		GUI_Delay(100);
	}
}
#endif

