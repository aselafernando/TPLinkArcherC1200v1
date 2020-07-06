#ifndef _TP_FAKELIST_H
#define _TP_FAKELIST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "WM.h"
#include "DIALOG_Intern.h"

#include "TP_Widget.h"

typedef enum _TP_FAKELIST_BITMAP_POS
{
	POS_LEFT = 0,
	POS_RIGHT,
} TP_FAKELIST_BITMAP_POS;

#define TP_FAKELIST_LINE_COLOR 0xb5b5b5
#define TP_FAKELIST_ITEM_BKCOLOR 0xdcdcdc
#define TP_FAKELIST_BLANK_COLOR 0xdcdcdc

typedef void (*pHandleFun)(WM_HWIN hWin);

typedef WM_HMEM TP_FAKELIST_Handle;

typedef struct _fakelistNodeItem
{
	int id;
	int index;
	GUI_RECT rect;
	char * pTextL;
	const GUI_BITMAP * pBitmapL;
	char * pTextR;
	const GUI_BITMAP * pBitmapR;
	pHandleFun handle;
} fakelistNodeItem;

typedef struct _fakelistListNode {
	fakelistNodeItem item;
	struct _fakelistListNode * next;
} fakelistListNode;

typedef struct _fakelistItemList {
	fakelistListNode * head;
	fakelistListNode * tail;
	int count;
} fakelistItemList;

typedef struct _fakeListPartZoneProps {
	char * pText;
	const GUI_BITMAP * pBitmap;
	int space;
} fakelistPartZoneProps;

typedef struct _fakelistLineProps
{
	int height;
	int width;
	GUI_COLOR color;
} fakelistLineProps;


typedef struct _fakelistHeadProps
{
	int height;
	int width;
	GUI_COLOR bkColor;
	char * pText;
	const GUI_BITMAP * pBitmap;
	fakelistLineProps underline;
} fakelistHeadProps;

typedef struct _fakelistFooterProps
{
	int height;
	int width;
	GUI_COLOR bkColor;
	fakelistLineProps online;
} fakelistFooterProps;

typedef struct _fakelistBodyProps
{
	int height;
	int width;
	GUI_COLOR bkColor;
	int itemHeight;
	int itemWidth;
	GUI_COLOR itemBkColor;
	int borderSize[TP_BORDER_MAX];
	int space;
	int itemLeftBlankSize;
	fakelistLineProps underline;
	int selIndex;
	fakelistItemList list;
} fakelistBodyProps;

typedef struct _fakelistScrollbarProps
{
	int orgXPos;
	int orgYPos;
	int height;
	int width;
	int range[2];
	GUI_COLOR bkColor;
} fakelistScrollbarProps;


/*
id
原始桌面x0坐标
原始桌面y0坐标
原始x宽度
原始y高度
当前x高度
当前y高度
背景颜色
item条目字体
item条目字体颜色
头部
body内容
底部
滚动条窗口
滚动条窗口属性
*/
typedef struct _TP_FAKELIST_Obj
{
	int id;
	int orgXPos;
	int orgYPos;
	int orgXSize;
	int orgYSize;
	int xSize;
	int ySize;
	GUI_COLOR bkColor;
	const GUI_FONT * pFont;
	GUI_COLOR fontColor;
	fakelistHeadProps head;
	fakelistBodyProps body;
	fakelistFooterProps footer;
	WM_HWIN hWinScroll;
	fakelistScrollbarProps scrollbar;
	int selIndex;
	int isFocussable;
	int numExtraBytes;
} TP_FAKELIST_Obj;

void TP_FAKELIST_SetBkColor(GUI_COLOR color);
void TP_FAKELIST_SetDefaultBodyBkColor(GUI_COLOR color);
void TP_FAKELIST_SetDefaultBodyItemBkColor(GUI_COLOR color);
void TP_FAKELIST_SetDefaultHeadBkColor(GUI_COLOR color);
void TP_FAKELIST_SetDefaultFooterBkColor(GUI_COLOR color);
void TP_FAKELIST_SetDefaultItemHeight(int height);
void TP_FAKELIST_SetDefaultItemWidth(int width);
void TP_FAKELIST_SetDefaultFont(const GUI_FONT * pFont);
void TP_FAKELIST_SetDefaultFontColor(GUI_COLOR color);
void TP_FAKELIST_SetDefaultHeadHeight(int height);
void TP_FAKELIST_SetDefaultFooterHeight(int height);
void TP_FAKELIST_SetDefaultScrollColor(GUI_COLOR color);
void TP_FAKELIST_SetDefaultScrollWidth(int width);
void TP_FAKELIST_SetDefaultScrollHeight(int height);
void TP_FAKELIST_SetDefaultBodyLeftBlankWidth(int width);

void TP_FAKELIST_SetFocussable(TP_FAKELIST_Handle hWin, int state);
void TP_FAKELIST_DrawHead(TP_FAKELIST_Obj * pObj);
void TP_FAKELIST_DrawFooter(TP_FAKELIST_Obj * pObj);
void TP_FAKELIST_DrawBody(TP_FAKELIST_Obj * pObj);
void TP_FAKELIST_OnPaint(TP_FAKELIST_Handle hWin, TP_FAKELIST_Obj * pObj);
void TP_FAKELIST_AddItem(TP_FAKELIST_Handle hWin, int id, int flagL, char * sTextL, const GUI_BITMAP * pBitmapL, int flagR, char * sTextR, const GUI_BITMAP * pBitmapR, pHandleFun func);

void TP_FAKELIST_SetSelBitmap(TP_FAKELIST_Handle hWin, int sel, const GUI_BITMAP * pBitmap, TP_FAKELIST_BITMAP_POS flag);
void TP_FAKELIST_HandleSel(TP_FAKELIST_Handle hWin, int sel);
int TP_FAKELIST_GetSelFromPos(TP_FAKELIST_Obj * pObj, int x, int y);
int TP_FAKELIST_GetSelIndex(TP_FAKELIST_Handle hWin);
void TP_FAKELIST_SetSelIndex(TP_FAKELIST_Handle hWin, int sel);

TP_FAKELIST_Handle TP_FAKELIST_CreateIndirect(const GUI_WIDGET_CREATE_INFO * pCreateInofo, WM_HWIN hParent, int x0, int y0, WM_CALLBACK * cb);

int TP_FAKELIST_SetUserData(TP_FAKELIST_Handle hWin, void * pSrc, int size);
int TP_FAKELIST_GetUserData(TP_FAKELIST_Handle hWin, void * pDest, int size);

void TP_FAKELIST_NotifyScrollbarYOffset(TP_FAKELIST_Handle hWin, TP_FAKELIST_Obj * pObj, int offset);
void TP_FAKELIST_SCROLLBAR_Callback(WM_MESSAGE * pMsg);
void TP_FAKELIST_Callback(WM_MESSAGE * pMsg);
TP_FAKELIST_Handle TP_FAKELIST_Create(int x0, int y0, int xSize, int ySize, WM_HWIN hParent, U8 style, WM_CALLBACK * cb, int numExtraBytes, int id);
	
void fakelistList_init(fakelistItemList * list);
int fakelistList_Clear(fakelistItemList * list);
void fakelistList_addItem(fakelistItemList * list, fakelistListNode * node);
int fakelistList_delItem(fakelistItemList * list, fakelistListNode * node);
fakelistListNode * newFakelistListNode(int id, int index, GUI_RECT rect, char * sTextL, const GUI_BITMAP * pBitmapL, char * sTextR, const GUI_BITMAP * pBitmapR, pHandleFun func);
fakelistListNode * findFakelistNodeByPos(fakelistItemList * list, int x, int y);
fakelistListNode * findFakelistNodeById(fakelistItemList * list, int id);
fakelistListNode * findFakelistNodeByIndex(fakelistItemList * list, int index);
int updateFakelistNodeBitmapByIndex(fakelistItemList * list, int index, const GUI_BITMAP * pBitmap, TP_FAKELIST_BITMAP_POS flag);

#endif

