/*TP_Toast.c*/

#include <string.h> // Definition of NULL
#include <stdlib.h> // malloc() and free()

#include "TP_Widget.h"
#include "TP_Toast.h"

/*设置图片测试*/
#if 1

extern GUI_CONST_STORAGE GUI_BITMAP bmTOAST_Wps;
#define DELAY_VALUE 1000

#endif

#define TP_TOAST_DEFAULT_ID	                        0
#define TP_TOAST_DEFAULT_BK_ALPHA                   100
#define TP_TOAST_DEFAULT_TEXT_ALPHA                 0
#define TP_TOAST_DEFAULT_BK_COLOR                   GUI_BLACK
#define TP_TOAST_DEFAULT_TEXT_COLOR                 GUI_WHITE
#define TP_TOAST_DEFAULT_AA_FACTOR                  4
#define TP_TOAST_DEFAULT_RADIUS                     8  
#define TP_TOAST_DEFAULT_LAST_TIME                  3000
#define TP_TOAST_DEFAULT_FADE_TIME                  500
#define TP_TOAST_DEFAULT_TEXT_FONT                  &GUI_Font24_ASCII
#define TP_TOAST_DEFAULT_TEXT_ALIGN                 GUI_TA_HCENTER|GUI_TA_VCENTER
#define TP_TOAST_DEFAULT_NUM_EXTRA_BYTES            0

#define TP_TOAST_DEFAULT_TEXT_FONT                  &GUI_Font24_ASCII
/*********************************************************************
*
*滑动按钮的默认属性值
*/
static TP_TOAST_Obj TP_Toast_Default = 
{
	TP_TOAST_DEFAULT_ID,
	
    NULL,       /*默认无背景图片*/

    {

        TP_TOAST_DEFAULT_BK_ALPHA,                   /*默认背景半透明*/
           
        TP_TOAST_DEFAULT_TEXT_ALPHA                  /*默认字体不透明*/
    },

    {
        TP_TOAST_DEFAULT_BK_COLOR,                   /*默认背景、字体颜色*/

        TP_TOAST_DEFAULT_TEXT_COLOR
    },   
    TP_TOAST_DEFAULT_AA_FACTOR,                      /*默认抗锯齿因子*/

    TP_TOAST_DEFAULT_RADIUS,                         /*默认窗口圆角半径*/

    TP_TOAST_DEFAULT_LAST_TIME,                      /*默认Toast持续时间*/

    0,                                               /*TIMER的默认记录句柄，0不具有实际意义，每次创建控件后，会被更新*/

    TP_TOAST_DEFAULT_FADE_TIME,                      /*默认Toast淡出时间*/

	TP_TOAST_DEFAULT_TEXT_FONT,

    TP_TOAST_DEFAULT_TEXT_ALIGN,                     /*默认对齐方式*/

    NULL,

    TP_TOAST_DEFAULT_NUM_EXTRA_BYTES	             /*默认额外存储空间*/
};

const GUI_FONT* TP_Toast_GetDefaultFont(void)
{
	return TP_Toast_Default.pFont;
}

const GUI_FONT* TP_Toast_SetDefaultFont(const GUI_FONT* pFont)
{
	const GUI_FONT* pPreFont = TP_Toast_Default.pFont;

	TP_Toast_Default.pFont = (GUI_FONT*)pFont;
	return pPreFont;
}

GUI_COLOR TP_Toast_GetDefaultColorIndex(int index)
{
	if (index < TP_TOAST_BKGROUND
		|| index > TP_TOAST_TEXT)
	{
		return RET_FAIL;
	}
	return TP_Toast_Default.colorIndex[index];
}

void TP_Toast_SetDefaultColorIndex(GUI_COLOR color, int index)
{
	if (index < TP_TOAST_BKGROUND
		|| index > TP_TOAST_TEXT)
	{
		return ;
	}
	TP_Toast_Default.colorIndex[index] = color;
}
U8 TP_Toast_GetDefaultToastAlpha(int index)
{
	if (index < TP_TOAST_BKGROUND
		|| index > TP_TOAST_TEXT)
	{
		return RET_FAIL;
	}
	return TP_Toast_Default.toastAlpha[index];
}

void TP_Toast_SetDefaultToastAlpha(U8 alpha, int index)
{
	if (index < TP_TOAST_BKGROUND
		|| index > TP_TOAST_TEXT)
	{
		return ;
	}
	TP_Toast_Default.toastAlpha[index] = alpha;
}

int	TP_Toast_GetDefaultTextAlign(void)
{
	return TP_Toast_Default.textAlign;
}
void	TP_Toast_SetDefaultTextAlign(int align)
{
	TP_Toast_Default.textAlign = align;
}
int	TP_Toast_GetDefaultAaFactor(void)
{
	return TP_Toast_Default.aaFactor;
}
void	TP_Toast_SetDefaultAaFactor(int factor)
{
	TP_Toast_Default.aaFactor = factor;
}
U8	TP_Toast_GetDefaultRadius(void)
{
	return TP_Toast_Default.radius;
}
void	TP_Toast_SetDefaultRadius(int radius)
{
	TP_Toast_Default.radius = radius;
}
int	TP_Toast_GetDefaultLastTime(void)
{
	return TP_Toast_Default.lastTime;
}
void	TP_Toast_SetDefaultLastTime(int time)
{
	TP_Toast_Default.lastTime = time;
}
int	TP_Toast_GetDefaultFadeTime(void)
{
	return TP_Toast_Default.fadeTime;
}
void	TP_Toast_SetDefaultFadeTime(int time)
{
	TP_Toast_Default.fadeTime = time;
}
int	TP_Toast_GetDefaultNumExtraBytes(void)
{
	return TP_Toast_Default.numExtraBytes;
}
void	TP_Toast_SetDefaultNumExtraBytes(int bytes)
{
	TP_Toast_Default.numExtraBytes = bytes;
}
int TP_Toast_GetId(TP_TOAST_Handle hObj)
{
	if (hObj) 
	{
		TP_TOAST_Obj obj;
		TP_GUI_LOCK();
		WM_GetUserData(hObj, &obj, sizeof(TP_TOAST_Obj));
		TP_GUI_UNLOCK();
		return obj.id;
	}
	return RET_FAIL;
}
void TP_Toast_SetId(TP_TOAST_Handle hObj,int id)
{
	if (hObj) 
	{
		TP_TOAST_Obj obj;

		TP_GUI_LOCK();
		WM_GetUserData(hObj, &obj, sizeof(TP_TOAST_Obj));
		obj.id = id;
		WM_SetUserData(hObj, &obj, sizeof(TP_TOAST_Obj));
		TP_GUI_UNLOCK();
	}
}
const GUI_BITMAP* TP_Toast_GetBitmap(TP_TOAST_Handle hObj) 
{
	if (hObj) 
	{
		TP_TOAST_Obj obj;
		TP_GUI_LOCK();
		WM_GetUserData(hObj, &obj, sizeof(TP_TOAST_Obj));
		return obj.pBkBitmap;
		TP_GUI_UNLOCK();
	}
    return NULL;
}
void TP_Toast_SetBitmapEx(TP_TOAST_Handle hObj, const GUI_BITMAP *pBitmap, int x, int y) 
{
	if (hObj && pBitmap) 
	{
		TP_TOAST_Obj obj;
		TP_GUI_LOCK();
		WM_GetUserData(hObj, &obj, sizeof(TP_TOAST_Obj));
		obj.pBkBitmap = pBitmap;
		WM_SetUserData(hObj, &obj, sizeof(TP_TOAST_Obj));
		TP_GUI_UNLOCK();
	}
}
void TP_Toast_SetBitmap(TP_TOAST_Handle hObj, const GUI_BITMAP *pBitmap) 
{
	TP_Toast_SetBitmapEx(hObj, pBitmap, 0, 0);
}
U8 TP_Toast_GetToastAlpha(TP_TOAST_Handle hObj,int index)
{
    if (index < TP_TOAST_BKGROUND
		|| index > TP_TOAST_TEXT)
	{
		return RET_FAIL;
	}
    if (hObj) 
	{
	    TP_TOAST_Obj obj;
        TP_GUI_LOCK();
	    WM_GetUserData(hObj, &obj, sizeof(obj));
        TP_GUI_UNLOCK();
        return obj.toastAlpha[index];
    }
    return RET_FAIL;
}
void TP_Toast_SetToastAlpha (TP_TOAST_Handle hObj, int index, U8 Alpha)
{
    if (index < TP_TOAST_BKGROUND
		|| index > TP_TOAST_TEXT)
	{
		return ;
	}
    if (hObj) 
	{
	    TP_TOAST_Obj obj;
        TP_GUI_LOCK();
	    WM_GetUserData(hObj, &obj, sizeof(obj));
	    obj.toastAlpha[index] = Alpha;
	    WM_SetUserData(hObj, &obj, sizeof(obj));
        TP_GUI_UNLOCK();
    }
}
GUI_COLOR TP_Toast_GetColorIndex(TP_TOAST_Handle hObj, int index)
{
    if (index < TP_TOAST_BKGROUND
		|| index > TP_TOAST_TEXT)
	{
		return RET_FAIL;
	}
    if (hObj) 
	{
		TP_TOAST_Obj obj;
		TP_GUI_LOCK();
		WM_GetUserData(hObj, &obj, sizeof(TP_TOAST_Obj));
		TP_GUI_UNLOCK();
        return obj.colorIndex[index];
	}
    return RET_FAIL;
}
void TP_Toast_SetColorIndex(TP_TOAST_Handle hObj, int index, GUI_COLOR color)
{
    if (index < TP_TOAST_BKGROUND
		|| index > TP_TOAST_TEXT)
	{
		return ;
	}
    if (hObj) 
	{
		TP_TOAST_Obj obj;
		TP_GUI_LOCK();
		WM_GetUserData(hObj, &obj, sizeof(TP_TOAST_Obj));
        obj.colorIndex[index] = color;
		WM_SetUserData(hObj, &obj, sizeof(TP_TOAST_Obj));
		TP_GUI_UNLOCK();
	}
}
U8 TP_Toast_GetAaFactor(TP_TOAST_Handle hObj)
{
    if (hObj) 
	{
	    TP_TOAST_Obj obj;
        TP_GUI_LOCK();
	    WM_GetUserData(hObj, &obj, sizeof(obj));
        TP_GUI_UNLOCK();
        return obj.aaFactor;
    }
    return RET_FAIL;
}
void TP_Toast_SetAaFactor(TP_TOAST_Handle hObj, U8 factor)
{
    if (hObj) 
	{
	    TP_TOAST_Obj obj;
        TP_GUI_LOCK();
	    WM_GetUserData(hObj, &obj, sizeof(obj));
	    obj.aaFactor = factor;
	    WM_SetUserData(hObj, &obj, sizeof(obj));
        TP_GUI_UNLOCK();
    }
}
U8 TP_Toast_GetRadius(TP_TOAST_Handle hObj)
{
    if (hObj) 
	{
	    TP_TOAST_Obj obj;
        TP_GUI_LOCK();
	    WM_GetUserData(hObj, &obj, sizeof(obj));
        TP_GUI_UNLOCK();
        return obj.radius;
    }
    return RET_FAIL;
}
void TP_Toast_SetRadius(TP_TOAST_Handle hObj, U8 radius)
{
    if (hObj) 
	{
	    TP_TOAST_Obj obj;
        TP_GUI_LOCK();
	    WM_GetUserData(hObj, &obj, sizeof(obj));
	    obj.radius = radius;
	    WM_SetUserData(hObj, &obj, sizeof(obj));
        TP_GUI_UNLOCK();
    }
}int TP_Toast_GetLastTime(TP_TOAST_Handle hObj)
{
    if (hObj) 
	{
	    TP_TOAST_Obj obj;
        TP_GUI_LOCK();
	    WM_GetUserData(hObj, &obj, sizeof(obj));
        TP_GUI_UNLOCK();
        return obj.lastTime;
    }
    return RET_FAIL;
}
void TP_Toast_SetLastTime(TP_TOAST_Handle hObj, int time)
{
    if (hObj) 
	{
	    TP_TOAST_Obj obj;
        TP_GUI_LOCK();
	    WM_GetUserData(hObj, &obj, sizeof(obj));
	    obj.lastTime = time;
        WM_RestartTimer(obj.hTimer, time);
	    WM_SetUserData(hObj, &obj, sizeof(obj));
        TP_GUI_UNLOCK();
    }
}
int TP_Toast_GetFadeTime(TP_TOAST_Handle hObj)
{
    if (hObj) 
	{
	    TP_TOAST_Obj obj;
        TP_GUI_LOCK();
	    WM_GetUserData(hObj, &obj, sizeof(obj));
        TP_GUI_UNLOCK();
        return obj.fadeTime;
    }
    return RET_FAIL;
}
void TP_Toast_SetFadeTime(TP_TOAST_Handle hObj, int time)
{
    if (hObj) 
	{
	    TP_TOAST_Obj obj;
        TP_GUI_LOCK();
	    WM_GetUserData(hObj, &obj, sizeof(obj));
	    obj.fadeTime = time;
	    WM_SetUserData(hObj, &obj, sizeof(obj));
        TP_GUI_UNLOCK();
    }
}

const GUI_FONT *  TP_Toast_GetFont(TP_TOAST_Handle hObj, const GUI_FONT * pFont)
{
	if (hObj) 
	{
	    TP_TOAST_Obj obj;
        TP_GUI_LOCK();
	    WM_GetUserData(hObj, &obj, sizeof(obj));
        TP_GUI_UNLOCK();
        return obj.pFont;
    }
    return NULL;
}

const GUI_FONT *  TP_Toast_SetFont(TP_TOAST_Handle hObj, const GUI_FONT * pFont)
{
    if (hObj) 
	{
	    TP_TOAST_Obj obj;
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
int TP_Toast_GetTextAlign(TP_TOAST_Handle hObj)
{
	if (hObj) 
	{
	    TP_TOAST_Obj obj;
        TP_GUI_LOCK();
	    WM_GetUserData(hObj, &obj, sizeof(obj));
        TP_GUI_UNLOCK();
        return obj.textAlign;
    }
    return RET_FAIL;
}
void TP_Toast_SetTextAlign(TP_TOAST_Handle hObj, int align)
{
	if (hObj) 
	{
	    TP_TOAST_Obj obj;
        TP_GUI_LOCK();
	    WM_GetUserData(hObj, &obj, sizeof(obj));
	    obj.textAlign = align;
	    WM_SetUserData(hObj, &obj, sizeof(obj));
        TP_GUI_UNLOCK();
    }
}
int	TP_Toast_GetText(TP_TOAST_Handle hObj, char * pBuffer, unsigned int maxLen)
{
	TP_TOAST_Obj obj;
	int len = 0;
	
    TP_GUI_LOCK();
	WM_GetUserData(hObj, &obj, sizeof(obj));
	if ((pBuffer == NULL) || (maxLen <= 0) || (obj.pText == NULL))
	{
		return RET_FAIL;
	}

	if (strlen(obj.pText) > maxLen)
	{
		len = maxLen;
	}
	else
	{
		len = strlen(obj.pText);
	}

	TP_MEMCPY(pBuffer, obj.pText, len);
    TP_GUI_UNLOCK();

	return RET_SUCCESS;
}

int TP_Toast_SetText(TP_TOAST_Handle hObj, const char* pText)
{
	TP_TOAST_Obj obj;
	int len = 0;

	TP_GUI_LOCK();
	WM_GetUserData(hObj, &obj, sizeof(obj));
	if (pText == NULL)
	{
		return RET_FAIL;
	}
	len = strlen(pText) + 1;
	
	if (obj.pText)
	{
		TP_FREE(obj.pText);
	}

	obj.pText = TP_MALLOC(len);

	if (obj.pText)
	{
		TP_MEMSET(obj.pText, 0, len);
		TP_MEMCPY(obj.pText, pText, len - 1);
	}

	WM_SetUserData(hObj, &obj, sizeof(obj));
    TP_GUI_UNLOCK();
	return RET_SUCCESS;
}
int TP_Toast_GetUserData(TP_TOAST_Handle hWin, void * pDest, int sizeOfBuffer)
{
	TP_TOAST_Obj	 obj;
	int			 numBytes;
	U8		   * pExtraBytes = NULL;

	if ((pDest == NULL) || (sizeOfBuffer <= 0))
	{
		return RET_SUCCESS;
	}
	
	WM_GetUserData(hWin, &obj, sizeof(TP_TOAST_Obj));
	pExtraBytes = (U8 *)TP_MALLOC(sizeof(TP_TOAST_Obj) + obj.numExtraBytes);
	if (pExtraBytes) 
	{
		WM_GetUserData(hWin, pExtraBytes, sizeof(TP_TOAST_Obj) + obj.numExtraBytes);
		
		if (sizeOfBuffer >= obj.numExtraBytes)
		{
			numBytes = obj.numExtraBytes;
		}
		else
		{
			numBytes = sizeOfBuffer;
		}
		
		TP_MEMCPY(pDest, pExtraBytes + sizeof(TP_TOAST_Obj), numBytes);
		TP_FREE(pExtraBytes);
		return numBytes;
	}
	return RET_SUCCESS;
}


int TP_Toast_SetUserData(TP_TOAST_Handle hWin, void * pSrc, int sizeOfBuffer)
{
	TP_TOAST_Obj	 obj;
	int			 numBytes;
	U8		   * pExtraBytes = NULL;

	if ((pSrc == NULL) || (sizeOfBuffer <= 0))
	{
		return RET_FAIL;
	}

	WM_GetUserData(hWin, &obj, sizeof(TP_TOAST_Obj));
	pExtraBytes = (U8 *)TP_MALLOC(sizeof(TP_TOAST_Obj) + obj.numExtraBytes);
	if (pExtraBytes)
	{
		WM_GetUserData(hWin, pExtraBytes, sizeof(TP_TOAST_Obj) + obj.numExtraBytes);

		if (sizeOfBuffer >= obj.numExtraBytes)
		{
		  numBytes = obj.numExtraBytes;
		}
		else
		{
		  numBytes = sizeOfBuffer;
		}
		TP_MEMSET(pExtraBytes + sizeof(TP_TOAST_Obj), 0, obj.numExtraBytes);
		TP_MEMCPY(pExtraBytes + sizeof(TP_TOAST_Obj), pSrc, numBytes);
		WM_SetUserData(hWin, pExtraBytes, sizeof(TP_TOAST_Obj) + obj.numExtraBytes);
		TP_FREE(pExtraBytes);
		return RET_SUCCESS;
	}

	return RET_FAIL;
}
void TP_Toast_Callback(WM_MESSAGE * pMsg)
{
	TP_TOAST_Handle	hWin;
	TP_TOAST_Obj	obj;
	GUI_RECT	WinRect;
    const GUI_FONT *  pOldFont;

	hWin = pMsg->hWin;
	WM_GetWindowRectEx(hWin, &WinRect); //获得当前窗口的桌面坐标
	
	GUI_MoveRect(&WinRect, -WinRect.x0, -WinRect.y0);
	WM_GetUserData(hWin, &obj, sizeof(TP_TOAST_Obj));
	
	switch (pMsg->MsgId) 
	{
		case WM_PAINT:
            if (WM_IsEnabled(hWin))
            {
                GUI_AA_SetFactor(obj.aaFactor);
                GUI_EnableAlpha(1);
    
                GUI_SetAlpha(obj.toastAlpha[TP_TOAST_BKGROUND]);
                if ( obj.pBkBitmap )
                {
                     GUI_DrawBitmap(obj.pBkBitmap,0,0);
                }
                else 
                {             
                     GUI_SetColor(obj.colorIndex[TP_TOAST_BKGROUND]);
                     GUI_FillRoundedRect(WinRect.x0, WinRect.y0, WinRect.x1, WinRect.y1, obj.radius);
                }
                GUI_SetColor (obj.colorIndex[TP_TOAST_TEXT]);                           
                GUI_SetAlpha(obj.toastAlpha[TP_TOAST_TEXT]);
                GUI_SetTextMode(GUI_TM_TRANS);

                pOldFont = GUI_GetFont();
                GUI_SetFont(obj.pFont);
                GUI_DispStringInRectWrap(obj.pText, &WinRect, obj.textAlign, GUI_WRAPMODE_WORD); 
                GUI_SetFont(pOldFont);
            }
            GUI_SetAlpha(0);
			break;
        /*
        case WM_CREATE:
            GUI_MEMDEV_FadeInWindow(hWin,2000);
            break;
        */
        case WM_TIMER:
            GUI_MEMDEV_FadeOutWindow(hWin,obj.fadeTime);
            WM_DeleteWindow(hWin);
            break;
	    case WM_GET_ID:
		    pMsg->Data.v = obj.id;
		    break;

	    case WM_SET_ID:
		    obj.id = pMsg->Data.v;
		    WM_SetUserData(hWin, &obj, sizeof(TP_TOAST_Obj));
		    break;

	    case WM_DELETE:
            TP_FREE(obj.pText);
            WM_SetUserData(hWin, &obj, sizeof(TP_TOAST_Obj));
		    break;

	    default:
		    WM_DefaultProc(pMsg);
		    break;
	}
}


TP_TOAST_Handle TP_Toast_Create(int x0, int y0, int xSize, int ySize,               \
								WM_HWIN hWinParent, U32 style, char *pText,         \
								WM_CALLBACK * pfCallback, int numExtraBytes, int id)               
{
	TP_TOAST_Handle	handle;
	TP_TOAST_Obj		obj;
	WM_CALLBACK     	*pCb;
    WM_HTIMER hTimer;

	if (pfCallback)
	{
		pCb = pfCallback;
	}
	else
	{
		pCb = TP_Toast_Callback;
	}

	obj	= TP_Toast_Default;	
	obj.id = id;

    if (pText)
    {
        obj.pText = pText;
    }
	
    obj.numExtraBytes = numExtraBytes;

	handle = WM_CreateWindowAsChild(x0, y0, xSize, ySize, hWinParent, \
		                            style | WM_CF_SHOW | WM_CF_HASTRANS | WM_CF_MEMDEV, \
                                    pCb, sizeof(TP_TOAST_Obj) + numExtraBytes);
    hTimer = WM_CreateTimer(handle, 0, obj.lastTime, 0);
    obj.hTimer = hTimer;

	WM_SetUserData(handle, &obj, sizeof(TP_TOAST_Obj));

	return handle;	
}