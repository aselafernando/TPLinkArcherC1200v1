/*TP_Switch.h*/

#ifndef __TP_SWITCH_H__
#define __TP_SWITCH_H__

#include "GUI.h"
#include "WM.h"




/*********************************************************************
*
*  Switch pics indices
*/
#define TP_SWITCH_PICS_OFF					0
#define TP_SWITCH_PICS_OFF_DIS			    1
#define TP_SWITCH_PICS_ON					2
#define TP_SWITCH_PICS_ON_DIS			    3
#define TP_SWITCH_PICS_MAX					4

#define TP_SWITCH_STATE_OFF		            0
#define TP_SWITCH_STATE_ON		            1

#define WM_GET_PARENT_BKCOLOR (WM_USER + 0)

typedef int	TP_SWITCH_STATE;
typedef U8 TP_SWITCH_PRESSED;


typedef WM_HMEM TP_SWITCH_Handle;


typedef struct {
	
	int id;
	
	const GUI_BITMAP * pBitmap[TP_SWITCH_PICS_MAX];
	
	TP_SWITCH_STATE state;	/*当前滑动按钮的开关状态*/
		
	U8 pressed;				/*当前滑动按钮是否被按下*/

    U8 focussable;

    int numExtraBytes;
	
}TP_SWITCH_Obj;

U8	    TP_Switch_GetDefaultState(void);
void	TP_Switch_SetDefaultState(U8 state);
U8	    TP_Switch_GetDefaultFocussable(void);
void	TP_Switch_SetDefaultFocussable(U8 focussable);

int  TP_Switch_GetId(TP_SWITCH_Handle hObj);
void TP_Switch_SetId(TP_SWITCH_Handle hObj,int id);

const GUI_BITMAP* TP_Switch_GetBitmap(TP_SWITCH_Handle hObj,    int index);
void              TP_Switch_SetBitmapEx(TP_SWITCH_Handle hObj,  int index, const GUI_BITMAP *pBitmap, int x, int y);
void              TP_Switch_SetBitmap(TP_SWITCH_Handle hObj,    int index, const GUI_BITMAP *pBitmap);

TP_SWITCH_STATE TP_Switch_GetState(TP_SWITCH_Handle hObj);
void            TP_Switch_SetState (TP_SWITCH_Handle hObj, TP_SWITCH_STATE state);

TP_SWITCH_PRESSED TP_Switch_GetPressed(TP_SWITCH_Handle hObj);
void              TP_Switch_SetPressed (TP_SWITCH_Handle hObj, U8 pressed);

int  TP_Switch_GetFocussable(TP_SWITCH_Handle hObj);
void TP_Switch_SetFocussable(TP_SWITCH_Handle hObj, int state);

int TP_Switch_GetUserData(TP_SWITCH_Handle hWin, void * pDest, int sizeOfBuffer);
int TP_Switch_SetUserData(TP_SWITCH_Handle hWin, void * pSrc, int sizeOfBuffer);

void TP_SWITCH_Callback(WM_MESSAGE * pMsg);

TP_SWITCH_Handle TP_Switch_Create(int x0, int y0, int xSize, int ySize,
									WM_HWIN hWinParent, U32 style, 
									WM_CALLBACK * pfCallback, int numExtraBytes, int id);


#endif/* __TP_SWITCH_H__ */