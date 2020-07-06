/*TP_Indicator.h*/

#ifndef __TP_INDICATOR_H__
#define __TP_INDICATOR_H__

#include "GUI.h"
#include "WM.h"




/*********************************************************************
*
*  Indicator pics indices
*/
#define TP_INDICATOR_HIGHLIGHT					0
#define TP_INDICATOR_DARK  		                1
#define TP_INDICATOR_BKGROUND					2
#define TP_INDICATOR_MAX					    3

typedef WM_HMEM TP_INDICATOR_Handle;


typedef struct {
	
	int id;
	
	const GUI_BITMAP * pBitmap[TP_INDICATOR_MAX];/*选中前景、未选中前景、背景图片*/

    U8 indicatorAlpha[TP_INDICATOR_MAX];         /*选中前景、未选中前景、背景透明系数*/

    GUI_COLOR colorIndex[TP_INDICATOR_MAX];      /*选中前景、未选中前景、背景颜色*/

    U8 aaFactor;        /*抗锯齿因子*/
	
	U8 page;	        /*当前页面号*/

    U8 num;             /*页面个数*/

    U8 size;            /*指示器圆直径大小*/
        
    U8 space;	        /*指示器圆间隔*/

    int numExtraBytes;
	
}TP_INDICATOR_Obj;

GUI_COLOR TP_Indicator_GetDefaultColorIndex(int index);
void      TP_Indicator_SetDefaultColorIndex(GUI_COLOR color, int index);
U8   TP_Indicator_GetDefaultIndicatorAlpha(int index);
void TP_Indicator_SetDefaultIndicatorAlpha(U8 alpha, int index);
U8	TP_Indicator_GetDefaultAaFactor(void);
void	TP_Indicator_SetDefaultAaFactor(U8 factor);
U8	TP_Indicator_GetDefaultPage(void);
void	TP_Indicator_SetDefaultPage(U8 page);
U8	TP_Indicator_GetDefaultNum(void);
void	TP_Indicator_SetDefaultNum(U8 num);
U8	TP_Indicator_GetDefaultSize(void);
void	TP_Indicator_SetDefaultSize(U8 size);
U8	TP_Indicator_GetDefaultSpace(void);
void	TP_Indicator_SetDefaultSpace(U8 space);

int  TP_Indicator_GetId(TP_INDICATOR_Handle hObj);
void TP_Indicator_SetId(TP_INDICATOR_Handle hObj, int id);

/*设置绘制indicator使用的图片*/
const GUI_BITMAP* TP_Indicator_GetBitmap(TP_INDICATOR_Handle hObj, int index);
void TP_Indicator_SetBitmapEx           (TP_INDICATOR_Handle hObj,  int index,  \
                                                const GUI_BITMAP *pBitmap, int x, int y) ;
void TP_Indicator_SetBitmap             (TP_INDICATOR_Handle hObj,  int index,  \
                                                const GUI_BITMAP *pBitmap) ;
/*获得和设置绘制indicator使用的颜色*/
GUI_COLOR TP_Indicator_GetColorIndex(TP_INDICATOR_Handle hObj,  int index);
void      TP_Indicator_SetColorIndex(TP_INDICATOR_Handle hObj,  int index, GUI_COLOR color);
/*获得和设置绘制indicator使用的抗锯齿因子*/
U8   TP_Indicator_GetAaFactor(TP_INDICATOR_Handle hObj);
void TP_Indicator_SetAaFactor(TP_INDICATOR_Handle hObj, U8 factor);
/*获得和设置绘制indicator使用的透明系数*/
U8   TP_Indicator_GetIndicatorAlpha (TP_INDICATOR_Handle hObj, int index);
void TP_Indicator_SetIndicatorAlpha (TP_INDICATOR_Handle hObj, int index, U8 Alpha);
/*获得和设置indicator当前指示的页面，从1开始为第一页*/
U8   TP_Indicator_GetPage (TP_INDICATOR_Handle hObj);
void TP_Indicator_SetPage (TP_INDICATOR_Handle hObj, U8 page);
/*获得和设置indicator的页面个数*/
U8   TP_Indicator_GetNum  (TP_INDICATOR_Handle hObj);
void TP_Indicator_SetNum  (TP_INDICATOR_Handle hObj, U8 num);
/*获得和设置绘制indicator使用的圆的直径*/
U8   TP_Indicator_GetSize (TP_INDICATOR_Handle hObj);
void TP_Indicator_SetSize (TP_INDICATOR_Handle hObj, U8 size);
/*获得和设置绘制indicator圆的间距（两圆最边缘距离）*/
U8   TP_Indicator_GetSpace(TP_INDICATOR_Handle hObj);
void TP_Indicator_SetSpace(TP_INDICATOR_Handle hObj, U8 space);
int TP_Indicator_GetUserData(TP_INDICATOR_Handle hWin, void * pDest, int sizeOfBuffer);
int TP_Indicator_SetUserData(TP_INDICATOR_Handle hWin, void * pSrc, int sizeOfBuffer);
/*indicatorm默认使用的回调函数*/
void TP_Indicator_Callback(WM_MESSAGE * pMsg);


TP_INDICATOR_Handle TP_Indicator_Create(int x0, int y0, int xSize, int ySize,   \
									WM_HWIN hWinParent, U32 style,              \
									WM_CALLBACK * pfCallback, int numExtraBytes, int id);


#endif/* __TP_INDICATOR_H__ */