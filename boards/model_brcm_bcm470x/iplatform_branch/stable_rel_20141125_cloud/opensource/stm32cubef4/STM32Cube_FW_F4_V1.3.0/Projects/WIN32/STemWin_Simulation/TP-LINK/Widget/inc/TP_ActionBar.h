#ifndef TP_ACTIONBAR_H
#define TP_ACTIONBAR_H

#include "GUI.h"
#include "WM.h"

typedef WM_HMEM TP_ACTIONBAR_HANDLE;

/* Action Bar左右上下padding的索引 */
typedef enum 
{
	TP_AB_PAD_INDEX_MIN = (0),
	TP_AB_PAD_INDEX_LEFT = (0),
	TP_AB_PAD_INDEX_RIGHT,
	TP_AB_PAD_INDEX_TOP,
	TP_AB_PAD_INDEX_BOTTOM,	
	TP_AB_PAD_INDEX_MAX
}TP_AB_PAD_INDEX;

/* Action Bar背景颜色，分为正常和被点击时的情况 */
typedef enum 
{
	TP_AB_BK_COLOR_INDEX_MIN = (0),
	TP_AB_BK_COLOR_INDEX_NORMAL = (0),
	TP_AB_BK_COLOR_INDEX_CLICKED,
	TP_AB_BK_COLOR_INDEX_MAX,
}TP_AB_BK_COLOR_INDEX;

/* 左边app icon区域和右边button区域中相邻元素的间隔索引 */
typedef enum
{
	TP_AB_SPACE_INDEX_MIN = (0),
	TP_AB_SPACE_INDEX_ICON = (0),
	TP_AB_SPACE_INDEX_BUTTON,
	TP_AB_SPACE_INDEX_MAX
}TP_AB_SPACE_INDEX;

/* 图片索引，至少分为正常时和被电击时两张图片 */
typedef enum 
{
	TP_AB_BM_INDEX_MIN = (0),
	TP_AB_BM_INDEX_NORMAL = (0),
	TP_AB_BM_INDEX_CLICKED,
	TP_AB_BM_INDEX_MAX,
}TP_AB_BM_INDEX;

/* 左边app icon区域各个元素的索引 */
typedef enum 
{
	TP_AB_ICON_INDEX_MIN	=	(0),
	TP_AB_ICON_INDEX_UP		=	(0),
	TP_AB_ICON_INDEX_HOME,
	TP_AB_ICON_INDEX_TITLE,
	TP_AB_ICON_INDEX_MAX
}TP_AB_ICON_INDEX;

/* 用于标记左边app icon区域中哪些元素存在 */
#define TP_AB_ICON_FLAG_UP			(1 << 0)
#define TP_AB_ICON_FLAG_HOME		(1 << 1)
#define TP_AB_ICON_FLAG_TITLE		(1 << 2)

/* 文本的最长长度 */
#define TP_AB_TEXT_MAX_LEN			(32)

#define TP_AB_RECT_CHANGE_FLAG		(1 << 0)/* 标记这个button的位置是否变化 */
#define TP_AB_MENU_ITEM_FLAG		(1 << 1)/* 标记这个button是否变为下拉菜单 */
#define TP_AB_MORE_BUTTON_FLAG		(1 << 2)

typedef struct 
{
	int id;
	CHAR text[TP_AB_TEXT_MAX_LEN];/* button要显示的文本 */
	const GUI_BITMAP* bitmap[TP_AB_BM_INDEX_MAX];/* button正常和被点击时的图片 */
}TP_AB_BUTTON;

typedef struct _TP_AB_BUTTON_ITEM 
{
	U8 flag;
	TP_AB_BUTTON button;
	GUI_RECT rect;
	WM_HMEM handle;
	struct _TP_AB_BUTTON_ITEM *next;
	struct _TP_AB_BUTTON_ITEM *prev;
}TP_AB_BUTTON_ITEM;

typedef struct {
	WM_HMEM handle;
	WM_HMEM menuHandle;
	GUI_RECT rect;
	const GUI_BITMAP *pBitMap[TP_AB_BM_INDEX_MAX];
}TP_AB_MENU_ITEM;

typedef struct 
{
	int itemNum;
	//int maxItemNum;
	GUI_RECT currentRect;
	TP_AB_BUTTON_ITEM *pItem;
	TP_AB_MENU_ITEM menu;
}TP_AB_BUTTON_LIST;

typedef struct 
{
	int id;
	const GUI_BITMAP *bitmap[TP_AB_BM_INDEX_MAX]; /* 回退按钮点击和正常时的图片*/
}TP_AB_ICON;

typedef struct 
{
	int id;
	CHAR title[TP_AB_TEXT_MAX_LEN];
}TP_AB_TITLE;

typedef struct 
{
	int id;
	int len;		/*up、home和title的长度*/
	WM_HMEM handle;	/* 句柄，暂时需要，后面可以考虑不要 */
	const GUI_BITMAP *bitmap[TP_AB_BM_INDEX_MAX];/* up或者home的图片*/
	CHAR *title;/* Title的文本 */
}TP_AB_ICON_TYPE;

/* 
Action Bar结构体
这里左边的区域统称为app icon区域，有up、home logo和title三部分，安卓里面
分别叫做HomeAsUp、home和title。
这里不是先View Control，因为我们暂没有这种需求
右边的按钮是button list，在安卓中称为button flow。
*/
typedef struct 
{
	int id;		/* Action Bar的ID */
	GUI_COLOR textColor;
	const GUI_FONT *pFont;/*默认字体*/
	int height;	/* 整个ActionBar的高度*/
	GUI_COLOR bkColor[TP_AB_BK_COLOR_INDEX_MAX];/* 整个ActionBar的背景颜色和点击时的颜色*/
	const GUI_BITMAP *bkBitmap; /* 整个ActionBar的背景图片*/
	int padding[TP_AB_PAD_INDEX_MAX];/* ActionBar上下左右的pad大小 */
	int iconFlag;/* 标记up、home和title哪些是有的 */
	int space[TP_AB_SPACE_INDEX_MAX];/* 左边Icon之间的距离、右边button之间的距离 */
	TP_AB_BUTTON_LIST buttonList;	/* 整个button列表 */
	int NumExtraBytes;
	TP_AB_ICON_TYPE appIcon[TP_AB_ICON_INDEX_MAX];
}TP_ACTIONBAR_OBJ;

/*********************************************************************
*
*       actionbar message data
*/
typedef struct 
{
	U16 MsgType;
	U16 id;
} TP_ACTIONBAR_MSG;

#define TP_AB_BUTTON_ON_PRESSED		(0)
#define TP_AB_BUTTON_ON_RELEASED	(1)

GUI_COLOR TP_ActionBar_GetDefaultTitleColor();
void TP_ActionBar_SetDefaultTitleColor(GUI_COLOR color);
const GUI_FONT* TP_ActionBar_GetDefaultFont();
void TP_ActionBar_SetDefaultFont(const GUI_FONT *pFont);
int TP_ActionBar_GetDefaultHeight();
void TP_ActionBar_SetDefaultHeight(int height);
GUI_COLOR TP_ActionBar_GetDefaultBkColor(int index);
void TP_ActionBar_SetDefaultBkColor(int index, GUI_COLOR color);
const GUI_BITMAP* TP_ActionBar_GetDefaultBkBitmap();
void TP_ActionBar_SetDefaultBkBitmap(const GUI_BITMAP* pBitmap);
int TP_ActionBar_GetDefaultPad(int index);
void TP_ActionBar_SetDefaultPad(int index, int pad);
int TP_ActionBar_GetDefaultSpace(int index);
void TP_ActionBar_SetDefaultSpace(int index, int space);
void TP_ActionBar_SetDefaultMenuBitmap(int index, const GUI_BITMAP *pBitmap);
const GUI_BITMAP * TP_ActionBar_GetDefaultMenuBitmap(int index);

GUI_COLOR TP_ActionBar_GetTitleColor(TP_ACTIONBAR_HANDLE hObj);
void TP_ActionBar_SetTitleColor(TP_ACTIONBAR_HANDLE hObj, GUI_COLOR color);
const GUI_FONT* TP_ActionBar_GetFont(TP_ACTIONBAR_HANDLE hObj);
void TP_ActionBar_SetFont(TP_ACTIONBAR_HANDLE hObj, const GUI_FONT *pFont);
int TP_ActionBar_GetHeight(TP_ACTIONBAR_HANDLE hObj);
void TP_ActionBar_SetHeight(TP_ACTIONBAR_HANDLE hObj, int height);
GUI_COLOR TP_ActionBar_GetBkColor(TP_ACTIONBAR_HANDLE hObj, int index);
void TP_ActionBar_SetBkColor(TP_ACTIONBAR_HANDLE hObj, int index, GUI_COLOR color);
const GUI_BITMAP* TP_ActionBar_GetBkBitmap(TP_ACTIONBAR_HANDLE hObj);
void TP_ActionBar_SetBkBitmap(TP_ACTIONBAR_HANDLE hObj, const GUI_BITMAP* pBitmap);
int TP_ActionBar_GetPad(TP_ACTIONBAR_HANDLE hObj, int index);
void TP_ActionBar_SetPad(TP_ACTIONBAR_HANDLE hObj, int index, int pad);
int TP_ActionBar_GetSpace(TP_ACTIONBAR_HANDLE hObj, int index);
void TP_ActionBar_SetSpace(TP_ACTIONBAR_HANDLE hObj, int index, int space);

void TP_ActionBar_SetMenuBitmap(TP_ACTIONBAR_HANDLE hObj, int index, const GUI_BITMAP *pBitmap);
const GUI_BITMAP * TP_ActionBar_GetMenuBitmap(TP_ACTIONBAR_HANDLE hObj, int index);



int TP_ActionBar_AddButton(TP_ACTIONBAR_HANDLE hObj, TP_AB_BUTTON button);

/* 还没有完全实现 */
int TP_ActionBar_InsertButton(TP_ACTIONBAR_HANDLE	hObj, TP_AB_BUTTON button, int id);

/* 还没有完全实现 */
int TP_ActionBar_DeleteButton(TP_ACTIONBAR_HANDLE hObj, int id);

int TP_ActionBar_SetUpIcon(TP_ACTIONBAR_HANDLE hObj, TP_AB_ICON upIcon);

int TP_ActionBar_SetHomeIcon(TP_ACTIONBAR_HANDLE hObj, TP_AB_ICON homeIcon);

int TP_ActionBar_SetTitle(TP_ACTIONBAR_HANDLE hObj, TP_AB_TITLE title);

int TP_ActionBar_GetId(TP_ACTIONBAR_HANDLE hObj);

TP_MENU_Handle TP_ActionBar_Create(int height, WM_HWIN hParent, U32 winFlags, 
	int id, WM_CALLBACK *pfCallback, int NumExtraBytes);
#endif