#ifndef TP_ACTIONBAR_H
#define TP_ACTIONBAR_H

#include "GUI.h"
#include "WM.h"

typedef WM_HMEM TP_ACTIONBAR_HANDLE;

/* Action Bar��������padding������ */
typedef enum 
{
	TP_AB_PAD_INDEX_MIN = (0),
	TP_AB_PAD_INDEX_LEFT = (0),
	TP_AB_PAD_INDEX_RIGHT,
	TP_AB_PAD_INDEX_TOP,
	TP_AB_PAD_INDEX_BOTTOM,	
	TP_AB_PAD_INDEX_MAX
}TP_AB_PAD_INDEX;

/* Action Bar������ɫ����Ϊ�����ͱ����ʱ����� */
typedef enum 
{
	TP_AB_BK_COLOR_INDEX_MIN = (0),
	TP_AB_BK_COLOR_INDEX_NORMAL = (0),
	TP_AB_BK_COLOR_INDEX_CLICKED,
	TP_AB_BK_COLOR_INDEX_MAX,
}TP_AB_BK_COLOR_INDEX;

/* ���app icon������ұ�button����������Ԫ�صļ������ */
typedef enum
{
	TP_AB_SPACE_INDEX_MIN = (0),
	TP_AB_SPACE_INDEX_ICON = (0),
	TP_AB_SPACE_INDEX_BUTTON,
	TP_AB_SPACE_INDEX_MAX
}TP_AB_SPACE_INDEX;

/* ͼƬ���������ٷ�Ϊ����ʱ�ͱ����ʱ����ͼƬ */
typedef enum 
{
	TP_AB_BM_INDEX_MIN = (0),
	TP_AB_BM_INDEX_NORMAL = (0),
	TP_AB_BM_INDEX_CLICKED,
	TP_AB_BM_INDEX_MAX,
}TP_AB_BM_INDEX;

/* ���app icon�������Ԫ�ص����� */
typedef enum 
{
	TP_AB_ICON_INDEX_MIN	=	(0),
	TP_AB_ICON_INDEX_UP		=	(0),
	TP_AB_ICON_INDEX_HOME,
	TP_AB_ICON_INDEX_TITLE,
	TP_AB_ICON_INDEX_MAX
}TP_AB_ICON_INDEX;

/* ���ڱ�����app icon��������ЩԪ�ش��� */
#define TP_AB_ICON_FLAG_UP			(1 << 0)
#define TP_AB_ICON_FLAG_HOME		(1 << 1)
#define TP_AB_ICON_FLAG_TITLE		(1 << 2)

/* �ı�������� */
#define TP_AB_TEXT_MAX_LEN			(32)

#define TP_AB_RECT_CHANGE_FLAG		(1 << 0)/* ������button��λ���Ƿ�仯 */
#define TP_AB_MENU_ITEM_FLAG		(1 << 1)/* ������button�Ƿ��Ϊ�����˵� */
#define TP_AB_MORE_BUTTON_FLAG		(1 << 2)

typedef struct 
{
	int id;
	CHAR text[TP_AB_TEXT_MAX_LEN];/* buttonҪ��ʾ���ı� */
	const GUI_BITMAP* bitmap[TP_AB_BM_INDEX_MAX];/* button�����ͱ����ʱ��ͼƬ */
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
	const GUI_BITMAP *bitmap[TP_AB_BM_INDEX_MAX]; /* ���˰�ť���������ʱ��ͼƬ*/
}TP_AB_ICON;

typedef struct 
{
	int id;
	CHAR title[TP_AB_TEXT_MAX_LEN];
}TP_AB_TITLE;

typedef struct 
{
	int id;
	int len;		/*up��home��title�ĳ���*/
	WM_HMEM handle;	/* �������ʱ��Ҫ��������Կ��ǲ�Ҫ */
	const GUI_BITMAP *bitmap[TP_AB_BM_INDEX_MAX];/* up����home��ͼƬ*/
	CHAR *title;/* Title���ı� */
}TP_AB_ICON_TYPE;

/* 
Action Bar�ṹ��
������ߵ�����ͳ��Ϊapp icon������up��home logo��title�����֣���׿����
�ֱ����HomeAsUp��home��title��
���ﲻ����View Control����Ϊ������û����������
�ұߵİ�ť��button list���ڰ�׿�г�Ϊbutton flow��
*/
typedef struct 
{
	int id;		/* Action Bar��ID */
	GUI_COLOR textColor;
	const GUI_FONT *pFont;/*Ĭ������*/
	int height;	/* ����ActionBar�ĸ߶�*/
	GUI_COLOR bkColor[TP_AB_BK_COLOR_INDEX_MAX];/* ����ActionBar�ı�����ɫ�͵��ʱ����ɫ*/
	const GUI_BITMAP *bkBitmap; /* ����ActionBar�ı���ͼƬ*/
	int padding[TP_AB_PAD_INDEX_MAX];/* ActionBar�������ҵ�pad��С */
	int iconFlag;/* ���up��home��title��Щ���е� */
	int space[TP_AB_SPACE_INDEX_MAX];/* ���Icon֮��ľ��롢�ұ�button֮��ľ��� */
	TP_AB_BUTTON_LIST buttonList;	/* ����button�б� */
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

/* ��û����ȫʵ�� */
int TP_ActionBar_InsertButton(TP_ACTIONBAR_HANDLE	hObj, TP_AB_BUTTON button, int id);

/* ��û����ȫʵ�� */
int TP_ActionBar_DeleteButton(TP_ACTIONBAR_HANDLE hObj, int id);

int TP_ActionBar_SetUpIcon(TP_ACTIONBAR_HANDLE hObj, TP_AB_ICON upIcon);

int TP_ActionBar_SetHomeIcon(TP_ACTIONBAR_HANDLE hObj, TP_AB_ICON homeIcon);

int TP_ActionBar_SetTitle(TP_ACTIONBAR_HANDLE hObj, TP_AB_TITLE title);

int TP_ActionBar_GetId(TP_ACTIONBAR_HANDLE hObj);

TP_MENU_Handle TP_ActionBar_Create(int height, WM_HWIN hParent, U32 winFlags, 
	int id, WM_CALLBACK *pfCallback, int NumExtraBytes);
#endif