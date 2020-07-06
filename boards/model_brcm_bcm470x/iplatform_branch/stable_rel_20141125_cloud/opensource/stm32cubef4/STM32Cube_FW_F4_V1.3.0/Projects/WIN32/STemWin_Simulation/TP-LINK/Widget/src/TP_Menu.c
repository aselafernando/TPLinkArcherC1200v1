#include <stdio.h>
#include <string.h> // Definition of NULL
#include <stdlib.h> // malloc() and free()

#include "TP_Menu.h"
#include "TP_Global.h"

#define TP_MENU_LINE_COLOR					GUI_GRAY

#define	TP_MENU_BK_COLOR_ENABLED_SEL		GUI_DARKGRAY
#define	TP_MENU_BK_COLOR_ENABLED_NORMAL		GUI_GRAY
#define	TP_MENU_BK_COLOR_DISABLED_SEL		GUI_DARKGRAY
#define	TP_MENU_BK_COLOR_DISABLED_NORMAL	GUI_DARKGRAY

#define	TP_MENU_TEXT_COLOR_ENABLED_SEL		GUI_WHITE
#define	TP_MENU_TEXT_COLOR_ENABLED_NORMAL	GUI_WHITE
#define	TP_MENU_TEXT_COLOR_DISABLED_SEL		GUI_WHITE
#define	TP_MENU_TEXT_COLOR_DISABLED_NORMAL	GUI_WHITE

#define TP_MENU_DEFAULT_FONT		GUI_DEFAULT_FONT
#define TP_MENU_DEFAULT_LEFT_BOARDER_WIDTH	(20)
#define TP_MENU_DEFAULT_RIGHT_BOARDER_WIDTH	(40)
#define TP_MENU_I2T_SPACE					(20)
#define TP_MENU_DEFAULT_HEIGHT				(20)
#define TP_MENU_DEFAULT_TOP_BOARDER_HEIGHT	(5)
#define TP_MENU_DEFAULT_BOTTOM_BOARDER_HEIGHT	(5)

#define TP_MENU_SEPARATOR_LINE_HEIGHT		(1)

#define TP_MENU_TEXT_DEFAULT_ALIGN		(GUI_TA_LEFT | GUI_TA_VCENTER)

#define TP_MENU_IS_POINT_IN_RECT(rect, x, y) \
	((((x) >= (rect).x0) && ((x) <= (rect).x1)) && (((y) >= (rect).y0) && ((y) <= (rect).y1)))


#define WM_PID_STATE_PRESSED	(1)
#define WM_PID_STATE_UNPRESSED	(0)


/*********************************************************************
*
* TP_Checkbox_Obj
*//**/
static TP_MENU_OBJ TP_Menu_Default = 
{
	0,
	{
		TP_MENU_LINE_COLOR,
		{
			TP_MENU_BK_COLOR_ENABLED_SEL,
			TP_MENU_BK_COLOR_ENABLED_NORMAL,
			TP_MENU_BK_COLOR_DISABLED_SEL,
			TP_MENU_BK_COLOR_DISABLED_NORMAL,
		},
		{
			TP_MENU_TEXT_COLOR_ENABLED_SEL,
			TP_MENU_TEXT_COLOR_ENABLED_NORMAL,
			TP_MENU_TEXT_COLOR_DISABLED_SEL,
			TP_MENU_TEXT_COLOR_DISABLED_SEL,
		},
		GUI_FONT_24_ASCII,
		TP_MENU_DEFAULT_LEFT_BOARDER_WIDTH,
		TP_MENU_DEFAULT_RIGHT_BOARDER_WIDTH,
		TP_MENU_I2T_SPACE,
		TP_MENU_DEFAULT_HEIGHT,
	},			/* 用户可配置的属性 */
	{NULL},		/* 菜单列表 */
	0,			/* 菜单项的数量 */
	0,			/* 额外的存储空间数量 */
	0,			/* 被按下的菜单项的ID */
};

static int TP_Menu_GetMaxTextXSize(const TP_MENU_ITEM_HEAD *pItemHead, const GUI_FONT *pFont)
{
	TP_MENU_ITEM_LIST *pList = NULL;
	int xSize = 0;
	int tmp= 0;

	if (pItemHead == NULL)
	{
		return RET_FAIL;
	}

	pList = pItemHead->next;

	GUI_SetFont(pFont);
	while(pList != NULL)
	{
		if (pList->item.pText)
		{
			tmp = GUI_GetStringDistX(pList->item.pText);
			xSize = (tmp > xSize)? tmp : xSize;
		}
		pList = pList->next;
	}

	return xSize;
}


static int TP_Menu_GetItemIdOfPid(const TP_MENU_ITEM_HEAD *pItemHead, int x, int y)
{
	TP_MENU_ITEM_LIST *pList = NULL;
	//int xSize = 0;

	if (pItemHead == NULL)
	{
		return RET_FAIL;
	}

	pList = pItemHead->next;

	while(pList != NULL)
	{
		if (TP_MENU_IS_POINT_IN_RECT(pList->rect, x, y))
		{
			return pList->item.id;
		}
		pList = pList->next;
	}

	return RET_FAIL;
}


static int TP_Menu_AddList(TP_MENU_ITEM_HEAD *pItemHead, TP_MENU_ITEM item)
{
	TP_MENU_ITEM_LIST *pNewItemList = NULL;
	TP_MENU_ITEM_LIST *pTmpList = NULL;
	TP_MENU_ITEM *pNewItem = NULL;
	
	if ((pItemHead == NULL) || (item.pText == NULL))
	{
		return RET_FAIL;
	}

	pNewItemList = (TP_MENU_ITEM_LIST *)malloc(sizeof(TP_MENU_ITEM_LIST));

	if (pNewItemList == NULL)
	{
		return RET_FAIL;
	}
	pNewItemList->next = NULL;
	pNewItem = &(pNewItemList->item);

	pNewItem->flags = item.flags;
	pNewItem->hSubmenu = item.hSubmenu;
	pNewItem->id = item.id;
	pNewItem->pBitmap = item.pBitmap;

	pNewItem->pText = malloc(strlen(item.pText) + 1);
	if (pNewItem->pText == NULL)
	{
		return RET_FAIL;
	}
	memset(pNewItem->pText, 0, strlen(item.pText) + 1);
	GUI_MEMCPY(pNewItem->pText, item.pText, strlen(item.pText));

	if (pItemHead->next == NULL)/* 第1个节点不存在的情况 */
	{
		pItemHead->next = pNewItemList;
		return RET_SUCCESS;
	}

	pTmpList = pItemHead->next;
	while (pTmpList != NULL)
	{
		if (pTmpList->next == NULL)
		{
			pTmpList->next = pNewItemList;
			return RET_SUCCESS;
		}

		pTmpList = pTmpList->next;
	}

	return RET_FAIL;
}

static int TP_Menu_InsertItemById(TP_MENU_ITEM_HEAD *pItemHead, U16 itemId, const TP_MENU_ITEM * pItemData)
{
	TP_MENU_ITEM_LIST *pNewItemList = NULL;
	TP_MENU_ITEM_LIST *pTmpList = NULL, *pPreList = NULL;
	TP_MENU_ITEM *pNewItem = NULL;
	
	if ((pItemHead == NULL) || (pItemData->pText == NULL))
	{
		return RET_FAIL;
	}

	pNewItemList = (TP_MENU_ITEM_LIST *)malloc(sizeof(TP_MENU_ITEM_LIST));

	if (pNewItemList == NULL)
	{
		return RET_FAIL;
	}
	pNewItemList->next = NULL;
	pNewItem = &(pNewItemList->item);

	pNewItem->flags = pItemData->flags;
	pNewItem->hSubmenu = pItemData->hSubmenu;
	pNewItem->id = pItemData->id;
	pNewItem->pBitmap = pItemData->pBitmap;

	pNewItem->pText = malloc(strlen(pItemData->pText) + 1);
	if (pNewItem->pText == NULL)
	{
		return RET_FAIL;
	}
	memset(pNewItem->pText, 0, strlen(pItemData->pText) + 1);
	GUI_MEMCPY(pNewItem->pText, pItemData->pText, strlen(pItemData->pText));

	if (pItemHead->next == NULL)/* 链表为空是返回错误*/
	{
		return RET_FAIL;
	}

	pTmpList = pItemHead->next;

	if (pTmpList->item.id == itemId)/* 如果在第1个节点前插入 */
	{
		pNewItemList->next = pTmpList;
		pItemHead->next = pNewItemList;
		return RET_SUCCESS;
	}

	pPreList = pTmpList;
	pTmpList = pTmpList->next;
	while (pTmpList != NULL)/* 第2个节点开始插入 */
	{
		if (pTmpList->item.id == itemId)
		{
			pPreList->next = pNewItemList;
			pNewItemList->next = pTmpList;
			return RET_SUCCESS;
		}

		pPreList = pTmpList;
		pTmpList = pTmpList->next;
	}

	return RET_FAIL;
}


static void TP_Menu_FreeItem(TP_MENU_ITEM_LIST *pItem)
{
	TP_MENU_ITEM item;
	if (pItem ==  NULL)
	{
		return;
	}

	item = pItem->item;
	if (item.pText)
	{
		free(item.pText);
	}

	free(pItem);
}

static int TP_Menu_DeleteItemById(TP_MENU_ITEM_HEAD *pItemHead, U16 itemId)
{
	TP_MENU_ITEM_LIST *pPreList = NULL;
	TP_MENU_ITEM_LIST *pTmpList = NULL;
	
	if ((pItemHead == NULL) || (pItemHead->next == NULL))
	{
		return RET_FAIL;
	}

	pTmpList = pItemHead->next;
	

	/* 如果第1个节点就是要删除的节点，则要先处理 */
	if (pTmpList->item.id == itemId)
	{
		pItemHead->next = pTmpList->next;
		TP_Menu_FreeItem(pTmpList);
		return RET_SUCCESS;
	}

	pPreList = pTmpList;
	pTmpList = pTmpList->next;
	while (pTmpList != NULL)
	{
		if (pTmpList->item.id == itemId)
		{
			pPreList->next = pTmpList->next;
			TP_Menu_FreeItem(pTmpList);
			return RET_SUCCESS;
		}

		pPreList = pTmpList;
		pTmpList = pTmpList->next;		
	}

	return RET_FAIL;
}

static void TP_Menu_DeleteAllItem(TP_MENU_ITEM_HEAD *pItemHead)
{
	TP_MENU_ITEM_LIST *pDelList = NULL;
	TP_MENU_ITEM_LIST *pTmpList = NULL;
	
	if ((pItemHead == NULL) || (pItemHead->next == NULL))
	{
		return;
	}

	pTmpList = pItemHead->next;
	while (pTmpList != NULL)
	{
		pDelList = pTmpList;
		pTmpList = pTmpList->next;
		TP_Menu_FreeItem(pDelList);
	}
}

static int TP_Menu_DisableItemById(TP_MENU_ITEM_HEAD *pItemHead, U16 itemId)
{
	TP_MENU_ITEM_LIST *pTmpList = NULL;
	
	if ((pItemHead == NULL) || (pItemHead->next == NULL))
	{
		return RET_FAIL;
	}

	pTmpList = pTmpList->next;
	while (pTmpList != NULL)
	{
		if (pTmpList->item.id == itemId)
		{
			pTmpList->item.flags |= TP_MENU_IF_DISABLED;
			return RET_SUCCESS;
		}
		pTmpList = pTmpList->next;		
	}

	return RET_FAIL;
}


static int TP_Menu_EnableItemById(TP_MENU_ITEM_HEAD *pItemHead, U16 itemId)
{
	TP_MENU_ITEM_LIST *pTmpList = NULL;
	
	if ((pItemHead == NULL) || (pItemHead->next == NULL))
	{
		return RET_FAIL;
	}

	pTmpList = pTmpList->next;
	while (pTmpList != NULL)
	{
		if (pTmpList->item.id == itemId)
		{
			pTmpList->item.flags &= (~TP_MENU_IF_DISABLED);
			return RET_SUCCESS;
		}
		pTmpList = pTmpList->next;		
	}

	return RET_FAIL;
}

static int  TP_Menu_GetItemById (TP_MENU_ITEM_HEAD *pItemHead, U16 itemId, TP_MENU_ITEM* pItemData)
{
	TP_MENU_ITEM_LIST *pTmpList = NULL;
	
	if ((pItemHead == NULL) || (pItemHead->next == NULL) || (pItemData == NULL))
	{
		return RET_FAIL;
	}

	pTmpList = pTmpList->next;
	while (pTmpList != NULL)
	{
		if (pTmpList->item.id == itemId)
		{
			GUI_MEMCPY(pItemData, &(pTmpList->item), sizeof(TP_MENU_ITEM));
			if (pTmpList->item.pText)
			{
				pItemData->pText = malloc(strlen(pTmpList->item.pText) + 1);
				if (pItemData->pText == NULL)
				{
					return RET_FAIL;
				}
				memset(pItemData->pText, 0, strlen(pTmpList->item.pText) + 1);
				GUI_MEMCPY(pItemData->pText, pTmpList->item.pText, strlen(pTmpList->item.pText));
			}
			return RET_SUCCESS;
		}
		pTmpList = pTmpList->next;		
	}

	return RET_FAIL;
}


int TP_Menu_GetMenuXSize(TP_MENU_Handle hObj)
{
	TP_MENU_OBJ obj;
	WM_GetUserData(hObj, &obj, sizeof(TP_MENU_OBJ));

	return obj.props.borderWidth[TP_MENU_BORDER_INDEX_LEFT]
			+ TP_Menu_GetMaxTextXSize(&(obj.itemHead), obj.props.pFont)
			+ obj.props.borderWidth[TP_MENU_BORDER_INDEX_RIGHT]
			+ 2 * TP_MENU_SEPARATOR_LINE_HEIGHT;
}

static int TP_Menu_GetMenuItemYSize(TP_MENU_Handle hObj)
{
	TP_MENU_OBJ obj;
	WM_GetUserData(hObj, &obj, sizeof(TP_MENU_OBJ));

	return TP_MENU_DEFAULT_TOP_BOARDER_HEIGHT + 
		GUI_GetFontDistY() + TP_MENU_DEFAULT_BOTTOM_BOARDER_HEIGHT;
}

static int TP_Menu_GetMenuYSize(TP_MENU_Handle hObj)
{
	TP_MENU_OBJ obj;
	int yItemSize = TP_Menu_GetMenuItemYSize(hObj);
	
	WM_GetUserData(hObj, &obj, sizeof(TP_MENU_OBJ));
	return (yItemSize + TP_MENU_SEPARATOR_LINE_HEIGHT) * obj.ItemNum 
								+ TP_MENU_SEPARATOR_LINE_HEIGHT;
}

void TP_Menu_AddItem(TP_MENU_Handle hObj, const TP_MENU_ITEM* pItem)
{
	TP_MENU_OBJ obj;
	int xSize, ySize;
	//int yItemSize;
	int oldXSize, oldYSize;
	
	if (pItem == NULL)
	{
		return;
	}

	WM_GetUserData(hObj, &obj, sizeof(TP_MENU_OBJ));
	if (TP_Menu_AddList(&(obj.itemHead), *pItem) == RET_FAIL)/* 一般不会失败 */
	{
		return;
	}
	obj.ItemNum++;
	WM_SetUserData(hObj, &obj, sizeof(TP_MENU_OBJ));

	oldXSize = WM_GetWindowSizeX(hObj);
	oldYSize = WM_GetWindowSizeY(hObj);

	xSize = TP_Menu_GetMenuXSize(hObj);
	//yItemSize = TP_Menu_GetMenuItemYSize(hObj);
	ySize = TP_Menu_GetMenuYSize(hObj);

	if ((xSize != oldXSize) || (ySize != oldYSize))
	{
		WM_ResizeWindow(hObj, xSize - oldXSize, ySize - oldYSize);
	}
}

void     TP_Menu_InsertItem(TP_MENU_Handle hObj, U16 itemId, const TP_MENU_ITEM* pItemData)
{
	TP_MENU_OBJ obj;
	int xSize, ySize;
	//int yItemSize;
	int oldXSize, oldYSize;
	
	WM_GetUserData(hObj, &obj, sizeof(TP_MENU_OBJ));
	if (TP_Menu_InsertItemById(&(obj.itemHead), itemId, pItemData) == RET_FAIL)
	{
		return;
	}
	obj.ItemNum++;
	WM_SetUserData(hObj, &obj, sizeof(TP_MENU_OBJ));

	oldXSize = WM_GetWindowSizeX(hObj);
	oldYSize = WM_GetWindowSizeY(hObj);

	xSize = TP_Menu_GetMenuXSize(hObj);
	//yItemSize = TP_Menu_GetMenuItemYSize(hObj);
	ySize = TP_Menu_GetMenuYSize(hObj);

	if ((xSize != oldXSize) || (ySize != oldYSize))
	{
		WM_ResizeWindow(hObj, xSize - oldXSize, ySize - oldYSize);
	}
}

void      TP_Menu_DeleteItem   (TP_MENU_Handle hObj, U16 itemId)
{
	TP_MENU_OBJ obj;
	int xSize, ySize;
	//int yItemSize;
	int oldXSize, oldYSize;

	WM_GetUserData(hObj, &obj, sizeof(TP_MENU_OBJ));
	if (TP_Menu_DeleteItemById(&(obj.itemHead), itemId)== RET_FAIL)
	{
		return;
	}

	obj.ItemNum--;
	WM_SetUserData(hObj, &obj, sizeof(TP_MENU_OBJ));

	if (obj.ItemNum == 0)
	{
		WM_HideWindow(hObj);
		WM_InvalidateWindow(hObj);
	}
	
	oldXSize = WM_GetWindowSizeX(hObj);
	oldYSize = WM_GetWindowSizeY(hObj);

	xSize = TP_Menu_GetMenuXSize(hObj);
	//yItemSize = TP_Menu_GetMenuItemYSize(hObj);
	ySize = TP_Menu_GetMenuYSize(hObj);

	if ((xSize != oldXSize) || (ySize != oldYSize))
	{
		WM_ResizeWindow(hObj, xSize - oldXSize, ySize - oldYSize);
	}
}

void      TP_Menu_ClearItem   (TP_MENU_Handle hObj)
{
	TP_MENU_OBJ obj;
	int xSize, ySize;
	//int yItemSize;
	int oldXSize, oldYSize;

	WM_GetUserData(hObj, &obj, sizeof(TP_MENU_OBJ));
	TP_Menu_DeleteAllItem(&(obj.itemHead));
	obj.ItemNum = 0;
	WM_SetUserData(hObj, &obj, sizeof(TP_MENU_OBJ));
	
	oldXSize = WM_GetWindowSizeX(hObj);
	oldYSize = WM_GetWindowSizeY(hObj);

	xSize = TP_Menu_GetMenuXSize(hObj);
	//yItemSize = TP_Menu_GetMenuItemYSize(hObj);
	ySize = TP_Menu_GetMenuYSize(hObj);

	if ((xSize != oldXSize) || (ySize != oldYSize))
	{
		WM_ResizeWindow(hObj, xSize - oldXSize, ySize - oldYSize);
	}

	if (obj.ItemNum == 0)
	{
		WM_HideWindow(hObj);
		WM_InvalidateWindow(hObj);
	}
}


void TP_Menu_DisableItem  (TP_MENU_Handle hObj, U16 itemId)
{
	TP_MENU_OBJ obj;

	WM_GetUserData(hObj, &obj, sizeof(TP_MENU_OBJ));
	TP_Menu_DisableItemById(&(obj.itemHead), itemId);
	WM_SetUserData(hObj, &obj, sizeof(TP_MENU_OBJ));
}

void     TP_Menu_EnableItem   (TP_MENU_Handle hObj, U16 itemId)
{
	TP_MENU_OBJ obj;
	
	WM_GetUserData(hObj, &obj, sizeof(TP_MENU_OBJ));
	TP_Menu_EnableItemById(&(obj.itemHead), itemId);
	WM_SetUserData(hObj, &obj, sizeof(TP_MENU_OBJ));

}

/******************************************************************************
* FUNCTION      : TP_Menu_Popup()
* AUTHOR        : Huangwenzhong <Huangwenzhong@tp-link.net>
* DESCRIPTION   : 在指定位置打开菜单 
* INPUT         : xSize 和ySize暂时不起作用
*
* OUTPUT        : N/A
* RETURN        : N/A
* OTHERS        :
******************************************************************************/
void TP_Menu_Popup (TP_MENU_Handle hObj, WM_HWIN hDestWin, int x, int y, int xSize, int ySize)
{
	WM_AttachWindowAt(hObj, hDestWin, x, y);
	WM_ShowWindow(hObj);
}

void TP_Menu_Close (TP_MENU_Handle hObj)
{
	WM_HideWindow(hObj);
}


/******************************************************************************
* FUNCTION      : TP_Menu_Attach()
* AUTHOR        : Huangwenzhong <Huangwenzhong@tp-link.net>
* DESCRIPTION   : 将菜单附加到新的窗口中 
* INPUT         :xSize 和ySize暂时不起作用
*
* OUTPUT        : N/A
* RETURN        : N/A
* OTHERS        :
******************************************************************************/
void TP_Menu_Attach (TP_MENU_Handle hObj, WM_HWIN hDestWin, int x, int y, int xSize, int ySize)
{
	WM_AttachWindowAt(hObj, hDestWin, x, y);
}

unsigned  TP_Menu_GetNumItems(TP_MENU_Handle hObj)
{
	TP_MENU_OBJ obj;
	
	WM_GetUserData(hObj, &obj, sizeof(TP_MENU_OBJ));

	return obj.ItemNum;
}


/******************************************************************************
* FUNCTION      : TP_Menu_GetItem()
* AUTHOR        : Huangwenzhong <Huangwenzhong@tp-link.net>
* DESCRIPTION   : 返回指定ID的item数据 
* INPUT         : N/A
*
* OUTPUT        : N/A
* RETURN        : N/A
* OTHERS        :这个返回的是引用数据，而非拷贝
******************************************************************************/
void  TP_Menu_GetItem (TP_MENU_Handle hObj, U16 itemId, TP_MENU_ITEM* pItemData)
{
	TP_MENU_OBJ obj;
	if (pItemData == NULL)
	{
		return;
	}
	
	WM_GetUserData(hObj, &obj, sizeof(TP_MENU_OBJ));
	TP_Menu_GetItemById(&(obj.itemHead), itemId, pItemData);
}

WM_HWIN   TP_Menu_GetOwner     (TP_MENU_Handle hObj)
{
	return WM_GetParent(hObj);
}

int      TP_Menu_GetUserData  (TP_MENU_Handle hObj, void * pDest, int SizeOfBuffer)
{
	TP_MENU_OBJ  obj;
	int 		 NumBytes;
	U8		   * pExtraBytes = NULL;

	if ((pDest == NULL) || (SizeOfBuffer <= 0))
	{
		return 0;
	}
	
	WM_GetUserData(hObj, &obj, sizeof(TP_MENU_OBJ));
	pExtraBytes = (U8 *)malloc(sizeof(TP_MENU_OBJ) + obj.NumExtraBytes);
	if (pExtraBytes) 
	{
		WM_GetUserData(hObj, pExtraBytes, sizeof(TP_MENU_OBJ) + obj.NumExtraBytes);
		
		if (SizeOfBuffer >= obj.NumExtraBytes)
		{
			NumBytes = obj.NumExtraBytes;
		}
		else
		{
			NumBytes = SizeOfBuffer;
		}
		
		GUI_MEMCPY(pDest, pExtraBytes + sizeof(TP_MENU_OBJ), NumBytes);
		free(pExtraBytes);
		return NumBytes;
	}
	return 0;
}


int TP_Menu_SetUserData  (TP_MENU_Handle hObj, const void * pSrc, int SizeOfBuffer)
{
	TP_MENU_OBJ  obj;
	int 		 NumBytes;
	U8		   * pExtraBytes;

	if ((pSrc == NULL) || (SizeOfBuffer <= 0))
	{
		return RET_FAIL;
	}

	WM_GetUserData(hObj, &obj, sizeof(TP_MENU_OBJ));
	pExtraBytes = (U8 *)malloc(sizeof(TP_MENU_OBJ) + obj.NumExtraBytes);
	if (pExtraBytes)
	{
		WM_GetUserData(hObj, pExtraBytes, sizeof(TP_MENU_OBJ) + obj.NumExtraBytes);

		if (SizeOfBuffer >= obj.NumExtraBytes)
		{
		  NumBytes = obj.NumExtraBytes;
		}
		else
		{
		  NumBytes = SizeOfBuffer;
		}
		memset(pExtraBytes + sizeof(TP_MENU_OBJ), 0, obj.NumExtraBytes);
		GUI_MEMCPY(pExtraBytes + sizeof(TP_MENU_OBJ), pSrc, NumBytes);
		WM_SetUserData(hObj, pExtraBytes, sizeof(TP_MENU_OBJ) + obj.NumExtraBytes);
		free(pExtraBytes);
		return RET_SUCCESS;
	}

	return RET_FAIL;
}

GUI_COLOR TP_Menu_GetBkColor (TP_MENU_Handle hObj, INT32 colorIdx)
{
	TP_MENU_OBJ obj;

	if ((colorIdx < TP_MENU_ITEM_BK_CI_MIN) || (colorIdx >= TP_MENU_ITEM_BK_CI_MAX))
	{
		return GUI_INVALID_COLOR;
	}
	WM_GetUserData(hObj, &obj, sizeof(TP_MENU_OBJ));
	return obj.props.itemBkColor[colorIdx];
}

void TP_Menu_SetBkColor   (TP_MENU_Handle hObj, INT32 colorIdx, GUI_COLOR color)
{
	TP_MENU_OBJ obj;

	if ((colorIdx < TP_MENU_ITEM_BK_CI_MIN) || (colorIdx >= TP_MENU_ITEM_BK_CI_MAX))
	{
		return;
	}
	WM_GetUserData(hObj, &obj, sizeof(TP_MENU_OBJ));
	obj.props.itemBkColor[colorIdx] = color;
	WM_SetUserData(hObj, &obj, sizeof(TP_MENU_OBJ));
}

U8 TP_Menu_GetBorderSize(TP_MENU_Handle hObj, INT32 idx)
{
	TP_MENU_OBJ obj;

	if ((idx >= TP_MENU_BORDER_INDEX_MIN) && (idx < TP_MENU_BORDER_INDEX_MAX))
	{
		WM_GetUserData(hObj, &obj, sizeof(TP_MENU_OBJ));
		return obj.props.borderWidth[idx];
	}

	return 0;
}


void TP_Menu_SetBorderSize(TP_MENU_Handle hObj, INT32 idx, U8 BorderSize)
{
	TP_MENU_OBJ obj;

	if ((idx >= TP_MENU_BORDER_INDEX_MIN) && (idx < TP_MENU_BORDER_INDEX_MAX))
	{
		WM_GetUserData(hObj, &obj, sizeof(TP_MENU_OBJ));
		obj.props.borderWidth[idx] = BorderSize;
		WM_SetUserData(hObj, &obj, sizeof(TP_MENU_OBJ));
	}
	
}

const GUI_FONT*  TP_Menu_GetFont(TP_MENU_Handle hObj, const GUI_FONT * pFont)
{
	TP_MENU_OBJ obj;
	
	WM_GetUserData(hObj, &obj, sizeof(TP_MENU_OBJ));
	return obj.props.pFont;
}

void      TP_Menu_SetFont      (TP_MENU_Handle hObj, const GUI_FONT * pFont)
{
	TP_MENU_OBJ obj;
	
	WM_GetUserData(hObj, &obj, sizeof(TP_MENU_OBJ));
	obj.props.pFont = pFont;
	WM_SetUserData(hObj, &obj, sizeof(TP_MENU_OBJ));
}

GUI_COLOR TP_Menu_GetTextColor (TP_MENU_Handle hObj, INT32 idx)
{
	TP_MENU_OBJ obj;

	if (idx < TP_MENU_ITEM_TEXT_CI_MIN || idx >= TP_MENU_ITEM_TEXT_CI_MAX)
	{
		return GUI_INVALID_COLOR;
	}
	
	WM_GetUserData(hObj, &obj, sizeof(TP_MENU_OBJ));
	return obj.props.textColor[idx];
}


void TP_Menu_SetTextColor (TP_MENU_Handle hObj, INT32 idx, GUI_COLOR color)
{
	TP_MENU_OBJ obj;

	if (idx < TP_MENU_ITEM_TEXT_CI_MIN || idx >= TP_MENU_ITEM_TEXT_CI_MAX)
	{
		return;
	}
	
	WM_GetUserData(hObj, &obj, sizeof(TP_MENU_OBJ));
	obj.props.textColor[idx] = color;
	WM_SetUserData(hObj, &obj, sizeof(TP_MENU_OBJ));
}

GUI_COLOR TP_Menu_GetDefaultTextColor (INT32 idx)
{
	if (idx < TP_MENU_ITEM_TEXT_CI_MIN || idx >= TP_MENU_ITEM_TEXT_CI_MAX)
	{
		return GUI_INVALID_COLOR;
	}
	return TP_Menu_Default.props.textColor[idx];
}

void TP_Menu_SetDefaultTextColor  (INT32 idx, GUI_COLOR color)
{
	if (idx < TP_MENU_ITEM_TEXT_CI_MIN || idx >= TP_MENU_ITEM_TEXT_CI_MAX)
	{
		return;
	}
	TP_Menu_Default.props.textColor[idx] = color;
}

GUI_COLOR TP_Menu_GetDefaultBkColor (INT32 idx)
{
	if (idx < TP_MENU_ITEM_BK_CI_MIN || idx >= TP_MENU_ITEM_BK_CI_MAX)
	{
		return GUI_INVALID_COLOR;
	}
	return TP_Menu_Default.props.itemBkColor[idx];
}

void TP_Menu_SetDefaultBkColor(INT32 idx, GUI_COLOR color)
{
	if (idx < TP_MENU_ITEM_BK_CI_MIN || idx >= TP_MENU_ITEM_BK_CI_MAX)
	{
		return;
	}
	TP_Menu_Default.props.itemBkColor[idx] = color;
}

const GUI_FONT * TP_Menu_GetDefaultFont(void)
{
	return TP_Menu_Default.props.pFont;
}

void TP_Menu_SetDefaultFont(const GUI_FONT * pFont)
{
	TP_Menu_Default.props.pFont = pFont;
}

U8 TP_Menu_GetDefaultBorderSize (INT32 idx)
{
	if ((idx >= TP_MENU_BORDER_INDEX_MIN) && (idx < TP_MENU_BORDER_INDEX_MAX))
	{
		return TP_Menu_Default.props.borderWidth[idx];
	}

	return 0;
}

void TP_Menu_SetDefaultBorderSize (INT32 idx, U8 BorderSize)
{
	if ((idx >= TP_MENU_BORDER_INDEX_MIN) && (idx < TP_MENU_BORDER_INDEX_MAX))
	{
		TP_Menu_Default.props.borderWidth[idx] = BorderSize;
	}
}

int TP_Menu_GetId(TP_MENU_Handle hObj)
{
	TP_MENU_OBJ obj;
	WM_GetUserData(hObj, &obj, sizeof(TP_MENU_OBJ));
	return obj.id;
}


void TP_Menu_Callback(WM_MESSAGE * pMsg)
{
	TP_MENU_Handle	hWin;
	GUI_PID_STATE	*pState = NULL;
	TP_MENU_OBJ obj;
	GUI_RECT	WinRect, itemRect, tmpRect;
	WM_MESSAGE msg;
	TP_MENU_MSG menuMsg;

	WM_PID_STATE_CHANGED_INFO *pPidState = NULL;	

	GUI_COLOR itemBkColor, textColor;
	int itemNum = 0;
	const GUI_FONT *pFont = NULL;
	int x, y;
	int itemId;
	
	int i = 1;
	TP_MENU_ITEM_LIST *pList = NULL;
	int itemIdx = TP_MENU_ITEM_BK_CI_ENABLED_NORMAL;	/* Menu Item菜单项背景颜色 */
	int textIdx = TP_MENU_ITEM_TEXT_CI_ENABLED_NORMAL;	/* 菜单项文本颜色 */

	int yItemSize = 0;

	hWin = pMsg->hWin;
	WM_GetWindowRectEx(hWin, &WinRect);
	GUI_MoveRect(&WinRect, -WinRect.x0, -WinRect.y0);
	WM_GetUserData(hWin, &obj, sizeof(TP_MENU_OBJ));
	itemNum = obj.ItemNum;
	pFont = obj.props.pFont;
		
	switch (pMsg->MsgId) {
	case WM_PAINT:

		if ((itemNum == 0) || (obj.itemHead.next == NULL))
		{
			return;
		}

		GUI_SetFont(pFont);

		GUI_SetColor(obj.props.bkColor);/* 显示线条颜色 */
		GUI_FillRectEx(&WinRect);

		yItemSize = TP_Menu_GetMenuItemYSize(hWin);
		
		textColor = obj.props.textColor[textIdx];
			
		itemRect = WinRect;
		itemRect.x0 += TP_MENU_SEPARATOR_LINE_HEIGHT;
		itemRect.x1 -= TP_MENU_SEPARATOR_LINE_HEIGHT;

		pList = obj.itemHead.next;
		while (pList != NULL)
		{
			itemRect.y0 = WinRect.y0 + i * TP_MENU_SEPARATOR_LINE_HEIGHT + (i - 1) * yItemSize;
			itemRect.y1 = itemRect.y0 + yItemSize - 1;

			if ((obj.pressedId > 0) && (pList->item.id == obj.pressedId))
			{
				itemIdx = TP_MENU_ITEM_BK_CI_ENABLED_SEL;
			}
			else
			{
				itemIdx = TP_MENU_ITEM_BK_CI_ENABLED_NORMAL;
			}
			itemBkColor = obj.props.itemBkColor[itemIdx];
			GUI_SetColor(itemBkColor);
			GUI_FillRectEx(&itemRect);/* 填充方框的颜色 */
			pList->rect = itemRect;		/* 记录对应菜单项的矩形区域 */

			if (pList->item.pText)/* 这个条件必须要成立，否则是有问题的 */
			{	
				tmpRect = itemRect;
				tmpRect.x0 += obj.props.borderWidth[TP_MENU_BORDER_INDEX_LEFT];
				tmpRect.x1 -= obj.props.borderWidth[TP_MENU_BORDER_INDEX_RIGHT];
				GUI_SetColor(textColor);
				GUI_SetTextMode(GUI_TM_TRANS);
				GUI_DispStringInRect(pList->item.pText, &tmpRect, TP_MENU_TEXT_DEFAULT_ALIGN);
			}

			pList = pList->next;
			i++;
		}

		break;

	case WM_TOUCH:

		pState = (GUI_PID_STATE *)pMsg->Data.p;
		if (pState == NULL)
		{
			return;
		}
		x = pState->x;
		y = pState->y;
		
		if (pState->Pressed)
		{
			itemId = TP_Menu_GetItemIdOfPid(&(obj.itemHead),  x, y);
			if ((obj.pressedId > 0) && (itemId > 0) && (itemId != obj.pressedId))
			{
				obj.pressedId = 0;
				WM_SetUserData(hWin, &obj, sizeof(TP_MENU_OBJ));
				WM_InvalidateWindow(hWin);
			}			
		}

		break;
#if GUI_SUPPORT_MOUSE
	case WM_MOUSEOVER_END:/* 鼠标移出菜单则要处理 */
		if (obj.pressedId > 0)
		{
			obj.pressedId = 0;
			WM_SetUserData(hWin, &obj, sizeof(TP_MENU_OBJ));
			WM_InvalidateWindow(hWin);
		}
		break;
#endif

	case WM_PID_STATE_CHANGED:
		pPidState = (WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p);
		if (pPidState == NULL)
		{
			return;
		}
		x = pPidState->x;
		y = pPidState->y;
		if (WM_IsEnabled(hWin))
		{
			itemId = TP_Menu_GetItemIdOfPid(&(obj.itemHead),  x, y);

			if (itemId > 0)
			{
				if ((pPidState->StatePrev == WM_PID_STATE_UNPRESSED)
					&& (pPidState->State == WM_PID_STATE_PRESSED))
				{
					msg.MsgId = WM_TP_MENU;
					msg.hWin = WM_GetParent(hWin);
					msg.hWinSrc = hWin;
					msg.Data.p = &menuMsg;
					menuMsg.ItemId = itemId;
					menuMsg.MsgType = TP_MENU_ON_ITEMPRESSED;
					/* 发消息告知父窗口某个菜单项被按下 */
					WM_SendToParent(hWin, &msg);
					
					obj.pressedId = itemId;
					WM_SetFocus(hWin);
					WM_SetUserData(hWin, &obj, sizeof(TP_MENU_OBJ));
				}
				else if ((pPidState->StatePrev == WM_PID_STATE_PRESSED)
					&& (pPidState->State == WM_PID_STATE_UNPRESSED))
				{
					if (itemId == obj.pressedId)/* 点击了某个菜单项 */
					{
						msg.MsgId = WM_TP_MENU;
						msg.hWin = WM_GetParent(hWin);
						msg.hWinSrc = hWin;
						msg.Data.p = &menuMsg;
						menuMsg.ItemId = itemId;
						menuMsg.MsgType = TP_MENU_ON_ITEMSELECT;
						/* 发消息告知父窗口某个菜单项已被选择 */
						WM_SendToParent(hWin, &msg);

						obj.pressedId = 0;
						WM_SetUserData(hWin, &obj, sizeof(TP_MENU_OBJ));
						WM_HideWindow(hWin);
					}
				}
				WM_InvalidateWindow(hWin);
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
	case WM_DELETE:
		TP_Menu_DeleteAllItem(&(obj.itemHead));
		WM_SetUserData(hWin, &obj, sizeof(TP_MENU_OBJ));
		break;
	case WM_GET_ID:
		pMsg->Data.v = obj.id;
		break;
	case WM_SET_ID:
		obj.id = pMsg->Data.v;
		WM_SetUserData(hWin, &obj, sizeof(TP_MENU_OBJ));
		break;
	default:
		WM_DefaultProc(pMsg);
		break;
	}
}

TP_MENU_Handle TP_Menu_Create(int x0, int y0, int xSize, int ySize, 
	WM_HWIN hParent, U32 winFlags, int id, WM_CALLBACK *pfCallback, int NumExtraBytes)
{
	TP_MENU_Handle	handle;
	TP_MENU_OBJ 	obj;
	WM_CALLBACK 		*pCb = NULL;

	if (pfCallback)
	{
		pCb = pfCallback;
	}
	else
	{
		pCb = TP_Menu_Callback;
	}

	obj = TP_Menu_Default;
	obj.id = id;
	obj.NumExtraBytes = NumExtraBytes;

	handle = WM_CreateWindowAsChild(x0, y0, xSize, ySize, hParent, 
				winFlags, pCb, sizeof(TP_MENU_OBJ) + NumExtraBytes);
	WM_SetUserData(handle, &obj, sizeof(TP_MENU_OBJ));
	return handle;

}

