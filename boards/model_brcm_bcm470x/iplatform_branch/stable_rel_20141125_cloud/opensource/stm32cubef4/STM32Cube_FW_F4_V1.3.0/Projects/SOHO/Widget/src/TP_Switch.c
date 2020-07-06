/*TP_Switch.c*/

#include <string.h> // Definition of NULL
#include <stdlib.h> // malloc() and free()

#include "TP_Widget.h"
#include "TP_Switch.h"

extern GUI_CONST_STORAGE GUI_BITMAP bmSWITCH_Off;
extern GUI_CONST_STORAGE GUI_BITMAP bmSWITCH_OffDis;
extern GUI_CONST_STORAGE GUI_BITMAP bmSWITCH_On;
extern GUI_CONST_STORAGE GUI_BITMAP bmSWITCH_OnDis;

#define TP_SWITCH_DEFAULT_ID	            0
#define TP_SWITCH_DEFAULT_STATE             0
#define TP_SWITCH_DEFAULT_FOCUSSABLE        0

/*********************************************************************
*
*滑动按钮的默认属性值
*/
static TP_SWITCH_Obj TP_Switch_Default = 
{
	TP_SWITCH_DEFAULT_ID,
	
	{		
		&bmSWITCH_Off,
	
		&bmSWITCH_OffDis,
	
		&bmSWITCH_On,
		
		&bmSWITCH_OnDis
	},
	
	TP_SWITCH_DEFAULT_STATE,	                     /*默认当前滑动按钮为OFF状态*/
	
	0,				                                 /*默认当前滑动按钮未被按下*/

    TP_SWITCH_DEFAULT_FOCUSSABLE,                     /*默认不接受输入焦点*/

    0
	
};

U8	TP_Switch_GetDefaultState(void)
{
	return TP_Switch_Default.state;
}
void	TP_Switch_SetDefaultState(U8 state)
{
	TP_Switch_Default.state = state;
}
U8	TP_Switch_GetDefaultFocussable(void)
{
	return TP_Switch_Default.focussable;
}
void	TP_Switch_SetDefaultFocussable(U8 focussable)
{
	TP_Switch_Default.focussable = focussable;
}

int TP_Switch_GetId(TP_SWITCH_Handle hObj)
{
	if (hObj) 
	{
		TP_SWITCH_Obj obj;

		TP_GUI_LOCK();
		WM_GetUserData(hObj, &obj, sizeof(TP_SWITCH_Obj));
		TP_GUI_UNLOCK();
		return obj.id;
	}
	return RET_FAIL;
}
void TP_Switch_SetId(TP_SWITCH_Handle hObj,int id)
{
	if (hObj) 
	{
		TP_SWITCH_Obj obj;

		TP_GUI_LOCK();
		WM_GetUserData(hObj, &obj, sizeof(TP_SWITCH_Obj));
		obj.id = id;
		WM_SetUserData(hObj, &obj, sizeof(TP_SWITCH_Obj));
		TP_GUI_UNLOCK();
	}
}
const GUI_BITMAP* TP_Switch_GetBitmap(TP_SWITCH_Handle hObj,  int index) 
{
    if (index < TP_SWITCH_PICS_OFF  \
		|| index > TP_SWITCH_PICS_ON_DIS)
	{
		return NULL;
	}
	if (hObj) 
	{
		TP_SWITCH_Obj obj;
		TP_GUI_LOCK();
		WM_GetUserData(hObj, &obj, sizeof(TP_SWITCH_Obj));
	    return obj.pBitmap[index];
		TP_GUI_UNLOCK();
	}
	return NULL;
}
void TP_Switch_SetBitmapEx(TP_SWITCH_Handle hObj,  int index, const GUI_BITMAP *pBitmap, int x, int y) 
{
    if (index < TP_SWITCH_PICS_OFF  \
		|| index > TP_SWITCH_PICS_ON_DIS)
	{
		return ;
	}
	if (hObj && pBitmap) 
	{
		TP_SWITCH_Obj obj;
		TP_GUI_LOCK();
		WM_GetUserData(hObj, &obj, sizeof(TP_SWITCH_Obj));
		obj.pBitmap[index] = pBitmap;
		WM_SetUserData(hObj, &obj, sizeof(TP_SWITCH_Obj));
		TP_GUI_UNLOCK();
	}
}

void TP_Switch_SetBitmap(TP_SWITCH_Handle hObj,  int index, const GUI_BITMAP *pBitmap) 
{
	TP_Switch_SetBitmapEx(hObj, index, pBitmap, 0, 0);
}

TP_SWITCH_STATE TP_Switch_GetState(TP_SWITCH_Handle hObj)
{
    if (hObj) 
	{
	    TP_SWITCH_Obj obj;
        TP_GUI_LOCK();
	    WM_GetUserData(hObj, &obj, sizeof(obj));
        TP_GUI_UNLOCK();
        return obj.state;
    }
    return RET_FAIL;
}
void TP_Switch_SetState (TP_SWITCH_Handle hObj, TP_SWITCH_STATE state)
{
    if (hObj) 
	{
	    TP_SWITCH_Obj obj;
        TP_GUI_LOCK();
	    WM_GetUserData(hObj, &obj, sizeof(obj));
	    obj.state = state;
	    WM_SetUserData(hObj, &obj, sizeof(obj));
        TP_GUI_UNLOCK();
    }
}



TP_SWITCH_PRESSED TP_Switch_GetPressed(TP_SWITCH_Handle hObj)
{
    if (hObj) 
	{
	    TP_SWITCH_Obj obj;
        TP_GUI_LOCK();
	    WM_GetUserData(hObj, &obj, sizeof(obj));
        TP_GUI_UNLOCK();
	    return obj.pressed;
    }
    return RET_FAIL;
}
void TP_Switch_SetPressed (TP_SWITCH_Handle hObj, U8 pressed)
{
    if (hObj) 
	{
	    TP_SWITCH_Obj obj;  
        TP_GUI_LOCK();
	    WM_GetUserData(hObj, &obj, sizeof(obj));
	    obj.pressed = pressed;
	    WM_SetUserData(hObj, &obj, sizeof(obj));
        TP_GUI_UNLOCK();
    }
}

int TP_Switch_GetFocussable(TP_SWITCH_Handle hObj)
{
    if (hObj) 
	{
	    TP_SWITCH_Obj obj;
        TP_GUI_LOCK();
	    WM_GetUserData(hObj, &obj, sizeof(obj));
        TP_GUI_UNLOCK();
	    return obj.focussable;
    }
    return RET_FAIL;
}
void TP_Switch_SetFocussable(TP_SWITCH_Handle hObj, int state)
{
    if (hObj) 
	{
	    TP_SWITCH_Obj obj;  
        TP_GUI_LOCK();
	    WM_GetUserData(hObj, &obj, sizeof(obj));
	    obj.focussable = state;
	    WM_SetUserData(hObj, &obj, sizeof(obj));
        TP_GUI_UNLOCK();
    }
}
int TP_Switch_GetUserData(TP_SWITCH_Handle hWin, void * pDest, int sizeOfBuffer)
{
	TP_SWITCH_Obj	 obj;
	int			 numBytes;
	U8		   * pExtraBytes = NULL;

	if ((pDest == NULL) || (sizeOfBuffer <= 0))
	{
		return RET_SUCCESS;
	}
	
	WM_GetUserData(hWin, &obj, sizeof(TP_SWITCH_Obj));
	pExtraBytes = (U8 *)TP_MALLOC(sizeof(TP_SWITCH_Obj) + obj.numExtraBytes);
	if (pExtraBytes) 
	{
		WM_GetUserData(hWin, pExtraBytes, sizeof(TP_SWITCH_Obj) + obj.numExtraBytes);
		
		if (sizeOfBuffer >= obj.numExtraBytes)
		{
			numBytes = obj.numExtraBytes;
		}
		else
		{
			numBytes = sizeOfBuffer;
		}
		
		TP_MEMCPY(pDest, pExtraBytes + sizeof(TP_SWITCH_Obj), numBytes);
		TP_FREE(pExtraBytes);
		return numBytes;
	}
	return RET_SUCCESS;
}


int TP_Switch_SetUserData(TP_SWITCH_Handle hWin, void * pSrc, int sizeOfBuffer)
{
	TP_SWITCH_Obj	 obj;
	int			 numBytes;
	U8		   * pExtraBytes = NULL;

	if ((pSrc == NULL) || (sizeOfBuffer <= 0))
	{
		return RET_FAIL;
	}

	WM_GetUserData(hWin, &obj, sizeof(TP_SWITCH_Obj));
	pExtraBytes = (U8 *)TP_MALLOC(sizeof(TP_SWITCH_Obj) + obj.numExtraBytes);
	if (pExtraBytes)
	{
		WM_GetUserData(hWin, pExtraBytes, sizeof(TP_SWITCH_Obj) + obj.numExtraBytes);

		if (sizeOfBuffer >= obj.numExtraBytes)
		{
		  numBytes = obj.numExtraBytes;
		}
		else
		{
		  numBytes = sizeOfBuffer;
		}
		TP_MEMSET(pExtraBytes + sizeof(TP_SWITCH_Obj), 0, obj.numExtraBytes);
		TP_MEMCPY(pExtraBytes + sizeof(TP_SWITCH_Obj), pSrc, numBytes);
		WM_SetUserData(hWin, pExtraBytes, sizeof(TP_SWITCH_Obj) + obj.numExtraBytes);
		TP_FREE(pExtraBytes);
		return RET_SUCCESS;
	}

	return RET_FAIL;
}
void TP_Switch_Callback(WM_MESSAGE * pMsg)
{
	TP_SWITCH_Handle	hWin;
	TP_SWITCH_Obj	obj;
	GUI_RECT	WinRect;

    GUI_PID_STATE	*pState;	
    int index;
	U8	Pressed;
	WM_PID_STATE_CHANGED_INFO *pPidState;	

	hWin = pMsg->hWin;
	WM_GetWindowRectEx(hWin, &WinRect); //获得当前窗口的桌面坐标
	
	WM_GetUserData(hWin, &obj, sizeof(TP_SWITCH_Obj));
	
	switch (pMsg->MsgId) 
	{
		case WM_PAINT:
            
            if ( (WinRect.x1 - WinRect.x0) != (obj.pBitmap[0]->XSize - 1)  || \
                    (WinRect.y1 - WinRect.y0) != (obj.pBitmap[0]->YSize - 1)  )
            {
                return ;//实际窗口大小与滑钮大小不一致
            }
            	
    		if (WM_IsEnabled(hWin))
    		{
    			if (obj.state)
    			{	
                    index = TP_SWITCH_PICS_ON;
    			}
    			else
    			{
                    index = TP_SWITCH_PICS_OFF;
    			}
    		}
    		else
    		{
    			if (obj.state)
    			{			
                    index = TP_SWITCH_PICS_ON_DIS;
    			}
    			else
    			{
                    index = TP_SWITCH_PICS_OFF_DIS;
    			}
    		}
            if( obj.pBitmap[index] )
            {              
                GUI_DrawBitmap(obj.pBitmap[index],0,0);            
            }
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
    			    WM_SetUserData(hWin, &obj, sizeof(TP_SWITCH_Obj));
    			    if (Pressed)
    			    {
    				    WM_SetFocus(hWin);
    			    }
    			    WM_InvalidateWindow(hWin);
        
    			    if (Pressed)
    			    {
    				    /* 告诉父窗口自己被点击 */
    				    WM_NotifyParent(hWin, WM_NOTIFICATION_CLICKED);
    			    }
    			    else
    			    {
    				    /* 告诉父窗口自己的点击已被释放 */
    				    WM_NotifyParent(hWin, WM_NOTIFICATION_RELEASED);
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
		        if (WM_IsEnabled(hWin))
		        {
			        if ( (pPidState->StatePrev == WM_PID_STATE_PRESSED) && \
                         (pPidState->State     == WM_PID_STATE_UNPRESSED)   )
			        {
				        obj.state = (obj.state == TP_SWITCH_STATE_ON)?     \
					                              (TP_SWITCH_STATE_OFF) : (TP_SWITCH_STATE_ON);

				        WM_SetUserData(hWin, &obj, sizeof(TP_SWITCH_Obj));
				        WM_InvalidateWindow(hWin);

				        /* 告诉父窗口自己的状态发生了变化 */
				        WM_NotifyParent(hWin, WM_NOTIFICATION_VALUE_CHANGED);
			        }
		        }
            }
		    break;
	    case WM_GET_ID:
		    pMsg->Data.v = obj.id;
		    break;
	    case WM_SET_ID:
		    obj.id = pMsg->Data.v;
		    WM_SetUserData(hWin, &obj, sizeof(TP_SWITCH_Obj));
		    break;
	    case WM_DELETE:
		    break;
	    default:
		    WM_DefaultProc(pMsg);
		    break;
	}
}


TP_SWITCH_Handle TP_Switch_Create(int x0, int y0, int xSize, int ySize,
								WM_HWIN hWinParent, U32 style, 
								WM_CALLBACK * pfCallback, int numExtraBytes, int id)
{
	TP_SWITCH_Handle	handle;
	TP_SWITCH_Obj		obj;
	WM_CALLBACK     	*pCb;

	if (pfCallback)
	{
		pCb = pfCallback;
	}
	else
	{
		pCb = TP_Switch_Callback;
	}

	obj	= TP_Switch_Default;	
	obj.id = id;
		
	handle = WM_CreateWindowAsChild(x0, y0, xSize, ySize, hWinParent, \
		                                    style | WM_CF_SHOW | WM_CF_HASTRANS | WM_CF_MEMDEV, \
                                            pCb, sizeof(TP_SWITCH_Obj) + numExtraBytes);
  
	WM_SetUserData(handle, &obj, sizeof(TP_SWITCH_Obj));

	return handle;
	
}