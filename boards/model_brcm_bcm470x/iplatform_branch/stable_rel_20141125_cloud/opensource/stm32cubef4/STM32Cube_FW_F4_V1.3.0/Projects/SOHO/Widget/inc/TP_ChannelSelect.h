/*TP_ChannelSelect.h*/

#ifndef __TP_CHANNELSELECT_H__
#define __TP_CHANNELSELECT_H__

#include "GUI.h"
#include "WM.h"
/*********************************************************************
*
*  ChannelSelect pics indices
*/
#define TP_CHANNELSELECT_BK_COLOR              0
#define TP_CHANNELSELECT_TEXT_OFF_COLOR             1
#define TP_CHANNELSELECT_TEXT_ON_COLOR              2
#define TP_CHANNELSELECT_COLOR_MAX                  3

#define TP_CHANNELSELECT_STATE_FIRST		        1
#define TP_CHANNELSELECT_STATE_SECOND		        2

#define TP_CHANNELSELECT_CHANNEL_FIRST              1
#define TP_CHANNELSELECT_CHANNEL_SECOND             2

typedef int	TP_CHANNELSELECT_STATE;
typedef U8 TP_CHANNELSELECT_PRESSED;


typedef WM_HMEM TP_CHANNELSELECT_Handle;


typedef struct {
	
	int id;
	
	const GUI_BITMAP * pBitmap;

    GUI_COLOR colorIndex[TP_CHANNELSELECT_COLOR_MAX];

    U8 aaFactor;

    const GUI_FONT * pFont;

    char * pFirstText;

    char * pSecondText;
	
	TP_CHANNELSELECT_STATE state;	/*当前被选中的频道*/
		
	U8 pressed;				/*当前滑动按钮是否被按下*/

    U8 focussable;

    int numExtraBytes;
	
}TP_CHANNELSELECT_Obj;
const GUI_FONT* TP_ChannelSelect_GetDefaultFont(void);
const GUI_FONT* TP_ChannelSelect_SetDefaultFont(const GUI_FONT* pFont);
GUI_COLOR TP_ChannelSelect_GetDefaultColorIndex(int index);
void      TP_ChannelSelect_SetDefaultColorIndex(GUI_COLOR color, int index);
int	      TP_ChannelSelect_GetDefaultAaFactor(void);
void	  TP_ChannelSelect_SetDefaultAaFactor(int factor);
int	      TP_ChannelSelect_GetDefaultNumExtraBytes(void);
void	  TP_ChannelSelect_SetDefaultNumExtraBytes(int bytes);
int       TP_ChannelSelect_GetId(TP_CHANNELSELECT_Handle hObj);
void      TP_ChannelSelect_SetId(TP_CHANNELSELECT_Handle hObj,int id);
GUI_COLOR TP_ChannelSelect_GetColorIndex(TP_CHANNELSELECT_Handle hObj, int index);
void      TP_ChannelSelect_SetColorIndex(TP_CHANNELSELECT_Handle hObj, int index, GUI_COLOR color);
U8        TP_ChannelSelect_GetAaFactor(TP_CHANNELSELECT_Handle hObj);
void      TP_ChannelSelect_SetAaFactor(TP_CHANNELSELECT_Handle hObj, U8 factor);
const GUI_FONT *  TP_ChannelSelect_GetFont(TP_CHANNELSELECT_Handle hObj, const GUI_FONT * pFont);
const GUI_FONT *  TP_ChannelSelect_SetFont(TP_CHANNELSELECT_Handle hObj, const GUI_FONT * pFont);
int	      TP_ChannelSelect_GetText(TP_CHANNELSELECT_Handle hObj, int index, char * pBuffer, unsigned int maxLen);
int       TP_ChannelSelect_SetText(TP_CHANNELSELECT_Handle hObj, int index, const char* pText);
U8        TP_ChannelSelect_GetState(TP_CHANNELSELECT_Handle hObj);
void      TP_ChannelSelect_SetState(TP_CHANNELSELECT_Handle hObj, U8 state);
int       TP_ChannelSelect_GetUserData(TP_CHANNELSELECT_Handle hWin, void * pDest, int sizeOfBuffer);
int       TP_ChannelSelect_SetUserData(TP_CHANNELSELECT_Handle hWin, void * pSrc, int sizeOfBuffer);
void TP_ChannelSelect_Callback(WM_MESSAGE * pMsg);
TP_CHANNELSELECT_Handle TP_ChannelSelect_Create(int x0, int y0, int xSize, int ySize,   \
								                WM_HWIN hWinParent, U32 style,   \
								                WM_CALLBACK * pfCallback, int numExtraBytes, int id);

#endif/* __TP_CHANNELSELECT_H__ */