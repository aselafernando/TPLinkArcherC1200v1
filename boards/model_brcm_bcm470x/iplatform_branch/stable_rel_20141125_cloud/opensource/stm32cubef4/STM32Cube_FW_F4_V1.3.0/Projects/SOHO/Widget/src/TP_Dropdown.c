#include "TP_Dropdown.h"

extern GUI_CONST_STORAGE GUI_FONT GUI_FontF16ASCIIHNPMD;
extern GUI_CONST_STORAGE GUI_BITMAP bmDP_TickBmp;

TP_SCROLLBAR_Props TP_DROPDOWN_DefaultScroll = 
{
	0,
	0,
	50,
	2,
	{0, 0},
	TP_DROPDOWN_LINE_COLOR
};

dropdownLineProps TP_DROPDOWN_DefaultLine =  
{
	1,
	400,
	TP_DROPDOWN_LINE_COLOR
};

dropdownHeadProps TP_DROPDOWN_DefaultHead = 
{
	14,
	400,
	TP_DROPDOWN_BLANK_COLOR,
	NULL,
	0,
	{0}
};

dropdownFooterProps TP_DROPDOWN_DefaultFooter = 
{
	14,
	400,
	TP_DROPDOWN_BLANK_COLOR,
	{0}
};

dropdownBodyProps TP_DROPDOWN_DefaultBody =  
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

TP_DROPDOWN_Obj TP_DROPDOWN_Default = 
{
	0,
	0,
	0,
	0,
	0,
	0,
	GUI_WHITE,
	{14, 20, 14, 20},
	&GUI_FontF16ASCIIHNPMD,
	GUI_BLACK,
	&bmDP_TickBmp,
	0,
	-1,
	0,
	//TP_SCROLLBAR_DefaultProps
	{0},
	{0},
	{0},
	{0},
	0,
	0
};

void TP_DROPDOWN_SetDefaultScrollWidth(int width)
{
	TP_DROPDOWN_DefaultScroll.width = width;
}

void TP_DROPDOWN_SetDefaultScorllHeight(int height)
{
	TP_DROPDOWN_DefaultScroll.height = height;
}

void TP_DROPDOWN_SetDefaultScorllColor(GUI_COLOR color)
{
	TP_DROPDOWN_DefaultScroll.bkColor = color;
}

void TP_DROPDOWN_SetDefaultHeadBkColor(GUI_COLOR color)
{
	TP_DROPDOWN_DefaultHead.bkColor = color;
}

void TP_DROPDOWN_SetDefaultHeadHeight(int height)
{
	TP_DROPDOWN_DefaultHead.height = height;
}

void TP_DROPDOWN_SetDefaultFooterHeight(int height)
{
	TP_DROPDOWN_DefaultFooter.height = height;
}

void TP_DROPDOWN_SetDefaultFooterBkColor(GUI_COLOR color)
{
	TP_DROPDOWN_DefaultFooter.bkColor = color;
}

void TP_DROPDOWN_SetDefaultBkColor(GUI_COLOR color)
{
	TP_DROPDOWN_Default.bkColor = color;
}

void TP_DROPDOWN_SetDefaultBorderSize(int index, int size)
{
	TP_DROPDOWN_Default.borderSize[index] = size;
}

void TP_DROPDOWN_SetDefaultFont(const GUI_FONT * pFont)
{
	TP_DROPDOWN_Default.pFont = pFont;
}

void TP_DROPDOWN_SetDefaultFontColor(GUI_COLOR color)
{
	TP_DROPDOWN_Default.fontColor = color;
}

void TP_DROPDOWN_SetDefaultItemBkColor(GUI_COLOR color)
{
	TP_DROPDOWN_DefaultBody.itemBkColor = color;
}

void TP_DROPDOWN_SetDefaultItemWidth(int width)
{
	TP_DROPDOWN_DefaultBody.itemWidth = width;
}

void TP_DROPDOWN_SetDefaultItemHeight(int height)
{
	TP_DROPDOWN_DefaultBody.itemHeight = height;
}

void TP_DROPDOWN_SetDefaultLeftBlankWidth(int width)
{
	TP_DROPDOWN_DefaultBody.itemLeftBlankSize = width;
}

void TP_DROPDOWN_SetBkColor(TP_DROPDOWN_Handle hWin, GUI_COLOR color)
{
	if (hWin)
	{
		TP_DROPDOWN_Obj hObj;
		WM_GetUserData(hWin, &hObj, sizeof(TP_DROPDOWN_Obj));
		hObj.bkColor = color;
		WM_SetUserData(hWin, &hObj, sizeof(TP_DROPDOWN_Obj));
	}
}

void TP_DROPDOWN_SetBorderSize(TP_DROPDOWN_Handle hWin, int index, int size)
{
	if (hWin)
	{
		TP_DROPDOWN_Obj hObj;
		WM_GetUserData(hWin, &hObj, sizeof(TP_DROPDOWN_Obj));
		hObj.borderSize[index] = size;
		WM_SetUserData(hWin, &hObj, sizeof(TP_DROPDOWN_Obj));
	}
}

void TP_DROPDOWN_SetFont(TP_DROPDOWN_Handle hWin, const GUI_FONT * pFont)
{
	if (hWin)
	{
		TP_DROPDOWN_Obj hObj;
		WM_GetUserData(hWin, &hObj, sizeof(TP_DROPDOWN_Obj));
		hObj.pFont = pFont;
		WM_SetUserData(hWin, &hObj, sizeof(TP_DROPDOWN_Obj));
	}
}

void TP_DROPDOWN_SetFontColor(TP_DROPDOWN_Handle hWin, GUI_COLOR color)
{
	if (hWin)
	{
		TP_DROPDOWN_Obj hObj;
		WM_GetUserData(hWin, &hObj, sizeof(TP_DROPDOWN_Obj));
		hObj.fontColor = color;
		WM_SetUserData(hWin, &hObj, sizeof(TP_DROPDOWN_Obj));
	}
}

void TP_DROPDOWN_SetItemBkColor(TP_DROPDOWN_Handle hWin, GUI_COLOR color)
{
	if (hWin)
	{
		TP_DROPDOWN_Obj hObj;
		WM_GetUserData(hWin, &hObj, sizeof(TP_DROPDOWN_Obj));
		hObj.body.itemBkColor = color;
		WM_SetUserData(hWin, &hObj, sizeof(TP_DROPDOWN_Obj));
	}
}

void TP_DROPDOWN_SetItemWidth(TP_DROPDOWN_Handle hWin, int width)
{
	if (hWin)
	{
		TP_DROPDOWN_Obj hObj;
		WM_GetUserData(hWin, &hObj, sizeof(TP_DROPDOWN_Obj));
		hObj.body.itemWidth = width;
		WM_SetUserData(hWin, &hObj, sizeof(TP_DROPDOWN_Obj));
	}
}

void TP_DROPDOWN_SetItemHeight(TP_DROPDOWN_Handle hWin, int height)
{
	if (hWin)
	{
		TP_DROPDOWN_Obj hObj;
		WM_GetUserData(hWin, &hObj, sizeof(TP_DROPDOWN_Obj));
		hObj.body.itemHeight = height;
		WM_SetUserData(hWin, &hObj, sizeof(TP_DROPDOWN_Obj));
	}
}

void TP_DROPDOWN_SetFocussable(TP_DROPDOWN_Handle hWin, int state)
{
	if (hWin)
	{
		TP_DROPDOWN_Obj hObj;
		WM_GetUserData(hWin, &hObj, sizeof(TP_DROPDOWN_Obj));
		hObj.isFocussable = state;
		WM_SetUserData(hWin, &hObj, sizeof(TP_DROPDOWN_Obj));
	}
}

void TP_DROPDOWN_AddItem(TP_DROPDOWN_Handle hWin, char * sText, int id)
{
	if (hWin)
	{
		TP_DROPDOWN_Obj hObj;
		dropdownItemList list;
		dropdownBodyProps body;
		dropdownHeadProps head;
		dropdownFooterProps footer;
		dropdownListNode * node;
		int count;
		GUI_RECT rClient;
		
		WM_GetUserData(hWin, &hObj, sizeof(TP_DROPDOWN_Obj));

		if (sText == NULL)
		{
			return;
		}
		body = hObj.body;
		head = hObj.head;
		footer = hObj.footer;
		list = body.itemList;
		count = list.count;

		rClient.x0 = 0;
		rClient.y0 = head.height + (hObj.body.itemHeight * count);
		rClient.x1 = rClient.x0 + hObj.body.itemWidth;
		rClient.y1 = rClient.y0 + hObj.body.itemHeight;
		
		count++;

		node = newDropdownListNode(id, count - 1, rClient, sText);
		dropdownList_addItem(&hObj.body.itemList, node);
		
		if ((rClient.y1 + footer.height) > hObj.ySize)
		{
			hObj.ySize = rClient.y1 + footer.height;
			hObj.body.height = hObj.ySize - (hObj.head.height + hObj.footer.height);
			WM_SetSize(hWin, hObj.xSize, hObj.ySize);
		}
		
		WM_SetUserData(hWin, &hObj, sizeof(TP_DROPDOWN_Obj));
	}
}

int TP_DROPDOWN_GetSelFromPos(TP_DROPDOWN_Obj * pObj, int x, int y)
{
	int sel = -1;
	dropdownListNode * node;

	node = findDropdownNodeByPos(&pObj->body.itemList, x, y);
	if (node)
	{
		sel = node->item.index;
	}

	return sel;
}

int TP_DROPDOWN_GetSelIndex(TP_DROPDOWN_Handle hWin)
{
	int sel = -1;
	if (hWin)
	{
		TP_DROPDOWN_Obj hObj;
		WM_GetUserData(hWin, &hObj, sizeof(TP_DROPDOWN_Obj));
		sel = hObj.selectIndex;
	}

	return sel;
}

void TP_DROPDOWN_SetSelIndex(TP_DROPDOWN_Handle hWin, int sel)
{
	if (hWin)
	{
		TP_DROPDOWN_Obj hObj;
		WM_GetUserData(hWin, &hObj, sizeof(TP_DROPDOWN_Obj));
		if (sel != hObj.selectIndex)
		{
			hObj.selectIndex = sel;
			WM_NotifyParent(hWin, WM_NOTIFICATION_SEL_CHANGED);
		}
		WM_SetUserData(hWin, &hObj, sizeof(TP_DROPDOWN_Obj));
	}
}

int TP_DROPDOWN_GetNumItems(TP_DROPDOWN_Handle hWin)
{
	int count = 0;
	if (hWin)
	{
		TP_DROPDOWN_Obj hObj;
		WM_GetUserData(hWin, &hObj, sizeof(TP_DROPDOWN_Obj));
		count = hObj.body.itemList.count;
	}
	return count;
}

void TP_DROPDOWN_GetItemText(TP_DROPDOWN_Handle hWin, int index, char * pBuffer, int maxSize)
{
	if (hWin)
	{
		TP_DROPDOWN_Obj hObj;
		dropdownListNode * node;
		dropdownItemList list;
		char * pText;
		WM_GetUserData(hWin, &hObj, sizeof(TP_DROPDOWN_Obj));
		list = hObj.body.itemList;
		node = findDropdownNodeByIndex(&list, index);
		if (node)
		{
			int len;
			pText = node->item.pText;
			len = strlen(pText);
			if (len > (maxSize - 1))
			{
				len = maxSize - 1;
			}
			memcpy(pBuffer, pText, len);
			pBuffer[len] = '\0';
		}
	}
}

void TP_DROPDOWN_NotityOwner(TP_DROPDOWN_Handle hWin, TP_DROPDOWN_Obj * pObj, int notification)
{
	WM_HWIN hOwner;
	WM_MESSAGE msg = {0};
	
	hOwner = pObj->hOwner ? pObj->hOwner : WM_GetParent(hWin);
	msg.MsgId = WM_NOTIFY_PARENT;
	msg.Data.v = notification;
	msg.hWinSrc = hWin;
	WM_SendMessage(hOwner, &msg);
}

void TP_DROPDOWN_NotifyParentYOffset(TP_DROPDOWN_Handle hWin, int offset)
{
	WM_MESSAGE msg = {0};
	msg.MsgId = TP_MSG_DROPDOWN_Y_MOVE;
	msg.Data.v = offset;
	WM_SendMessage(WM_GetParent(hWin), &msg);
}

void TP_DROPDOWN_NotifyScrollbarYOffset(TP_DROPDOWN_Handle hWin, TP_DROPDOWN_Obj * pObj, int offset)
{
	WM_MESSAGE msg = {0};
	msg.MsgId = TP_MSG_DROPDOWN_Y_MOVE;
	msg.Data.v = offset;
	WM_SendMessage(pObj->hScrollbar, &msg);
}

void TP_DROPDOWN_DrawHead(TP_DROPDOWN_Obj * pObj)
{
	GUI_RECT rClient = {0};
	dropdownHeadProps head = pObj->head;
	dropdownLineProps underline = head.underline;

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

void TP_DROPDOWN_DrawFooter(TP_DROPDOWN_Obj * pObj)
{
#define _TP_FOOTER_ONLINE_
#undef _TP_FOOTER_ONLINE_
	dropdownFooterProps footer = pObj->footer;
#ifdef _TP_FOOTER_ONLINE_
	dropdownLineProps online = footer.online;
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

void TP_DROPDOWN_DrawBody(TP_DROPDOWN_Obj * pObj)
{
	dropdownBodyProps body = pObj->body;
	GUI_RECT rClient = {0};
	dropdownItemList list = body.itemList;
	dropdownListNode * node;
	dropdownNodeItem item;
	int fontHeight;
	int x0, y0;

	if (list.count == 0)
	{
		return;
	}

	rClient.x0 = 0;
	rClient.x1 = body.width;
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

		if (item.pText)
		{
			x0 = rClient.x0 + body.itemLeftBlankSize + pObj->borderSize[TP_BORDER_LEFT];
			y0 = rClient.y0 + (rClient.y1 - rClient.y0 - fontHeight) / 2;
			GUI_DispStringAt(item.pText, x0, y0);
		}

		if (pObj->selectIndex == node->item.index)
		{
			if (pObj->selectBitmap)
			{
				int x0 = rClient.x1 - pObj->borderSize[TP_BORDER_RIGHT] - pObj->selectBitmap->XSize;
				int y0 = rClient.y0 + ((rClient.y1 - rClient.y0 - pObj->selectBitmap->YSize) / 2);
				GUI_DrawBitmap(pObj->selectBitmap, x0, y0);
			}
		}
		
		GUI_SetClipRect(NULL);

		rClient.y0 = rClient.y1 - body.underline.height;
		if (node->next != NULL)
		{
			rClient.x0 = pObj->body.itemLeftBlankSize;
		}
		else
		{
			rClient.x1 = pObj->body.width;
		}

		GUI_SetClipRect(&rClient);
		GUI_SetBkColor(body.underline.color);
		GUI_Clear();
		GUI_SetClipRect(NULL);
		
		node = node->next;
	}
}

void TP_DROPDOWN_OnPaint(TP_DROPDOWN_Handle hWin, TP_DROPDOWN_Obj * pObj)
{
	GUI_SetBkColor(pObj->bkColor);
	GUI_Clear();

	TP_DROPDOWN_DrawHead(pObj);
	TP_DROPDOWN_DrawBody(pObj);
	TP_DROPDOWN_DrawFooter(pObj);
}

void TP_DROPDOWN_Callback(WM_MESSAGE * pMsg)
{
	WM_HWIN hWin;
	TP_DROPDOWN_Obj hObj;
	GUI_PID_STATE * pState;
	WM_PID_STATE_CHANGED_INFO * pInfo;
	int offset;
	int curYPos;
	static int pressXPos = -1;
	static int pressYPos = -1;

	hWin = pMsg->hWin;
	WM_GetUserData(hWin, &hObj, sizeof(TP_DROPDOWN_Obj));
	curYPos = WM_GetWindowOrgY(hWin);
	switch (pMsg->MsgId)
	{
	case WM_PAINT:
		TP_DROPDOWN_OnPaint(hWin, &hObj);
		break;

	case WM_TOUCH:
		pState = (GUI_PID_STATE *)(pMsg->Data.p);
		if (pState)
		{
#if 0
			if (pState->Pressed == 0)
			{
				int sel;
				sel = TP_DROPDOWN_GetSelFromPos(&hObj, pState->x, pState->y);
				if (sel >= 0)
				{
					TP_DROPDOWN_SetSelIndex(hWin, sel);
					WM_InvalidateWindow(hWin);
				}
				WM_NotifyParent(hWin, WM_NOTIFICATION_RELEASED);
				//TP_DROPDOWN_NotityOwner(hWin, &hObj, WM_NOTIFICATION_RELEASED);
			}
			else
			{
				pressXPos = pState->x;
				pressYPos = pState->y;
				WM_NotifyParent(hWin, WM_NOTIFICATION_CLICKED);
			}
#endif
		}
		else
		{
			WM_NotifyParent(hWin, WM_NOTIFICATION_MOVED_OUT);
		}
		break;

#if 1
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
					sel = TP_DROPDOWN_GetSelFromPos(&hObj, pInfo->x, pInfo->y);
					if (sel >= 0)
					{
						TP_DROPDOWN_SetSelIndex(hWin, sel);
						WM_InvalidateWindow(hWin);
					}
				}
			}
			WM_NotifyParent(hWin, WM_NOTIFICATION_RELEASED);
		}
		break;
#endif

	case WM_MOVE:
		if (hObj.ySize - hObj.orgYSize > 0)
		{
			offset = ((hObj.orgYPos - curYPos) * hObj.scrollbarProps.range[1]) / (hObj.ySize - hObj.orgYSize);
			if (offset > 0)
			{
				TP_DROPDOWN_NotifyScrollbarYOffset(hWin, &hObj, offset);
				WM_ShowWindow(hObj.hScrollbar);
			}
		}
		break;

	case WM_SET_ID:
		hObj.id = pMsg->Data.v;
		WM_SetUserData(hWin, &hObj, sizeof(TP_DROPDOWN_Obj));
		WM_InvalidateWindow(hWin);
		break;

	case WM_GET_ID:
		pMsg->Data.v = hObj.id;
		break;

	case WM_SET_FOCUS:
		if (pMsg->Data.v && (hObj.isFocussable))
		{
			pMsg->Data.v = 0;
		}
		WM_InvalidateWindow(hWin);

	case WM_DELETE:
		dropdownList_Clear(&hObj.body.itemList);
		break;
		
	default:
		WM_DefaultProc(pMsg);
	}
}

void TP_DROPDOWN_SCROLLBAR_Callback(WM_MESSAGE * pMsg)
{
	WM_HWIN hWin;
	TP_SCROLLBAR_Props hObj;
	int absoluteOffset;
	static int showFlag = 0;
	
	hWin = pMsg->hWin;
	WM_GetUserData(hWin, &hObj, sizeof(TP_SCROLLBAR_Props));
	switch (pMsg->MsgId)
	{
	case WM_PAINT:
		GUI_SetBkColor(hObj.bkColor);
		GUI_Clear();
		break;

	case TP_MSG_DROPDOWN_Y_MOVE:
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

int TP_DROPDOWN_SetUserData(TP_DROPDOWN_Handle hWin, void * pSrc, int size)
{
	TP_DROPDOWN_Obj hObj;
	U8 * pExtraBytes;
	int numBytes;

	WM_LOCK();
	if (size <= 0)
	{
		return 0;
	}
	WM_GetUserData(hWin, &hObj, sizeof(TP_DROPDOWN_Obj));

	pExtraBytes = (U8 *)malloc(sizeof(TP_DROPDOWN_Obj) + hObj.numExtraBytes);
	if (pExtraBytes)
	{
		WM_GetUserData(hWin, pExtraBytes, sizeof(TP_DROPDOWN_Obj) + hObj.numExtraBytes);
		if (size >= hObj.numExtraBytes)
		{
			numBytes = hObj.numExtraBytes;
		}
		else
		{
			numBytes = size;
		}
		GUI_MEMCPY(pExtraBytes + sizeof(TP_DROPDOWN_Obj), pSrc, numBytes);
		WM_SetUserData(hWin, pExtraBytes, sizeof(TP_DROPDOWN_Obj) + hObj.numExtraBytes);
		free(pExtraBytes);
		return numBytes;
	}
	return 0;
	WM_UNLOCK();
}

int TP_DROPDOWN_GetUserData(TP_DROPDOWN_Handle hWin, void * pDest, int size)
{
	TP_DROPDOWN_Obj hObj;
	U8 * pExtraBytes;
	int numBytes;

	WM_LOCK();
	if (size <= 0)
	{
		return 0;
	}
	WM_GetUserData(hWin, &hObj, sizeof(TP_DROPDOWN_Obj));
	pExtraBytes = (U8 *)malloc(sizeof(TP_DROPDOWN_Obj) + hObj.numExtraBytes);
	if (pExtraBytes)
	{
		WM_GetUserData(hWin, pExtraBytes, sizeof(TP_DROPDOWN_Obj) + hObj.numExtraBytes);
		if (size >= hObj.numExtraBytes)
		{
			numBytes = hObj.numExtraBytes;
		}
		else
		{
			numBytes = size;
		}
		GUI_MEMCPY(pDest, pExtraBytes + sizeof(TP_DROPDOWN_Obj), numBytes);
		free(pExtraBytes);
		return numBytes;
	}
	return 0;
	WM_UNLOCK();
}

TP_DROPDOWN_Handle TP_DROPDOWN_Create(int x0, int y0, int xSize, int ySize, WM_HWIN hParent, U8 style, WM_CALLBACK * cb, int numExtraBytes, int id)
{
	TP_DROPDOWN_Handle hWin;
	TP_DROPDOWN_Obj hObj;
	TP_SCROLLBAR_Props hScrollBarProps;
	dropdownHeadProps head;
	dropdownFooterProps footer;
	dropdownBodyProps body;
	WM_CALLBACK * pUsed;

	hObj = TP_DROPDOWN_Default;
	hScrollBarProps = TP_DROPDOWN_DefaultScroll;
	head = TP_DROPDOWN_DefaultHead;
	body = TP_DROPDOWN_DefaultBody;
	footer = TP_DROPDOWN_DefaultFooter;
	hObj.id = id;
	hObj.numExtraBytes = numExtraBytes;
	head.underline = TP_DROPDOWN_DefaultLine;
	footer.online = TP_DROPDOWN_DefaultLine;
	body.underline = TP_DROPDOWN_DefaultLine;
	
	if (cb)
	{
		pUsed = cb;
	}
	else
	{
		pUsed = TP_DROPDOWN_Callback;
	}

	dropdownList_init(&hObj.body.itemList);
	hWin = WM_CreateWindowAsChild(x0, y0, xSize, ySize, hParent, style | WM_CF_MEMDEV | WM_CF_MOTION_Y, pUsed, sizeof(TP_DROPDOWN_Obj) + numExtraBytes);
	if (hWin)
	{
		int scrollbar_x0 = x0 + xSize - hObj.scrollbarProps.width - 3;
		int scrollbar_y0 = y0;
		hObj.xSize = xSize;
		hObj.ySize = ySize;
		hObj.orgYSize = ySize;
		hObj.orgXPos = WM_GetWindowOrgX(hWin);
		hObj.orgYPos = WM_GetWindowOrgY(hWin);
		hObj.hOwner = hParent;

		hObj.hScrollbar = WM_CreateWindowAsChild(scrollbar_x0, scrollbar_y0, hScrollBarProps.width, hScrollBarProps.height, hParent, WM_CF_MEMDEV | WM_CF_HIDE, TP_DROPDOWN_SCROLLBAR_Callback, sizeof(TP_SCROLLBAR_Props));
		hScrollBarProps.orgXPos = WM_GetWindowOrgX(hObj.hScrollbar);
		hScrollBarProps.orgYPos = WM_GetWindowOrgY(hObj.hScrollbar);
		hScrollBarProps.range[0] = 0;
		hScrollBarProps.range[1] = WM_GetWindowSizeY(hParent) - hScrollBarProps.height - scrollbar_y0;

		hObj.scrollbarProps = hScrollBarProps;
		
		head.width = xSize;
		hObj.head = head;

		footer.width = xSize;
		hObj.footer = footer;

		body.width = xSize;
		body.height = ySize - (hObj.head.height + hObj.footer.height);
		hObj.body = body;
		
		WM_SetUserData(hObj.hScrollbar, &hScrollBarProps, sizeof(TP_SCROLLBAR_Props));
		WM_SetUserData(hWin, &hObj, sizeof(TP_DROPDOWN_Obj));
	}

	return hWin;
}

