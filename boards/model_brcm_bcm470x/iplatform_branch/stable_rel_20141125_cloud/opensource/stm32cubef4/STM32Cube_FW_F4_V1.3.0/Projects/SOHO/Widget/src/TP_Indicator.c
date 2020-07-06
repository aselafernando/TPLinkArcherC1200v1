/*TP_Indicator.c*/

#include <string.h> // Definition of NULL
#include <stdlib.h> // malloc() and free()

#include "TP_Widget.h"
#include "TP_Indicator.h"

/*设置图片测试*/
#if 1

extern GUI_CONST_STORAGE GUI_BITMAP bmINDICATOR_Wps;
extern GUI_CONST_STORAGE GUI_BITMAP bmINDICATOR_Highlight;
extern GUI_CONST_STORAGE GUI_BITMAP bmINDICATOR_Dark;
extern GUI_CONST_STORAGE GUI_BITMAP bmINDICATOR_BkGround;

#define DELAY_VALUE 1000

#endif

#define TP_INDICATOR_DEFAULT_ID	                    0
#define TP_INDICATOR_DEFAULT_HIGHLIGHT_ALPHA        0
#define TP_INDICATOR_DEFAULT_DARK_ALPHA             100
#define TP_INDICATOR_DEFAULT_BKGROUND_ALPHA         0
#define TP_INDICATOR_DEFAULT_HIGHLIGHT_COLOR        GUI_WHITE
#define TP_INDICATOR_DEFAULT_DARK_COLOR             GUI_DARKGRAY
#define TP_INDICATOR_DEFAULT_BKGROUND_COLOR         GUI_CYAN
#define TP_INDICATOR_DEFAULT_AA_FACTOR	            4
#define TP_INDICATOR_DEFAULT_PAGE	                1
#define TP_INDICATOR_DEFAULT_NUM                    3
#define TP_INDICATOR_DEFAULT_SIZE	                8
#define TP_INDICATOR_DEFAULT_SPACE                  10

/*********************************************************************
*
*滑动按钮的默认属性值
*/
static TP_INDICATOR_Obj TP_Indicator_Default = 
{
	TP_INDICATOR_DEFAULT_ID,
	
    {NULL,NULL,NULL},   /*默认无背景图片*/

    {
        TP_INDICATOR_DEFAULT_HIGHLIGHT_ALPHA,                  /*默认选中前景不透明*/

        TP_INDICATOR_DEFAULT_DARK_ALPHA ,                /*默认未选中前景半透明*/

        TP_INDICATOR_DEFAULT_BKGROUND_ALPHA                  /*默认背景不透明*/
    },
    {
        TP_INDICATOR_DEFAULT_HIGHLIGHT_COLOR,
        TP_INDICATOR_DEFAULT_DARK_COLOR,
        TP_INDICATOR_DEFAULT_BKGROUND_COLOR
    },    /*默认控件的指示、非指示、背景颜色*/

    TP_INDICATOR_DEFAULT_AA_FACTOR,                  /*默认抗锯齿因子*/
	
	TP_INDICATOR_DEFAULT_PAGE,	                /*默认当前指示第一页*/
	
	TP_INDICATOR_DEFAULT_NUM,				    /*默认存在3个页面*/

    TP_INDICATOR_DEFAULT_SIZE,                  /*默认指示器直径大小*/

    TP_INDICATOR_DEFAULT_SPACE,                 /*默认指示器圆点间距*/

    0
	
};

GUI_COLOR TP_Indicator_GetDefaultColorIndex(int index)
{
	if (index < TP_INDICATOR_HIGHLIGHT    \
		|| index > TP_INDICATOR_BKGROUND)
	{
		return RET_FAIL;
	}
	return TP_Indicator_Default.colorIndex[index];
}
void TP_Indicator_SetDefaultColorIndex(GUI_COLOR color, int index)
{
	if (index < TP_INDICATOR_HIGHLIGHT    \
		|| index > TP_INDICATOR_BKGROUND)
	{
		return ;
	}
	TP_Indicator_Default.colorIndex[index] = color;
}
U8 TP_Indicator_GetDefaultIndicatorAlpha(int index)
{
	if (index <TP_INDICATOR_HIGHLIGHT   \
		|| index > TP_INDICATOR_BKGROUND)
	{
		return RET_FAIL;
	}
	return TP_Indicator_Default.indicatorAlpha[index];
}
void TP_Indicator_SetDefaultIndicatorAlpha(U8 alpha, int index)
{
	if (index < TP_INDICATOR_HIGHLIGHT    \
		|| index > TP_INDICATOR_BKGROUND)
	{
		return ;
	}
	TP_Indicator_Default.indicatorAlpha[index] = alpha;
}
U8	TP_Indicator_GetDefaultAaFactor(void)
{
	return TP_Indicator_Default.aaFactor;
}
void	TP_Indicator_SetDefaultAaFactor(U8 factor)
{
	TP_Indicator_Default.aaFactor = factor;
}
U8	TP_Indicator_GetDefaultPage(void)
{
	return TP_Indicator_Default.page;
}
void	TP_Indicator_SetDefaultPage(U8 page)
{
	TP_Indicator_Default.page = page;
}
U8	TP_Indicator_GetDefaultNum(void)
{
	return TP_Indicator_Default.num;
}
void	TP_Indicator_SetDefaultNum(U8 num)
{
	TP_Indicator_Default.num = num;
}
U8	TP_Indicator_GetDefaultSize(void)
{
	return TP_Indicator_Default.size;
}
void	TP_Indicator_SetDefaultSize(U8 size)
{
	TP_Indicator_Default.size = size;
}
U8	TP_Indicator_GetDefaultSpace(void)
{
	return TP_Indicator_Default.space;
}
void	TP_Indicator_SetDefaultSpace(U8 space)
{
	TP_Indicator_Default.space = space;
}

int TP_Indicator_GetId(TP_INDICATOR_Handle hObj)
{
	if (hObj) 
	{
		TP_INDICATOR_Obj obj;
		TP_GUI_LOCK();
		WM_GetUserData(hObj, &obj, sizeof(TP_INDICATOR_Obj));
		TP_GUI_UNLOCK();
		return obj.id;
	}
	return RET_FAIL;
}
void TP_Indicator_SetId(TP_INDICATOR_Handle hObj,int id)
{
	if (hObj) 
	{
		TP_INDICATOR_Obj obj;

		TP_GUI_LOCK();
		WM_GetUserData(hObj, &obj, sizeof(TP_INDICATOR_Obj));
		obj.id = id;
		WM_SetUserData(hObj, &obj, sizeof(TP_INDICATOR_Obj));
		TP_GUI_UNLOCK();
	}
}
const GUI_BITMAP* TP_Indicator_GetBitmap(TP_INDICATOR_Handle hObj, int index) 
{
    if (index < TP_INDICATOR_HIGHLIGHT  \
		|| index > TP_INDICATOR_BKGROUND )
	{
		return NULL;
	}
	if (hObj) 
	{
		TP_INDICATOR_Obj obj;
		TP_GUI_LOCK();
		WM_GetUserData(hObj, &obj, sizeof(TP_INDICATOR_Obj));
		return obj.pBitmap[index];
		TP_GUI_UNLOCK();
	}
    return NULL;
}
void TP_Indicator_SetBitmapEx(TP_INDICATOR_Handle hObj, int index, const GUI_BITMAP *pBitmap, int x, int y) 
{
    if (index < TP_INDICATOR_HIGHLIGHT   \
		|| index > TP_INDICATOR_BKGROUND)
	{
		return ;
	}
	if (hObj && pBitmap) 
	{
		TP_INDICATOR_Obj obj;
		TP_GUI_LOCK();
		WM_GetUserData(hObj, &obj, sizeof(TP_INDICATOR_Obj));
		obj.pBitmap[index] = pBitmap;
		WM_SetUserData(hObj, &obj, sizeof(TP_INDICATOR_Obj));
		TP_GUI_UNLOCK();
	}
}

void TP_Indicator_SetBitmap(TP_INDICATOR_Handle hObj, int index, const GUI_BITMAP *pBitmap) 
{
	TP_Indicator_SetBitmapEx(hObj, index, pBitmap, 0, 0);
}

GUI_COLOR TP_Indicator_GetColorIndex(TP_INDICATOR_Handle hObj, int index)
{
    if (index < TP_INDICATOR_HIGHLIGHT  \
		|| index > TP_INDICATOR_BKGROUND)
	{
		return RET_FAIL;
	}
    if (hObj) 
	{
		TP_INDICATOR_Obj obj;
		TP_GUI_LOCK();
		WM_GetUserData(hObj, &obj, sizeof(TP_INDICATOR_Obj));
		TP_GUI_UNLOCK();
        return obj.colorIndex[index];
	}
    return RET_FAIL;
}
void TP_Indicator_SetColorIndex(TP_INDICATOR_Handle hObj, int index, GUI_COLOR color)
{
    if (index < TP_INDICATOR_HIGHLIGHT  \
		|| index > TP_INDICATOR_BKGROUND)
	{
		return ;
	}
    if (hObj) 
	{
		TP_INDICATOR_Obj obj;
		TP_GUI_LOCK();
		WM_GetUserData(hObj, &obj, sizeof(TP_INDICATOR_Obj));
        obj.colorIndex[index] = color;
		WM_SetUserData(hObj, &obj, sizeof(TP_INDICATOR_Obj));
		TP_GUI_UNLOCK();
	}
}
U8 TP_Indicator_GetAaFactor(TP_INDICATOR_Handle hObj)
{
    if (hObj) 
	{
	    TP_INDICATOR_Obj obj;
        TP_GUI_LOCK();
	    WM_GetUserData(hObj, &obj, sizeof(obj));
        TP_GUI_UNLOCK();
        return obj.aaFactor;
    }
    return RET_FAIL;
}
void TP_Indicator_SetAaFactor(TP_INDICATOR_Handle hObj, U8 factor)
{
    if (hObj) 
	{
	    TP_INDICATOR_Obj obj;
        TP_GUI_LOCK();
	    WM_GetUserData(hObj, &obj, sizeof(obj));
	    obj.aaFactor = factor;
	    WM_SetUserData(hObj, &obj, sizeof(obj));
        TP_GUI_UNLOCK();
        WM_InvalidateWindow(hObj);
    }
}

U8 TP_Indicator_GetIndicatorAlpha(TP_INDICATOR_Handle hObj,int index)
{
     if (index < TP_INDICATOR_HIGHLIGHT   \
		|| index > TP_INDICATOR_BKGROUND)
	{
		return RET_FAIL;
	}
    if (hObj) 
	{
	    TP_INDICATOR_Obj obj;
        TP_GUI_LOCK();
	    WM_GetUserData(hObj, &obj, sizeof(obj));
        TP_GUI_UNLOCK();
        return obj.indicatorAlpha[index];
    }
    return RET_FAIL;
}
void TP_Indicator_SetIndicatorAlpha (TP_INDICATOR_Handle hObj, int index, U8 Alpha)
{
     if (index < TP_INDICATOR_HIGHLIGHT   \
		|| index > TP_INDICATOR_BKGROUND)
	{
		return ;
	}
    if (hObj) 
	{
	    TP_INDICATOR_Obj obj;
        TP_GUI_LOCK();
	    WM_GetUserData(hObj, &obj, sizeof(obj));
	    obj.indicatorAlpha[index] = Alpha;
	    WM_SetUserData(hObj, &obj, sizeof(obj));
        TP_GUI_UNLOCK();
        WM_InvalidateWindow(hObj);
    }
}
U8 TP_Indicator_GetPage(TP_INDICATOR_Handle hObj)
{
    if (hObj) 
	{
	    TP_INDICATOR_Obj obj;
        TP_GUI_LOCK();
	    WM_GetUserData(hObj, &obj, sizeof(obj));
        TP_GUI_UNLOCK();
        return obj.page;
    }
    return RET_FAIL;
}
void TP_Indicator_SetPage (TP_INDICATOR_Handle hObj, U8 page)
{
    if (hObj) 
	{
	    TP_INDICATOR_Obj obj;
        TP_GUI_LOCK();
	    WM_GetUserData(hObj, &obj, sizeof(obj));
	    obj.page = page;
	    WM_SetUserData(hObj, &obj, sizeof(obj));
        TP_GUI_UNLOCK();
        WM_InvalidateWindow(hObj);
    }
}
U8 TP_Indicator_GetNum(TP_INDICATOR_Handle hObj)
{
    if (hObj) 
	{
	    TP_INDICATOR_Obj obj;
        TP_GUI_LOCK();
	    WM_GetUserData(hObj, &obj, sizeof(obj));
        TP_GUI_UNLOCK();
        return obj.num;
    }
    return RET_FAIL;
}
void TP_Indicator_SetNum (TP_INDICATOR_Handle hObj, U8 num)
{
    if (hObj) 
	{
	    TP_INDICATOR_Obj obj;
        TP_GUI_LOCK();
	    WM_GetUserData(hObj, &obj, sizeof(obj));
	    obj.num = num;
	    WM_SetUserData(hObj, &obj, sizeof(obj));
        TP_GUI_UNLOCK();
        WM_InvalidateWindow(hObj);
    }
}
U8 TP_Indicator_GetSize(TP_INDICATOR_Handle hObj)
{
    if (hObj) 
	{
	    TP_INDICATOR_Obj obj;
        TP_GUI_LOCK();
	    WM_GetUserData(hObj, &obj, sizeof(obj));
        TP_GUI_UNLOCK();
        return obj.size;
    }
    return RET_FAIL;
}
void TP_Indicator_SetSize (TP_INDICATOR_Handle hObj, U8 size)
{
    if (hObj) 
	{
	    TP_INDICATOR_Obj obj;
        TP_GUI_LOCK();
	    WM_GetUserData(hObj, &obj, sizeof(obj));
	    obj.size = size;
	    WM_SetUserData(hObj, &obj, sizeof(obj));
        TP_GUI_UNLOCK();
        WM_InvalidateWindow(hObj);
    }
}
U8 TP_Indicator_GetSpace(TP_INDICATOR_Handle hObj)
{
    if (hObj) 
	{
	    TP_INDICATOR_Obj obj;
        TP_GUI_LOCK();
	    WM_GetUserData(hObj, &obj, sizeof(obj));
        TP_GUI_UNLOCK();
        return obj.space;
    }
    return RET_FAIL;
}
void TP_Indicator_SetSpace (TP_INDICATOR_Handle hObj, U8 space)
{
    if (hObj) 
	{
	    TP_INDICATOR_Obj obj;
        TP_GUI_LOCK();
	    WM_GetUserData(hObj, &obj, sizeof(obj));
	    obj.space = space;
	    WM_SetUserData(hObj, &obj, sizeof(obj));
        TP_GUI_UNLOCK();
        WM_InvalidateWindow(hObj);
    }
}

int TP_Indicator_GetUserData(TP_INDICATOR_Handle hWin, void * pDest, int sizeOfBuffer)
{
	TP_INDICATOR_Obj	 obj;
	int			 numBytes;
	U8		   * pExtraBytes = NULL;

	if ((pDest == NULL) || (sizeOfBuffer <= 0))
	{
		return RET_SUCCESS;
	}
	
	WM_GetUserData(hWin, &obj, sizeof(TP_INDICATOR_Obj));
	pExtraBytes = (U8 *)TP_MALLOC(sizeof(TP_INDICATOR_Obj) + obj.numExtraBytes);
	if (pExtraBytes) 
	{
		WM_GetUserData(hWin, pExtraBytes, sizeof(TP_INDICATOR_Obj) + obj.numExtraBytes);
		
		if (sizeOfBuffer >= obj.numExtraBytes)
		{
			numBytes = obj.numExtraBytes;
		}
		else
		{
			numBytes = sizeOfBuffer;
		}
		
		TP_MEMCPY(pDest, pExtraBytes + sizeof(TP_INDICATOR_Obj), numBytes);
		TP_FREE(pExtraBytes);
		return numBytes;
	}
	return RET_SUCCESS;
}


int TP_Indicator_SetUserData(TP_INDICATOR_Handle hWin, void * pSrc, int sizeOfBuffer)
{
	TP_INDICATOR_Obj	 obj;
	int			 numBytes;
	U8		   * pExtraBytes = NULL;

	if ((pSrc == NULL) || (sizeOfBuffer <= 0))
	{
		return RET_FAIL;
	}

	WM_GetUserData(hWin, &obj, sizeof(TP_INDICATOR_Obj));
	pExtraBytes = (U8 *)TP_MALLOC(sizeof(TP_INDICATOR_Obj) + obj.numExtraBytes);
	if (pExtraBytes)
	{
		WM_GetUserData(hWin, pExtraBytes, sizeof(TP_INDICATOR_Obj) + obj.numExtraBytes);

		if (sizeOfBuffer >= obj.numExtraBytes)
		{
		  numBytes = obj.numExtraBytes;
		}
		else
		{
		  numBytes = sizeOfBuffer;
		}
		TP_MEMSET(pExtraBytes + sizeof(TP_INDICATOR_Obj), 0, obj.numExtraBytes);
		TP_MEMCPY(pExtraBytes + sizeof(TP_INDICATOR_Obj), pSrc, numBytes);
		WM_SetUserData(hWin, pExtraBytes, sizeof(TP_INDICATOR_Obj) + obj.numExtraBytes);
		TP_FREE(pExtraBytes);
		return RET_SUCCESS;
	}

	return RET_FAIL;
}
void TP_Indicator_Callback(WM_MESSAGE * pMsg)
{
    U8 num;
    U8 page;
    U8 xEdge;
    U8 yEdge;
    U8 index;

	TP_INDICATOR_Handle	hWin;
	TP_INDICATOR_Obj	obj;
	GUI_RECT	WinRect;	

	hWin = pMsg->hWin;
	WM_GetWindowRectEx(hWin, &WinRect); //获得当前窗口的桌面坐标
	
	GUI_MoveRect(&WinRect, -WinRect.x0, -WinRect.y0);
	WM_GetUserData(hWin, &obj, sizeof(TP_INDICATOR_Obj));
	
	switch (pMsg->MsgId) 
	{
		case WM_PAINT:
            if (WM_IsEnabled(hWin))
            {
                GUI_EnableAlpha(1);
                GUI_SetAlpha(obj.indicatorAlpha[TP_INDICATOR_BKGROUND]);
                if ( obj.pBitmap[TP_INDICATOR_BKGROUND] )
                {
                     GUI_DrawBitmap(obj.pBitmap[TP_INDICATOR_BKGROUND],0,0);
                }
                else 
                {
                     GUI_SetColor(obj.colorIndex[TP_INDICATOR_BKGROUND]);
                     GUI_FillRoundedRect(WinRect.x0, WinRect.y0, WinRect.x1, WinRect.y1,8);
                }

                num = 0;
                page = obj.page;
                
                if ( obj.pBitmap[TP_INDICATOR_HIGHLIGHT] && obj.pBitmap[TP_INDICATOR_DARK] )
                {
                    xEdge = (WinRect.x1 - ( obj.num*(obj.pBitmap[0]->XSize) + (obj.num-1)*obj.space) ) / 2;
                    yEdge = (WinRect.y1 - (obj.pBitmap[0]->YSize)) / 2;
                    while ( num < obj.num )
                    {
                        if ( (num + 1)  == page )
                        {                          
                            index =  TP_INDICATOR_HIGHLIGHT;                  
                        }
                        else 
                        {                          
                            index =  TP_INDICATOR_DARK; 
                        }
                        GUI_SetAlpha  (obj.indicatorAlpha[index]);
                        GUI_DrawBitmap( obj.pBitmap[index], xEdge + num*(obj.space + obj.pBitmap[index]->XSize), yEdge );
                        num ++;
                    }  

                }
                else 
                {
                    xEdge = (WinRect.x1 - (obj.num*obj.size + (obj.num-1)*obj.space) ) / 2;
                    yEdge = (WinRect.y1 - obj.size) / 2;
                    while ( num < obj.num )
                    {
                        GUI_SetAlpha(0);
                        if ( (num + 1)  == page )
                        {
                            index =  TP_INDICATOR_HIGHLIGHT;                  
                        }
                        else 
                        {
                            index =  TP_INDICATOR_DARK;                        
                        }
                        GUI_SetAlpha    (obj.indicatorAlpha[index]);
                        GUI_SetColor    (obj.colorIndex[index]);                           
                        //GUI_AA_SetFactor(obj.aaFactor);

                        GUI_AA_FillCircle( xEdge + obj.size/2 + num*(obj.space + obj.size)  , \
                                        yEdge + obj.size/2, obj.size/2);

                        num ++;
                    }  
                }
            }
            GUI_SetAlpha(0);
			break;

	    case WM_GET_ID:
		    pMsg->Data.v = obj.id;
		    break;

	    case WM_SET_ID:
		    obj.id = pMsg->Data.v;
		    WM_SetUserData(hWin, &obj, sizeof(TP_INDICATOR_Obj));
		    break;

	    case WM_DELETE:
		    break;

	    default:
		    WM_DefaultProc(pMsg);
		    break;
	}
}


TP_INDICATOR_Handle TP_Indicator_Create(int x0, int y0, int xSize, int ySize,   \
								WM_HWIN hWinParent, U32 style,                  \
								WM_CALLBACK * pfCallback, int numExtraBytes, int id)               
{
	TP_INDICATOR_Handle	handle;
	TP_INDICATOR_Obj		obj;
	WM_CALLBACK     	*pCb;

	if (pfCallback)
	{
		pCb = pfCallback;
	}
	else
	{
		pCb = TP_Indicator_Callback;
	}

	obj	= TP_Indicator_Default;	
	obj.id = id;
		
	handle = WM_CreateWindowAsChild(x0, y0, xSize, ySize, hWinParent, \
		                                    style | WM_CF_SHOW | WM_CF_HASTRANS | WM_CF_MEMDEV, \
                                            pCb, sizeof(TP_INDICATOR_Obj) + numExtraBytes);
  
	WM_SetUserData(handle, &obj, sizeof(TP_INDICATOR_Obj));

	return handle;
	
}