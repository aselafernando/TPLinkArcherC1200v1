/*TP_ChannelSelect.c*/

#include <string.h> // Definition of NULL
#include <stdlib.h> // malloc() and free()

#include "TP_Widget.h"
#include "TP_ChannelSelect.h"


#define TP_CHANNELSELECT_DEFAULT_ID	               0
#define TP_CHANNELSELECT_DEFAULT_STATE             2
#define TP_CHANNELSELECT_DEFAULT_FOCUSSABLE        0

#define TP_CHANNELSELECT_DEFAULT_AA_FACTOR         8

#define TP_CHANNELSELECT_DEFAULT_BK_COLOR          0x4fcb88
#define TP_CHANNELSELECT_DEFAULT_TEXT_OFF_COLOR    0x4fcb88 
#define TP_CHANNELSELECT_DEFAULT_TEXT_ON_COLOR     GUI_WHITE

#define TP_CHANNELSELECT_DEFAULT_TEXT_FONT         &GUI_Font24_ASCII
/*********************************************************************
*
*选择器的默认属性值
*/
static TP_CHANNELSELECT_Obj TP_ChannelSelect_Default = 
{
	TP_CHANNELSELECT_DEFAULT_ID,
			
	NULL,
	
    {
        TP_CHANNELSELECT_DEFAULT_BK_COLOR,

        TP_CHANNELSELECT_DEFAULT_TEXT_OFF_COLOR,//0xf1f8fa,

       TP_CHANNELSELECT_DEFAULT_TEXT_ON_COLOR

     },

     TP_CHANNELSELECT_DEFAULT_AA_FACTOR,

     TP_CHANNELSELECT_DEFAULT_TEXT_FONT,

     NULL, 

     NULL,

	TP_CHANNELSELECT_DEFAULT_STATE,	                     /*默认当被选中的频道*/
	
	0,				                                 /*默认当前未被按下*/

    TP_CHANNELSELECT_DEFAULT_FOCUSSABLE,                     /*默认不接受输入焦点*/

    0
	
};
const GUI_FONT* TP_ChannelSelect_GetDefaultFont(void)
{
	return TP_ChannelSelect_Default.pFont;
}

const GUI_FONT* TP_ChannelSelect_SetDefaultFont(const GUI_FONT* pFont)
{
	const GUI_FONT* pPreFont = TP_ChannelSelect_Default.pFont;

	TP_ChannelSelect_Default.pFont = (GUI_FONT*)pFont;
	return pPreFont;
}

GUI_COLOR TP_ChannelSelect_GetDefaultColorIndex(int index)
{
	if (index < TP_CHANNELSELECT_BK_COLOR
		|| index > TP_CHANNELSELECT_TEXT_ON_COLOR)
	{
		return RET_FAIL;
	}
	return TP_ChannelSelect_Default.colorIndex[index];
}

void TP_ChannelSelect_SetDefaultColorIndex(GUI_COLOR color, int index)
{
	if (index < TP_CHANNELSELECT_BK_COLOR
		|| index > TP_CHANNELSELECT_TEXT_ON_COLOR)
	{
		return ;
	}
	TP_ChannelSelect_Default.colorIndex[index] = color;
}
int	TP_ChannelSelect_GetDefaultAaFactor(void)
{
	return TP_ChannelSelect_Default.aaFactor;
}
void	TP_ChannelSelect_SetDefaultAaFactor(int factor)
{
	TP_ChannelSelect_Default.aaFactor = factor;
}
int	TP_ChannelSelect_GetDefaultNumExtraBytes(void)
{
	return TP_ChannelSelect_Default.numExtraBytes;
}
void	TP_ChannelSelect_SetDefaultNumExtraBytes(int bytes)
{
	TP_ChannelSelect_Default.numExtraBytes = bytes;
}
int TP_ChannelSelect_GetId(TP_CHANNELSELECT_Handle hObj)
{
	if (hObj) 
	{
		TP_CHANNELSELECT_Obj obj;
		TP_GUI_LOCK();
		WM_GetUserData(hObj, &obj, sizeof(TP_CHANNELSELECT_Obj));
		TP_GUI_UNLOCK();
		return obj.id;
	}
	return RET_FAIL;
}
void TP_ChannelSelect_SetId(TP_CHANNELSELECT_Handle hObj,int id)
{
	if (hObj) 
	{
		TP_CHANNELSELECT_Obj obj;
		TP_GUI_LOCK();
		WM_GetUserData(hObj, &obj, sizeof(TP_CHANNELSELECT_Obj));
		obj.id = id;
		WM_SetUserData(hObj, &obj, sizeof(TP_CHANNELSELECT_Obj));
		TP_GUI_UNLOCK();
	}
}
GUI_COLOR TP_ChannelSelect_GetColorIndex(TP_CHANNELSELECT_Handle hObj, int index)
{
    if (index < TP_CHANNELSELECT_BK_COLOR
		|| index > TP_CHANNELSELECT_TEXT_ON_COLOR)
	{
		return RET_FAIL;
	}
    if (hObj) 
	{
		TP_CHANNELSELECT_Obj obj;
		TP_GUI_LOCK();
		WM_GetUserData(hObj, &obj, sizeof(TP_CHANNELSELECT_Obj));
		TP_GUI_UNLOCK();
        return obj.colorIndex[index];
	}
    return RET_FAIL;
}
void TP_ChannelSelect_SetColorIndex(TP_CHANNELSELECT_Handle hObj, int index, GUI_COLOR color)
{
    if (index < TP_CHANNELSELECT_BK_COLOR
		|| index > TP_CHANNELSELECT_TEXT_ON_COLOR)
	{
		return ;
	}
    if (hObj) 
	{
		TP_CHANNELSELECT_Obj obj;
		TP_GUI_LOCK();
		WM_GetUserData(hObj, &obj, sizeof(TP_CHANNELSELECT_Obj));
        obj.colorIndex[index] = color;
		WM_SetUserData(hObj, &obj, sizeof(TP_CHANNELSELECT_Obj));
		TP_GUI_UNLOCK();
	}
}
U8 TP_ChannelSelect_GetAaFactor(TP_CHANNELSELECT_Handle hObj)
{
    if (hObj) 
	{
	    TP_CHANNELSELECT_Obj obj;
        TP_GUI_LOCK();
	    WM_GetUserData(hObj, &obj, sizeof(obj));
        TP_GUI_UNLOCK();
        return obj.aaFactor;
    }
    return RET_FAIL;
}
void TP_ChannelSelect_SetAaFactor(TP_CHANNELSELECT_Handle hObj, U8 factor)
{
    if (hObj) 
	{
	    TP_CHANNELSELECT_Obj obj;
        TP_GUI_LOCK();
	    WM_GetUserData(hObj, &obj, sizeof(obj));
	    obj.aaFactor = factor;
	    WM_SetUserData(hObj, &obj, sizeof(obj));
        TP_GUI_UNLOCK();
    }
}
const GUI_FONT *  TP_ChannelSelect_GetFont(TP_CHANNELSELECT_Handle hObj, const GUI_FONT * pFont)
{
	if (hObj) 
	{
	    TP_CHANNELSELECT_Obj obj;
        TP_GUI_LOCK();
	    WM_GetUserData(hObj, &obj, sizeof(obj));
        TP_GUI_UNLOCK();
        return obj.pFont;
    }
    return NULL;
}

const GUI_FONT *  TP_ChannelSelect_SetFont(TP_CHANNELSELECT_Handle hObj, const GUI_FONT * pFont)
{
    if (hObj) 
	{
	    TP_CHANNELSELECT_Obj obj;
	    const GUI_FONT* pPreFont = NULL;
        TP_GUI_LOCK();
	    WM_GetUserData(hObj, &obj, sizeof(obj));
	    pPreFont = obj.pFont;
	    obj.pFont = (GUI_FONT *)pFont;
	    WM_SetUserData(hObj, &obj, sizeof(obj));
        TP_GUI_UNLOCK();
	    return pPreFont;
    }
    return NULL;
}
int	TP_ChannelSelect_GetText(TP_CHANNELSELECT_Handle hObj, int index, char * pBuffer, unsigned int maxLen)
{
    TP_CHANNELSELECT_Obj obj;
    int len = 0;
    if ( index < TP_CHANNELSELECT_CHANNEL_FIRST || index > TP_CHANNELSELECT_CHANNEL_SECOND )
    {
         return RET_FAIL;      
    }
    if ((pBuffer == NULL) || (maxLen <= 0))
	{
		return RET_FAIL;
	}
	
    TP_GUI_LOCK();
	WM_GetUserData(hObj, &obj, sizeof(obj));
	
    if ( TP_CHANNELSELECT_CHANNEL_FIRST == index  && obj.pFirstText != NULL)
    {
	    if (strlen(obj.pFirstText) > maxLen)
	    {
		    len = maxLen;
	    }
	    else
	    {
		    len = strlen(obj.pFirstText);
	    }
        TP_MEMCPY(pBuffer, obj.pFirstText, len);
        TP_GUI_UNLOCK();
	    return RET_SUCCESS;
    }
    if ( TP_CHANNELSELECT_CHANNEL_SECOND == index  && obj.pSecondText != NULL)
    {
	    if (strlen(obj.pSecondText) > maxLen)
	    {
		    len = maxLen;
	    }
	    else
	    {
		    len = strlen(obj.pSecondText);
	    }
        TP_MEMCPY(pBuffer, obj.pSecondText, len);
        TP_GUI_UNLOCK();
	    return RET_SUCCESS;
    }
    return RET_FAIL;
	
}
int TP_ChannelSelect_SetText(TP_CHANNELSELECT_Handle hObj, int index, const char* pText)
{
    TP_CHANNELSELECT_Obj obj;

	int len = 0;
    if ( index < TP_CHANNELSELECT_CHANNEL_FIRST || index > TP_CHANNELSELECT_CHANNEL_SECOND )
    {
         return RET_FAIL;      
    }
	
	TP_GUI_LOCK();
	WM_GetUserData(hObj, &obj, sizeof(TP_CHANNELSELECT_Obj));
	if (pText == NULL)
	{
		return RET_FAIL;
	}
	len = strlen(pText) + 1;
	if ( TP_CHANNELSELECT_CHANNEL_FIRST == index )
    {
	    if (obj.pFirstText)
	    {
		    TP_FREE(obj.pFirstText);
	    }
	    obj.pFirstText = TP_MALLOC(len);
	    if (obj.pFirstText)
	    {
		    TP_MEMSET(obj.pFirstText, 0, len);
		    TP_MEMCPY(obj.pFirstText, pText, len - 1);
	    }
    }
    else if ( TP_CHANNELSELECT_CHANNEL_SECOND == index )
    {
        if (obj.pSecondText)
	    {
		    TP_FREE(obj.pSecondText);
	    }
	    obj.pSecondText = TP_MALLOC(len);
	    if (obj.pSecondText)
	    {
		    TP_MEMSET(obj.pSecondText, 0, len);
		    TP_MEMCPY(obj.pSecondText, pText, len - 1);
	    }
    }
	WM_SetUserData(hObj, &obj, sizeof(TP_CHANNELSELECT_Obj));
    TP_GUI_UNLOCK();
	return RET_SUCCESS;
}
U8 TP_ChannelSelect_GetState(TP_CHANNELSELECT_Handle hObj)
{
    if (hObj) 
    {
	TP_CHANNELSELECT_Obj obj;
        TP_GUI_LOCK();
	WM_GetUserData(hObj, &obj, sizeof(obj));
        TP_GUI_UNLOCK();
        return obj.state;
    }
    return RET_FAIL;
}
void TP_ChannelSelect_SetState(TP_CHANNELSELECT_Handle hObj, U8 state)
{
    if (hObj) 
    {
	TP_CHANNELSELECT_Obj obj;
        TP_GUI_LOCK();
	WM_GetUserData(hObj, &obj, sizeof(obj));
	obj.state = state;
	WM_SetUserData(hObj, &obj, sizeof(obj));
        TP_GUI_UNLOCK();
    }
}
int TP_ChannelSelect_GetUserData(TP_CHANNELSELECT_Handle hWin, void * pDest, int sizeOfBuffer)
{
	TP_CHANNELSELECT_Obj	 obj;
	int			 numBytes;
	U8		   * pExtraBytes = NULL;

	if ((pDest == NULL) || (sizeOfBuffer <= 0))
	{
		return RET_SUCCESS;
	}
	
	WM_GetUserData(hWin, &obj, sizeof(TP_CHANNELSELECT_Obj));
	pExtraBytes = (U8 *)TP_MALLOC(sizeof(TP_CHANNELSELECT_Obj) + obj.numExtraBytes);
	if (pExtraBytes) 
	{
		WM_GetUserData(hWin, pExtraBytes, sizeof(TP_CHANNELSELECT_Obj) + obj.numExtraBytes);
		
		if (sizeOfBuffer >= obj.numExtraBytes)
		{
			numBytes = obj.numExtraBytes;
		}
		else
		{
			numBytes = sizeOfBuffer;
		}
		
		TP_MEMCPY(pDest, pExtraBytes + sizeof(TP_CHANNELSELECT_Obj), numBytes);
		TP_FREE(pExtraBytes);
		return numBytes;
	}
	return RET_SUCCESS;
}


int TP_ChannelSelect_SetUserData(TP_CHANNELSELECT_Handle hWin, void * pSrc, int sizeOfBuffer)
{
	TP_CHANNELSELECT_Obj	 obj;
	int			 numBytes;
	U8		   * pExtraBytes = NULL;

	if ((pSrc == NULL) || (sizeOfBuffer <= 0))
	{
		return RET_FAIL;
	}

	WM_GetUserData(hWin, &obj, sizeof(TP_CHANNELSELECT_Obj));
	pExtraBytes = (U8 *)TP_MALLOC(sizeof(TP_CHANNELSELECT_Obj) + obj.numExtraBytes);
	if (pExtraBytes)
	{
		WM_GetUserData(hWin, pExtraBytes, sizeof(TP_CHANNELSELECT_Obj) + obj.numExtraBytes);

		if (sizeOfBuffer >= obj.numExtraBytes)
		{
		  numBytes = obj.numExtraBytes;
		}
		else
		{
		  numBytes = sizeOfBuffer;
		}
		TP_MEMSET(pExtraBytes + sizeof(TP_CHANNELSELECT_Obj), 0, obj.numExtraBytes);
		TP_MEMCPY(pExtraBytes + sizeof(TP_CHANNELSELECT_Obj), pSrc, numBytes);
		WM_SetUserData(hWin, pExtraBytes, sizeof(TP_CHANNELSELECT_Obj) + obj.numExtraBytes);
		TP_FREE(pExtraBytes);
		return RET_SUCCESS;
	}

	return RET_FAIL;
}
void TP_ChannelSelect_Callback(WM_MESSAGE * pMsg)
{
    TP_CHANNELSELECT_Handle	hWin;
	TP_CHANNELSELECT_Obj	obj;
    const GUI_FONT * pOldFont;
	GUI_RECT	WinRect;
    GUI_RECT  FirstWinRect;
    GUI_RECT  SecondWinRect;

    GUI_PID_STATE	*pState;	
    int index;
	U8	Pressed;
	WM_PID_STATE_CHANGED_INFO *pPidState;

	hWin = pMsg->hWin;
	WM_GetWindowRectEx(hWin, &WinRect); //获得当前窗口的桌面坐标
    GUI_MoveRect(&WinRect, -WinRect.x0, -WinRect.y0);

    FirstWinRect.x0 = WinRect.x0;
    FirstWinRect.x1 = WinRect.x1/2;
    FirstWinRect.y0 = WinRect.y0;
    FirstWinRect.y1 = WinRect.y1;

    SecondWinRect.x0 = WinRect.x1/2;
    SecondWinRect.x1 = WinRect.x1;
    SecondWinRect.y0 = WinRect.y0;
    SecondWinRect.y1 = WinRect.y1;
	WM_GetUserData(hWin, &obj, sizeof(TP_CHANNELSELECT_Obj));
	
	switch (pMsg->MsgId) 
	{
        case WM_PAINT:

            GUI_AA_SetFactor(obj.aaFactor);

            GUI_AA_EnableHiRes();
            GUI_SetColor(obj.colorIndex[TP_CHANNELSELECT_BK_COLOR]);
            GUI_AA_DrawArc(WinRect.y1/2 * obj.aaFactor, WinRect.y1/2 * obj.aaFactor ,  \
                           WinRect.y1/2 * obj.aaFactor, WinRect.y1/2 * obj.aaFactor, 90,  270  );
            GUI_AA_DrawArc( (WinRect.x1 - WinRect.y1/2) * obj.aaFactor, WinRect.y1/2 * obj.aaFactor, \
                            WinRect.y1/2 * obj.aaFactor, WinRect.y1/2 * obj.aaFactor,-90, 90 );
            GUI_AA_DisableHiRes();

            GUI_DrawHLine( 0, WinRect.y1/2, WinRect.x1 - WinRect.y1/2 );
            GUI_DrawHLine( WinRect.y1, WinRect.y1/2, WinRect.x1 - WinRect.y1/2 );

            pOldFont = GUI_GetFont();
            GUI_SetFont(obj.pFont);
            GUI_SetTextMode(GUI_TM_TRANS);
            GUI_AA_SetFactor(obj.aaFactor);
            if ( 1 == obj.state )
            { 
                 if ( obj.pBitmap )
                 { 
                     //GUI_DrawBitmap(&bmImage_000, FirstWinRect.x0, FirstWinRect.y0);
                 }
                 else
                 {
                     GUI_SetColor(obj.colorIndex[TP_CHANNELSELECT_BK_COLOR]);
                     GUI_FillRect(FirstWinRect.y1/2, 0,  FirstWinRect.x1, FirstWinRect.y1);
                     GUI_AA_FillCircle(FirstWinRect.y1/2, FirstWinRect.y1/2,  FirstWinRect.y1/2);
                 }

                 index = TP_CHANNELSELECT_TEXT_ON_COLOR;
                 GUI_SetColor(obj.colorIndex[index]);
                 GUI_DispStringInRectWrap(obj.pFirstText, &FirstWinRect, GUI_TA_HCENTER|GUI_TA_VCENTER, GUI_WRAPMODE_WORD); 

                 index = TP_CHANNELSELECT_TEXT_OFF_COLOR;
                 GUI_SetColor(obj.colorIndex[index]);
                 GUI_DispStringInRectWrap(obj.pSecondText, &SecondWinRect, GUI_TA_HCENTER|GUI_TA_VCENTER, GUI_WRAPMODE_WORD); 
            }
            else if ( 2 == obj.state )
            {        
                 if ( obj.pBitmap )
                 { 
                     //GUI_DrawBitmapEx(&bmImage_000, SecondWinRect.x1, SecondWinRect.y1, 0, 0, -1000, -1000);
                 }
                 else
                 {
                     GUI_SetColor(obj.colorIndex[TP_CHANNELSELECT_BK_COLOR]);
                     GUI_FillRect(SecondWinRect.x0, SecondWinRect.y0,  SecondWinRect.x1 - SecondWinRect.y1/2, SecondWinRect.y1);
                     GUI_AA_FillCircle(SecondWinRect.x1 - SecondWinRect.y1/2, SecondWinRect.y1/2,  SecondWinRect.y1/2);
                 }                
                 index = TP_CHANNELSELECT_TEXT_ON_COLOR;
                 GUI_SetColor(obj.colorIndex[index]);
                 GUI_DispStringInRectWrap(obj.pSecondText, &SecondWinRect, GUI_TA_HCENTER|GUI_TA_VCENTER, GUI_WRAPMODE_WORD); 

                 index = TP_CHANNELSELECT_TEXT_OFF_COLOR;
                 GUI_SetColor(obj.colorIndex[index]);
                 GUI_DispStringInRectWrap(obj.pFirstText, &FirstWinRect, GUI_TA_HCENTER|GUI_TA_VCENTER, GUI_WRAPMODE_WORD); 
            }
            
            GUI_SetFont(pOldFont);            
			break;
        case WM_TOUCH:
            if (WM_IsEnabled(hWin))
    		{
    		    pState = (GUI_PID_STATE *)pMsg->Data.p;
        		
    		    if (pState)
    		    {
    			    Pressed = pState->Pressed;
    		    }      		
    		    else
    		    {
                    Pressed = obj.pressed;
    		    }
    		    if (obj.pressed != Pressed)
    		    {
    			    obj.pressed = Pressed;
    			    WM_SetUserData(WM_GetParent(hWin), &obj, sizeof(TP_CHANNELSELECT_Obj));
    			    if (Pressed)
    			    {
    				    WM_SetFocus(hWin);
    			    }
    		    }
            }
    		break;
	    case WM_SET_FOCUS:
            if ( pMsg->Data.v  && ( obj.focussable == 1) )
		    {
		      pMsg->Data.v = 0;
		    } 
		    WM_InvalidateWindow(hWin);
		    break;
	    case WM_PID_STATE_CHANGED:
            if (WM_IsEnabled(hWin))
    	    {
		        pPidState = (WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p);		  
		        if ( (pPidState->StatePrev == WM_PID_STATE_PRESSED) && \
                     (pPidState->State     == WM_PID_STATE_UNPRESSED)   )
		        {	
                    if ( pPidState->x < WinRect.x1/2 )
                    {
                        if ( obj.state == 2 )
                        {
                            obj.state = 1;
                            WM_SetUserData(hWin, &obj, sizeof(TP_CHANNELSELECT_Obj));
			                WM_InvalidateWindow(hWin);                        
                            WM_NotifyParent(hWin, WM_NOTIFICATION_VALUE_CHANGED);
                        }
                    }
                    else 
                    {
                        if ( obj.state == 1 )
                        {
                            obj.state = 2;
                            WM_SetUserData(hWin, &obj, sizeof(TP_CHANNELSELECT_Obj));
			                WM_InvalidateWindow(hWin);                        
                            WM_NotifyParent(hWin, WM_NOTIFICATION_VALUE_CHANGED);
                        }
                    }
		        }
            }
		    break;  
        case WM_DELETE:
            TP_FREE( obj.pFirstText );
            TP_FREE( obj.pSecondText);
            break;
        case WM_GET_ID:
		    pMsg->Data.v = obj.id;
		    break;
	    case WM_SET_ID:
		    obj.id = pMsg->Data.v;
		    WM_SetUserData(hWin, &obj, sizeof(TP_CHANNELSELECT_Obj));
		    break;
        default:
		    WM_DefaultProc(pMsg);
		    break;
    }
}


TP_CHANNELSELECT_Handle TP_ChannelSelect_Create(int x0, int y0, int xSize, int ySize,   \
								WM_HWIN hWinParent, U32 style,   \
								WM_CALLBACK * pfCallback, int numExtraBytes, int id)
{
    TP_CHANNELSELECT_Handle	hHandle;
	TP_CHANNELSELECT_Obj	obj;
	WM_CALLBACK     	    *pCb;

	obj	= TP_ChannelSelect_Default;	
	obj.id = id;
    if ( pfCallback )
    {
        pCb = pfCallback;
    }
    else
    {
        pCb = TP_ChannelSelect_Callback;
    }
		
    hHandle = WM_CreateWindowAsChild(x0, y0, xSize, ySize, hWinParent, \
		                                    style | WM_CF_SHOW | WM_CF_HASTRANS | WM_CF_MEMDEV, \
                                            pCb, sizeof(TP_CHANNELSELECT_Obj) + numExtraBytes);
    WM_SetUserData(hHandle, &obj, sizeof(TP_CHANNELSELECT_Obj));
	return hHandle;	
}