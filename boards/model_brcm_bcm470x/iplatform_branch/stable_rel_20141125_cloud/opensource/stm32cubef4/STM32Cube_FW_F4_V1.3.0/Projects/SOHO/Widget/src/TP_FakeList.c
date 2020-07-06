#include "TP_FakeList.h"

extern GUI_CONST_STORAGE GUI_FONT GUI_FontF16ASCIIHNPMD;
extern GUI_CONST_STORAGE GUI_BITMAP bmDP_TickBmp;
extern GUI_CONST_STORAGE GUI_BITMAP bmFAKELIST_RightArrowBmp;
extern GUI_CONST_STORAGE GUI_BITMAP bmFAKELIST_SwitchOnBmp;

fakelistLineProps TP_FAKELIST_DefaultLine =  
{
	1,
	400,
	TP_FAKELIST_LINE_COLOR
};

fakelistHeadProps TP_FAKELIST_DefaultHead = 
{
	14,
	400,
	TP_FAKELIST_BLANK_COLOR,
	NULL,
	0,
	{0}
};

fakelistFooterProps TP_FAKELIST_DefaultFooter = 
{
	14,
	400,
	TP_FAKELIST_BLANK_COLOR,
	{0}
};

fakelistBodyProps TP_FAKELIST_DefaultBody =  
{
	0,
	400,
	GUI_WHITE,
	42,
	400,
	GUI_WHITE,
	{14, 20, 14, 20},
	5,
	40,
	{0},
	-1,
	{0}
};

fakelistScrollbarProps TP_FAKELIST_DefaultSCrollbar = 
{
	0,
	0,
	50,
	2,
	{0, 0},
	TP_FAKELIST_LINE_COLOR
};

TP_FAKELIST_Obj TP_FAKELIST_Default = 
{
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	GUI_WHITE,
	&GUI_FontF16ASCIIHNPMD,
	GUI_BLACK,
	{0},
	{0},
	{0},
	0,
	{0},
	-1,
	1,
	0
};

void TP_FAKELIST_SetBkColor(GUI_COLOR color)
{
	TP_FAKELIST_Default.bkColor = color;
}

void TP_FAKELIST_SetDefaultBodyItemBkColor(GUI_COLOR color)
{
	TP_FAKELIST_DefaultBody.itemBkColor = color;
}

void TP_FAKELIST_SetDefaultBodyBkColor(GUI_COLOR color)
{
	TP_FAKELIST_DefaultBody.bkColor = color;
}

void TP_FAKELIST_SetDefaultHeadBkColor(GUI_COLOR color)
{
	TP_FAKELIST_DefaultHead.bkColor = color;
}

void TP_FAKELIST_SetDefaultFooterBkColor(GUI_COLOR color)
{
	TP_FAKELIST_DefaultFooter.bkColor = color;
}

void TP_FAKELIST_SetDefaultItemHeight(int height)
{
	TP_FAKELIST_DefaultBody.itemHeight = height;
}

void TP_FAKELIST_SetDefaultItemWidth(int width)
{
	TP_FAKELIST_DefaultBody.itemWidth = width;
}

void TP_FAKELIST_SetDefaultFont(const GUI_FONT * pFont)
{
	TP_FAKELIST_Default.pFont = pFont;
}

void TP_FAKELIST_SetDefaultFontColor(GUI_COLOR color)
{
	TP_FAKELIST_Default.fontColor = color;
}

void TP_FAKELIST_SetDefaultHeadHeight(int height)
{
	TP_FAKELIST_DefaultHead.height = height;
}

void TP_FAKELIST_SetDefaultFooterHeight(int height)
{
	TP_FAKELIST_DefaultFooter.height = height;
}

void TP_FAKELIST_SetDefaultScrollColor(GUI_COLOR color)
{
	TP_FAKELIST_DefaultSCrollbar.bkColor = color;
}

void TP_FAKELIST_SetDefaultScrollWidth(int width)
{
	TP_FAKELIST_DefaultSCrollbar.width = width;
}

void TP_FAKELIST_SetDefaultScrollHeight(int height)
{
	TP_FAKELIST_DefaultSCrollbar.height = height;
}

void TP_FAKELIST_SetDefaultBodyLeftBlankWidth(int width)
{
	TP_FAKELIST_DefaultBody.itemLeftBlankSize = width;
}

void TP_FAKELIST_SetFocussable(TP_FAKELIST_Handle hWin, int state)
{
	if (hWin)
	{
		TP_FAKELIST_Obj hObj;
		WM_GetUserData(hWin, &hObj, sizeof(TP_FAKELIST_Obj));
		hObj.isFocussable = state;
		WM_SetUserData(hWin, &hObj, sizeof(TP_FAKELIST_Obj));
	}
}

void TP_FAKELIST_DrawHead(TP_FAKELIST_Obj * pObj)
{
	GUI_RECT rClient = {0};
	fakelistHeadProps head = pObj->head;
	fakelistLineProps underline = head.underline;

	rClient.x1 = head.width;
	rClient.y1 = head.height - underline.height;	
	GUI_SetClipRect(&rClient);
	GUI_SetBkColor(head.bkColor);
	GUI_Clear();
	GUI_SetClipRect(NULL);

	rClient.y0 = head.height - underline.height;
	rClient.y1 = head.height;
	GUI_SetClipRect(&rClient);
	GUI_SetBkColor(underline.color);
	GUI_Clear();
	GUI_SetClipRect(NULL);
}

void TP_FAKELIST_DrawFooter(TP_FAKELIST_Obj * pObj)
{
#define _TP_FOOTER_ONLINE_
#undef _TP_FOOTER_ONLINE_
  
	fakelistFooterProps footer = pObj->footer;
#ifdef _TP_FOOTER_ONLINE_
	fakelistLineProps online = footer.online;
#endif
	GUI_RECT rClient = {0};

#ifdef _TP_FOOTER_ONLINE_
	rClient.x1 = footer.width;
	rClient.y0 = (pObj->ySize - footer.height) + online.height;
	rClient.y1 = pObj->ySize;
	GUI_SetClipRect(&rClient);
	GUI_SetBkColor(footer.bkColor);
	GUI_Clear();
	GUI_SetClipRect(NULL);

	rClient.y0 = pObj->ySize - footer.height;
	rClient.y1 = rClient.y0 + online.height;
	GUI_SetClipRect(&rClient);
	GUI_SetBkColor(online.color);
	GUI_Clear();
	GUI_SetClipRect(NULL);
#else
	rClient.x1 = footer.width;
	rClient.y0 = pObj->ySize - footer.height;
	rClient.y1 = pObj->ySize;
	GUI_SetClipRect(&rClient);
	GUI_SetBkColor(footer.bkColor);
	GUI_Clear();
	GUI_SetClipRect(NULL);
#endif
}

void TP_FAKELIST_DrawBody(TP_FAKELIST_Obj * pObj)
{
	fakelistBodyProps body = pObj->body;
	GUI_RECT rClient = {0};
	fakelistItemList list = body.list;
	fakelistListNode * node;
	fakelistNodeItem item;
	int fontHeight;
	int x0, x1, y0;
	int bitmapR_x0 = 0;

	if (list.count == 0)
	{
		return;
	}

	rClient.x0 = 0;
	rClient.x1 = pObj->xSize;
	rClient.y0 = pObj->head.height;
	rClient.y1 = pObj->ySize - pObj->footer.height;
	GUI_SetClipRect(&rClient);
	GUI_SetBkColor(body.bkColor);
	GUI_Clear();
	GUI_SetClipRect(NULL);

	memset(&rClient, 0, sizeof(GUI_RECT));

	x0 = 0;
	node = list.head;
	while (node)
	{
		item = node->item;
		rClient = item.rect;
		
		GUI_SetClipRect(&rClient);
		GUI_SetBkColor(body.itemBkColor);
		GUI_Clear();
		GUI_SetFont(pObj->pFont);
		fontHeight = GUI_GetFontDistY();
		GUI_SetColor(pObj->fontColor);

		if (item.pBitmapL)
		{
			x1 = body.itemLeftBlankSize - body.space;
			x0 = x1 - item.pBitmapL->XSize;
			y0 = rClient.y0 + ((rClient.y1 - rClient.y0 - item.pBitmapL->YSize) / 2);
			GUI_DrawBitmap(item.pBitmapL, x0, y0);
		}
		
		if (item.pTextL)
		{
			x0 = rClient.x0 + body.itemLeftBlankSize;
			y0 = rClient.y0 + (rClient.y1 - rClient.y0 - fontHeight) / 2;
			GUI_DispStringAt(item.pTextL, x0, y0);
		}

		if (item.pBitmapR)
		{
			x0 = rClient.x1 - body.borderSize[TP_BORDER_RIGHT] - item.pBitmapR->XSize;
			bitmapR_x0 = x0;
			y0 = rClient.y0 + ((rClient.y1 - rClient.y0 - item.pBitmapR->YSize) / 2);
			GUI_DrawBitmap(item.pBitmapR, x0, y0);
		}

		if (item.pTextR)
		{
			if (bitmapR_x0 > 0)
			{
				x1 = bitmapR_x0 - body.space;
			}
			else
			{
				x1 = rClient.x1 - body.borderSize[TP_BORDER_RIGHT];
			}
			x0 = x1 - GUI_GetStringDistX(item.pTextR);
			y0 = rClient.y0 + (rClient.y1 - rClient.y0 - fontHeight) / 2;
			GUI_DispStringAt(item.pTextR, x0, y0);
		}
		
		GUI_SetClipRect(NULL);

		rClient.y0 = rClient.y1 - body.underline.height;
		if (node->next != NULL)
		{
			rClient.x0 = pObj->body.itemLeftBlankSize;
		}
		else
		{
			rClient.x1 = rClient.x0 + pObj->body.width;
		}

		GUI_SetClipRect(&rClient);
		GUI_SetBkColor(body.underline.color);
		GUI_Clear();
		GUI_SetClipRect(NULL);
		
		node = node->next;
	}
}

void TP_FAKELIST_OnPaint(TP_FAKELIST_Handle hWin, TP_FAKELIST_Obj * pObj)
{
	GUI_SetBkColor(pObj->bkColor);
	GUI_Clear();
	
	TP_FAKELIST_DrawHead(pObj);
	TP_FAKELIST_DrawBody(pObj);
	TP_FAKELIST_DrawFooter(pObj);
}

void TP_FAKELIST_AddItem(TP_FAKELIST_Handle hWin, int id, int flagL, char * sTextL, const GUI_BITMAP * pBitmapL, int flagR, char * sTextR, const GUI_BITMAP * pBitmapR, pHandleFun func)
{
	if (hWin)
	{
		TP_FAKELIST_Obj hObj;
		fakelistItemList list;
		fakelistListNode * node = NULL;
		GUI_RECT rClient = {0};
		int count;
		
		WM_GetUserData(hWin, &hObj, sizeof(TP_FAKELIST_Obj));

		list = hObj.body.list;
		count = list.count;

		rClient.x0 = 0;
		rClient.x1 = rClient.x0 + hObj.body.itemWidth;
		rClient.y0 = hObj.head.height + (hObj.body.itemHeight * count);
		rClient.y1 = rClient.y0 + hObj.body.itemHeight;

		count++;
		
		node = newFakelistListNode(id, count - 1, rClient, sTextL, pBitmapL, sTextR, pBitmapR, func);
		if (node)
		{
			fakelistList_addItem(&hObj.body.list, node);
		}

		if (rClient.y1 > hObj.body.height)
		{
			hObj.ySize += (rClient.y1 - hObj.body.height);
			hObj.body.height += (rClient.y1 - hObj.body.height);
			WM_SetSize(hWin, hObj.xSize, hObj.ySize);
		}

		WM_SetUserData(hWin, &hObj, sizeof(TP_FAKELIST_Obj));
	}
}

int TP_FAKELIST_GetSelFromPos(TP_FAKELIST_Obj * pObj, int x, int y)
{
	int sel = -1;
	fakelistListNode * node;

	node = findFakelistNodeByPos(&pObj->body.list, x, y);
	if (node)
	{
		sel = node->item.index;
	}

	return sel;
}

int TP_FAKELIST_GetSelIndex(TP_FAKELIST_Handle hWin)
{
	int sel = -1;
	if (hWin)
	{
		TP_FAKELIST_Obj hObj;
		WM_GetUserData(hWin, &hObj, sizeof(TP_FAKELIST_Obj));
		sel = hObj.selIndex;
	}

	return sel;
}

void TP_FAKELIST_SetSelBitmap(TP_FAKELIST_Handle hWin, int sel, const GUI_BITMAP * pBitmap, TP_FAKELIST_BITMAP_POS flag)
{
	if (sel == -1)
	{
		return;
	}

	if (hWin)
	{
		TP_FAKELIST_Obj hObj;
		WM_GetUserData(hWin, &hObj, sizeof(TP_FAKELIST_Obj));
		updateFakelistNodeBitmapByIndex(&hObj.body.list, sel, pBitmap, flag);
		WM_SetUserData(hWin, &hObj, sizeof(TP_FAKELIST_Obj));
	}
}

void TP_FAKELIST_HandleSel(TP_FAKELIST_Handle hWin, int sel)
{
	if (sel == -1)
	{
		return;
	}

	if (hWin)
	{
		TP_FAKELIST_Obj hObj;
		fakelistListNode * node = NULL;
		WM_GetUserData(hWin, &hObj, sizeof(TP_FAKELIST_Obj));
		node = findFakelistNodeByIndex(&hObj.body.list, sel);
		if (node)
		{
			if (node->item.handle)
			{
				node->item.handle(hWin);
			}
		}
	}
}

void TP_FAKELIST_SetSelIndex(TP_FAKELIST_Handle hWin, int sel)
{
	if (hWin)
	{
		TP_FAKELIST_Obj hObj;
		WM_GetUserData(hWin, &hObj, sizeof(TP_FAKELIST_Obj));
		if (sel != hObj.selIndex)
		{
			hObj.selIndex = sel;
			WM_NotifyParent(hWin, WM_NOTIFICATION_SEL_CHANGED);
		}
		WM_SetUserData(hWin, &hObj, sizeof(TP_FAKELIST_Obj));
	}
}

void TP_FAKELIST_NotifyScrollbarYOffset(TP_FAKELIST_Handle hWin, TP_FAKELIST_Obj * pObj, int offset)
{
	WM_MESSAGE msg = {0};
	msg.MsgId = TP_MSG_FAKELIST_Y_MOVE;
	msg.Data.v = offset;
	WM_SendMessage(pObj->hWinScroll, &msg);
}

int TP_FAKELIST_SetUserData(TP_FAKELIST_Handle hWin, void * pSrc, int size)
{
	TP_FAKELIST_Obj hObj;
	U8 * pExtraBytes;
	int numBytes;

	WM_LOCK();
	if (size <= 0)
	{
		return 0;
	}
	WM_GetUserData(hWin, &hObj, sizeof(TP_FAKELIST_Obj));

	pExtraBytes = (U8 *)malloc(sizeof(TP_FAKELIST_Obj) + hObj.numExtraBytes);
	if (pExtraBytes)
	{
		WM_GetUserData(hWin, pExtraBytes, sizeof(TP_FAKELIST_Obj) + hObj.numExtraBytes);
		if (size >= hObj.numExtraBytes)
		{
			numBytes = hObj.numExtraBytes;
		}
		else
		{
			numBytes = size;
		}
		GUI_MEMCPY(pExtraBytes + sizeof(TP_FAKELIST_Obj), pSrc, numBytes);
		WM_SetUserData(hWin, pExtraBytes, sizeof(TP_FAKELIST_Obj) + hObj.numExtraBytes);
		free(pExtraBytes);
		return numBytes;
	}
	return 0;
	WM_UNLOCK();
}

int TP_FAKELIST_GetUserData(TP_FAKELIST_Handle hWin, void * pDest, int size)
{
	TP_FAKELIST_Obj hObj;
	U8 * pExtraBytes;
	int numBytes;

	WM_LOCK();
	if (size <= 0)
	{
		return 0;
	}
	WM_GetUserData(hWin, &hObj, sizeof(TP_FAKELIST_Obj));
	pExtraBytes = (U8 *)malloc(sizeof(TP_FAKELIST_Obj) + hObj.numExtraBytes);
	if (pExtraBytes)
	{
		WM_GetUserData(hWin, pExtraBytes, sizeof(TP_FAKELIST_Obj) + hObj.numExtraBytes);
		if (size >= hObj.numExtraBytes)
		{
			numBytes = hObj.numExtraBytes;
		}
		else
		{
			numBytes = size;
		}
		GUI_MEMCPY(pDest, pExtraBytes + sizeof(TP_FAKELIST_Obj), numBytes);
		free(pExtraBytes);
		return numBytes;
	}
	return 0;
	WM_UNLOCK();
}


void TP_FAKELIST_Callback(WM_MESSAGE * pMsg)
{
	WM_HWIN hWin;
	TP_FAKELIST_Obj hObj;
	WM_PID_STATE_CHANGED_INFO * pInfo;
	static int pressXPos = -1;
	static int pressYPos = -1;
	int curYPos;
	int offset;
	
	hWin = pMsg->hWin;
	WM_GetUserData(hWin, &hObj, sizeof(TP_FAKELIST_Obj));
	curYPos = WM_GetWindowOrgY(hWin);
	switch (pMsg->MsgId)
	{
	case WM_PAINT:
		TP_FAKELIST_OnPaint(hWin, &hObj);
		break;

	case WM_TOUCH:
		break;

	case WM_PID_STATE_CHANGED:
		pInfo = (WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p);
		if (pInfo->State)
		{
			pressXPos = pInfo->x;
			pressYPos = pInfo->y;
			WM_NotifyParent(hWin, WM_NOTIFICATION_CLICKED);
		}
		else
		{
			if (pInfo->StatePrev == 1)
			{
				if ((pInfo->x == pressXPos) && (pInfo->y == pressYPos))
				{
					int sel;
					sel = TP_FAKELIST_GetSelFromPos(&hObj, pInfo->x, pInfo->y);
					if (sel >= 0)
					{
						TP_FAKELIST_SetSelIndex(hWin, sel);
						WM_InvalidateWindow(hWin);
						TP_FAKELIST_HandleSel(hWin, sel);
					}
				}
			}
		}
		break;

	case WM_MOVE:
		if ((hObj.ySize - hObj.orgYSize) > 0)
		{
			offset = ((hObj.orgYPos - curYPos) * hObj.scrollbar.range[1]) / (hObj.ySize - hObj.orgYSize);
			if (offset > 0)
			{
				TP_FAKELIST_NotifyScrollbarYOffset(hWin, &hObj, offset);
				WM_ShowWindow(hObj.hWinScroll);
			}
		}
		break;

	case WM_SET_FOCUS:
		if (pMsg->Data.v && (hObj.isFocussable))
		{
			pMsg->Data.v = 0;
		}
		WM_InvalidateWindow(hWin);
		
	case WM_SET_ID:
		hObj.id = pMsg->Data.v;
		WM_SetUserData(hWin, &hObj, sizeof(TP_FAKELIST_Obj));
		WM_InvalidateWindow(hWin);
		break;

	case WM_GET_ID:
		pMsg->Data.v = hObj.id;
		break;

	case WM_DELETE:
		fakelistList_Clear(&hObj.body.list);
		break;
		
	default:
		WM_DefaultProc(pMsg);
	}
}

TP_FAKELIST_Handle TP_FAKELIST_CreateIndirect(const GUI_WIDGET_CREATE_INFO * pCreateInofo, WM_HWIN hParent, int x0, int y0, WM_CALLBACK * cb)
{
	TP_FAKELIST_Handle hWin;

	hWin = TP_FAKELIST_Create(pCreateInofo->x0 + x0, pCreateInofo->y0 + y0, pCreateInofo->xSize, pCreateInofo->ySize, hParent, pCreateInofo->Flags, cb, pCreateInofo->NumExtraBytes, pCreateInofo->Id);

	return hWin;
}

void TP_FAKELIST_SCROLLBAR_Callback(WM_MESSAGE * pMsg)
{
	WM_HWIN hWin;
	fakelistScrollbarProps hObj;
	int absoluteOffset;
	static int showFlag = 0;

	hWin = pMsg->hWin;
	WM_GetUserData(hWin, &hObj, sizeof(fakelistScrollbarProps));
	switch (pMsg->MsgId)
	{
	case WM_PAINT:
		GUI_SetBkColor(hObj.bkColor);
		GUI_Clear();
		break;

	case TP_MSG_FAKELIST_Y_MOVE:
		showFlag = 1;
		absoluteOffset = hObj.orgYPos + pMsg->Data.v;

		if (pMsg->Data.v <= hObj.range[1])
		{
			WM_MoveTo(hWin, hObj.orgXPos, absoluteOffset);
		}
		WM_CreateTimer(hWin, 0, 1500, 0);
		showFlag = 0;
		break;

	case WM_TIMER:
		if (showFlag == 0)
		{
			WM_HideWindow(hWin);
		}
		break;

	default:
		WM_DefaultProc(pMsg);
	}
}

TP_FAKELIST_Handle TP_FAKELIST_Create(int x0, int y0, int xSize, int ySize, WM_HWIN hParent, U8 style, WM_CALLBACK * cb, int numExtraBytes, int id)
{
	TP_FAKELIST_Handle hWin;
	TP_FAKELIST_Obj hObj;
	fakelistHeadProps head;
	fakelistBodyProps body;
	fakelistFooterProps footer;
	fakelistLineProps line;
	fakelistScrollbarProps scrollbar;
	WM_CALLBACK * pUsed;

	hObj = TP_FAKELIST_Default;
	head = TP_FAKELIST_DefaultHead;
	body = TP_FAKELIST_DefaultBody;
	footer = TP_FAKELIST_DefaultFooter;
	line = TP_FAKELIST_DefaultLine;
	scrollbar = TP_FAKELIST_DefaultSCrollbar;
	head.underline = line;
	footer.online = line;
	body.underline = line;

	hObj.id = id;
	
	if (cb)
	{
		pUsed = cb;
	}
	else
	{
		pUsed = TP_FAKELIST_Callback;
	}

	hWin = WM_CreateWindowAsChild(x0, y0, xSize, ySize, hParent, style | WM_CF_MEMDEV | WM_CF_MOTION_Y, pUsed, sizeof(TP_FAKELIST_Obj) + numExtraBytes);
	if (hWin)
	{
		int scrollbar_x0 = x0 + xSize - hObj.scrollbar.width - 3;
		int scrollbar_y0 = y0;
		
		body.height = ySize - (head.height + footer.height);
		body.width = xSize;
		hObj.orgXPos = WM_GetWindowOrgX(hWin);
		hObj.orgYPos = WM_GetWindowOrgY(hWin);
		hObj.orgXSize = xSize;
		hObj.orgYSize = ySize;
		hObj.xSize = xSize;
		hObj.ySize = ySize;
		hObj.head = head;
		hObj.body = body;
		hObj.footer = footer;
		hObj.numExtraBytes = numExtraBytes;

		fakelistList_init(&hObj.body.list);

		hObj.hWinScroll = WM_CreateWindowAsChild(scrollbar_x0, scrollbar_y0, scrollbar.width, scrollbar.height, hParent, WM_CF_MEMDEV | WM_CF_HIDE, TP_FAKELIST_SCROLLBAR_Callback, sizeof(fakelistScrollbarProps));
		scrollbar.orgXPos = WM_GetWindowOrgX(hObj.hWinScroll);
		scrollbar.orgYPos = WM_GetWindowOrgY(hObj.hWinScroll);
		scrollbar.range[0] = 0;
		scrollbar.range[1] = WM_GetWindowSizeY(hParent) - scrollbar.height - scrollbar_y0;

		hObj.scrollbar = scrollbar;

		WM_SetUserData(hObj.hWinScroll, &scrollbar, sizeof(fakelistScrollbarProps));	
		WM_SetUserData(hWin, &hObj, sizeof(TP_FAKELIST_Obj));
	}
	
	return hWin;
}

