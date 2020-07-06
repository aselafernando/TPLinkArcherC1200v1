#ifndef _TP_EDIT_H
#define _TP_EDIT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "GUI.h"
#include "WM.h"

//#include "TP_Global.h"
#include "TP_Widget.h"

typedef WM_HMEM TP_EDIT_Handle;

#define TP_EDIT_FONT_DISABLED 0
#define TP_EDIT_FONT_ENABLED 1

#define TP_EDIT_BK_NORMAL 0
#define TP_EDIT_BK_FOCUS 1
#define TP_EDIT_BK_DISABLED 2

#if 0
#define TP_EDIT_BK_NORMAL_COLOR 0xb5b5b5
#define TP_EDIT_BK_FOCUS_COLOR 0xdcdcdc
#define TP_EDIT_BK_DISABLED_COLOR 0xc9c9c9
#endif

#define TP_EDIT_BK_NORMAL_COLOR 0xf1f8fa
#define TP_EDIT_BK_FOCUS_COLOR 0xf1f8fa
#define TP_EDIT_BK_DISABLED_COLOR 0xf1f8fa

#define TP_EDIT_CURSOR_BLINK_ON 1
#define TP_EDIT_CURSOR_BLINK_OFF 0

#define TP_EDIT_CURSOR_BLINK_FLAG_ON 0
#define TP_EDIT_CURSOR_BLINK_FLAG_OFF 1

#define TP_EDIT_AVALIABLE_SEEN_NORMAL 0
#define TP_EDIT_AVALIABLE_SEEN_OP 1

#define TP_TIMER_MOVE (TP_TIMER_ID0 + 1)
#define TP_TIMER_SHOW (TP_TIMER_ID0 + 2)

typedef struct _TP_EDIT_SLIDER_OBJ
{
	int id;
	int isClick;
	int orgXPos;
	int orgYPos;
	int sliderMinX;
	int sliderMaxX;
	const GUI_BITMAP * pSoliderBitmap;
	WM_HWIN hEditWin;
	WM_HTIMER hMoveTimer;
	WM_HTIMER hShowTimer;
	int charWidth;
} TP_EDIT_SLIDER_Obj;

typedef struct _TP_CURSOR_Props
{
	int cursorWidth; // 光标宽度
	GUI_COLOR cursorColor; // 光标颜色
	int isBlink; // 光标闪烁的开关
	WM_HTIMER blinkTimer; // 控制光标闪烁的定时器
	int blinkPeriod; // 光标闪烁周期
} TP_CURSOR_Props;

typedef struct _TP_SLIDER_Props
{
	int sliderXSize; // 下方游标宽度
	int sliderYSize; // 下方游标高度
} TP_SLIDER_Props;

typedef struct _TP_EDIT_Obj {
	int id;
	int isClick;
	GUI_COLOR aBkColor[3];
	GUI_COLOR aTextColor[2];
	const GUI_FONT * pFont;
	const GUI_BITMAP * pDelBitmap; // 最右边的x图标
	char aText[TP_MAX_STR_LEN + 1];
	int maxInputCharNum; // 允许用户输入的最多的字符个数
	int maxAvaliableSeenCharNum[2]; // 用户可见字符个数
	int rightMoveTimes; // 字符左移缩进的数目
	int blinkFlag;
	int aBorderSize[TP_BORDER_MAX]; // 上下左右边距
	WM_HWIN hSliderWin; // 下方游标窗口
	int cursorPos; // 光标位置
	TP_CURSOR_Props cursorProps; // 光标属性
	TP_SLIDER_Props sliderProps; // 游标属性
	int isFocussable;
	int isFirstClick;
	int sliderOffset;
	GUI_COLOR borderColor; // 输入框边框颜色
	int numExtraBytes;
} TP_EDIT_Obj;

typedef enum _CURSOR_OP
{
	CLICKOP=0,
	MOVERIGHTOP,
	MOVELEFTOP,
	DELOP,
	ADDOP
} CURSOR_OP;

void TP_EDIT_SetDefaultDelIconBitmap(const GUI_BITMAP * pBitmap);
void TP_EDIT_SetDefaultMaxAvaliableSeenCharNum(int num);
void TP_EDIT_SetDefaultMaxInputCharNum(int num);
void TP_EDIT_SetDefaultBorderColor(GUI_COLOR color);
void TP_EDIT_SetDefaultBkColor(int index, GUI_COLOR color);
GUI_COLOR TP_EDIT_GetDefaultBkColor(int index);
void TP_EDIT_SetDefaultTextColor(int index, GUI_COLOR color);
GUI_COLOR TP_EDIT_GetDefaultTextColor(int index);
void TP_EDIT_SetDefaultCurosorWidth(int width);
int TP_EDIT_GetDefaultCursorWidth();
void TP_EDIT_SetDefaultFont(const GUI_FONT * pFont);
const GUI_FONT * TP_EDIT_GetDefaultFont();

void TP_EDIT_SetBorderColor(TP_EDIT_Handle hWin, GUI_COLOR color);
void TP_EDIT_SetBkColor(TP_EDIT_Handle hWin, int index, GUI_COLOR color);
void TP_EDIT_SetText(TP_EDIT_Handle hWin, const char * sText);
void TP_EDIT_GetText(TP_EDIT_Handle hWin, char * sDest, int maxLen);
void TP_EDIT_SetTextColor(TP_EDIT_Handle hWin, int index, GUI_COLOR color);
void TP_EDIT_SetMaxInputNumChars(TP_EDIT_Handle hWin, int num);
int TP_EDIT_GetInputNumChars(TP_EDIT_Handle hWin);
void TP_EDIT_SetFont(TP_EDIT_Handle hWin, const GUI_FONT * pFont);
void TP_EDIT_SetFocussable(TP_EDIT_Handle hWin, int state);

int TP_EDIT_IsTouchClearIcon(TP_EDIT_Obj * pObj, GUI_RECT rect, int xPos);
void TP_EDIT_ClearText(TP_EDIT_Handle hWin);
void TP_EDIT_EraseText(TP_EDIT_Handle hWin, TP_EDIT_Obj * pObj);
void TP_EDIT_SetCursorColor(TP_EDIT_Handle hWin, GUI_COLOR color);
void TP_EDIT_EnableBlink(TP_EDIT_Handle hWin, int period, int Onff);
void TP_EDIT_SetCursorAtPixelPos(TP_EDIT_Handle hWin, TP_EDIT_Obj *pObj, int pos);
void TP_EDIT_SetCursorPos(TP_EDIT_Obj * pObj, int pos, CURSOR_OP op);
int TP_EDIT_IsInsertAvaliable(TP_EDIT_Obj * pObj);
int TP_EDIT_InsertChar(TP_EDIT_Handle hWin, TP_EDIT_Obj * pObj, U16 key);
void TP_EDIT_DeleteChar(TP_EDIT_Obj * pObj);
void TP_EDIT_OnPaint(TP_EDIT_Handle hWin, TP_EDIT_Obj * pObj);
void TP_EDIT_Callback(WM_MESSAGE * pMsg);
TP_EDIT_Handle TP_EDIT_Create(int x0, int y0, int xSize, int ySize, WM_HWIN hParent, U8 style, WM_CALLBACK * cb, int xSizeSlider, int ySizeSlider, int numExtraBytes, int id);

void TP_EDIT_SetCursorPosBySlider(TP_EDIT_Handle hWin, TP_EDIT_Obj * pObj, int pos);
void TP_EDIT_SLIDER_Callback(WM_MESSAGE * pMsg);
void TP_EDIT_SLIDER_Move(WM_HWIN hWin, TP_EDIT_SLIDER_Obj * hObj, int xPos);
void TP_EDIT_SLIDER_SetPos(WM_HWIN hWin, TP_EDIT_SLIDER_Obj * hObj, int offset);

int TP_EDIT_SetUserData(TP_EDIT_Handle hWin, void * pSrc, int size);
int TP_EDIT_GetUserData(TP_EDIT_Handle hWin, void * pDest, int size);

#endif
