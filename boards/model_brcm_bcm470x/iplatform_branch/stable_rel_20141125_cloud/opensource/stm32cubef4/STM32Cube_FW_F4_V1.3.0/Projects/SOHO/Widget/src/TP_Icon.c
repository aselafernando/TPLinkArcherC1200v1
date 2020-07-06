#include "TP_Icon.h"
#include "TP_Widget.h"


#define TP_ICON_DEFAULT_ID	(0)

#define TP_ICON_TEXT_WORD_NUM_MAX   5


/*********************************************************************
*
* ICON的默认属性值
*/
static TP_ICON_Obj TP_ICON_Default = 
{
	TP_ICON_DEFAULT_ID,                     /* 控件id */
    0,                                     /* 判断是否按下 */
    0,	                                    /* 是否接收输入焦点 */
    {GUI_LIGHTBLUE, GUI_LIGHTBLUE, GUI_LIGHTBLUE},    /* 三种状态下icon的背景颜色 */
    {NULL, NULL, NULL},                     /* 三种状态下icon对应的位图 */
    TP_ICON_BMP_ALIGN_HCENTER | TP_ICON_BMP_ALIGN_TOP,  /* IconBitmap的对齐方式 */
    {GUI_WHITE, GUI_WHITE, GUI_WHITE},      /* 三种状态下的Text颜色 */
    NULL,                                   /* icon的文本 */
    GUI_DEFAULT_FONT,	                    /*文本的字体*/
    GUI_TA_CENTER | GUI_TA_BOTTOM,          /* 文本的对齐方式 */
    10,		                                /* 位图与文本之间的距离 */
    5,                                                  /* 文本的行距 */
    0			                            /* 额外的存储空间数量 */
};


int TP_ICON_GetDefaultFocussable()
{
    return TP_ICON_Default.focussable;
}

void TP_ICON_SetDefaultFocussable(int state)
{
    TP_ICON_Default.focussable = state;
}

GUI_COLOR TP_ICON_GetDefaultBkColor(int index)
{
    if (index < 0 || index >= TP_ICON_STATE_NUM_MAX)
	{
		return GUI_INVALID_COLOR;
	}
    
	return TP_ICON_Default.iconBkColor[index];
}

void TP_ICON_SetDefaultBkColor( int index, GUI_COLOR color)
{
    if (index < 0 || index >= TP_ICON_STATE_NUM_MAX)
	{
		return;
	}
    
	TP_ICON_Default.iconBkColor[index] = color;
}

const GUI_BITMAP * TP_ICON_GetDefaultIconBitmap(int index)
{
    const GUI_BITMAP *pBitmap = NULL;
	if ((index >= 0) && (index < TP_ICON_STATE_NUM_MAX))
	{
		pBitmap = TP_ICON_Default.pIconBitmap[index];
	}

	return pBitmap;
}

void TP_ICON_SetDefaultIconBitmap(int index, const GUI_BITMAP *pBitmap)
{
	if ((index >= 0) && (index < TP_ICON_STATE_NUM_MAX))
	{
		TP_ICON_Default.pIconBitmap[index] = pBitmap;
	}
}

int TP_ICON_GetDefaultiIconBitmapAlign()
{
    return TP_ICON_Default.iconBitmapAlign;
}

void TP_ICON_SetDefaultIconBitmapAlign(int bitmapAlign)
{
    TP_ICON_Default.iconBitmapAlign = bitmapAlign;
}

GUI_COLOR TP_ICON_GetDefaultTextColor(int index)
{
    if (index < 0 || index >= TP_ICON_STATE_NUM_MAX)
	{
		return GUI_INVALID_COLOR;
	}
    
	return TP_ICON_Default.aTextColor[index];
}

void TP_ICON_SetDefaultTextColor( int index, GUI_COLOR color)
{
    if (index < 0 || index >= TP_ICON_STATE_NUM_MAX)
	{
		return;
	}
    
	TP_ICON_Default.aTextColor[index] = color;
}

const GUI_FONT * TP_ICON_GetDefaultFont()
{
    return TP_ICON_Default.pFont;
}

void TP_ICON_SetDefaultFont(const GUI_FONT* pFont)
{
    TP_ICON_Default.pFont = pFont;
}

int TP_ICON_GetDefaultTextAlign()
{
    return TP_ICON_Default.textAlign;
}

void TP_ICON_SetDefaultTextAlign(int textAlign)
{
    TP_ICON_Default.textAlign = textAlign;
}

int TP_ICON_GetDefaultSpace()
{
    return TP_ICON_Default.space;
}

void TP_ICON_SetDefaultSpace(int space)
{
    TP_ICON_Default.space = space;
}

int TP_ICON_GetDefaultLineSpace()
{
    return TP_ICON_Default.lineSpace;
}

void TP_ICON_SetDefaultLineSpace(int lineSpace)
{
    TP_ICON_Default.lineSpace = lineSpace;
}




/*********************************************************************
*
* get/set  attribute value
*/
int TP_ICON_GetId(TP_ICON_Handle hObj)
{
    if (hObj) 
	{
		TP_ICON_Obj obj;

		TP_GUI_LOCK();
		WM_GetUserData(hObj, &obj, sizeof(TP_ICON_Obj));
		TP_GUI_UNLOCK();
		return obj.id;
	}
    
	return -1;
}

int TP_ICON_GetPressedState(TP_ICON_Handle hObj)
{
    TP_ICON_Obj obj;
    WM_GetUserData(hObj, &obj, sizeof(obj));

    return obj.pressed;   
}

void TP_ICON_SetPressedState(TP_ICON_Handle hObj, int state)
{
    TP_ICON_Obj obj;
    TP_GUI_LOCK();
    WM_GetUserData(hObj, &obj, sizeof(obj));
    obj.pressed = state;
    WM_SetUserData(hObj, &obj, sizeof(obj));
    TP_GUI_UNLOCK();
}

int TP_ICON_GetFocussable(TP_ICON_Handle hObj)
{
    TP_ICON_Obj obj;
    WM_GetUserData(hObj, &obj, sizeof(obj));

    return obj.focussable;   
}

void TP_ICON_SetFocussable(TP_ICON_Handle hObj, int state)
{
    if (hObj) 
    {
        TP_ICON_Obj obj;
        TP_GUI_LOCK();
	    WM_GetUserData(hObj, &obj, sizeof(obj));
	    obj.focussable = state;
	    WM_SetUserData(hObj, &obj, sizeof(obj));
        TP_GUI_UNLOCK();
    }
}

GUI_COLOR TP_ICON_GetBkColor(TP_ICON_Handle hObj, int index)
{
    TP_ICON_Obj obj;

	if (index < 0 || index >= TP_ICON_STATE_NUM_MAX)
	{
		return GUI_INVALID_COLOR;
	}
	
	WM_GetUserData(hObj, &obj, sizeof(obj));

	return obj.iconBkColor[index];
}


void  TP_ICON_SetBkColor(TP_ICON_Handle hObj, int index, GUI_COLOR color)
{
    if (hObj) 
    {
        TP_ICON_Obj obj;
            
        if (index < 0 || index >= TP_ICON_STATE_NUM_MAX)
        {
            return;
        }

        TP_GUI_LOCK();
        WM_GetUserData(hObj, &obj, sizeof(obj));
        obj.iconBkColor[index] = color;
        WM_SetUserData(hObj, &obj, sizeof(obj));
        TP_GUI_UNLOCK();
    }
}

const GUI_BITMAP * TP_ICON_GetIconBitmap(TP_ICON_Handle hObj, int index)
{
    TP_ICON_Obj obj;

	if (index < 0 || index >= TP_ICON_STATE_NUM_MAX)
	{
		return NULL;
	}
	
	WM_GetUserData(hObj, &obj, sizeof(obj));

	return obj.pIconBitmap[index];
}


void TP_ICON_SetIconBitmap(TP_ICON_Handle hObj, int index, const GUI_BITMAP *pBitmap)
{
    if (hObj) 
    {
        TP_ICON_Obj obj;
            
        if (index < 0 || index >= TP_ICON_STATE_NUM_MAX)
        {
            return;
        }

        TP_GUI_LOCK();
        WM_GetUserData(hObj, &obj, sizeof(obj));
        obj.pIconBitmap[index] = pBitmap;
        WM_SetUserData(hObj, &obj, sizeof(obj));
        TP_GUI_UNLOCK();
    }
}

int TP_ICON_GetIconBitmapAlign(TP_ICON_Handle hObj)
{
    TP_ICON_Obj obj;
    WM_GetUserData(hObj, &obj, sizeof(obj));

    return obj.iconBitmapAlign;   
}

void TP_ICON_SetIconBitmapAlign(TP_ICON_Handle hObj, int bitmapAlign)
{
    if (hObj) 
    {
        TP_ICON_Obj obj;
        TP_GUI_LOCK();
	    WM_GetUserData(hObj, &obj, sizeof(obj));
	    obj.iconBitmapAlign = bitmapAlign;
	    WM_SetUserData(hObj, &obj, sizeof(obj));
        TP_GUI_UNLOCK();
    }
}

GUI_COLOR TP_ICON_GetTextColor(TP_ICON_Handle hObj, int index)
{
    TP_ICON_Obj obj;

	if (index < 0 || index >= TP_ICON_STATE_NUM_MAX)
	{
		return GUI_INVALID_COLOR;
	}
	
	WM_GetUserData(hObj, &obj, sizeof(obj));

	return obj.aTextColor[index];
}

void TP_ICON_SetTextColor(TP_ICON_Handle hObj, int index, GUI_COLOR color)
{
    if (hObj) 
    {
        TP_ICON_Obj obj;
            
        if (index < 0 || index >= TP_ICON_STATE_NUM_MAX)
        {
            return;
        }

        TP_GUI_LOCK();
        WM_GetUserData(hObj, &obj, sizeof(obj));
        obj.aTextColor[index] = color;
        WM_SetUserData(hObj, &obj, sizeof(obj));
        TP_GUI_UNLOCK();
    }
}


STATUS TP_ICON_GetText(TP_ICON_Handle hObj, char * pBuffer, unsigned int maxLen)
{
    TP_ICON_Obj obj;
	int len = 0;
	
	WM_GetUserData(hObj, &obj, sizeof(obj));
	if ((pBuffer == NULL) || (maxLen <= 0) || (obj.pText  == NULL))
	{
		return RET_FAIL;
	}

	if(strlen(obj.pText) > maxLen)
	{
		len = maxLen;
	}
	else
	{
		len = strlen(obj.pText);
	}

	TP_MEMCPY(pBuffer, obj.pText, len);
    
    return RET_SUCCESS;
}

void TP_ICON_SetText(TP_ICON_Handle hObj, const char * pText)
{
    TP_ICON_Obj obj;
	int len = 0;
	
	WM_GetUserData(hObj, &obj, sizeof(obj));
	if (pText == NULL)
	{
		return;
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
}

const GUI_FONT * TP_ICON_GetTextFont(TP_ICON_Handle hObj)
{
    TP_ICON_Obj obj;
    WM_GetUserData(hObj, &obj, sizeof(obj));

    return obj.pFont;   
}

void TP_ICON_SetTextFont(TP_ICON_Handle hObj, const GUI_FONT * pFont)
{
    if (hObj) 
    {
        TP_ICON_Obj obj;
        TP_GUI_LOCK();
	    WM_GetUserData(hObj, &obj, sizeof(obj));
	    obj.pFont = pFont;
	    WM_SetUserData(hObj, &obj, sizeof(obj));
        TP_GUI_UNLOCK();
    }
}


int TP_ICON_GetTextAlign(TP_ICON_Handle hObj)
{
    TP_ICON_Obj obj;
    WM_GetUserData(hObj, &obj, sizeof(obj));

    return obj.textAlign; 
}

void TP_ICON_SetTextAlign(TP_ICON_Handle hObj, int textAlign)
{
    if (hObj) 
    {
        TP_ICON_Obj obj;
        TP_GUI_LOCK();
	    WM_GetUserData(hObj, &obj, sizeof(obj));
	    obj.textAlign = textAlign;
	    WM_SetUserData(hObj, &obj, sizeof(obj));
        TP_GUI_UNLOCK();
    }
}

int TP_ICON_GetSpace(TP_ICON_Handle hObj)
{
    TP_ICON_Obj obj;
    WM_GetUserData(hObj, &obj, sizeof(obj));

    return obj.space;   
}
void TP_ICON_SetSpace(TP_ICON_Handle hObj, int space)
{
    if (hObj) 
    {
        TP_ICON_Obj obj;
        TP_GUI_LOCK();
        WM_GetUserData(hObj, &obj, sizeof(obj));
        obj.space = space;
        WM_SetUserData(hObj, &obj, sizeof(obj));
        TP_GUI_UNLOCK();
    }
}

int TP_ICON_GetLineSpace(TP_ICON_Handle hObj)
{
    TP_ICON_Obj obj;
    WM_GetUserData(hObj, &obj, sizeof(obj));

    return obj.lineSpace;   
}
void TP_ICON_SetLineSpace(TP_ICON_Handle hObj, int lineSpace)
{
    if (hObj) 
    {
        TP_ICON_Obj obj;
        TP_GUI_LOCK();
        WM_GetUserData(hObj, &obj, sizeof(obj));
        obj.lineSpace = lineSpace;
        WM_SetUserData(hObj, &obj, sizeof(obj));
        TP_GUI_UNLOCK();
    }
}

int TP_ICON_GetUserData(TP_ICON_Handle hWin, void * pDest, int sizeOfBuffer)
{
    TP_ICON_Obj	 obj;
	int			 numBytes;
	U8	        *pExtraBytes = NULL;

	if ((pDest == NULL) || (sizeOfBuffer <= 0))
	{
		return 0;
	}
	
	WM_GetUserData(hWin, &obj, sizeof(TP_ICON_Obj));
	pExtraBytes = (U8 *)TP_MALLOC(sizeof(TP_ICON_Obj) + obj.numExtraBytes);
	if (pExtraBytes) 
	{
		WM_GetUserData(hWin, pExtraBytes, sizeof(TP_ICON_Obj) + obj.numExtraBytes);
		
		if (sizeOfBuffer >= obj.numExtraBytes)
		{
			numBytes = obj.numExtraBytes;
		}
		else
		{
			numBytes = sizeOfBuffer;
		}
		
		TP_MEMCPY(pDest, pExtraBytes + sizeof(TP_ICON_Obj), numBytes);
		TP_FREE(pExtraBytes);
		return numBytes;
	}
	return 0;
}

int TP_ICON_SetUserData(TP_ICON_Handle hWin, void * pSrc, int sizeOfBuffer)
{
    TP_ICON_Obj	 obj;
	int			 numBytes;
	U8		   * pExtraBytes = NULL;

	if ((pSrc == NULL) || (sizeOfBuffer <= 0))
	{
		return 1;
	}

	WM_GetUserData(hWin, &obj, sizeof(TP_ICON_Obj));
	pExtraBytes = (U8 *)TP_MALLOC(sizeof(TP_ICON_Obj) + obj.numExtraBytes);
	if (pExtraBytes)
	{
		WM_GetUserData(hWin, pExtraBytes, sizeof(TP_ICON_Obj) + obj.numExtraBytes);

		if (sizeOfBuffer >= obj.numExtraBytes)
		{
            numBytes = obj.numExtraBytes;
		}
		else
		{
		    numBytes = sizeOfBuffer;
		}
		TP_MEMSET(pExtraBytes + sizeof(TP_ICON_Obj), 0, obj.numExtraBytes);
		TP_MEMCPY(pExtraBytes + sizeof(TP_ICON_Obj), pSrc, numBytes);
		WM_SetUserData(hWin, pExtraBytes, sizeof(TP_ICON_Obj) + obj.numExtraBytes);
		TP_FREE(pExtraBytes);
		return 0;
	}

	return 1;
}

void TP_ICON_Callback(WM_MESSAGE * pMsg)
{
    TP_ICON_Handle	hWin;
	GUI_PID_STATE	*pState;
	TP_ICON_Obj	obj;
	GUI_RECT	WinRect;
	U8	Pressed;
	int height, width, bmpXSize, bmpYSize, bmpx0, bmpy0;
	GUI_COLOR stateIndex;
    GUI_RECT	textRect;
    int fontSizeY; 
    char *words[TP_ICON_TEXT_WORD_NUM_MAX] = {0};
    char *tmpText;
    char *pChar;
    int index, flag, len;

	hWin = pMsg->hWin;
	WM_GetWindowRectEx(hWin, &WinRect);
	GUI_MoveRect(&WinRect, -WinRect.x0, -WinRect.y0);
	WM_GetUserData(hWin, &obj, sizeof(TP_ICON_Obj));

	switch (pMsg->MsgId) {
    case WM_PAINT:
        if (WM_IsEnabled(hWin))
		{
			if (obj.pressed)
			{
				stateIndex = TP_ICON_PRESSED;
			}
			else
			{
				stateIndex = TP_ICON_UNPRESSED;
			}
		}
		else
		{
			stateIndex = TP_ICON_DISABLED;
		}

        /* 显示背景颜色 */
		GUI_SetColor(obj.iconBkColor[stateIndex]);
		GUI_FillRectEx(&WinRect);

		height  = (WinRect.y1 - WinRect.y0);
		width   = (WinRect.x1 - WinRect.x0);

        textRect = WinRect;
    

        /* 显示icon位图 */
        if (obj.pIconBitmap[stateIndex]) 
        { 
            /* 获取对应状态下的bitmap的size */
		    bmpXSize = obj.pIconBitmap[stateIndex]->XSize;
            bmpYSize = obj.pIconBitmap[stateIndex]->YSize;
        
    		if (obj.iconBitmapAlign == (TP_ICON_BMP_ALIGN_HCENTER | TP_ICON_BMP_ALIGN_TOP))
    		{   
    	    	bmpx0 = WinRect.x0 + (width - bmpXSize) / 2;
    			bmpy0 = WinRect.y0;
                
                textRect.y0 = WinRect.y0 + bmpYSize + obj.space;
    		}
    		else if (obj.iconBitmapAlign == (TP_ICON_BMP_ALIGN_VCENTER| TP_ICON_BMP_ALIGN_LEFT))
    		{
    	    	bmpx0 = WinRect.x0;
    			bmpy0 = WinRect.y0 + (height - bmpYSize) / 2;

                textRect.x0 = WinRect.x0 + bmpXSize + obj.space;
    		}

            /* draw bitmap */
            GUI_DrawBitmap(obj.pIconBitmap[stateIndex], bmpx0, bmpy0);
	    }

        /* 显示文本 */
		if (obj.pText)
		{
            len = strlen(obj.pText) + 1;
	        tmpText = (char *)TP_MALLOC(len);
            pChar = tmpText;
            if (tmpText)
		    {
        		TP_MEMSET(tmpText, 0, len);
        		TP_MEMCPY(tmpText, obj.pText, len - 1);
            }
            
            index = 0;
            flag = 1;
            while( *pChar != '\0') 
            {
                if(*pChar != ' ' && flag == 1)
    		{   
                    words[index] = pChar;
                    index++;
                    flag = 0; 
                }

                if(*pChar == ' ')
                {
                    *pChar = '\0';
                    flag = 1; 
                }
                
                pChar++;
            }

                GUI_SetFont(obj.pFont);
                GUI_SetColor(obj.aTextColor[stateIndex]);
                GUI_SetTextMode(GUI_TEXTMODE_TRANS);
            fontSizeY = GUI_GetFontSizeY();

		    /* 居中、底部显示文本 */
		    if (obj.iconBitmapAlign == (TP_ICON_BMP_ALIGN_HCENTER | TP_ICON_BMP_ALIGN_TOP))
    		{   
    		    index = 0;
                while(words[index] != NULL) 
                {
                    textRect.y0 = textRect.y0 +  index * (fontSizeY + obj.lineSpace);
                    GUI_DispStringInRectWrap(words[index], &textRect, GUI_TA_HCENTER | GUI_TA_TOP, GUI_WRAPMODE_NONE);
                    index++;
                }
    		}
            /* 靠右、单行显示文本 */
    		else if (obj.iconBitmapAlign == (TP_ICON_BMP_ALIGN_VCENTER| TP_ICON_BMP_ALIGN_LEFT))
    		{
                GUI_DispStringInRectWrap(obj.pText, &textRect, GUI_TA_HCENTER | GUI_TA_LEFT, GUI_WRAPMODE_NONE);
    		}   


            if (tmpText)
        	{
        		TP_FREE(tmpText);
        	}
		}
        
		break;

	case WM_TOUCH:
		pState = (GUI_PID_STATE *)pMsg->Data.p;
		if (pState)
		{
			Pressed = pState->Pressed;
		}
		else
		{
			Pressed = 0;
		}

		if (obj.pressed != Pressed)
		{
			obj.pressed = Pressed;
			WM_SetUserData(hWin, &obj, sizeof(TP_ICON_Obj));
			if (Pressed && obj.focussable)
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
		break;
	case WM_SET_FOCUS:
		if (pMsg->Data.v && obj.focussable)
		{
		    pMsg->Data.v = 0;
		} 
		WM_InvalidateWindow(hWin);
		break;
        
#if 0
	case WM_PID_STATE_CHANGED:
		pPidState = (WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p);
		if (WM_IsEnabled(hWin))
		{
            if (pPidState) 
		    {
    			if (pPidState->State) 
    			{
    				if (obj.focussable)
    				{
    				    WM_SetFocus(pMsg->hWin);
    			    } 
    			} 
		    }

            WM_InvalidateWindow(hWin);
		}
		break;
#endif

	case WM_GET_ID:
		pMsg->Data.v = obj.id;
		break;
	case WM_SET_ID:
		obj.id = pMsg->Data.v;
		WM_SetUserData(hWin, &obj, sizeof(TP_ICON_Obj));
		break;
	case WM_DELETE:
		if (obj.pText)
		{
		    TP_FREE(obj.pText);
		    WM_SetUserData(hWin, &obj, sizeof(TP_ICON_Obj));
		}
		
		break;
	default:
		WM_DefaultProc(pMsg);
		break;
	}
}

TP_ICON_Handle TP_ICON_Create(int x0, int y0, int xSize, int ySize,
								WM_HWIN hWinParent, U32 style, const char *pText,
								WM_CALLBACK * pfCallback, int numExtraBytes, int id)
{
    TP_ICON_Handle	handle;
	TP_ICON_Obj		obj;
	WM_CALLBACK     *pCb;

	if (pfCallback)
	{
		pCb = pfCallback;
	}
	else
	{
		pCb = TP_ICON_Callback;
	}

	obj	= TP_ICON_Default;
	
	obj.numExtraBytes = numExtraBytes;
	obj.id = id;
	if (pText)
	{
		obj.pText = TP_MALLOC(strlen(pText) + 1);
		TP_MEMSET(obj.pText, 0, strlen(pText) + 1);
		TP_MEMCPY(obj.pText, pText, strlen(pText));
	}
	
	handle = WM_CreateWindowAsChild(x0, y0, xSize, ySize, hWinParent, 
		style | WM_CF_MEMDEV, pCb, sizeof(TP_ICON_Obj) + numExtraBytes);
	WM_SetUserData(handle, &obj, sizeof(TP_ICON_Obj));
	return handle;
}




#if  0
/* 下面开始是测试代码 */
static void Test_cbBkWindow(WM_MESSAGE * pMsg)
{
	WM_HMEM hWin;
	//static char showMsg[64] = "TP_Check - Sample";

	int NCode;
	hWin = pMsg->hWin;
    
    switch (pMsg->MsgId) {
        case WM_PAINT:
            GUI_SetBkColor(GUI_LIGHTBLUE);
            GUI_Clear();
            //GUI_SetColor(GUI_BLACK);
            //GUI_SetFont(&GUI_Font24_ASCII);
            //GUI_DispStringHCenterAt(showMsg, 160, 150);
            break;
	
    	case WM_NOTIFY_PARENT:
    		NCode = pMsg->Data.v;
    		switch (NCode) {
        		case WM_NOTIFICATION_RELEASED:
        			GUI_MessageBox("release", "message", GUI_MESSAGEBOX_CF_MOVEABLE);
        			break;
        	}
    		break;

        case WM_SET_FOCUS:
        	if (pMsg->Data.v)
        	{
        		pMsg->Data.v = 0;
        	}
        	WM_InvalidateWindow(hWin);
        	break;
            
        default:
            WM_DefaultProc(pMsg);
  }
}


extern GUI_CONST_STORAGE GUI_BITMAP bmicon_wlan_configuration_normal;
extern GUI_CONST_STORAGE GUI_BITMAP bmicon_wlan_configuration_pressed;

extern GUI_CONST_STORAGE GUI_BITMAP bmicon_about_normal;
extern GUI_CONST_STORAGE GUI_BITMAP bmicon_about_pressed;

extern GUI_CONST_STORAGE GUI_BITMAP bmicon_clock_normal;
extern GUI_CONST_STORAGE GUI_BITMAP bmicon_clock_pressed;

extern GUI_CONST_STORAGE GUI_BITMAP bmicon_device_control_normal;
extern GUI_CONST_STORAGE GUI_BITMAP bmicon_device_control_pressed;

extern GUI_CONST_STORAGE GUI_BITMAP bmicon_firmware_upgrade_normal;
extern GUI_CONST_STORAGE GUI_BITMAP bmicon_firmware_upgrade_pressed;

extern GUI_CONST_STORAGE GUI_BITMAP bmicon_mode_change_normal;
extern GUI_CONST_STORAGE GUI_BITMAP bmicon_mode_change_pressed;

extern GUI_CONST_STORAGE GUI_BITMAP bmicon_parental_control_normal;
extern GUI_CONST_STORAGE GUI_BITMAP bmicon_parental_control_pressed;

extern GUI_CONST_STORAGE GUI_BITMAP bmicon_password_normal;
extern GUI_CONST_STORAGE GUI_BITMAP bmicon_password_pressed;

extern GUI_CONST_STORAGE GUI_BITMAP bmicon_printer_sever_normal;
extern GUI_CONST_STORAGE GUI_BITMAP bmicon_printer_sever_pressed;

extern GUI_CONST_STORAGE GUI_BITMAP bmicon_quick_setup_normal;
extern GUI_CONST_STORAGE GUI_BITMAP bmicon_quick_setup_pressed;



/*********************************************************************
*
*       MainTask
*/
void MainTask(void) {
	TP_ICON_Handle hIcon0, hIcon1, hIcon2, hIcon3, hIcon4, hIcon5, hIcon6, hIcon7, hIcon8, hIcon9;
	WM_HMEM hTest;

	GUI_Init();
    
    /* 创建主窗口 */
	hTest = WM_CreateWindow(0, 0, LCD_GetXSize(), LCD_GetYSize(),
					WM_CF_SHOW | WM_CF_MEMDEV, Test_cbBkWindow, 0);
    
	
    hIcon0 = TP_ICON_Create(25, 20, 70, 105, hTest, WM_CF_SHOW, "WLAN Configuration", NULL, 0, 0);
	TP_ICON_SetIconBitmap(hIcon0, TP_ICON_UNPRESSED, &bmicon_wlan_configuration_normal);
    TP_ICON_SetIconBitmap(hIcon0, TP_ICON_PRESSED, &bmicon_wlan_configuration_pressed);

	hIcon1 = TP_ICON_Create(115, 20, 70, 105, hTest, WM_CF_SHOW, "About", NULL, 0, 1);
	TP_ICON_SetIconBitmap(hIcon1, TP_ICON_UNPRESSED, &bmicon_about_normal);
    TP_ICON_SetIconBitmap(hIcon1, TP_ICON_PRESSED, &bmicon_about_pressed);

    hIcon2 = TP_ICON_Create(205, 20, 70, 105, hTest, WM_CF_SHOW, "Clock", NULL, 0, 2);
    TP_ICON_SetIconBitmap(hIcon2, TP_ICON_UNPRESSED, &bmicon_clock_normal);
    TP_ICON_SetIconBitmap(hIcon2, TP_ICON_PRESSED, &bmicon_clock_pressed);

    hIcon3 = TP_ICON_Create(295, 20, 70, 105, hTest, WM_CF_SHOW, "Device Control", NULL, 0, 3);
    TP_ICON_SetIconBitmap(hIcon3, TP_ICON_UNPRESSED, &bmicon_device_control_normal);
    TP_ICON_SetIconBitmap(hIcon3, TP_ICON_PRESSED, &bmicon_device_control_pressed);

    hIcon4 = TP_ICON_Create(385, 20, 70, 105, hTest, WM_CF_SHOW, "Firmware Upgrade", NULL, 0, 4);
    TP_ICON_SetIconBitmap(hIcon4, TP_ICON_UNPRESSED, &bmicon_firmware_upgrade_normal);
    TP_ICON_SetIconBitmap(hIcon4, TP_ICON_PRESSED, &bmicon_firmware_upgrade_pressed);

    //row 2
    hIcon5 = TP_ICON_Create(25, 145, 70, 105, hTest, WM_CF_SHOW, "Mode Change", NULL, 0, 5);
    TP_ICON_SetIconBitmap(hIcon5, TP_ICON_UNPRESSED, &bmicon_mode_change_normal);
    TP_ICON_SetIconBitmap(hIcon5, TP_ICON_PRESSED, &bmicon_mode_change_pressed);

    hIcon6 = TP_ICON_Create(115, 145, 70, 105, hTest, WM_CF_SHOW, "Parental Control", NULL, 0, 6);
    TP_ICON_SetIconBitmap(hIcon6, TP_ICON_UNPRESSED, &bmicon_parental_control_normal);
    TP_ICON_SetIconBitmap(hIcon6, TP_ICON_PRESSED, &bmicon_parental_control_pressed);

    hIcon7 = TP_ICON_Create(205, 145, 70, 105, hTest, WM_CF_SHOW, "Password", NULL, 0, 7);
    TP_ICON_SetIconBitmap(hIcon7, TP_ICON_UNPRESSED, &bmicon_password_normal);
    TP_ICON_SetIconBitmap(hIcon7, TP_ICON_PRESSED, &bmicon_password_pressed);

    
    hIcon8 = TP_ICON_Create(295, 145, 70, 105, hTest, WM_CF_SHOW, "Printer Server", NULL, 0, 8);
    TP_ICON_SetIconBitmap(hIcon8, TP_ICON_UNPRESSED, &bmicon_printer_sever_normal);
    TP_ICON_SetIconBitmap(hIcon8, TP_ICON_PRESSED, &bmicon_printer_sever_pressed);

    hIcon9 = TP_ICON_Create(385, 145, 70, 105, hTest, WM_CF_SHOW, "Quick Setup", NULL, 0, 9);
    TP_ICON_SetIconBitmap(hIcon9, TP_ICON_UNPRESSED, &bmicon_quick_setup_normal);
    TP_ICON_SetIconBitmap(hIcon9, TP_ICON_PRESSED, &bmicon_quick_setup_pressed);

    
	while (1) {
		GUI_Delay(100);
	}
}

#endif

