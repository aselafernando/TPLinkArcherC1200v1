/*TP_Slider.c*/

#include <string.h> // Definition of NULL
#include <stdlib.h> // malloc() and free()

#include "TP_Widget.h"
#include "TP_Slider.h"


#define TP_SLIDER_DEFAULT_ID	            0
#define TP_SLIDER_DEFAULT_VALUE             100
#define TP_SLIDER_DEFAULT_FOCUSSABLE        0

#define TP_SLIDER_DEFAULT_AA_FACTOR         8

#define TP_SLIDER_DEFAULT_SLOT_COLOR        0xc1c6c8  
#define TP_SLIDER_DEFAULT_FILL_COLOR        0x96cc56
#define TP_SLIDER_DEFAULT_CIRCLE_COLOR      GUI_WHITE

#define TP_SLIDER_DEFAULT_ZONE              20

#define TP_SLIDER_DEFAULT_RADIUS            8  
#define TP_SLIDER_DEFAULT_WIDTH             4
#define TP_SLIDER_DEFAULT_LENGTH            160

#define TP_SLIDER_DEFAULT_TEXT_FONT         &GUI_Font24_ASCII

extern GUI_CONST_STORAGE GUI_BITMAP bmSlider_Circle;

/*********************************************************************
*
*滑动按钮的默认属性值
*/
static TP_SLIDER_Obj TP_Slider_Default = 
{
	TP_SLIDER_DEFAULT_ID,

    TP_SLIDER_DEFAULT_RADIUS,

    TP_SLIDER_DEFAULT_WIDTH,

    TP_SLIDER_DEFAULT_LENGTH,
			
    NULL,//&bmSlider_Circle,
	
    {
        TP_SLIDER_DEFAULT_SLOT_COLOR,

        TP_SLIDER_DEFAULT_FILL_COLOR,

        TP_SLIDER_DEFAULT_CIRCLE_COLOR

     },

     0,

    TP_SLIDER_DEFAULT_ZONE,                          /*默认的有效点击范围*/

    TP_SLIDER_DEFAULT_AA_FACTOR,

	TP_SLIDER_DEFAULT_VALUE,	                     /*默认当前值*/
	
	0,				                                 /*默认当前滑动按钮未被按下*/

    TP_SLIDER_DEFAULT_FOCUSSABLE,                     /*默认不接受输入焦点*/

    0                                                 /*默认0额外空间*/
	
};
int	TP_Slider_GetDefaultRadius(void)
{
	return TP_Slider_Default.radius;
}
void	TP_Slider_SetDefaultRadius(int radius)
{
	TP_Slider_Default.radius = radius;
}
int	TP_Slider_GetDefaultWidth(void)
{
	return TP_Slider_Default.width;
}
void	TP_Slider_SetDefaultWidth(int width)
{
	TP_Slider_Default.width = width;
}
int	TP_Slider_GetDefaultLength(void)
{
	return TP_Slider_Default.length;
}
void	TP_Slider_SetDefaultLength(int length)
{
	TP_Slider_Default.length = length;
}
const GUI_BITMAP* TP_Slider_GetDefaultBitmap(void)
{
	return TP_Slider_Default.pBitmap;
}
void TP_Slider_SetDefaultBitmap(const GUI_BITMAP* pBitmap)
{
	TP_Slider_Default.pBitmap = pBitmap;
}
GUI_COLOR TP_Slider_GetDefaultColorIndex(int index)
{
	if (index < TP_SLIDER_SLOT_COLOR
		|| index > TP_SLIDER_CIRCLE_COLOR)
	{
		return RET_FAIL;
	}
	return TP_Slider_Default.colorIndex[index];
}

void TP_Slider_SetDefaultColorIndex(GUI_COLOR color, int index)
{
	if (index < TP_SLIDER_SLOT_COLOR
		|| index > TP_SLIDER_CIRCLE_COLOR)
	{
		return ;
	}
	TP_Slider_Default.colorIndex[index] = color;
}
int	TP_Slider_GetDefaultZone(void)
{
	return TP_Slider_Default.zone;
}
void	TP_Slider_SetDefaultZone(int zone)
{
	TP_Slider_Default.zone = zone;
}
int	TP_Slider_GetDefaultAaFactor(void)
{
	return TP_Slider_Default.aaFactor;
}
void	TP_Slider_SetDefaultAaFactor(int factor)
{
	TP_Slider_Default.aaFactor = factor;
}
int	TP_Slider_GetDefaultValue(void)
{
	return TP_Slider_Default.value;
}
void	TP_Slider_SetDefaultValue(int value)
{
	TP_Slider_Default.value = value;
}
int	TP_Slider_GetDefaultPressed(void)
{
	return TP_Slider_Default.pressed;
}
void	TP_Slider_SetDefaultPressed(int pressed)
{
	TP_Slider_Default.pressed = pressed;
}
int	TP_Slider_GetDefaultFocussable(void)
{
	return TP_Slider_Default.focussable;
}
void	TP_Slider_SetDefaultFocussable(int focussable)
{
	TP_Slider_Default.focussable = focussable;
}
int	TP_Slider_GetDefaultNumExtraBytes(void)
{
	return TP_Slider_Default.numExtraBytes;
}
void	TP_Slider_SetDefaultNumExtraBytes(int bytes)
{
	TP_Slider_Default.numExtraBytes = bytes;
}
int TP_Slider_GetId(TP_SLIDER_Handle hObj)
{
	if (hObj) 
	{
		TP_SLIDER_Obj obj;
		TP_GUI_LOCK();
		WM_GetUserData(hObj, &obj, sizeof(TP_SLIDER_Obj));
		TP_GUI_UNLOCK();
		return obj.id;
	}
	return RET_FAIL;
}
void TP_Slider_SetId(TP_SLIDER_Handle hObj,int id)
{
	if (hObj) 
	{
		TP_SLIDER_Obj obj;

		TP_GUI_LOCK();
		WM_GetUserData(hObj, &obj, sizeof(TP_SLIDER_Obj));
		obj.id = id;
		WM_SetUserData(hObj, &obj, sizeof(TP_SLIDER_Obj));
		TP_GUI_UNLOCK();
	}
}
int TP_Slider_GetRadius(TP_SLIDER_Handle hObj)
{
	if (hObj) 
	{
		TP_SLIDER_Obj obj;
		TP_GUI_LOCK();
		WM_GetUserData(hObj, &obj, sizeof(TP_SLIDER_Obj));
		TP_GUI_UNLOCK();
		return obj.radius;
	}
	return RET_FAIL;
}
void TP_Slider_SetRadius(TP_SLIDER_Handle hObj,int radius)
{
	if (hObj) 
	{
		TP_SLIDER_Obj obj;

		TP_GUI_LOCK();
		WM_GetUserData(hObj, &obj, sizeof(TP_SLIDER_Obj));
		obj.radius = radius;
		WM_SetUserData(hObj, &obj, sizeof(TP_SLIDER_Obj));
		TP_GUI_UNLOCK();
	}
}
int TP_Slider_GetWidth(TP_SLIDER_Handle hObj)
{
	if (hObj) 
	{
		TP_SLIDER_Obj obj;
		TP_GUI_LOCK();
		WM_GetUserData(hObj, &obj, sizeof(TP_SLIDER_Obj));
		TP_GUI_UNLOCK();
		return obj.width;
	}
	return RET_FAIL;
}
void TP_Slider_SetWidth(TP_SLIDER_Handle hObj,int width)
{
	if (hObj) 
	{
		TP_SLIDER_Obj obj;

		TP_GUI_LOCK();
		WM_GetUserData(hObj, &obj, sizeof(TP_SLIDER_Obj));
		obj.width = width;
		WM_SetUserData(hObj, &obj, sizeof(TP_SLIDER_Obj));
		TP_GUI_UNLOCK();
	}
}
int TP_Slider_GetLength(TP_SLIDER_Handle hObj)
{
	if (hObj) 
	{
		TP_SLIDER_Obj obj;
		TP_GUI_LOCK();
		WM_GetUserData(hObj, &obj, sizeof(TP_SLIDER_Obj));
		TP_GUI_UNLOCK();
		return obj.length;
	}
	return RET_FAIL;
}
void TP_Slider_SetLength(TP_SLIDER_Handle hObj,int length)
{
	if (hObj) 
	{
		TP_SLIDER_Obj obj;

		TP_GUI_LOCK();
		WM_GetUserData(hObj, &obj, sizeof(TP_SLIDER_Obj));
		obj.length = length;
		WM_SetUserData(hObj, &obj, sizeof(TP_SLIDER_Obj));
		TP_GUI_UNLOCK();
	}
}
const GUI_BITMAP* TP_Slider_GetBitmap(TP_SLIDER_Handle hObj)
{
	if (hObj) 
	{
		TP_SLIDER_Obj obj;
		TP_GUI_LOCK();
		WM_GetUserData(hObj, &obj, sizeof(TP_SLIDER_Obj));
		TP_GUI_UNLOCK();
		return obj.pBitmap;
	}
	return NULL;
}
void TP_Slider_SetBitmap(TP_SLIDER_Handle hObj,const GUI_BITMAP* pBitmap)
{
    if (hObj)
	{
		TP_SLIDER_Obj obj;
		TP_GUI_LOCK();
		WM_GetUserData(hObj, &obj, sizeof(TP_SLIDER_Obj));
		obj.pBitmap = pBitmap;
		WM_SetUserData(hObj, &obj, sizeof(TP_SLIDER_Obj));
		TP_GUI_UNLOCK();
	}
}
GUI_COLOR TP_Slider_GetColorIndex(TP_SLIDER_Handle hObj, int index)
{
    if (index < TP_SLIDER_SLOT_COLOR
		|| index > TP_SLIDER_CIRCLE_COLOR)
	{
		return RET_FAIL;
	}
    if (hObj) 
	{
		TP_SLIDER_Obj obj;
		TP_GUI_LOCK();
		WM_GetUserData(hObj, &obj, sizeof(TP_SLIDER_Obj));
		TP_GUI_UNLOCK();
        return obj.colorIndex[index];
	}
    return RET_FAIL;
}
void TP_Slider_SetColorIndex(TP_SLIDER_Handle hObj, int index, GUI_COLOR color)
{
    if (index < TP_SLIDER_SLOT_COLOR
		|| index > TP_SLIDER_CIRCLE_COLOR)
	{
		return ;
	}
    if (hObj) 
	{
		TP_SLIDER_Obj obj;
		TP_GUI_LOCK();
		WM_GetUserData(hObj, &obj, sizeof(TP_SLIDER_Obj));
        obj.colorIndex[index] = color;
		WM_SetUserData(hObj, &obj, sizeof(TP_SLIDER_Obj));
		TP_GUI_UNLOCK();
	}
}
int TP_Slider_GetZone(TP_SLIDER_Handle hObj)
{
	if (hObj) 
	{
		TP_SLIDER_Obj obj;
		TP_GUI_LOCK();
		WM_GetUserData(hObj, &obj, sizeof(TP_SLIDER_Obj));
		TP_GUI_UNLOCK();
		return obj.zone;
	}
	return RET_FAIL;
}
void TP_Slider_SetZone(TP_SLIDER_Handle hObj,int zone)
{
	if (hObj) 
	{
		TP_SLIDER_Obj obj;

		TP_GUI_LOCK();
		WM_GetUserData(hObj, &obj, sizeof(TP_SLIDER_Obj));
		obj.zone = zone;
		WM_SetUserData(hObj, &obj, sizeof(TP_SLIDER_Obj));
		TP_GUI_UNLOCK();
	}
}
U8 TP_Slider_GetAaFactor(TP_SLIDER_Handle hObj)
{
        if (hObj) 
	{
	    TP_SLIDER_Obj obj;
            TP_GUI_LOCK();
	    WM_GetUserData(hObj, &obj, sizeof(obj));
            TP_GUI_UNLOCK();
            return obj.aaFactor;
        }
        return RET_FAIL;
}
void TP_Slider_SetAaFactor(TP_SLIDER_Handle hObj, U8 factor)
{
        if (hObj) 
	{
	    TP_SLIDER_Obj obj;
            TP_GUI_LOCK();
	    WM_GetUserData(hObj, &obj, sizeof(obj));
	    obj.aaFactor = factor;
	    WM_SetUserData(hObj, &obj, sizeof(obj));
            TP_GUI_UNLOCK();
        }
}
U8 TP_Slider_GetValue(TP_SLIDER_Handle hObj)
{
        if (hObj) 
	{
	    TP_SLIDER_Obj obj;
            TP_GUI_LOCK();
            WM_GetUserData(hObj, &obj, sizeof(obj));
            TP_GUI_UNLOCK();
            return ( (obj.value - obj.radius - (WM_GetWindowSizeX(hObj) - obj.length)/2 + 1) * 100 / ( obj.length - obj.radius * 2) );
         }
        return RET_FAIL;
}
void TP_Slider_SetValue(TP_SLIDER_Handle hObj, U8 value)
{
    if (hObj) 
	{
	    TP_SLIDER_Obj obj;
            TP_GUI_LOCK();
	    WM_GetUserData(hObj, &obj, sizeof(obj));
	    obj.value = (obj.length - obj.radius * 2) *value/100 + obj.radius + (WM_GetWindowSizeX(hObj) - obj.length)/2 ;
	    WM_SetUserData(hObj, &obj, sizeof(obj));
            TP_GUI_UNLOCK();
        }
}
int TP_Slider_GetPressed(TP_SLIDER_Handle hObj)
{
	if (hObj) 
	{
		TP_SLIDER_Obj obj;
		TP_GUI_LOCK();
		WM_GetUserData(hObj, &obj, sizeof(TP_SLIDER_Obj));
		TP_GUI_UNLOCK();
		return obj.pressed;
	}
	return RET_FAIL;
}
void TP_Slider_SetPressed(TP_SLIDER_Handle hObj,int pressed)
{
	if (hObj) 
	{
		TP_SLIDER_Obj obj;

		TP_GUI_LOCK();
		WM_GetUserData(hObj, &obj, sizeof(TP_SLIDER_Obj));
		obj.pressed = pressed;
		WM_SetUserData(hObj, &obj, sizeof(TP_SLIDER_Obj));
		TP_GUI_UNLOCK();
	}
}
int TP_Slider_GetFocussable(TP_SLIDER_Handle hObj)
{
	if (hObj) 
	{
		TP_SLIDER_Obj obj;
		TP_GUI_LOCK();
		WM_GetUserData(hObj, &obj, sizeof(TP_SLIDER_Obj));
		TP_GUI_UNLOCK();
		return obj.focussable;
	}
	return RET_FAIL;
}
void TP_Slider_SetFocussable(TP_SLIDER_Handle hObj,int focussable)
{
	if (hObj) 
	{
		TP_SLIDER_Obj obj;

		TP_GUI_LOCK();
		WM_GetUserData(hObj, &obj, sizeof(TP_SLIDER_Obj));
		obj.focussable = focussable;
		WM_SetUserData(hObj, &obj, sizeof(TP_SLIDER_Obj));
		TP_GUI_UNLOCK();
	}
}
int TP_Slider_GetNumExtraBytes(TP_SLIDER_Handle hObj)
{
	if (hObj) 
	{
		TP_SLIDER_Obj obj;
		TP_GUI_LOCK();
		WM_GetUserData(hObj, &obj, sizeof(TP_SLIDER_Obj));
		TP_GUI_UNLOCK();
		return obj.numExtraBytes ;
	}
	return RET_FAIL;
}
void TP_Slider_SetNumExtraBytes(TP_SLIDER_Handle hObj,int numExtraBytes )
{
	if (hObj) 
	{
		TP_SLIDER_Obj obj;

		TP_GUI_LOCK();
		WM_GetUserData(hObj, &obj, sizeof(TP_SLIDER_Obj));
		obj.numExtraBytes  = numExtraBytes ;
		WM_SetUserData(hObj, &obj, sizeof(TP_SLIDER_Obj));
		TP_GUI_UNLOCK();
	}
}

int TP_Slider_GetUserData(TP_SLIDER_Handle hWin, void * pDest, int sizeOfBuffer)
{
	TP_SLIDER_Obj	 obj;
	int			 numBytes;
	U8		   * pExtraBytes = NULL;

	if ((pDest == NULL) || (sizeOfBuffer <= 0))
	{
		return RET_SUCCESS;
	}
	
	WM_GetUserData(hWin, &obj, sizeof(TP_SLIDER_Obj));
	pExtraBytes = (U8 *)TP_MALLOC(sizeof(TP_SLIDER_Obj) + obj.numExtraBytes);
	if (pExtraBytes) 
	{
		WM_GetUserData(hWin, pExtraBytes, sizeof(TP_SLIDER_Obj) + obj.numExtraBytes);
		
		if (sizeOfBuffer >= obj.numExtraBytes)
		{
			numBytes = obj.numExtraBytes;
		}
		else
		{
			numBytes = sizeOfBuffer;
		}
		
		TP_MEMCPY(pDest, pExtraBytes + sizeof(TP_SLIDER_Obj), numBytes);
		TP_FREE(pExtraBytes);
		return numBytes;
	}
	return RET_SUCCESS;
}


int TP_Slider_SetUserData(TP_SLIDER_Handle hWin, void * pSrc, int sizeOfBuffer)
{
	TP_SLIDER_Obj	    obj;
	int	            numBytes;
	U8		    *pExtraBytes = NULL;

	if ((pSrc == NULL) || (sizeOfBuffer <= 0))
	{
		return RET_FAIL;
	}

	WM_GetUserData(hWin, &obj, sizeof(TP_SLIDER_Obj));
	pExtraBytes = (U8 *)TP_MALLOC(sizeof(TP_SLIDER_Obj) + obj.numExtraBytes);
	if (pExtraBytes)
	{
		WM_GetUserData(hWin, pExtraBytes, sizeof(TP_SLIDER_Obj) + obj.numExtraBytes);

		if (sizeOfBuffer >= obj.numExtraBytes)
		{
		  numBytes = obj.numExtraBytes;
		}
		else
		{
		  numBytes = sizeOfBuffer;
		}
		TP_MEMSET(pExtraBytes + sizeof(TP_SLIDER_Obj), 0, obj.numExtraBytes);
		TP_MEMCPY(pExtraBytes + sizeof(TP_SLIDER_Obj), pSrc, numBytes);
		WM_SetUserData(hWin, pExtraBytes, sizeof(TP_SLIDER_Obj) + obj.numExtraBytes);
		TP_FREE(pExtraBytes);
		return RET_SUCCESS;
	}

	return RET_FAIL;
}
void TP_Slider_Callback(WM_MESSAGE * pMsg)
{
    TP_SLIDER_Handle	hWin;
	TP_SLIDER_Obj	obj;
	GUI_RECT	WinRect;
    int Xcoord;
    int CircleRadius = bmSlider_Circle.XSize/2;
    int Offset;

    GUI_PID_STATE	*pState;	
    WM_PID_STATE_CHANGED_INFO *pPidState;	
    U8	Pressed;

    hWin = pMsg->hWin;
    WM_GetWindowRectEx(hWin, &WinRect); //获得当前窗口的桌面坐标
    GUI_MoveRect(&WinRect, -WinRect.x0, -WinRect.y0);
    

    WM_GetUserData(hWin, &obj, sizeof(TP_SLIDER_Obj));
    Offset = (WinRect.x1 - obj.length)/2 + 1;
	
	switch (pMsg->MsgId) 
	{
            case WM_PAINT:
                GUI_SetBkColor(GUI_BLACK);
                GUI_Clear();
                
                GUI_SetColor(obj.colorIndex[TP_SLIDER_SLOT_COLOR]);
                GUI_FillRoundedRect( Offset + 1, WinRect.y1/2 - obj.width/2, WinRect.x1 - Offset - 1, WinRect.y1/2 + obj.width/2 - 1, 1);

                Xcoord = obj.value < (obj.radius + Offset) ? (obj.radius + Offset):obj.value;
                Xcoord = obj.value > (WinRect.x1 - obj.radius - Offset)  ?  (WinRect.x1 - obj.radius - Offset):Xcoord;
             
               

                GUI_SetColor(obj.colorIndex[TP_SLIDER_FILL_COLOR]);
                GUI_FillRoundedRect(Offset, WinRect.y1/2 - obj.width/2, Xcoord - 1, WinRect.y1/2 + obj.width/2 - 1, 1);

                if ( obj.pBitmap )
                {
                    GUI_DrawBitmap(obj.pBitmap, Xcoord - CircleRadius , WinRect.y1/2 - CircleRadius);
                }
                else
                {
                    GUI_AA_SetFactor(obj.aaFactor);
                    GUI_AA_EnableHiRes();
                    GUI_SetColor(obj.colorIndex[TP_SLIDER_CIRCLE_COLOR]);
                    GUI_AA_FillCircle( Xcoord * obj.aaFactor, (WinRect.y1/2)* obj.aaFactor, (obj.radius)* obj.aaFactor );
                    GUI_AA_DisableHiRes();        
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
                          Pressed = 0;
                      }	
                      if (Pressed)
		              {
		                  if ( pState->x != obj.value && (1 == obj.flagStateOn) )
		                  {
		                      obj.pressed = Pressed;
		                      obj.value = pState->x < (obj.radius + Offset) ? (obj.radius + Offset):pState->x;
		                      obj.value = pState->x > ( Offset + obj.length  - obj.radius )  ?   \
		                                      ( Offset + obj.length - obj.radius ) : obj.value; 
		                      WM_InvalidateWindow(hWin);
		                      WM_SetUserData(hWin, &obj, sizeof(TP_SLIDER_Obj));
		                      WM_NotifyParent(hWin, WM_NOTIFICATION_VALUE_CHANGED);
		                   }
		              }
		              else
		              {
		                   if ( 1 == obj.flagStateOn )
		                    {   
		                         obj.flagStateOn = 0;
		                         WM_SetUserData(hWin, &obj, sizeof(TP_SLIDER_Obj));
		                    }
		
		              }		    		    
                      if (Pressed)
                      {
                           WM_SetFocus(hWin);
                       }
                                               
                   }
                   break;
#if  0     
        case WM_MOUSEOVER_END:
            if (WM_IsEnabled(hWin))
    	    {
                if ( 1 == obj.flagStateOn )
                {
                     obj.flagStateOn = 0;
                     WM_SetUserData(hWin, &obj, sizeof(TP_SLIDER_Obj));
                }
            }
            break; 
#endif
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
			        if ( (pPidState->StatePrev == WM_PID_STATE_UNPRESSED) && \
                                 (pPidState->State     == WM_PID_STATE_PRESSED)   )
                                {  
                                      if ( pPidState->x == obj.value )
                                      {
                                          obj.flagStateOn = 1;
                                          WM_SetUserData(hWin, &obj, sizeof(TP_SLIDER_Obj));
                                      }
                                      else if(  pPidState->x < obj.value  && (pPidState->x + obj.zone) >  obj.value )
                                      {
                                          obj.flagStateOn = 1;
                                          WM_SetUserData(hWin, &obj, sizeof(TP_SLIDER_Obj));
                                      }
                                      else if ( pPidState->x > obj.value && (pPidState->x - obj.zone) <  obj.value )
                                      {
                                          obj.flagStateOn = 1;
                                          WM_SetUserData(hWin, &obj, sizeof(TP_SLIDER_Obj));
                                      }
                                      else
                                      {
                                          obj.flagStateOn = 0;
                                          WM_SetUserData(hWin, &obj, sizeof(TP_SLIDER_Obj));
                                      }
                                }
                                else if ( (pPidState->StatePrev == WM_PID_STATE_PRESSED) && \
                                              (pPidState->State == WM_PID_STATE_UNPRESSED)   ) 
                                {
                                           obj.flagStateOn = 0;
                                           WM_SetUserData(hWin, &obj, sizeof(TP_SLIDER_Obj));
                                }
                        }
                    }
                    break;  
        case WM_DELETE:
                    break;
        case WM_GET_ID:
		    pMsg->Data.v = obj.id;
		    break;
	case WM_SET_ID:
		    obj.id = pMsg->Data.v;
		    WM_SetUserData(hWin, &obj, sizeof(TP_SLIDER_Obj));
		    break;
        default:
		    WM_DefaultProc(pMsg);
		    break;
    }
}


TP_SLIDER_Handle TP_Slider_Create(int x0, int y0, int xSize, int ySize,   \
								WM_HWIN hWinParent, U32 style,   \
								WM_CALLBACK * pfCallback, int numExtraBytes, int id)
{
    TP_SLIDER_Handle	hHandle;
	TP_SLIDER_Obj	obj;
	WM_CALLBACK     	    *pCb;

	obj	= TP_Slider_Default;	
	obj.id = id;
    if ( pfCallback )
    {
        pCb = pfCallback;
    }
    else
    {
        pCb = TP_Slider_Callback;
    }
		
    hHandle = WM_CreateWindowAsChild(x0, y0, xSize, ySize, hWinParent, \
		                                    style | WM_CF_SHOW | WM_CF_HASTRANS | WM_CF_MEMDEV, \
                                            pCb, sizeof(TP_SLIDER_Obj) + numExtraBytes);
    WM_SetUserData(hHandle, &obj, sizeof(TP_SLIDER_Obj));
	return hHandle;	
}
#if   0
static void Test_cbBkWindow(WM_MESSAGE * pMsg) {
	WM_HMEM hWin;
	//static char showMsg[64] = "TP_Slider - Sample";

	int NCode;
    int times = 0;

	hWin = pMsg->hWin;
    switch (pMsg->MsgId) 
    {
	    case WM_PAINT:
	    
        //GUI_DrawBitmap(&bmbkGround, 0 , 0);
        GUI_Clear();
        GUI_SetTextMode(GUI_TEXTMODE_TRANS);
	    GUI_SetColor(GUI_WHITE);
	    GUI_SetFont(&GUI_Font24_ASCII);
	    GUI_DispDecAt( TP_Slider_GetValue(WM_GetFirstChild(hWin)),400, 100, 3);
	    break;
	
	    case WM_NOTIFY_PARENT:
		    NCode = pMsg->Data.v;
		    switch (NCode) {
		    case WM_NOTIFICATION_CLICKED:
			    //GUI_MessageBox("click", "message", GUI_MESSAGEBOX_CF_MOVEABLE);
                //GUI_Delay(50);   
			    break;
		    case WM_NOTIFICATION_RELEASED:
			    //GUI_MessageBox("release", "message", GUI_MESSAGEBOX_CF_MOVEABLE);
			    break;
		    case WM_NOTIFICATION_VALUE_CHANGED:
                WM_InvalidateWindow(hWin);
			    //GUI_MessageBox("change", "message", GUI_MESSAGEBOX_CF_MOVEABLE);
                times ++;
			    break;
		    }
		    break;
#if 0
	    case WM_SET_FOCUS:
		    if (pMsg->Data.v)
		    {
			    pMsg->Data.v = 0;
		    }
		    WM_InvalidateWindow(hWin);
		    break;
#endif
        default:
            WM_DefaultProc(pMsg);
    }
}
/*********************************************************************
*
*       MainTask
*/
void MainTask(void) {
	TP_SLIDER_Handle hSlider;
    TP_SLIDER_Handle hSliderEx;
    WM_HMEM hTemp;

	WM_HMEM hTest;
    WM_HMEM hTestEx;
    GUI_COLOR bkColor;

	GUI_Init();
	hTest = WM_CreateWindow(0, 0, LCD_GetXSize(), LCD_GetYSize(), \
					WM_CF_SHOW | WM_CF_MEMDEV , Test_cbBkWindow, 0);

    hSlider   = TP_Slider_Create(30, 220, 200, 40, hTest, 0 ,NULL, 0, 0); 
    TP_Slider_SetValue(hSlider, 20);
    //TP_Slider_SetBitmap(hSlider, &bmSlider_Circle);
	while (1) 
    {
        TP_Slider_SetBitmap(hSlider, NULL);
        WM_InvalidateWindow(hTest);
        GUI_Delay(1000);
        
        TP_Slider_SetBitmap(hSlider, &bmSlider_Circle);
        WM_InvalidateWindow(hTest);
        GUI_Delay(1000); 
        /*
        TP_Slider_SetValue(hSlider, 0);
        WM_InvalidateWindow(hSlider);
        WM_InvalidateWindow(hTest);
        GUI_Delay(1000); 
        TP_Slider_SetValue(hSlider, 1);
        WM_InvalidateWindow(hSlider);
        WM_InvalidateWindow(hTest);
        GUI_Delay(1000);
        TP_Slider_SetValue(hSlider, 19);
        WM_InvalidateWindow(hSlider);
        WM_InvalidateWindow(hTest);
        GUI_Delay(1000);
        TP_Slider_SetValue(hSlider, 20);
        WM_InvalidateWindow(hSlider);
        WM_InvalidateWindow(hTest);
        GUI_Delay(1000);
        TP_Slider_SetValue(hSlider, 21);
        WM_InvalidateWindow(hSlider);
        WM_InvalidateWindow(hTest);
        GUI_Delay(1000);
        TP_Slider_SetValue(hSlider, 99);
        WM_InvalidateWindow(hSlider);
        WM_InvalidateWindow(hTest);
        GUI_Delay(1000);
        TP_Slider_SetValue(hSlider, 100);
        WM_InvalidateWindow(hSlider);
        WM_InvalidateWindow(hTest);
        GUI_Delay(1000);
        */
	}
}
#endif