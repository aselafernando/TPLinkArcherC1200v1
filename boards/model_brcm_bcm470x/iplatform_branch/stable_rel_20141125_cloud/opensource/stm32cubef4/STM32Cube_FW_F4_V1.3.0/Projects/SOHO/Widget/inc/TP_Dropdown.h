#ifndef _TP_DROPDOWN_H
#define _TP_DROPDOWN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "WM.h"
#include "TP_Widget.h"

#define TP_DROPDOWN_LINE_COLOR 0xb5b5b5
#define TP_DROPDOWN_ITEM_BKCOLOR 0xdcdcdc
#define TP_DROPDOWN_BLANK_COLOR 0xdcdcdc

typedef WM_HMEM TP_DROPDOWN_Handle;

typedef struct _dropdownNodeItem {
	int id;
	int index;
	GUI_RECT rect;
	char * pText;
} dropdownNodeItem;

typedef struct _dropdownListNode {
	dropdownNodeItem item;
	struct _dropdownListNode * next;
} dropdownListNode;

typedef struct _dropdownItemList {
	dropdownListNode * head;
	dropdownListNode * tail;
	int count;
} dropdownItemList;

typedef struct _TP_SCROLLBAR_Props
{
	int orgXPos;
	int orgYPos;
	int height;
	int width;
	int range[2];
	GUI_COLOR bkColor;
} TP_SCROLLBAR_Props;

#define TP_DROPDOWN_LINE_COLOR 0xb5b5b5
#define TP_DROPDOWN_ITEM_BKCOLOR 0xdcdcdc
#define TP_DROPDOWN_BLANK_COLOR 0xdcdcdc

typedef struct _dropdownLineProps
{
	int height;
	int width;
	GUI_COLOR color;
} dropdownLineProps;


typedef struct _dropdownHeadProps
{
	int height;
	int width;
	GUI_COLOR bkColor;
	char * pText;
	const GUI_BITMAP * pBitmap;
	dropdownLineProps underline;
} dropdownHeadProps;

typedef struct _dropdownFooterProps
{
	int height;
	int width;
	GUI_COLOR bkColor;
	dropdownLineProps online;
} dropdownFooterProps;

typedef struct _dropdownBodyProps
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
	dropdownLineProps underline;
	int selIndex;
	dropdownItemList itemList;
} dropdownBodyProps;

typedef struct _TP_DORPDOWN_Obj
{
	int id;
	int orgXPos;
	int orgYPos;
	int orgYSize;
	int xSize;
	int ySize;
	GUI_COLOR bkColor;
	int borderSize[TP_BORDER_MAX];
	const GUI_FONT * pFont;
	GUI_COLOR fontColor;
	const GUI_BITMAP * selectBitmap;
	WM_HWIN hOwner;
	int selectIndex;
	WM_HWIN hScrollbar;
	TP_SCROLLBAR_Props scrollbarProps;
	dropdownHeadProps head;
	dropdownBodyProps body;
	dropdownFooterProps footer;
	int isFocussable;
	int numExtraBytes;
} TP_DROPDOWN_Obj;

void TP_DROPDOWN_SetDefaultScrollWidth(int width);
void TP_DROPDOWN_SetDefaultScorllHeight(int height);
void TP_DROPDOWN_SetDefaultScorllColor(GUI_COLOR color);
void TP_DROPDOWN_SetDefaultHeadBkColor(GUI_COLOR color);
void TP_DROPDOWN_SetDefaultHeadHeight(int height);
void TP_DROPDOWN_SetDefaultFooterHeight(int height);
void TP_DROPDOWN_SetDefaultFooterBkColor(GUI_COLOR color);
void TP_DROPDOWN_SetDefaultBkColor(GUI_COLOR color);
void TP_DROPDOWN_SetDefaultBorderSize(int index, int size);
void TP_DROPDOWN_SetDefaultFont(const GUI_FONT * pFont);
void TP_DROPDOWN_SetDefaultFontColor(GUI_COLOR color);
void TP_DROPDOWN_SetDefaultItemBkColor(GUI_COLOR color);
void TP_DROPDOWN_SetDefaultItemWidth(int width);
void TP_DROPDOWN_SetDefaultItemHeight(int height);
void TP_DROPDOWN_SetDefaultLeftBlankWidth(int width);

void TP_DROPDOWN_SetBkColor(TP_DROPDOWN_Handle hWin, GUI_COLOR color);
void TP_DROPDOWN_SetBorderSize(TP_DROPDOWN_Handle hWin, int index, int size);
void TP_DROPDOWN_SetFont(TP_DROPDOWN_Handle hWin, const GUI_FONT * pFont);
void TP_DROPDOWN_SetFontColor(TP_DROPDOWN_Handle hWin, GUI_COLOR color);
void TP_DROPDOWN_SetItemBkColor(TP_DROPDOWN_Handle hWin, GUI_COLOR color);
void TP_DROPDOWN_SetItemWidth(TP_DROPDOWN_Handle hWin, int width);
void TP_DROPDOWN_SetItemHeight(TP_DROPDOWN_Handle hWin, int height);
void TP_DROPDOWN_SetFocussable(TP_DROPDOWN_Handle hWin, int state);

void TP_DROPDOWN_DrawHead(TP_DROPDOWN_Obj * pObj);
void TP_DROPDOWN_DrawFooter(TP_DROPDOWN_Obj * pObj);
void TP_DROPDOWN_OnPaint(TP_DROPDOWN_Handle hWin, TP_DROPDOWN_Obj * pObj);

void TP_DROPDOWN_AddItem(TP_DROPDOWN_Handle hWin, char * sText, int id);

void TP_DROPDOWN_NotifyScrollbarYOffset(TP_DROPDOWN_Handle hWin, TP_DROPDOWN_Obj * pObj, int offset);
void TP_DROPDOWN_NotifyParentYOffset(TP_DROPDOWN_Handle hWin, int offset);
void TP_DROPDOWN_GetItemText(TP_DROPDOWN_Handle hWin, int index, char * pBuffer, int maxSize);
int TP_DROPDOWN_GetNumItems(TP_DROPDOWN_Handle hWin);
int TP_DROPDOWN_GetSelIndex(TP_DROPDOWN_Handle hWin);
void TP_DROPDOWN_SetSelIndex(TP_DROPDOWN_Handle hWin, int sel);
int TP_DROPDOWN_GetSelFromPos(TP_DROPDOWN_Obj * pObj, int x, int y);
void TP_DROPDOWN_NotityOwner(TP_DROPDOWN_Handle hWin, TP_DROPDOWN_Obj * pObj, int notification);

void dropdownList_init(dropdownItemList * list);
int dropdownList_Clear(dropdownItemList * list);
void dropdownList_addItem(dropdownItemList * list, dropdownListNode * node);
int dropdownList_delItem(dropdownItemList * list, dropdownListNode * node);
dropdownListNode * newDropdownListNode(int id, int index, GUI_RECT rect, char * sText);
dropdownListNode * findDropdownNodeByPos(dropdownItemList * list, int x, int y);
dropdownListNode * findDropdownNodeById(dropdownItemList * list, int id);
dropdownListNode * findDropdownNodeByIndex(dropdownItemList * list, int index);

void TP_DROPDOWN_Callback(WM_MESSAGE * pMsg);
TP_DROPDOWN_Handle TP_DROPDOWN_Create(int x0, int y0, int xSize, int ySize, WM_HWIN hParent, U8 style, WM_CALLBACK * cb, int numExtraBytes, int id);

int TP_DROPDOWN_SetUserData(TP_DROPDOWN_Handle hWin, void * pSrc, int size);
int TP_DROPDOWN_GetUserData(TP_DROPDOWN_Handle hWin, void * pDest, int size);

#endif

