/*TP_Slider.h*/

#ifndef __TP_SLIDER_H__
#define __TP_SLIDER_H__

#include "GUI.h"
#include "WM.h"
/*********************************************************************
*
*  ChannelSelect pics indices
*/
#define TP_SLIDER_SLOT_COLOR              0
#define TP_SLIDER_FILL_COLOR              1   
#define TP_SLIDER_CIRCLE_COLOR            2
#define TP_SLIDER_COLOR_MAX               3 

#define TP_SLIDER_STATE_FIRST		      1
#define TP_SLIDER_STATE_SECOND		      2

#define TP_SLIDER_CHANNEL_FIRST           1
#define TP_SLIDER_CHANNEL_SECOND          2

typedef int	TP_SLIDER_STATE;
typedef U8 TP_SLIDER_PRESSED;


typedef WM_HMEM TP_SLIDER_Handle;


typedef struct {
	
	int id;

    U8 radius;              /*滑动圆球的半径*/

    U8 width;               /*滑动槽的宽度（高度）*/

	int length;             /*滑动槽的长度*/

	const GUI_BITMAP * pBitmap;  /*以图片代替手动绘制的滑动圆球*/

    GUI_COLOR colorIndex[TP_SLIDER_COLOR_MAX];  

    U8 flagStateOn;          /*回调函数内部进行辅助消息判定*/

    U8 zone;                /*设置点击的有效范围（与当前value值比较， 单位像素）*/

    U8 aaFactor;
	
	U8 value;	            /*当前被选中的频道*/
		
	U8 pressed;				/*当前滑动按钮是否被按下*/

    U8 focussable;

    int numExtraBytes;
	
}TP_SLIDER_Obj;
int		TP_Slider_GetDefaultRadius(void);
void	TP_Slider_SetDefaultRadius(int radius);
int		TP_Slider_GetDefaultWidth(void);
void	TP_Slider_SetDefaultWidth(int width);
int		TP_Slider_GetDefaultLength(void);
void	TP_Slider_SetDefaultLength(int length);
const GUI_BITMAP* TP_Slider_GetDefaultBitmap(void);
void 		      TP_Slider_SetDefaultBitmap(const GUI_BITMAP* pBitmap);
GUI_COLOR 	      TP_Slider_GetDefaultColorIndex(int index);
void 		      TP_Slider_SetDefaultColorIndex(GUI_COLOR color, int index);
int		TP_Slider_GetDefaultZone(void);
void	TP_Slider_SetDefaultZone(int zone);
int		TP_Slider_GetDefaultAaFactor(void);
void	TP_Slider_SetDefaultAaFactor(int factor);
int		TP_Slider_GetDefaultValue(void);
void	TP_Slider_SetDefaultValue(int value);
int		TP_Slider_GetDefaultPressed(void);
void	TP_Slider_SetDefaultPressed(int pressed);
int		TP_Slider_GetDefaultFocussable(void);
void	TP_Slider_SetDefaultFocussable(int focussable);
int		TP_Slider_GetDefaultNumExtraBytes(void);
void	TP_Slider_SetDefaultNumExtraBytes(int bytes);
int 	TP_Slider_GetId(TP_SLIDER_Handle hObj);
void 	TP_Slider_SetId(TP_SLIDER_Handle hObj,int id);
int 	TP_Slider_GetRadius(TP_SLIDER_Handle hObj);
void 	TP_Slider_SetRadius(TP_SLIDER_Handle hObj,int radius);
int 	TP_Slider_GetWidth(TP_SLIDER_Handle hObj);
void 	TP_Slider_SetWidth(TP_SLIDER_Handle hObj,int width);
int 	TP_Slider_GetLength(TP_SLIDER_Handle hObj);
void 	TP_Slider_SetLength(TP_SLIDER_Handle hObj,int length);
const GUI_BITMAP* TP_Slider_GetBitmap(TP_SLIDER_Handle hObj);
void 			  TP_Slider_SetBitmap(TP_SLIDER_Handle hObj,const GUI_BITMAP* pBitmap);
GUI_COLOR 		  TP_Slider_GetColorIndex(TP_SLIDER_Handle hObj, int index);
void 			  TP_Slider_SetColorIndex(TP_SLIDER_Handle hObj, int index, GUI_COLOR color);
int 	TP_Slider_GetZone(TP_SLIDER_Handle hObj);
void 	TP_Slider_SetZone(TP_SLIDER_Handle hObj,int zone);
U8 		TP_Slider_GetAaFactor(TP_SLIDER_Handle hObj);
void 	TP_Slider_SetAaFactor(TP_SLIDER_Handle hObj, U8 factor);
U8 		TP_Slider_GetValue(TP_SLIDER_Handle hObj);
void 	TP_Slider_SetValue(TP_SLIDER_Handle hObj, U8 value);
int 	TP_Slider_GetPressed(TP_SLIDER_Handle hObj);
void 	TP_Slider_SetPressed(TP_SLIDER_Handle hObj,int pressed);
int 	TP_Slider_GetFocussable(TP_SLIDER_Handle hObj);
void 	TP_Slider_SetFocussable(TP_SLIDER_Handle hObj,int focussable);
int 	TP_Slider_GetNumExtraBytes(TP_SLIDER_Handle hObj);
void 	TP_Slider_SetNumExtraBytes(TP_SLIDER_Handle hObj,int numExtraBytes );
int 	TP_Slider_GetUserData(TP_SLIDER_Handle hWin, void * pDest, int sizeOfBuffer);
int 	TP_Slider_SetUserData(TP_SLIDER_Handle hWin, void * pSrc, int sizeOfBuffer);
void 	TP_Slider_Callback(WM_MESSAGE * pMsg);

TP_SLIDER_Handle TP_Slider_Create(int x0, int y0, int xSize, int ySize,   \
								  WM_HWIN hWinParent, U32 style,   \
								  WM_CALLBACK * pfCallback, int numExtraBytes, int id);

#endif/* __TP_SLIDER_H__ */