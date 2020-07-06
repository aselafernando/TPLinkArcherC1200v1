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
#define TP_MENU_IF_DISABLED		(1 << 0)/*  标记此菜单项已被禁用 */
#define TP_MENU_IF_SEPARATOR	(1 << 1)/* 标记此菜单项只是分隔符 */

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
	U16           flags;/* 取值为TP_MENU_IF_XXX */
	TP_MENU_Handle   hSubmenu;	/* 如果不为0，表示是一个子菜单，暂不支持 */
} TP_MENU_ITEM;

typedef struct _TP_MENU_ITEM_LIST
{
	TP_MENU_ITEM item;	/* 菜单项数据 */
	GUI_RECT	rect;	/* 本菜单项所在的矩形区域 */
	struct _TP_MENU_ITEM_LIST *next;
}TP_MENU_ITEM_LIST;

typedef struct 
{
	TP_MENU_ITEM_LIST *next;
}TP_MENU_ITEM_HEAD;

typedef struct 
{
	GUI_COLOR bkColor;								/* 方框的背景颜色，一般是菜单周围的线条颜色 */	
	GUI_COLOR itemBkColor[TP_MENU_ITEM_BK_CI_MAX];		/*菜单项的背景颜色*/
	GUI_COLOR textColor[TP_MENU_ITEM_TEXT_CI_MAX];		/*框的颜色*/
	const GUI_FONT  *pFont;								/*文本的字体*/
	U8		borderWidth[TP_MENU_BORDER_INDEX_MAX];		/*左边框到文本(或者图标)的像素距离*/
	U8		i2tSpace;								/* 图标跟文本之间的像素距离 */
	int	height;										/*框的默认高度*/
}TP_MENU_PROPS;


/*********************************************************************
*
* TP_Checkbox_Obj
*//**/
typedef struct 
{
	int id;
	TP_MENU_PROPS props;			/* 用户可配置的属性 */
	TP_MENU_ITEM_HEAD itemHead;		/* 菜单列表,这是个头指针 */
	int	ItemNum;					/* 菜单项的数量 */
	int NumExtraBytes;				/* 额外的存储空间数量 */
	int pressedId;					/* 被按下的菜单项的ID */
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

