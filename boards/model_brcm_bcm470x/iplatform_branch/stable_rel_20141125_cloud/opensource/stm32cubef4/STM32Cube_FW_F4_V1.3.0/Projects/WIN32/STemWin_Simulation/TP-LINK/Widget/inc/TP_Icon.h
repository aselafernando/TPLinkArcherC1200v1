#ifndef __TP_ICON_H__
#define __TP_ICON_H__

#include <string.h>
#include <stdlib.h>
#include "GUI.h"
#include "WM.h"

#ifndef STATUS
	#define STATUS int
#endif

typedef WM_HMEM TP_ICON_Handle;


/* 定义icon的三种状态：按下，未按下，禁用 */
#define TP_ICON_UNPRESSED   0
#define TP_ICON_PRESSED     1
#define TP_ICON_DISABLED    2
#define TP_ICON_STATE_NUM_MAX   3


/* BitMap alignment flags, horizontal */
#define TP_ICON_BMP_ALIGN_LEFT      (1 << 0)
#define TP_ICON_BMP_ALIGN_RIGHT     (1 << 1)
#define TP_ICON_BMP_ALIGN_HCENTER   (3 << 0)


/* BitMap alignment flags, vertical */
#define TP_ICON_BMP_ALIGN_TOP       (1 << 2)
#define TP_ICON_BMP_ALIGN_BOTTOM    (1 << 3)
#define TP_ICON_BMP_ALIGN_VCENTER   (3 << 2)



/*********************************************************************
*
* TP_ICON_Obj
*/
typedef struct 
{
	int id;                                                 /* 控件id */
    int pressed;                                            /* 判断是否按下 */
    int focussable;	                                        /* 是否接收输入焦点 */
    GUI_COLOR iconBkColor[TP_ICON_STATE_NUM_MAX];           /* 三种状态下icon的背景颜色 */
    const GUI_BITMAP *pIconBitmap[TP_ICON_STATE_NUM_MAX];   /* 三种状态下icon对应的位图 */
    int iconBitmapAlign;                                    /* IconBitmap的对齐方式 */

    GUI_COLOR aTextColor[TP_ICON_STATE_NUM_MAX];            /* 三种状态下的Text颜色 */
    CHAR *pText;                                            /* icon的文本 */
    const GUI_FONT	*pFont;		                            /*文本的字体*/
    int textAlign;                                          /* 文本的对齐方式 */
    int space;		                                        /* 位图与文本之间的距离 */
    int lineSpace;                                          /* 文本的行距 */
    
    int numExtraBytes;			                            /* 额外的存储空间数量 */
} TP_ICON_Obj;


/*********************************************************************
*
* get/set default attribute value
*/
int TP_ICON_GetDefaultFocussable();
void TP_ICON_SetDefaultFocussable(int state);

GUI_COLOR TP_ICON_GetDefaultBkColor(int index);
void TP_ICON_SetDefaultBkColor( int index, GUI_COLOR color);

const GUI_BITMAP * TP_ICON_GetDefaultIconBitmap(int index);
void TP_ICON_SetDefaultIconBitmap(int index, const GUI_BITMAP *pBitmap);

int TP_ICON_GetDefaultiIconBitmapAlign();
void TP_ICON_SetDefaultIconBitmapAlign(int bitmapAlign); 

GUI_COLOR TP_ICON_GetDefaultTextColor(int index);
void TP_ICON_SetDefaultTextColor( int index, GUI_COLOR color);
    
const GUI_FONT * TP_ICON_GetDefaultFont();
void TP_ICON_SetDefaultFont(const GUI_FONT* pFont);

int TP_ICON_GetDefaultTextAlign();
void TP_ICON_SetDefaultTextAlign(int textAlign);

int TP_ICON_GetDefaultSpace();
void TP_ICON_SetDefaultSpace(int space);

int TP_ICON_GetDefaultLineSpace();
void TP_ICON_SetDefaultLineSpace(int lineSpace);






/*********************************************************************
*
* get/set  attribute value
*/
int TP_ICON_GetId(TP_ICON_Handle hObj);

int TP_ICON_GetPressedState(TP_ICON_Handle hObj);
void TP_ICON_SetPressedState(TP_ICON_Handle hObj, int state);

int TP_ICON_GetFocussable(TP_ICON_Handle hObj);
void TP_ICON_SetFocussable(TP_ICON_Handle hObj, int state);

GUI_COLOR TP_ICON_GetBkColor(TP_ICON_Handle hObj, int index);
void  TP_ICON_SetBkColor(TP_ICON_Handle hObj, int index, GUI_COLOR color);

const GUI_BITMAP * TP_ICON_GetIconBitmap(TP_ICON_Handle hObj, int index);
void TP_ICON_SetIconBitmap(TP_ICON_Handle hObj, int index, const GUI_BITMAP *pBitmap);

int TP_ICON_GetIconBitmapAlign(TP_ICON_Handle hObj);
void TP_ICON_SetIconBitmapAlign(TP_ICON_Handle hObj, int bitmapAlign);

GUI_COLOR TP_ICON_GetTextColor(TP_ICON_Handle hObj, int index);
void TP_ICON_SetTextColor(TP_ICON_Handle hObj, int index, GUI_COLOR color);

STATUS TP_ICON_GetText(TP_ICON_Handle hObj, char * pBuffer, unsigned int maxLen);
void TP_ICON_SetText(TP_ICON_Handle hObj, const char * pText);

const GUI_FONT * TP_ICON_GetTextFont(TP_ICON_Handle hObj);
void TP_ICON_SetTextFont(TP_ICON_Handle hObj, const GUI_FONT * pFont);

int TP_ICON_GetTextAlign(TP_ICON_Handle hObj);
void TP_ICON_SetTextAlign(TP_ICON_Handle hObj, int textAlign);

int TP_ICON_GetSpace(TP_ICON_Handle hObj);
void TP_ICON_SetSpace(TP_ICON_Handle hObj, int space);

int TP_ICON_GetLineSpace(TP_ICON_Handle hObj);
void TP_ICON_SetLineSpace(TP_ICON_Handle hObj, int lineSpace);

int TP_ICON_GetUserData(TP_ICON_Handle hWin, void * pDest, int sizeOfBuffer);
int TP_ICON_SetUserData(TP_ICON_Handle hWin, void * pSrc, int sizeOfBuffer);

void TP_ICON_Callback(WM_MESSAGE * pMsg);
TP_ICON_Handle TP_ICON_Create(int x0, int y0, int xSize, int ySize,
								WM_HWIN hWinParent, U32 style, const char *pText,
								WM_CALLBACK * pfCallback, int numExtraBytes, int id);



#endif
