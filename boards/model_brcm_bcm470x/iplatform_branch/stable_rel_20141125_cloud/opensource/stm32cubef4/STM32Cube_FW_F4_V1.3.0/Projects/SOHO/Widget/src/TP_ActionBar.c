#include <stdlib.h>

#include "TP_Widget.h"

#include "TP_Bmp.h"
#include "TP_Button.h"
#include "TP_Menu.h"
#include "TP_Text.h"
#include "TP_ActionBar.h"

#define TP_AB_DEFAULT_HEIGHT			UI_ACTION_BAR_HEIGHT
#define TP_AB_DEFAULT_NORMAL_COLOR		(0xbb9b07)
#define TP_AB_DEFAULT_CLICK_COLOR		(0xa09b07)
#define TP_AB_DEFAULT_LEFT_PAD			(16)
#define TP_AB_DEFAULT_TOP_PAD			(2)
#define TP_AB_DEFAULT_BOTTOM_PAD		(2)
#define TP_AB_DEFAULT_RIGHT_PAD			(16)

#define TP_AB_DEFAULT_ICON_UP_LEN		(20)
#define TP_AB_DEFAULT_ICON_HOME_LEN		(20)
#define TP_AB_DEFAULT_ICON_TITLE_LEN	(40)

#define TP_AB_DEFAULT_TEXT_COLOR		(0xfffffe)


#define TP_AB_SPACE_ICON	(5)
#define TP_AB_SPACE_BUTTON	(5)

#define TP_AB_DEFAULT_TITLE_FONT		&GUI_Font16_ASCII
#define TP_AB_DEFAULT_SUB_TITLE_FONT	&GUI_Font13_ASCII
#define TP_AB_DEFAULT_MENU_TEXT_FONT	&GUI_Font13_ASCII

#define TP_AB_DEFAULT_MENU_BUTTON_ID	(10000000)
#define TP_AB_DEFAULT_MENU_ID			(10000001)

TP_ACTIONBAR_OBJ TP_ActionBar_Default = 
{
	0,									/* id */
	TP_AB_DEFAULT_TEXT_COLOR,			/* text color */
	0,									/* height */
	{									/* background color */
		TP_AB_DEFAULT_NORMAL_COLOR, 
		TP_AB_DEFAULT_CLICK_COLOR
	},
	NULL,								/* background image */
	{									/* pad */
		TP_AB_DEFAULT_LEFT_PAD,
		TP_AB_DEFAULT_RIGHT_PAD,
		TP_AB_DEFAULT_TOP_PAD,
		TP_AB_DEFAULT_BOTTOM_PAD
	},
	0,										/* icon flag */
	{TP_AB_SPACE_ICON, TP_AB_SPACE_BUTTON},	/* default space */

	{										/* button list */
		0,
		//0,
		{0, 0, 0, 0},
		NULL,
		{
			0,								/* menu button id */
			0,								/* menu id */
			{0, 0, 0, 0},
			{NULL, NULL},
			TP_AB_DEFAULT_MENU_TEXT_FONT,	/* menu text font */
		},		
	},
	0,										/* extra memory */
	{
		{									/* up icon data */
			0,
			TP_AB_DEFAULT_ICON_UP_LEN,
			0,
			{NULL, NULL},
			NULL,
			NULL,
		},
		{									/* home icon data */
			0,
			TP_AB_DEFAULT_ICON_HOME_LEN,
			0,
			{NULL, NULL},
			NULL,
			NULL,
		},
		{									/* title */
			0,
			TP_AB_DEFAULT_ICON_TITLE_LEN,
			0,
			{NULL, NULL},
			NULL,
			TP_AB_DEFAULT_TITLE_FONT,
		},
		{									/* sub title */
			0,
			TP_AB_DEFAULT_ICON_TITLE_LEN,
			0,
			{NULL, NULL},
			NULL,
			TP_AB_DEFAULT_SUB_TITLE_FONT,
		}
	},
	TP_AB_TITLE_CENTER_ALIGN,
};

GUI_COLOR TP_ActionBar_GetDefaultTitleColor()
{
	return TP_ActionBar_Default.textColor;
}

void TP_ActionBar_SetDefaultTitleColor(GUI_COLOR color)
{
	TP_ActionBar_Default.textColor = color;
}


int TP_ActionBar_GetDefaultHeight()
{
	return TP_ActionBar_Default.height;
}

void TP_ActionBar_SetDefaultHeight(int height)
{
	TP_ActionBar_Default.height = height;
}

GUI_COLOR TP_ActionBar_GetDefaultBkColor(int index)
{
	GUI_COLOR color = GUI_INVALID_COLOR;
	if ((index >= TP_AB_BK_COLOR_INDEX_MIN) && (index < TP_AB_BK_COLOR_INDEX_MAX))
	{
		color = TP_ActionBar_Default.bkColor[index];
	}

	return color;
}

void TP_ActionBar_SetDefaultBkColor(int index, GUI_COLOR color)
{
	if ((index >= TP_AB_BK_COLOR_INDEX_MIN) && (index < TP_AB_BK_COLOR_INDEX_MAX))
	{
		TP_ActionBar_Default.bkColor[index] = color;
	}
}
const GUI_BITMAP* TP_ActionBar_GetDefaultBkBitmap()
{
	return TP_ActionBar_Default.bkBitmap;
}
void TP_ActionBar_SetDefaultBkBitmap(const GUI_BITMAP* pBitmap)
{
	if (pBitmap)
	{
		TP_ActionBar_Default.bkBitmap = pBitmap;
	}
}

int TP_ActionBar_GetDefaultPad(int index)
{
	int pad = 0;
	if ((index >= TP_AB_PAD_INDEX_MIN) && (index < TP_AB_PAD_INDEX_MAX))
	{
		pad = TP_ActionBar_Default.padding[index];
	}

	return pad;
}

void TP_ActionBar_SetDefaultPad(int index, int pad)
{
	if ((index >= TP_AB_PAD_INDEX_MIN) && (index < TP_AB_PAD_INDEX_MAX))
	{
		TP_ActionBar_Default.padding[index] = pad;
	}
}

int TP_ActionBar_GetDefaultSpace(int index)
{
	int space = 0;

	if ((index >= TP_AB_SPACE_INDEX_MIN) && (index < TP_AB_SPACE_INDEX_MAX))
	{
		space = TP_ActionBar_Default.space[index];
	}

	return space;
}
void TP_ActionBar_SetDefaultSpace(int index, int space)
{
	if ((index >= TP_AB_SPACE_INDEX_MIN) && (index < TP_AB_SPACE_INDEX_MAX))
	{
		TP_ActionBar_Default.space[index] = space;
	}
}

void TP_ActionBar_SetDefaultMenuBitmap(int index, const GUI_BITMAP *pBitmap)
{
	if ((index >= TP_AB_BM_INDEX_MIN) && (index < TP_AB_BM_INDEX_MAX))
	{
		TP_ActionBar_Default.buttonList.menu.pBitMap[index] = pBitmap;
	}
}

const GUI_BITMAP * TP_ActionBar_GetDefaultMenuBitmap(int index)
{
	const GUI_BITMAP *pBitmap = NULL;
	if ((index >= TP_AB_BM_INDEX_MIN) && (index < TP_AB_BM_INDEX_MAX))
	{
		pBitmap = TP_ActionBar_Default.buttonList.menu.pBitMap[index];
	}

	return pBitmap;
}

GUI_COLOR TP_ActionBar_GetTitleColor(TP_ACTIONBAR_HANDLE hObj)
{
	TP_ACTIONBAR_OBJ obj;
	WM_GetUserData(hObj, &obj, sizeof(TP_ACTIONBAR_OBJ));

	return obj.textColor;
}

void TP_ActionBar_SetTitleColor(TP_ACTIONBAR_HANDLE hObj, GUI_COLOR color)
{
	TP_ACTIONBAR_OBJ obj;
	WM_GetUserData(hObj, &obj, sizeof(TP_ACTIONBAR_OBJ));

	obj.textColor = color;

	WM_SetUserData(hObj, &obj, sizeof(TP_ACTIONBAR_OBJ));
}

int TP_ActionBar_GetHeight(TP_ACTIONBAR_HANDLE hObj)
{
	TP_ACTIONBAR_OBJ obj;
	WM_GetUserData(hObj, &obj, sizeof(TP_ACTIONBAR_OBJ));

	return obj.height;
}

void TP_ActionBar_SetHeight(TP_ACTIONBAR_HANDLE hObj, int height)
{
	TP_ACTIONBAR_OBJ obj;

	WM_GetUserData(hObj, &obj, sizeof(TP_ACTIONBAR_OBJ));

	obj.height = height;

	WM_SetUserData(hObj, &obj, sizeof(TP_ACTIONBAR_OBJ));
}

GUI_COLOR TP_ActionBar_GetBkColor(TP_ACTIONBAR_HANDLE hObj, int index)
{
	TP_ACTIONBAR_OBJ obj;
	GUI_COLOR color = GUI_INVALID_COLOR;
	WM_GetUserData(hObj, &obj, sizeof(TP_ACTIONBAR_OBJ));

	if ((index >= TP_AB_BK_COLOR_INDEX_MIN) && (index < TP_AB_BK_COLOR_INDEX_MAX))
	{
		color = obj.bkColor[index];
	}
	return color;
}

void TP_ActionBar_SetBkColor(TP_ACTIONBAR_HANDLE hObj, int index, GUI_COLOR color)
{
	TP_ACTIONBAR_OBJ obj;

	WM_GetUserData(hObj, &obj, sizeof(TP_ACTIONBAR_OBJ));

	if ((index >= TP_AB_BK_COLOR_INDEX_MIN) && (index < TP_AB_BK_COLOR_INDEX_MAX))
	{
		obj.bkColor[index] = color;
		WM_SetUserData(hObj, &obj, sizeof(TP_ACTIONBAR_OBJ));
	}
}

const GUI_BITMAP* TP_ActionBar_GetBkBitmap(TP_ACTIONBAR_HANDLE hObj)
{
	TP_ACTIONBAR_OBJ obj;

	WM_GetUserData(hObj, &obj, sizeof(TP_ACTIONBAR_OBJ));

	return obj.bkBitmap;
}

void TP_ActionBar_SetBkBitmap(TP_ACTIONBAR_HANDLE hObj, const GUI_BITMAP* pBitmap)
{
	TP_ACTIONBAR_OBJ obj;

	if (pBitmap == NULL)
	{
		return;
	}
	
	WM_GetUserData(hObj, &obj, sizeof(TP_ACTIONBAR_OBJ));

	obj.bkBitmap = pBitmap;

	WM_SetUserData(hObj, &obj, sizeof(TP_ACTIONBAR_OBJ));
}

int TP_ActionBar_GetPad(TP_ACTIONBAR_HANDLE hObj, int index)
{
	TP_ACTIONBAR_OBJ obj;
	int padding =  0;

	WM_GetUserData(hObj, &obj, sizeof(TP_ACTIONBAR_OBJ));

	if ((index >= TP_AB_PAD_INDEX_MIN) && (index < TP_AB_PAD_INDEX_MAX))
	{
		padding = obj.padding[index];
	}
	
	return padding;
}
void TP_ActionBar_SetPad(TP_ACTIONBAR_HANDLE hObj, int index, int pad)
{
	TP_ACTIONBAR_OBJ obj;

	WM_GetUserData(hObj, &obj, sizeof(TP_ACTIONBAR_OBJ));

	if ((index >= TP_AB_PAD_INDEX_MIN) && (index < TP_AB_PAD_INDEX_MAX))
	{
		obj.padding[index] = pad;
		WM_SetUserData(hObj, &obj, sizeof(TP_ACTIONBAR_OBJ));
	}
}

int TP_ActionBar_GetSpace(TP_ACTIONBAR_HANDLE hObj, int index)
{
	TP_ACTIONBAR_OBJ obj;
	int space = 0;

	WM_GetUserData(hObj, &obj, sizeof(TP_ACTIONBAR_OBJ));
	if ((index >= TP_AB_SPACE_INDEX_MIN) && (index < TP_AB_SPACE_INDEX_MAX))
	{
		space =  obj.space[index];
	}

	return space;
}

void TP_ActionBar_SetSpace(TP_ACTIONBAR_HANDLE hObj, int index, int space)
{
	TP_ACTIONBAR_OBJ obj;

	WM_GetUserData(hObj, &obj, sizeof(TP_ACTIONBAR_OBJ));

	if ((index >= TP_AB_SPACE_INDEX_MIN) && (index < TP_AB_SPACE_INDEX_MAX))
	{
		obj.space[index] = space;
		WM_SetUserData(hObj, &obj, sizeof(TP_ACTIONBAR_OBJ));
	}
}

void TP_ActionBar_SetMenuBitmap(TP_ACTIONBAR_HANDLE hObj, int index, const GUI_BITMAP *pBitmap)
{
	TP_ACTIONBAR_OBJ obj;

	if (pBitmap == NULL)
	{
		return;
	}

	WM_GetUserData(hObj, &obj, sizeof(TP_ACTIONBAR_OBJ));

	if ((index >= TP_AB_BM_INDEX_MIN) && (index < TP_AB_BM_INDEX_MAX))
	{
		obj.buttonList.menu.pBitMap[index] = pBitmap;
		WM_SetUserData(hObj, &obj, sizeof(TP_ACTIONBAR_OBJ));
	}
}

const GUI_BITMAP * TP_ActionBar_GetMenuBitmap(TP_ACTIONBAR_HANDLE hObj, int index)
{
	TP_ACTIONBAR_OBJ obj;
	const GUI_BITMAP *pBitmap = NULL;

	WM_GetUserData(hObj, &obj, sizeof(TP_ACTIONBAR_OBJ));
	if ((index >= TP_AB_BM_INDEX_MIN) && (index < TP_AB_BM_INDEX_MAX))
	{
		pBitmap = obj.buttonList.menu.pBitMap[index];
	}

	return pBitmap;
}

/*�������UP��ť���򷵻�UP��ť�ľ�������*/
/*��ͼƬ�Ͱ���ͼƬ��С����Ӧ*/
static GUI_RECT TP_ActionBar_GetUpRect(TP_ACTIONBAR_HANDLE	hObj)
{
	GUI_RECT winRect, rect;
	TP_ACTIONBAR_OBJ obj;
	int xSize, ySize, yPad;
	const GUI_BITMAP *pBitMap = NULL;

	rect.x0 = rect.x1 = 0; 
	rect.y0 = rect.y1 = 0;

	WM_GetWindowRectEx(hObj, &winRect);
	GUI_MoveRect(&winRect, -winRect.x0, -winRect.y0);
	WM_GetUserData(hObj, &obj, sizeof(TP_ACTIONBAR_OBJ));

	if (!(TP_AB_ICON_FLAG_UP & obj.iconFlag))
	{
		return rect;
	}

	rect.x0 = winRect.x0 + obj.padding[TP_AB_PAD_INDEX_LEFT];
	pBitMap = obj.appIcon[TP_AB_ICON_INDEX_UP].bitmap[TP_AB_BM_INDEX_NORMAL];
	if (pBitMap)
	{
		xSize = pBitMap->XSize;
		ySize = pBitMap->YSize;

		/*����Ҫ��֤AB�ĸ߶�Ҫ��ͼƬ�ĸ߶ȴ�*/
		yPad = (winRect.y1 - winRect.y0 + 1 - ySize) / 2;

		rect.y0 = winRect.y0 + yPad;
		rect.x1 = rect.x0 + xSize - 1;
		rect.y1 = rect.y0 + ySize - 1;
	}
	else
	{
		rect.y0 = winRect.y0 + obj.padding[TP_AB_PAD_INDEX_TOP];
		rect.x1 = winRect.x0 + obj.appIcon[TP_AB_ICON_INDEX_UP].len - 1; 
		rect.y1 = winRect.y1 - obj.padding[TP_AB_PAD_INDEX_BOTTOM] + 1;
	}

	return rect;
}

/*�������Homeͼ�꣬�򷵻����������*/
static GUI_RECT TP_ActionBar_GetHomeRect(TP_ACTIONBAR_HANDLE	hObj)
{
	GUI_RECT winRect, rect, upRect;
	TP_ACTIONBAR_OBJ obj;
	int xSize, ySize, yPad;
	const GUI_BITMAP *pBitMap = NULL;

	rect.x0 = rect.x1 = 0; 
	rect.y0 = rect.y1 = 0;

	WM_GetWindowRectEx(hObj, &winRect);
	GUI_MoveRect(&winRect, -winRect.x0, -winRect.y0);
	WM_GetUserData(hObj, &obj, sizeof(TP_ACTIONBAR_OBJ));

	if (!(TP_AB_ICON_FLAG_HOME & obj.iconFlag))
	{
		return rect;
	}

	if (TP_AB_ICON_FLAG_UP & obj.iconFlag)
	{
		upRect = TP_ActionBar_GetUpRect(hObj);
		rect.x0 = upRect.x1 + obj.space[TP_AB_SPACE_INDEX_ICON] + 1;
	}
	else
	{
		rect.x0 = winRect.x0 + obj.padding[TP_AB_PAD_INDEX_LEFT];
	}

	pBitMap = obj.appIcon[TP_AB_ICON_INDEX_HOME].bitmap[TP_AB_BM_INDEX_NORMAL];
	if (pBitMap)
	{
		xSize = pBitMap->XSize;
		ySize = pBitMap->YSize;

		/*����Ҫ��֤AB�ĸ߶�Ҫ��ͼƬ�ĸ߶ȴ�*/
		yPad = (winRect.y1 - winRect.y0 + 1 - ySize) / 2;

		rect.y0 = winRect.y0 + yPad;
		rect.x1 = rect.x0 + xSize - 1;
		rect.y1 = rect.y0 + ySize - 1;
	}
	else
	{
		rect.y0 = winRect.y0 + obj.padding[TP_AB_PAD_INDEX_TOP];
		rect.x1 = winRect.x0 + obj.appIcon[TP_AB_ICON_INDEX_HOME].len - 1; 
		rect.y1 = winRect.y1 - obj.padding[TP_AB_PAD_INDEX_BOTTOM] + 1;
	}

	return rect;
}

static GUI_RECT TP_ActionBar_GetSubTitleRect(TP_ACTIONBAR_HANDLE	hObj)
{
	GUI_RECT winRect, rect, upRect, homeRect;
	TP_ACTIONBAR_OBJ obj;
	int ySize = 0, yPad = 0;
	int titleLen = 0;
	int subTitleLen = 0;
	int winXsize = 0;

	rect.x0 = rect.x1 = 0; 
	rect.y0 = rect.y1 = 0;

	WM_GetWindowRectEx(hObj, &winRect);
	GUI_MoveRect(&winRect, -winRect.x0, -winRect.y0);
	WM_GetUserData(hObj, &obj, sizeof(TP_ACTIONBAR_OBJ));
	winXsize = winRect.x1 - winRect.x0 + 1;

	if (!(TP_AB_ICON_FLAG_TITLE & obj.iconFlag))
	{
		return rect;
	}

	/*�������Ҫ��ʵ��title�ı��ĳ���Ϊ׼*/
	/*Ҳ����˵�û�Ҫ����API������ȷ��iconLen*/

	if ((obj.appIcon[TP_AB_ICON_INDEX_TITLE].pFont) 
		&& (obj.appIcon[TP_AB_ICON_INDEX_TITLE].title))
	{
		GUI_SetFont(obj.appIcon[TP_AB_ICON_INDEX_TITLE].pFont);
		titleLen = GUI_GetStringDistX((const char*)obj.appIcon[TP_AB_ICON_INDEX_TITLE].title);
	}

	if ((obj.appIcon[TP_AB_ICON_INDEX_SUB_TITLE].pFont) 
		&& (obj.appIcon[TP_AB_ICON_INDEX_SUB_TITLE].title))
	{
		GUI_SetFont(obj.appIcon[TP_AB_ICON_INDEX_SUB_TITLE].pFont);
		subTitleLen = GUI_GetStringDistX((const char*)obj.appIcon[TP_AB_ICON_INDEX_SUB_TITLE].title);
	}

	if (obj.titleAlign == TP_AB_TITLE_LEFT_ALIGN)
	{
		if (TP_AB_ICON_FLAG_HOME & obj.iconFlag)
		{
			homeRect = TP_ActionBar_GetHomeRect(hObj);
			rect.x0 = homeRect.x1 + obj.space[TP_AB_SPACE_INDEX_ICON]  +  1;
		}
		else if (TP_AB_ICON_FLAG_UP & obj.iconFlag)
		{
			upRect = TP_ActionBar_GetUpRect(hObj);
			rect.x0 = upRect.x1 + obj.space[TP_AB_SPACE_INDEX_ICON] + 1;
		}
		else
		{
			rect.x0 = winRect.x0 + obj.padding[TP_AB_PAD_INDEX_LEFT];
		}

		rect.x0 = rect.x0 + titleLen;		
	}
	else
	{
		rect.x0 = winRect.x0 + winXsize / 2 - (titleLen + subTitleLen) / 2 + titleLen;
		
	}
	
	rect.x1 = rect.x0 + subTitleLen - 1;
	
	ySize = GUI_GetFontSizeY();
	yPad = (winRect.y1 - winRect.y0 + 1 - ySize) / 2;
	rect.y0 = winRect.y0 + yPad;
	rect.y1 = rect.y0 + ySize - 1;

	return rect;
}


static GUI_RECT TP_ActionBar_GetTitleRect(TP_ACTIONBAR_HANDLE	hObj)
{
	GUI_RECT winRect, rect, upRect, homeRect;
	TP_ACTIONBAR_OBJ obj;
	int ySize = 0, yPad = 0;
	int titleLen = 0;
	int subTitleLen = 0;
	int winXsize = 0;

	rect.x0 = rect.x1 = 0; 
	rect.y0 = rect.y1 = 0;

	WM_GetWindowRectEx(hObj, &winRect);
	GUI_MoveRect(&winRect, -winRect.x0, -winRect.y0);
	WM_GetUserData(hObj, &obj, sizeof(TP_ACTIONBAR_OBJ));
	winXsize = winRect.x1 - winRect.x0 + 1;

	if (!(TP_AB_ICON_FLAG_TITLE & obj.iconFlag))
	{
		return rect;
	}

	/*�������Ҫ��ʵ��title�ı��ĳ���Ϊ׼*/
	/*Ҳ����˵�û�Ҫ����API������ȷ��iconLen*/

	if ((obj.appIcon[TP_AB_ICON_INDEX_TITLE].pFont) 
		&& (obj.appIcon[TP_AB_ICON_INDEX_TITLE].title))
	{
		GUI_SetFont(obj.appIcon[TP_AB_ICON_INDEX_TITLE].pFont);
		titleLen = GUI_GetStringDistX((const char*)obj.appIcon[TP_AB_ICON_INDEX_TITLE].title);
	}

	if ((obj.appIcon[TP_AB_ICON_INDEX_SUB_TITLE].pFont) 
		&& (obj.appIcon[TP_AB_ICON_INDEX_SUB_TITLE].title))
	{
		GUI_SetFont(obj.appIcon[TP_AB_ICON_INDEX_SUB_TITLE].pFont);
		subTitleLen = GUI_GetStringDistX((const char*)obj.appIcon[TP_AB_ICON_INDEX_SUB_TITLE].title);
	}

	if (obj.titleAlign == TP_AB_TITLE_LEFT_ALIGN)
	{
		if (TP_AB_ICON_FLAG_HOME & obj.iconFlag)
		{
			homeRect = TP_ActionBar_GetHomeRect(hObj);
			rect.x0 = homeRect.x1 + obj.space[TP_AB_SPACE_INDEX_ICON]  +  1;
		}
		else if (TP_AB_ICON_FLAG_UP & obj.iconFlag)
		{
			upRect = TP_ActionBar_GetUpRect(hObj);
			rect.x0 = upRect.x1 + obj.space[TP_AB_SPACE_INDEX_ICON] + 1;
		}
		else
		{
			rect.x0 = winRect.x0 + obj.padding[TP_AB_PAD_INDEX_LEFT];
		}

	}
	else
	{
		rect.x0 = winRect.x0 + winXsize / 2 - (titleLen + subTitleLen) / 2;
		
	}
	
	rect.x1 = rect.x0 + titleLen - 1;
	
	ySize = GUI_GetFontSizeY();
	yPad = (winRect.y1 - winRect.y0 + 1 - ySize) / 2;
	rect.y0 = winRect.y0 + yPad;
	rect.y1 = rect.y0 + ySize - 1;

	return rect;
}

/*�ͷ���pItemָ���Ŀռ�*/
static void TP_ActionBar_FreeItem(TP_AB_BUTTON_ITEM *pItem)
{
	if (pItem)
	{
		free(pItem);
	}
}

static int TP_ActionBar_AddButtonToList(TP_AB_BUTTON_LIST *pList, TP_AB_BUTTON button)
{
	TP_AB_BUTTON_ITEM *pItem = pList->pItem;
	TP_AB_BUTTON_ITEM *pNewItem = NULL;

	pNewItem = (TP_AB_BUTTON_ITEM *)malloc(sizeof(TP_AB_BUTTON_ITEM));

	if (pNewItem == NULL)
	{
		return RET_FAIL;
	}

	memset(pNewItem, 0, sizeof(TP_AB_BUTTON_ITEM));
	GUI_MEMCPY(&(pNewItem->button), &button, sizeof(TP_AB_BUTTON));

	if (pItem == NULL)/* ֮ǰû��Ԫ�� */
	{
		pList->pItem = pNewItem;
		pNewItem->next = NULL;
		pNewItem->prev = NULL;
		pList->itemNum++;
		return RET_SUCCESS;
	}
	
	while (pItem->next)
	{
		pItem = pItem->next;
	}
	pList->itemNum++;

	pItem->next = pNewItem;
	pNewItem->next = NULL;
	pNewItem->prev = pItem;

	return RET_SUCCESS;
}

static int TP_ActionBar_InsertButtonById(TP_AB_BUTTON_LIST *pList, TP_AB_BUTTON button, int id)
{
	TP_AB_BUTTON_ITEM *pItem = pList->pItem;
	TP_AB_BUTTON_ITEM *pNewItem = NULL;

	pNewItem = (TP_AB_BUTTON_ITEM *)malloc(sizeof(TP_AB_BUTTON_ITEM));

	if (pNewItem == NULL)
	{
		return RET_FAIL;
	}
	memset(pNewItem, 0, sizeof(TP_AB_BUTTON_ITEM));
	GUI_MEMCPY(&(pNewItem->button), &button, sizeof(TP_AB_BUTTON));

	if (pItem == NULL)
	{
		return RET_FAIL;
	}

	/*����ǵ�һ��Ԫ��ǰ����*/
	if (pItem->button.id == id)
	{
		pItem->prev = pNewItem;
		
		pNewItem->next = pItem;
		pNewItem->prev = NULL;
		
		pList->pItem = pNewItem;
		pList->itemNum++;
		return RET_SUCCESS;
	}

	pItem = pItem->next;	
	while (pItem)
	{
		if (pItem->button.id == id)/* ����ڵ� */
		{
			pNewItem->next = pItem;
			pNewItem->prev = pItem->prev;
			
			pItem->prev->next = pNewItem;
			pItem->prev = pNewItem;

			pList->itemNum++;
			return RET_SUCCESS;
		}
		pItem = pItem->next;
	}

	return RET_FAIL;
}


static int TP_ActionBar_DeleteButtonById(TP_AB_BUTTON_LIST *pList, int id)
{
	TP_AB_BUTTON_ITEM *pItem = pList->pItem;

	if (pItem == NULL)
	{
		return RET_FAIL;
	}

	/*����ǵ�һ��Ԫ��*/
	if (pItem->button.id == id)
	{
		TP_ActionBar_FreeItem(pItem);
		pList->pItem = NULL;
		pList->itemNum--;
		return RET_SUCCESS;
	}
	

	pItem = pItem->next;	
	while (pItem)
	{
		if (pItem->button.id == id)
		{
			pItem->prev->next = pItem->next;

			if (pItem->next)
			{
				pItem->next->prev = pItem->prev;
			}
			TP_ActionBar_FreeItem(pItem);
			pList->itemNum--;
			return RET_SUCCESS;
		}
		pItem = pItem->next;
	}

	return RET_FAIL;
}

static int TP_ActionBar_DeleteAllButton(TP_AB_BUTTON_LIST *pList)
{
	TP_AB_BUTTON_ITEM *pItem = pList->pItem, *pFreeItem = NULL;

	if (pItem == NULL)
	{
		return RET_SUCCESS;
	}

	while (pItem)
	{
		pFreeItem = pItem;
		pItem = pItem->next;
		TP_ActionBar_FreeItem(pFreeItem);
	}

	return RET_SUCCESS;
}

static int TP_ActionBar_GetButtonWidth(TP_AB_BUTTON list)
{
	int len = 0;

	if (list.bitmap[TP_AB_BM_INDEX_NORMAL])
	{
		len = list.bitmap[TP_AB_BM_INDEX_NORMAL]->XSize;
	}
	else if (list.text)
	{
		/*
		* �����text�Ļ��������Ȳ�Ҫ����text�Ŀ�ȣ��������ѿ�
		* ������ʱ��text�Ŀ�Ȼ����ϼ���һ��ֵ��10 ��Ч��Ҳ���ÿ����������
		* ������һ�°ɣ���һ�����ֵΪ����
		* �߶�ͬ��
		* tips
		*/
		GUI_SetFont(list.pFont);
		len = GUI_GetStringDistX((const char*)list.text) + 10;
	}

	return len;
}

static int TP_ActionBar_GetButtonHeight(TP_AB_BUTTON list)
{
	int height = 0;

	if (list.bitmap[TP_AB_BM_INDEX_NORMAL])
	{
		height = list.bitmap[TP_AB_BM_INDEX_NORMAL]->YSize;
	}
	else if (list.text)
	{
		/*
		* �����text�Ļ�������߶Ȳ�Ҫ����text�ĸ߶ȣ��������ѿ�
		* ������ʱ��text�ĸ߶Ȼ����ϼ���һ��ֵ����������10��10 ��Ч��Ҳ���ÿ����������
		* ������һ�°ɣ���һ�����ֵΪ����
		* ���ͬ��
		* tips
		*/
		GUI_SetFont(list.pFont);
		height = GUI_GetFontDistY() + 10;
	}

	return height;
}

/*
static int TP_ActionBar_GetButtonTotalWidth(TP_ACTIONBAR_HANDLE hObj)
{
	TP_ACTIONBAR_OBJ obj;
	TP_AB_BUTTON_ITEM *pItem = NULL;
	int space = 0;
	int len = 0;
	
	WM_GetUserData(hObj, &obj, sizeof(TP_ACTIONBAR_OBJ));

	space = obj.space[TP_AB_SPACE_INDEX_BUTTON];
	pItem = obj.buttonList.pItem;

	while (pItem)
	{
		len += TP_ActionBar_GetButtonWidth(pItem->button, obj.pFont);
		pItem = pItem->next;
		if (pItem)
		{
			len += space;
		}
	}

	return len;
}
*/

static void TP_ActionBar_RecomputeButtonRect(TP_ACTIONBAR_HANDLE hObj)
{
	TP_ACTIONBAR_OBJ obj;
	TP_AB_BUTTON_ITEM *pItem = NULL;
	int len = 0, buttonWith = 0;
	int buttonMaxLen = 0, space = 0;
	int buttonWidth, buttonHeight;
	GUI_RECT rect;

	const GUI_BITMAP *pMenuBitMap = NULL;
	TP_AB_MENU_ITEM *pMenu = NULL;

	WM_GetUserData(hObj, &obj, sizeof(TP_ACTIONBAR_OBJ));
	buttonMaxLen = (LCD_GetXSize() / 2) - obj.padding[TP_AB_PAD_INDEX_RIGHT];
	space = obj.space[TP_AB_SPACE_INDEX_BUTTON];
	pItem = obj.buttonList.pItem;
	pMenu = &(obj.buttonList.menu);
	pMenuBitMap = pMenu->pBitMap[TP_AB_BM_INDEX_NORMAL];
	
	while (pItem)/* ��������button����Ҫ�ĳ��� */
	{
		buttonWith = TP_ActionBar_GetButtonWidth(pItem->button);
		//pItem->flag &= ~(TP_AB_MENU_ITEM_FLAG);
		len += buttonWith;
		pItem = pItem->next;
		if (pItem)
		{
			len += space;
		}
	}

	/* 
	������ȴ���action bar�ܹ�����button�ĳ��ȣ�
	����Ҫ��Ӳ˵���ť
	*/
	if (len > buttonMaxLen)
	{
		len = 0;
		pItem = obj.buttonList.pItem;
		while (pItem)
		{
			/* Ԥ���˵���ť�Ŀռ� */
			buttonMaxLen = (LCD_GetXSize() / 2) - obj.padding[TP_AB_PAD_INDEX_RIGHT]
								- pMenuBitMap->XSize;		
			buttonWith = TP_ActionBar_GetButtonWidth(pItem->button);

			if ((len + buttonWith) <= buttonMaxLen)/* �����Щ��ť�ǿ���ֱ�ӷ���action bar�� */
			{
				pItem->flag &= ~(TP_AB_MENU_ITEM_FLAG);
			}
			else/* �����Щ��ť��Ҫ�ŵ������˵��� */
			{
				/* 
				���ԭ������action bar�еģ�������Ҫ�ŵ�
				menu�е�button��Ҫ��ɾ��
				*/
				if (pItem->handle > 0)
				{
					WM_DeleteWindow(pItem->handle);
					pItem->handle = 0;
				}
				pItem->flag |= TP_AB_MENU_ITEM_FLAG;
				pItem->rect.x0 = 0;
				pItem->rect.x1 = 0;
				pItem->rect.y0 = 0;
				pItem->rect.y1 = 0;
			}
			len += buttonWith;
			pItem = pItem->next;
		}
	}

	pItem = obj.buttonList.pItem;
	while ((pItem->next) && ((pItem->next->flag & TP_AB_MENU_ITEM_FLAG) == 0))
	{
		pItem = pItem->next;
	}

	len = LCD_GetXSize() - obj.padding[TP_AB_PAD_INDEX_RIGHT];
	if (pItem->next)/* ������в˵��ť */
	{
		len -= pMenuBitMap->XSize;
		
		pMenu->rect.x0 = len;/* ���˵��ť��λ */
		pMenu->rect.x1 = pMenu->rect.x0 + pMenuBitMap->XSize;
		pMenu->rect.y0 = (obj.height - pMenuBitMap->YSize) / 2;
		pMenu->rect.y1 = pMenu->rect.y0 + pMenuBitMap->YSize;
		
		len -= obj.space[TP_AB_SPACE_INDEX_BUTTON];
	}
	else
	{/* �������Ҫ�˵��ť��λΪ0 */
		pMenu->rect.x0 = 0;
		pMenu->rect.x1 = 0;
		pMenu->rect.y0 = 0;
		pMenu->rect.y1 = 0;
	}

	while (pItem)
	{
		buttonWidth = TP_ActionBar_GetButtonWidth(pItem->button);
		buttonHeight = TP_ActionBar_GetButtonHeight(pItem->button);

		rect = pItem->rect;
		pItem->rect.x1 = len;
		pItem->rect.x0 = pItem->rect.x1 - buttonWidth;

		pItem->rect.y0 = obj.padding[TP_AB_PAD_INDEX_TOP];
		/* 
		* ���button�ĸ߶Ȳ�����actionbar�߶ȵ�1/2����button���ϱ߾�y0Ϊactionbar�ĸ߶�
		* ��button�߶Ȳ��1/2
		* tips
		*/
		if (buttonHeight < obj.height / 2) {
			pItem->rect.y0 = (obj.height - buttonHeight) / 2;
		}
		pItem->rect.y1 = pItem->rect.y0 + buttonHeight;

		if (TP_CompareRect(rect, pItem->rect) != 0)
		{
			pItem->flag |= TP_AB_RECT_CHANGE_FLAG;
		}
		else
		{
			pItem->flag &= ~(TP_AB_RECT_CHANGE_FLAG);
		}

		len -= buttonWidth + obj.space[TP_AB_SPACE_INDEX_BUTTON];
		pItem = pItem->prev;
	}

	WM_SetUserData(hObj, &obj, sizeof(TP_ACTIONBAR_OBJ));
}


/*
*
* �������Ӧ���ǲ���һ�����ʹ���һ��Button��Ȼ���������������ȥ
* ���⣬Ӧ������һ��ָ���������һ���ڵ��ָ�룬��ȡ�䴰������rect��Ȼ���ж���һ����β���
*
*/
/*
* ���������һ���µ�button
*/
static int TP_ActionBar_SetUpButton(TP_ACTIONBAR_HANDLE hObj, TP_AB_BUTTON button) {
	TP_ACTIONBAR_OBJ obj;
	TP_AB_BUTTON_ITEM *pItem = NULL;
	int buttonWidth, buttonHeight;
	GUI_RECT menuRect;
	TP_AB_MENU_ITEM *pMenu = NULL;
	TP_MENU_ITEM menuItem;

	TP_ActionBar_RecomputeButtonRect(hObj);

	WM_GetUserData(hObj, &obj, sizeof(TP_ACTIONBAR_OBJ));

	pItem = obj.buttonList.pItem;

	while (pItem && ((pItem->flag & TP_AB_MENU_ITEM_FLAG) == 0))
	{	
		buttonWidth = TP_ActionBar_GetButtonWidth(pItem->button);
		buttonHeight = TP_ActionBar_GetButtonHeight(pItem->button);

		if (pItem->handle <= 0)
		{
			pItem->handle = TP_Button_Create(pItem->rect.x0, pItem->rect.y0, buttonWidth, buttonHeight, hObj, 
				WM_CF_SHOW | WM_CF_MEMDEV, NULL, 0, pItem->button.id);
			TP_Button_SetBkColor(pItem->handle, TP_BUTTON_UNPRESSED, obj.bkColor[TP_AB_BK_COLOR_INDEX_NORMAL]);
			TP_Button_SetBkColor(pItem->handle, TP_BUTTON_PRESSED, obj.bkColor[TP_AB_BK_COLOR_INDEX_CLICKED]);
			TP_Button_SetBkColorOn(pItem->handle, 1);
			TP_Button_SetBitmap(pItem->handle, TP_BUTTON_UNPRESSED, pItem->button.bitmap[TP_AB_BM_INDEX_NORMAL]);
			TP_Button_SetBitmap(pItem->handle, TP_BUTTON_PRESSED, pItem->button.bitmap[TP_AB_BM_INDEX_CLICKED]);

			/* ��ͼƬΪ�յ��������ʾ���� */
			//if ((pItem->button.text[0] != 0) && (pItem->button.bitmap[TP_AB_BM_INDEX_NORMAL] == NULL))
			//{
				TP_Button_SetText(pItem->handle, (char*)pItem->button.text);
			//}
		}
		else if (pItem->flag & TP_AB_RECT_CHANGE_FLAG);
		{
			WM_MoveTo(pItem->handle, pItem->rect.x0, pItem->rect.y0);
			pItem->flag &= ~(TP_AB_RECT_CHANGE_FLAG);
		}

		pItem = pItem->next;
	}

	pMenu = &(obj.buttonList.menu);
	menuRect = pMenu->rect;

	if ((menuRect.x0 != menuRect.x1) && (menuRect.y0 != menuRect.y1))/* �����Ҫ�˵���ť */
	{
		if (pMenu->handle <= 0)
		{
			pMenu->handle = TP_Button_Create(menuRect.x0, menuRect.y0,
				menuRect.x1 - menuRect.x0 + 1, menuRect.y1 - menuRect.y0 + 1,
				hObj, WM_CF_SHOW | WM_CF_MEMDEV, NULL, 0, TP_AB_DEFAULT_MENU_BUTTON_ID);
			TP_Button_SetBitmap(pMenu->handle, TP_BUTTON_UNPRESSED, pMenu->pBitMap[TP_AB_BM_INDEX_NORMAL]);
			TP_Button_SetBitmap(pMenu->handle, TP_BUTTON_PRESSED, pMenu->pBitMap[TP_AB_BM_INDEX_CLICKED]);
		}

		if (pMenu->menuHandle > 0)/* ����˵��Ѿ���������ɾ���ٽ����µ� */
		{

			WM_DeleteWindow(pMenu->menuHandle);
		}

		{
			pMenu->menuHandle = TP_Menu_Create(0, 0, 1, 1, WM_GetParent(hObj), 
				WM_CF_HIDE | WM_CF_MEMDEV, TP_AB_DEFAULT_MENU_ID, NULL, 0);	
			TP_Menu_SetFont(pMenu->menuHandle, pMenu->pFont);
		}

		if (pMenu->menuHandle > 0)
		{
			while (pItem)/* ��Ӳ˵��� */
			{
				if (pItem->flag & TP_AB_MENU_ITEM_FLAG)
				{
					menuItem.flags = 0;
					menuItem.hSubmenu = 0;
					menuItem.id = pItem->button.id;
					menuItem.pBitmap = NULL;
					menuItem.pText = pItem->button.text;
					TP_Menu_AddItem(pMenu->menuHandle, &menuItem);
				}
				pItem = pItem->next;
			}
		}
	}
	else if (pMenu->handle > 0)/* �������ҪMenu�˵���֮ǰ�Ѿ�������ɾ�� */
	{
		WM_DeleteWindow(pMenu->menuHandle);
		pMenu->menuHandle = 0;
	}
	WM_SetUserData(hObj, &obj, sizeof(TP_ACTIONBAR_OBJ));
	return RET_SUCCESS;
}

int TP_ActionBar_AddButton(TP_ACTIONBAR_HANDLE hObj, TP_AB_BUTTON button)
{
	TP_ACTIONBAR_OBJ obj;
	WM_GetUserData(hObj, &obj, sizeof(TP_ACTIONBAR_OBJ));

	if (TP_ActionBar_AddButtonToList(&(obj.buttonList), button) == RET_SUCCESS)
	{
		WM_SetUserData(hObj, &obj, sizeof(TP_ACTIONBAR_OBJ));
		
		if (TP_ActionBar_SetUpButton(hObj, button) == RET_SUCCESS) 
		{
			return RET_SUCCESS;
		}
	}
	
	return RET_FAIL;
}

/* ��û����ȫʵ�� */
int TP_ActionBar_InsertButton(TP_ACTIONBAR_HANDLE	hObj, TP_AB_BUTTON button, int id)
{
	TP_ACTIONBAR_OBJ obj;
	WM_GetUserData(hObj, &obj, sizeof(TP_ACTIONBAR_OBJ));

	if (TP_ActionBar_InsertButtonById(&(obj.buttonList), button, id) == RET_SUCCESS)
	{
		WM_SetUserData(hObj, &obj, sizeof(TP_ACTIONBAR_OBJ));
		return RET_SUCCESS;
	}


	return RET_FAIL;
}

/* ��û����ȫʵ�� */
int TP_ActionBar_DeleteButton(TP_ACTIONBAR_HANDLE hObj, int id)
{
	TP_ACTIONBAR_OBJ obj;
	WM_GetUserData(hObj, &obj, sizeof(TP_ACTIONBAR_OBJ));

	if (TP_ActionBar_DeleteButtonById(&(obj.buttonList), id) == RET_SUCCESS)
	{
		WM_SetUserData(hObj, &obj, sizeof(TP_ACTIONBAR_OBJ));
		return RET_SUCCESS;
	}


	return RET_FAIL;
}

int TP_ActionBar_SetUpIcon(TP_ACTIONBAR_HANDLE hObj, TP_AB_ICON upIcon)
{
	TP_ACTIONBAR_OBJ obj;
	GUI_RECT rect;
	TP_AB_ICON_TYPE *pIcon = NULL;

	WM_GetUserData(hObj, &obj, sizeof(TP_ACTIONBAR_OBJ));
	pIcon = &(obj.appIcon[TP_AB_ICON_INDEX_UP]);
	pIcon->bitmap[TP_AB_BM_INDEX_NORMAL] = upIcon.bitmap[TP_AB_BM_INDEX_NORMAL];
	pIcon->bitmap[TP_AB_BM_INDEX_CLICKED] = upIcon.bitmap[TP_AB_BM_INDEX_CLICKED];
	pIcon->id = upIcon.id;
	pIcon->title = NULL;
	
	obj.iconFlag |= TP_AB_ICON_FLAG_UP;
	WM_SetUserData(hObj, &obj, sizeof(TP_ACTIONBAR_OBJ));

	if (pIcon->handle > 0)
	{
		/*
			������ǵ�һ�����ã���ֻ���������ö�Ӧ��ͼƬ��
			��ͼƬ��СҪһ�£�һ������²����ظ�����ͼƬ
		*/
		TP_Button_SetBitmap(pIcon->handle, TP_BUTTON_UNPRESSED, pIcon->bitmap[TP_AB_BM_INDEX_NORMAL]);
		TP_Button_SetBitmap(pIcon->handle, TP_BUTTON_PRESSED, pIcon->bitmap[TP_AB_BM_INDEX_NORMAL]);
	}
	else
	{
		/*����ǵ�һ�����ã��򴴽���Ӧ�Ŀؼ�*/
		rect = TP_ActionBar_GetUpRect(hObj);
		pIcon->handle = TP_Button_Create(rect.x0, rect.y0, rect.x1 - rect.x0 + 1,
			rect.y1 - rect.y0 + 1, hObj, WM_CF_SHOW | WM_CF_MEMDEV, NULL, 0, pIcon->id);

		TP_Button_SetBitmap(pIcon->handle, TP_BUTTON_UNPRESSED, pIcon->bitmap[TP_AB_BM_INDEX_NORMAL]);
		TP_Button_SetBitmap(pIcon->handle, TP_BUTTON_PRESSED, pIcon->bitmap[TP_AB_BM_INDEX_CLICKED]);
		TP_Button_SetBkColor(pIcon->handle, TP_BUTTON_UNPRESSED, obj.bkColor[TP_AB_BK_COLOR_INDEX_NORMAL]);
		TP_Button_SetBkColor(pIcon->handle, TP_BUTTON_PRESSED, obj.bkColor[TP_AB_BK_COLOR_INDEX_CLICKED]);
		TP_Button_SetBkColorOn(pIcon->handle, 1);
	}
	return RET_SUCCESS;
}

int TP_ActionBar_SetHomeIcon(TP_ACTIONBAR_HANDLE hObj, TP_AB_ICON homeIcon)
{
	TP_ACTIONBAR_OBJ obj;
	GUI_RECT rect;
	TP_AB_ICON_TYPE *pIcon = NULL;
	
	WM_GetUserData(hObj, &obj, sizeof(TP_ACTIONBAR_OBJ));
	pIcon = &(obj.appIcon[TP_AB_ICON_INDEX_HOME]);
	pIcon->bitmap[TP_AB_BM_INDEX_NORMAL] = homeIcon.bitmap[TP_AB_BM_INDEX_NORMAL];
	pIcon->bitmap[TP_AB_BM_INDEX_CLICKED] = homeIcon.bitmap[TP_AB_BM_INDEX_CLICKED];
	pIcon->id = homeIcon.id;
	pIcon->title = NULL;

	obj.iconFlag |= TP_AB_ICON_FLAG_HOME;
	WM_SetUserData(hObj, &obj, sizeof(TP_ACTIONBAR_OBJ));
	
	if (pIcon->handle > 0)
	{
		/*
			������ǵ�һ�����ã���ֻ���������ö�Ӧ��ͼƬ��
			��ͼƬ��СҪһ�£�һ������²����ظ�����ͼƬ
		*/
		TP_BMP_SetBkBitmap(pIcon->handle, pIcon->bitmap[TP_AB_BM_INDEX_NORMAL]);
	}
	else
	{
		/*����ǵ�һ�����ã��򴴽���Ӧ�Ŀؼ�*/
		rect = TP_ActionBar_GetHomeRect(hObj);
		pIcon->handle = TP_BMP_Create(rect.x0, rect.y0, rect.x1 - rect.x0 + 1, rect.y1 - rect.y0 + 1, 
						hObj, WM_CF_SHOW | WM_CF_MEMDEV, NULL, 0, pIcon->id);
		TP_BMP_SetBkBitmap(pIcon->handle, pIcon->bitmap[TP_AB_BM_INDEX_NORMAL]);
		TP_BMP_SetBkColor(pIcon->handle, obj.bkColor[TP_AB_BK_COLOR_INDEX_NORMAL]);
	}

	return RET_SUCCESS;
}

/******************************************************************************
* FUNCTION      : TP_ActionBar_SetSubTitle()
* AUTHOR        : Huangwenzhong <Huangwenzhong@tp-link.net>
* DESCRIPTION   : ���ø����� 
* INPUT         : N/A
*
* OUTPUT        : N/A
* RETURN        : N/A
* OTHERS        :����и�����Ļ�����Ҫ������������ǰ����
******************************************************************************/
int TP_ActionBar_SetSubTitle(TP_ACTIONBAR_HANDLE hObj, TP_AB_TITLE title)
{
	TP_ACTIONBAR_OBJ obj;
	//GUI_RECT rect;
	TP_AB_ICON_TYPE *pIcon = NULL;
	
	WM_GetUserData(hObj, &obj, sizeof(TP_ACTIONBAR_OBJ));

	pIcon = &(obj.appIcon[TP_AB_ICON_INDEX_SUB_TITLE]);
	
	pIcon->title = malloc(strlen(title.title) + 1);
	if (pIcon->title == NULL)
	{
		return RET_FAIL;
	}
	memset(pIcon->title, 0, strlen(title.title) + 1);
	GUI_MEMCPY(pIcon->title, title.title, strlen(title.title));

	pIcon->bitmap[TP_AB_BM_INDEX_NORMAL] = NULL;
	pIcon->bitmap[TP_AB_BM_INDEX_CLICKED] = NULL;
	pIcon->id = title.id;
	pIcon->pFont = title.pFont;
	obj.iconFlag |= TP_AB_ICON_FLAG_SUB_TITLE;
	WM_SetUserData(hObj, &obj, sizeof(TP_ACTIONBAR_OBJ));
	return RET_SUCCESS;
}


int TP_ActionBar_SetTitle(TP_ACTIONBAR_HANDLE hObj, TP_AB_TITLE title)
{
	TP_ACTIONBAR_OBJ obj;
	GUI_RECT rect;
	TP_AB_ICON_TYPE *pIcon = NULL;
	
	WM_GetUserData(hObj, &obj, sizeof(TP_ACTIONBAR_OBJ));

	pIcon = &(obj.appIcon[TP_AB_ICON_INDEX_TITLE]);
	
	pIcon->title = malloc(strlen(title.title) + 1);
	if (pIcon->title == NULL)
	{
		return RET_FAIL;
	}
	memset(pIcon->title, 0, strlen(title.title) + 1);
	GUI_MEMCPY(pIcon->title, title.title, strlen(title.title));

	pIcon->bitmap[TP_AB_BM_INDEX_NORMAL] = NULL;
	pIcon->bitmap[TP_AB_BM_INDEX_CLICKED] = NULL;
	pIcon->id = title.id;
	obj.iconFlag |= TP_AB_ICON_FLAG_TITLE;
	WM_SetUserData(hObj, &obj, sizeof(TP_ACTIONBAR_OBJ));

	/*��������ʾ������*/
	if (pIcon->handle > 0)
	{
		/*
			������ǵ�һ�����ã���ֻ������������ʾ���ı�
		*/
		
		TP_Text_SetText(pIcon->handle, pIcon->title);
	}
	else
	{
		/*����ǵ�һ�����ã��򴴽���Ӧ�Ŀؼ�*/
		rect = TP_ActionBar_GetTitleRect(hObj);
		pIcon->handle = TP_Text_CreateEx(rect.x0, rect.y0, rect.x1 - rect.x0 + 1, rect.y1 - rect.y0 + 1, 
											hObj, WM_CF_SHOW | WM_CF_MEMDEV, 0, pIcon->id, pIcon->title);
		TP_Text_SetFont(pIcon->handle, pIcon->pFont);
		TP_Text_SetBkColor(pIcon->handle, obj.bkColor[TP_AB_BK_COLOR_INDEX_NORMAL]);
		TP_Text_SetTextColor(pIcon->handle, obj.textColor);
		TP_Text_SetTextAlign(pIcon->handle, GUI_TA_LEFT | GUI_TA_VCENTER);
	}

	/*��������ʾ������*/
	pIcon = &(obj.appIcon[TP_AB_ICON_INDEX_SUB_TITLE]);

	if (pIcon->handle > 0)
	{
		/*
			������ǵ�һ�����ã���ֻ������������ʾ���ı�
		*/
		
		TP_Text_SetText(pIcon->handle, pIcon->title);
	}
	else
	{
		/*����ǵ�һ�����ã��򴴽���Ӧ�Ŀؼ�*/
		rect = TP_ActionBar_GetSubTitleRect(hObj);
		pIcon->handle = TP_Text_CreateEx(rect.x0, rect.y0, rect.x1 - rect.x0 + 1, rect.y1 - rect.y0 + 1, 
											hObj, WM_CF_SHOW | WM_CF_MEMDEV, 0, pIcon->id, pIcon->title);
		TP_Text_SetFont(pIcon->handle, pIcon->pFont);
		TP_Text_SetBkColor(pIcon->handle, obj.bkColor[TP_AB_BK_COLOR_INDEX_NORMAL]);
		TP_Text_SetTextColor(pIcon->handle, obj.textColor);
		TP_Text_SetTextAlign(pIcon->handle, GUI_TA_LEFT | GUI_TA_VCENTER);
	}
	return RET_SUCCESS;
}


int TP_ActionBar_GetId(TP_ACTIONBAR_HANDLE hObj)
{
	TP_ACTIONBAR_OBJ obj;
	WM_GetUserData(hObj, &obj, sizeof(TP_ACTIONBAR_OBJ));
	return obj.id;
}

void TP_ActionBar_Callback(WM_MESSAGE * pMsg) 
{
	WM_HWIN hWin;
	GUI_RECT winRect;
	TP_ACTIONBAR_OBJ obj;
	//GUI_PID_STATE *pState;
	//int isClicked;

	//WM_HWIN hParentWin;
	//WM_PID_STATE_CHANGED_INFO * pPidState;
	WM_MESSAGE msg;
	//TP_MENU_MSG menuMsg;
	TP_ACTIONBAR_MSG abMsg;
	int id;
	//GUI_RECT rClient;
	WM_HMEM menuHandle;
	int menuXSize = 0;
	GUI_RECT menuButtonRect;
	int menuXPos = 0, menuYPos = 0;

	hWin = pMsg->hWin;
	WM_GetWindowRectEx(hWin, &winRect);
	GUI_MoveRect(&winRect, -winRect.x0, -winRect.y0);
	WM_GetUserData(hWin, &obj, sizeof(TP_ACTIONBAR_OBJ));
	switch (pMsg->MsgId) {
	case WM_PAINT:
		
		if (obj.bkBitmap)/*�б���ͼƬ�� ���Ʊ���ͼƬ */
		{
			GUI_DrawBitmap(obj.bkBitmap, winRect.x0, winRect.y0);
		}
		else/* ���Ʊ�����ɫ */
		{
			GUI_SetColor(obj.bkColor[TP_AB_BK_COLOR_INDEX_NORMAL]);
			GUI_FillRectEx(&winRect);
		}
		
		break;

	case WM_NOTIFY_PARENT:
		id = WM_GetId(pMsg->hWinSrc);
		if (id < 0) 
		{
			break;
		}
		switch (pMsg->Data.v) {
		case WM_NOTIFICATION_RELEASED:
			
			switch (id) 
			{
			/*
				case TP_AB_BUTTON_ID_0:
					GUI_MessageBox("click 0", "message", 0);
					break;
			*/
			case TP_AB_DEFAULT_MENU_BUTTON_ID:
				WM_GetWindowRectEx(pMsg->hWinSrc, &menuButtonRect);
				menuHandle = obj.buttonList.menu.menuHandle;
				menuXSize = TP_Menu_GetMenuXSize(menuHandle);
				menuXPos = menuButtonRect.x0 + ((menuButtonRect.x1 - menuButtonRect.x0 + 1) - menuXSize);
				menuYPos = menuButtonRect.y1 + 2;
				TP_Menu_Popup(menuHandle, WM_GetParent(hWin), menuXPos, menuYPos, 0, 0);
				break;
			default:
				msg.MsgId = WM_TP_ACTION_BAR;
				msg.hWin = WM_GetParent(hWin);
				msg.hWinSrc = hWin;
				msg.Data.p = &abMsg;
				abMsg.id = id;
				abMsg.MsgType = TP_AB_BUTTON_ON_RELEASED;
				/* ����Ϣ��֪������ĳ���˵������ */
				WM_SendToParent(hWin, &msg);
				break;
			}

			break;

		case WM_NOTIFICATION_CLICKED:
			switch (id) 
			{
			case TP_AB_DEFAULT_MENU_BUTTON_ID:/*do nothing*/
				break;
			default:
				msg.MsgId = WM_TP_ACTION_BAR;
				msg.hWin = WM_GetParent(hWin);
				msg.hWinSrc = hWin;
				msg.Data.p = &abMsg;
				abMsg.id = id;
				abMsg.MsgType = TP_AB_BUTTON_ON_PRESSED;
				/* ����Ϣ��֪������ĳ���˵������ */
				WM_SendToParent(hWin, &msg);
				break;
			}
			break;
		}
		break;

#if 0
	case WM_PID_STATE_CHANGED:
		pPidState = (WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p);
		hParentWin = WM_GetParent(pMsg->hWin);
		if (pPidState) {
			if (pPidState->State) {
				WM_NotifyParent(hWin, WM_NOTIFICATION_CLICKED);
				WM_SetFocus(pMsg->hWin);
			} else {
				msg.MsgId = WM_NOTIFY_PARENT;
				msg.Data.v = WM_NOTIFICATION_RELEASED;
				msg.Data.p = &menuMsg;
				menuMsg.ItemId = WM_GetId(pMsg->hWin);
				menuMsg.MsgType = WM_NOTIFICATION_RELEASED;
				WM_NotifyParent(hWin, WM_NOTIFICATION_RELEASED);
			}
		}
		WM_InvalidateWindow(pMsg->hWin);
		break;
#endif

/*
	case WM_TOUCH:
		pState = (GUI_PID_STATE *)(pMsg->Data.p);
		if (pState->Pressed) {
			isClicked = pState->Pressed;
		} else {
			isClicked = 0;
		}

		break;
*/
	case WM_DELETE:/*Ҫ���������Ϣ��ɾ����̬������ڴ�*/
		TP_ActionBar_DeleteAllButton(&(obj.buttonList));
		WM_SetUserData(hWin, &obj, sizeof(TP_ACTIONBAR_OBJ));
		break;
	case WM_GET_ID:
		pMsg->Data.v = obj.id;
		break;
	case WM_SET_ID:
		obj.id = pMsg->Data.v;
		WM_SetUserData(hWin, &obj, sizeof(TP_ACTIONBAR_OBJ));
		break;
	default:
		WM_DefaultProc(pMsg);
		break;
	}
}

TP_ACTIONBAR_HANDLE TP_ActionBar_Create(int x, int y, int width, int height, WM_HWIN hParent, U32 winFlags, 
	int id, WM_CALLBACK *pfCallback, int NumExtraBytes)
{
	TP_ACTIONBAR_HANDLE	handle;
	TP_ACTIONBAR_OBJ 	obj;
	WM_CALLBACK 		*pCb = NULL;

	if (pfCallback)
	{
		pCb = pfCallback;
	}
	else
	{
		pCb = TP_ActionBar_Callback;
	}

	obj = TP_ActionBar_Default;

	obj.NumExtraBytes = NumExtraBytes;
	obj.id = id;
	obj.height = height;

	handle = WM_CreateWindowAsChild(x, y, width, height, hParent, 
				winFlags, pCb, sizeof(TP_ACTIONBAR_OBJ) + NumExtraBytes);
	WM_SetUserData(handle, &obj, sizeof(TP_ACTIONBAR_OBJ));
	return handle;

}

