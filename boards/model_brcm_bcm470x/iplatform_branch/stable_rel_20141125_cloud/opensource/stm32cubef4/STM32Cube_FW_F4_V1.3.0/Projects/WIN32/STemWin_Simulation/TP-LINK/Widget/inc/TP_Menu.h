#ifndef __TP_MENU_H__
#define  __TP_MENU_H__

#include "GUI.h"
#include "WM.h"

/*Menu Background Color indices */
typedef enum 
{
	TP_MENU_ITEM_BK_CI_MIN				= (0),
	TP_MENU_ITEM_BK_CI_ENABLED_SEL		= (0),
	TP_MENU_ITEM_BK_CI_ENABLED_NORMAL,
	TP_MENU_ITEM_BK_CI_DISABLED_SEL,
	TP_MENU_ITEM_BK_CI_DISABLED_NORMAL,
	TP_MENU_ITEM_BK_CI_MAX,
}TP_MENU_BK_CI;


/*Menu Item Text Color indices */
typedef enum 
{
	TP_MENU_ITEM_TEXT_CI_MIN				= (0),
	TP_MENU_ITEM_TEXT_CI_ENABLED_SEL		= (0),
	TP_MENU_ITEM_TEXT_CI_ENABLED_NORMAL,
	TP_MENU_ITEM_TEXT_CI_DISABLED_SEL,
	TP_MENU_ITEM_TEXT_CI_DISABLED_NORMAL,
	TP_MENU_ITEM_TEXT_CI_MAX,
}TP_MENU_TEXT_CI;

typedef enum 
{
	TP_MENU_BORDER_INDEX_MIN	= (0),
	TP_MENU_BORDER_INDEX_LEFT	= (0),
	TP_MENU_BORDER_INDEX_RIGHT,
	TP_MENU_BORDER_INDEX_MAX
}TP_MENU_BORDER_INDEX;

#define TP_MENU_ON_ITEMPRESSED		(0)
#define TP_MENU_ON_ITEMSELECT		(1)


/*********************************************************************
*
*       Menu message data
*/
typedef struct 
{
	U16 MsgType;
	U16 ItemId;
} TP_MENU_MSG;

/* Menu Item Flags */
#define TP_MENU_IF_DISABLED		(1 << 0)/*  ��Ǵ˲˵����ѱ����� */
#define TP_MENU_IF_SEPARATOR	(1 << 1)/* ��Ǵ˲˵���ֻ�Ƿָ��� */

typedef WM_HMEM TP_MENU_Handle;

/*********************************************************************
*
*       Menu item data
*/
typedef struct 
{
	CHAR  		*pText;
	GUI_BITMAP	*pBitmap;
	U16           id;
	U16           flags;/* ȡֵΪTP_MENU_IF_XXX */
	TP_MENU_Handle   hSubmenu;	/* �����Ϊ0����ʾ��һ���Ӳ˵����ݲ�֧�� */
} TP_MENU_ITEM;

typedef struct _TP_MENU_ITEM_LIST
{
	TP_MENU_ITEM item;	/* �˵������� */
	GUI_RECT	rect;	/* ���˵������ڵľ������� */
	struct _TP_MENU_ITEM_LIST *next;
}TP_MENU_ITEM_LIST;

typedef struct 
{
	TP_MENU_ITEM_LIST *next;
}TP_MENU_ITEM_HEAD;

typedef struct 
{
	GUI_COLOR bkColor;								/* ����ı�����ɫ��һ���ǲ˵���Χ��������ɫ */	
	GUI_COLOR itemBkColor[TP_MENU_ITEM_BK_CI_MAX];		/*�˵���ı�����ɫ*/
	GUI_COLOR textColor[TP_MENU_ITEM_TEXT_CI_MAX];		/*�����ɫ*/
	const GUI_FONT  *pFont;								/*�ı�������*/
	U8		borderWidth[TP_MENU_BORDER_INDEX_MAX];		/*��߿��ı�(����ͼ��)�����ؾ���*/
	U8		i2tSpace;								/* ͼ����ı�֮������ؾ��� */
	int	height;										/*���Ĭ�ϸ߶�*/
}TP_MENU_PROPS;


/*********************************************************************
*
* TP_Checkbox_Obj
*//**/
typedef struct 
{
	int id;
	TP_MENU_PROPS props;			/* �û������õ����� */
	TP_MENU_ITEM_HEAD itemHead;		/* �˵��б�,���Ǹ�ͷָ�� */
	int	ItemNum;					/* �˵�������� */
	int NumExtraBytes;				/* ����Ĵ洢�ռ����� */
	int pressedId;					/* �����µĲ˵����ID */
}TP_MENU_OBJ;

void TP_Menu_AddItem(TP_MENU_Handle hObj, const TP_MENU_ITEM* pItem);
void     TP_Menu_InsertItem(TP_MENU_Handle hObj, U16 itemId, const TP_MENU_ITEM* pItemData);
void      TP_Menu_DeleteItem   (TP_MENU_Handle hObj, U16 itemId);
void      TP_Menu_ClearItem   (TP_MENU_Handle hObj);

void TP_Menu_DisableItem  (TP_MENU_Handle hObj, U16 itemId);
void     TP_Menu_EnableItem   (TP_MENU_Handle hObj, U16 itemId);
void TP_Menu_Popup (TP_MENU_Handle hObj, WM_HWIN hDestWin, int x, int y, int xSize, int ySize);
void TP_Menu_Attach (TP_MENU_Handle hObj, WM_HWIN hDestWin, int x, int y, int xSize, int ySize);

unsigned  TP_Menu_GetNumItems(TP_MENU_Handle hObj);
void  TP_Menu_GetItem (TP_MENU_Handle hObj, U16 itemId, TP_MENU_ITEM* pItemData);

WM_HWIN   TP_Menu_GetOwner     (TP_MENU_Handle hObj);
int      TP_Menu_GetUserData  (TP_MENU_Handle hObj, void * pDest, int SizeOfBuffer);
int TP_Menu_SetUserData  (TP_MENU_Handle hObj, const void * pSrc, int SizeOfBuffer);
GUI_COLOR TP_Menu_GetBkColor (TP_MENU_Handle hObj, INT32 colorIdx);

void TP_Menu_SetBkColor   (TP_MENU_Handle hObj, INT32 colorIdx, GUI_COLOR color);

U8 TP_Menu_GetBorderSize(TP_MENU_Handle hObj, INT32 idx);
void TP_Menu_SetBorderSize(TP_MENU_Handle hObj, INT32 idx, U8 BorderSize);
const GUI_FONT*  TP_Menu_GetFont(TP_MENU_Handle hObj, const GUI_FONT * pFont);
void      TP_Menu_SetFont      (TP_MENU_Handle hObj, const GUI_FONT * pFont);
GUI_COLOR TP_Menu_GetTextColor (TP_MENU_Handle hObj, INT32 idx);
void TP_Menu_SetTextColor (TP_MENU_Handle hObj, INT32 idx, GUI_COLOR color);
GUI_COLOR TP_Menu_GetDefaultTextColor (INT32 idx);
void TP_Menu_SetDefaultTextColor  (INT32 idx, GUI_COLOR color);
GUI_COLOR TP_Menu_GetDefaultBkColor (INT32 idx);
void TP_Menu_SetDefaultBkColor(INT32 idx, GUI_COLOR color);
const GUI_FONT * TP_Menu_GetDefaultFont(void);
void TP_Menu_SetDefaultFont(const GUI_FONT * pFont);
U8 TP_Menu_GetDefaultBorderSize (INT32 idx);
void TP_Menu_SetDefaultBorderSize (INT32 idx, U8 BorderSize);
TP_MENU_Handle TP_Menu_Create(int x0, int y0, int xSize, int ySize, 
	WM_HWIN hParent, U32 winFlags, int id, WM_CALLBACK *pfCallback, int NumExtraBytes);
int TP_Menu_GetId(TP_MENU_Handle hObj);
int TP_Menu_GetMenuXSize(TP_MENU_Handle hObj);

#endif

