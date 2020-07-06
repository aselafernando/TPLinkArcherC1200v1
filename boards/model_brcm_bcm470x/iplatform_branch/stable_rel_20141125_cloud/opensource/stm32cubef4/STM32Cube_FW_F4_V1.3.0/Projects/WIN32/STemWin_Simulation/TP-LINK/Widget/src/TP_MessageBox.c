/*! Copyright (c) 2014 Shenzhen TP-LINK Technologies Co.Ltd.
 *
 * \file     TP_MessageBox.c
 * \brief    TP_MessageBox控件实现
 */

#include "TP_MessageBox.h"

/**
 * \breif TP_MESSAGEBOX控件的默认属性
 */
TP_MESSAGEBOX_Obj TP_MessageBox_Default =
{
	TP_MESSAGEBOX_DEFAULT_ID,
    TP_MESSAGEBOX_DEFAULT_FOCUSSABLE,
    TP_MESSAGEBOX_DEFAULT_FOCUSCOLOR,
	TP_MESSAGEBOX_DEFAULT_TEXTCOLOR,
	TP_MESSAGEBOX_DEFAULT_PTEXTFONT,
	TP_MESSAGEBOX_DEFAULT_TEXTALIGN,
	TP_MESSAGEBOX_DEFAULT_TEXTMARGIN,
	TP_MESSAGEBOX_DEFAULT_PTEXT,
	TP_MESSAGEBOX_DEFAULT_PBKBITMAP,
	TP_MESSAGEBOX_DEFAULT_BKCOLOR,
	TP_MESSAGEBOX_DEFAULT_BUTTONNUM,
	TP_MESSAGEBOX_DEFAULT_BUTTONHEIGHT,
	TP_MESSAGEBOX_DEFAULT_AHBUTTON,
	TP_MESSAGEBOX_DEFAULT_NUMEXTRABYTES
};

/*******************
 * 属性Get/Set方法 *
 *******************/
TP_BUTTON_Handle TP_MessageBox_GetButtonHandle(TP_MESSAGEBOX_Handle hObj, unsigned int index)
{
    TP_MESSAGEBOX_Obj   obj;

    if (hObj && (index < TP_MESSAGEBOX_MAX_BUTTON))
    {
        TP_GUI_LOCK();
        WM_GetUserData(hObj, &obj, sizeof(TP_MESSAGEBOX_Obj));
        TP_GUI_UNLOCK();

        return obj.aHButton[index];
    }

    return WM_HMEM_NULL;
}

void TP_MessageBox_SetButtonHandle(TP_MESSAGEBOX_Handle hObj, unsigned int index, TP_BUTTON_Handle hButton)
{
    TP_MESSAGEBOX_Obj   obj;

    if (hObj && (index < TP_MESSAGEBOX_MAX_BUTTON))
    {
        TP_GUI_LOCK();
        WM_GetUserData(hObj, &obj, sizeof(TP_MESSAGEBOX_Obj));
        obj.aHButton[index] = hButton;
        WM_SetUserData(hObj, &obj, sizeof(TP_MESSAGEBOX_Obj));
        TP_GUI_UNLOCK();
    }
}

GUI_COLOR TP_MessageBox_GetBkColor(TP_MESSAGEBOX_Handle hObj)
{
    TP_MESSAGEBOX_Obj   obj;
    GUI_COLOR           color = GUI_INVALID_COLOR;

    if (hObj)
    {
        TP_GUI_LOCK();
        WM_GetUserData(hObj, &obj, sizeof(TP_MESSAGEBOX_Obj));
        TP_GUI_UNLOCK();

        color = obj.bkColor;
    }

    return color;
}

void TP_MessageBox_SetBkColor(TP_MESSAGEBOX_Handle hObj, GUI_COLOR color)
{
    TP_MESSAGEBOX_Obj   obj;

    if (hObj)
    {
        TP_GUI_LOCK();
        WM_GetUserData(hObj, &obj, sizeof(TP_MESSAGEBOX_Obj));
        obj.bkColor = color;
        WM_SetUserData(hObj, &obj, sizeof(TP_MESSAGEBOX_Obj));
        TP_GUI_UNLOCK();
    }
}

const GUI_BITMAP * TP_MessageBox_GetBkBitmap(TP_MESSAGEBOX_Handle hObj)
{
    TP_MESSAGEBOX_Obj   obj;

    if (hObj)
    {
        TP_GUI_LOCK();
        WM_GetUserData(hObj, &obj, sizeof(TP_MESSAGEBOX_Obj));
        TP_GUI_UNLOCK();

        return obj.pBkBitmap;
    }

    return NULL;
}

void TP_MessageBox_SetBkBitmap(TP_MESSAGEBOX_Handle hObj, const GUI_BITMAP * bitmap)
{
    TP_MESSAGEBOX_Obj   obj;

    if (hObj)
    {
        TP_GUI_LOCK();
        WM_GetUserData(hObj, &obj, sizeof(TP_MESSAGEBOX_Obj));
        obj.pBkBitmap = bitmap;
        WM_SetUserData(hObj, &obj, sizeof(TP_MESSAGEBOX_Obj));
        TP_GUI_UNLOCK();
    }
}

int TP_Message_GetText(TP_MESSAGEBOX_Handle hObj, char * pBuffer, int maxLen)
{
    int                 len = 0;
    TP_MESSAGEBOX_Obj   obj;

    if (hObj && (NULL != pBuffer))
    {
        TP_GUI_LOCK();
        WM_GetUserData(hObj, &obj, sizeof(TP_MESSAGEBOX_Obj));
        TP_GUI_UNLOCK();

        len = strlen(obj.pText);

        if (len > maxLen)
        {
            /* FIXME: 用户分配空间过小，暂时截断处理 */
            TP_MEMCPY(pBuffer, obj.pText, maxLen);
            pBuffer[maxLen - 1] = '\0';

            return maxLen;
        }
        else
        {
            TP_MEMCPY(pBuffer, obj.pText, len);

            return len;
        }
    }

    return RET_FAIL;
}

int TP_MessageBox_SetText(TP_MESSAGEBOX_Handle hObj, char * pText)
{
    int                 len = 0;
    TP_MESSAGEBOX_Obj   obj;

    if (hObj && (NULL != pText))
    {
        TP_GUI_LOCK();
        WM_GetUserData(hObj, &obj, sizeof(TP_MESSAGEBOX_Obj));
        TP_GUI_UNLOCK();
        if (NULL != obj.pText)
        {
            TP_FREE(obj.pText);
        }

        len = strlen(pText) + 1;
        obj.pText = TP_MALLOC(sizeof(char) * len);
        if (NULL != obj.pText)
        {
            TP_MEMSET(obj.pText, 0, len);
            TP_MEMCPY(obj.pText, pText, len);
            TP_GUI_LOCK();
            WM_SetUserData(hObj, &obj, sizeof(TP_MESSAGEBOX_Obj));
            TP_GUI_UNLOCK();

            return RET_SUCCESS;
        }
    }

    return RET_FAIL;
}

int TP_MessageBox_GetTextMargin(TP_MESSAGEBOX_Handle hObj)
{
    TP_MESSAGEBOX_Obj   obj;

    if (hObj)
    {
        TP_GUI_LOCK();
        WM_GetUserData(hObj, &obj, sizeof(TP_MESSAGEBOX_Obj));
        TP_GUI_UNLOCK();

        return obj.textMargin;
    }

    return RET_FAIL;
}

void TP_MessageBox_SetTextMargin(TP_MESSAGEBOX_Handle hObj, int margin)
{
    TP_MESSAGEBOX_Obj   obj;

    if (hObj)
    {
        TP_GUI_LOCK();
        WM_GetUserData(hObj, &obj, sizeof(TP_MESSAGEBOX_Obj));
        obj.textMargin = margin;
        WM_SetUserData(hObj, &obj, sizeof(TP_MESSAGEBOX_Obj));
        TP_GUI_UNLOCK();
    }
}

int TP_MessageBox_GetTextAlign(TP_MESSAGEBOX_Handle hObj)
{
    TP_MESSAGEBOX_Obj   obj;

    if (hObj)
    {
        TP_GUI_LOCK();
        WM_GetUserData(hObj, &obj, sizeof(TP_MESSAGEBOX_Obj));
        TP_GUI_UNLOCK();

        return obj.textAlign;
    }

    return RET_FAIL;
}

void TP_MessageBox_SetTextAlign(TP_MESSAGEBOX_Handle hObj, int align)
{
    TP_MESSAGEBOX_Obj   obj;

    if (hObj)
    {
        TP_GUI_LOCK();
        WM_GetUserData(hObj, &obj, sizeof(TP_MESSAGEBOX_Obj));
        obj.textAlign = align;
        WM_SetUserData(hObj, &obj, sizeof(TP_MESSAGEBOX_Obj));
        TP_GUI_UNLOCK();
    }
}

const GUI_FONT * TP_MessageBox_GetTextFont(TP_MESSAGEBOX_Handle hObj)
{
    TP_MESSAGEBOX_Obj   obj;

    if (hObj)
    {
        TP_GUI_LOCK();
        WM_GetUserData(hObj, &obj, sizeof(TP_MESSAGEBOX_Obj));
        TP_GUI_UNLOCK();

        return obj.pTextFont;
    }

    return NULL;
}

void TP_MessageBox_SetTextFont(TP_MESSAGEBOX_Handle hObj, const GUI_FONT * font)
{
    TP_MESSAGEBOX_Obj   obj;

    if (hObj)
    {
        TP_GUI_LOCK();
        WM_GetUserData(hObj, &obj, sizeof(TP_MESSAGEBOX_Obj));
        obj.pTextFont = font;
        WM_SetUserData(hObj, &obj, sizeof(TP_MESSAGEBOX_Obj));
        TP_GUI_UNLOCK();
    }
}

GUI_COLOR TP_MessageBox_GetTextColor(TP_MESSAGEBOX_Handle hObj)
{
    TP_MESSAGEBOX_Obj   obj;
    GUI_COLOR           color = GUI_INVALID_COLOR;

    if (hObj)
    {
        TP_GUI_LOCK();
        WM_GetUserData(hObj, &obj, sizeof(TP_MESSAGEBOX_Obj));
        TP_GUI_UNLOCK();

        color = obj.textColor;
    }

    return color;
}

void TP_MessageBox_SetTextColor(TP_MESSAGEBOX_Handle hObj, GUI_COLOR color)
{
    TP_MESSAGEBOX_Obj   obj;

    if (hObj)
    {
        TP_GUI_LOCK();
        WM_GetUserData(hObj, &obj, sizeof(TP_MESSAGEBOX_Obj));
        obj.textColor = color;
        WM_SetUserData(hObj, &obj, sizeof(TP_MESSAGEBOX_Obj));
        TP_GUI_UNLOCK();
    }
}

GUI_COLOR TP_MessageBox_GetFocusColor(TP_MESSAGEBOX_Handle hObj)
{
    TP_MESSAGEBOX_Obj   obj;
    GUI_COLOR           color = GUI_INVALID_COLOR;

    if (hObj)
    {
        TP_GUI_LOCK();
        WM_GetUserData(hObj, &obj, sizeof(TP_MESSAGEBOX_Obj));
        TP_GUI_UNLOCK();

        color = obj.focusColor;
    }

    return color;
}

void TP_MessageBox_SetFocusColor(TP_MESSAGEBOX_Handle hObj, GUI_COLOR color)
{
    TP_MESSAGEBOX_Obj   obj;

    if (hObj)
    {
        TP_GUI_LOCK();
        WM_GetUserData(hObj, &obj, sizeof(TP_MESSAGEBOX_Obj));
        obj.focusColor = color;
        WM_SetUserData(hObj, &obj, sizeof(TP_MESSAGEBOX_Obj));
        TP_GUI_UNLOCK();
    }
}

int TP_MessageBox_GetFocussable(TP_MESSAGEBOX_Handle hObj)
{
    TP_MESSAGEBOX_Obj   obj;

    if (hObj)
    {
        TP_GUI_LOCK();
        WM_GetUserData(hObj, &obj, sizeof(TP_MESSAGEBOX_Obj));
        TP_GUI_UNLOCK();

        return obj.focussable;
    }

    return RET_FAIL;
}

void TP_MessageBox_SetFocussable(TP_MESSAGEBOX_Handle hObj, int state)
{
    TP_MESSAGEBOX_Obj   obj;

    if (hObj)
    {
        TP_GUI_LOCK();
        WM_GetUserData(hObj, &obj, sizeof(TP_MESSAGEBOX_Obj));
        obj.focussable = state;
        WM_SetUserData(hObj, &obj, sizeof(TP_MESSAGEBOX_Obj));
        TP_GUI_UNLOCK();
    }
}

/***********************
 * 默认属性Get/Set方法 *
 ***********************/
TP_BUTTON_Handle TP_MessageBox_GetDefaultButtonHandle(unsigned int index)
{
    return TP_MessageBox_Default.aHButton[index];
}

void TP_MessageBox_SetDefaultButtonHandle(unsigned int index, TP_BUTTON_Handle hButton)
{
    TP_MessageBox_Default.aHButton[index] = hButton;
}

GUI_COLOR TP_MessageBox_GetDefaultBkColor()
{
    return TP_MessageBox_Default.bkColor;
}

void TP_MessageBox_SetDefaultBkColor(GUI_COLOR color)
{
    TP_MessageBox_Default.bkColor = color;
}

const GUI_BITMAP * TP_MessageBox_GetDefaultBkBitmap()
{
    return TP_MessageBox_Default.pBkBitmap;
}

void TP_MessageBox_SetDefaultBkBitmap(const GUI_BITMAP * bitmap)
{
    TP_MessageBox_Default.pBkBitmap = bitmap;
}

int TP_Message_GetDefaultText(char * pBuffer, int maxLen)
{
    int len = 0;

    if ((NULL != TP_MessageBox_Default.pText) && (NULL != pBuffer))
    {
        len = strlen(TP_MessageBox_Default.pText);

        if (len > maxLen)
        {
            /* FIXME: 用户分配空间过小，暂时截断处理 */
            TP_MEMCPY(pBuffer, TP_MessageBox_Default.pText, maxLen);
            pBuffer[maxLen - 1] = '\0';

            return maxLen;
        }
        else
        {
            TP_MEMCPY(pBuffer, TP_MessageBox_Default.pText, len);

            return len;
        }
    }

    return RET_FAIL;
}

int TP_MessageBox_SetDefaultText(char * pText)
{
    int len = 0;

    if (NULL != pText)
    {
        if (NULL != TP_MessageBox_Default.pText)
        {
            TP_FREE(TP_MessageBox_Default.pText);
        }

        len = strlen(pText) + 1;
        TP_MessageBox_Default.pText = TP_MALLOC(sizeof(char) * len);
        if (NULL != TP_MessageBox_Default.pText)
        {
            TP_MEMSET(TP_MessageBox_Default.pText, 0, len);
            TP_MEMCPY(TP_MessageBox_Default.pText, pText, len);

            return RET_SUCCESS;
        }
    }

    return RET_FAIL;
}

int TP_MessageBox_GetDefaultTextMargin()
{
    return TP_MessageBox_Default.textMargin;
}

void TP_MessageBox_SetDefaultTextMargin(int margin)
{
    TP_MessageBox_Default.textMargin = margin;
}

int TP_MessageBox_GetDefaultTextAlign()
{
    return TP_MessageBox_Default.textAlign;
}

void TP_MessageBox_SetDefaultTextAlign(int align)
{
    TP_MessageBox_Default.textAlign = align;
}

const GUI_FONT * TP_MessageBox_GetDefaultTextFont()
{
    return TP_MessageBox_Default.pTextFont;
}

void TP_MessageBox_SetDefaultTextFont(GUI_FONT * font)
{
    TP_MessageBox_Default.pTextFont = font;
}

GUI_COLOR TP_MessageBox_GetDefaultTextColor()
{
    return TP_MessageBox_Default.textColor;
}

void TP_MessageBox_SetDefaultTextColor(GUI_COLOR color)
{
    TP_MessageBox_Default.textColor = color;
}

GUI_COLOR TP_MessageBox_GetDefaultFocusColor()
{
    return TP_MessageBox_Default.focusColor;
}

void TP_MessageBox_SetDefaultFocusColor(GUI_COLOR color)
{
    TP_MessageBox_Default.focusColor = color;
}

int TP_MessageBox_GetDefaultFocussable()
{
    return TP_MessageBox_Default.focussable;
}

void TP_MessageBox_SetDefaultFocussable(int state)
{
    TP_MessageBox_Default.focussable = state;
}


/********************
 * 窗口处理相关方法 *
 ********************/
void TP_MessageBox_Callback(WM_MESSAGE * pMsg)
{
    TP_MESSAGEBOX_Handle            hWin;
    GUI_RECT                        winRect;
    GUI_RECT                        textRect;
    TP_MESSAGEBOX_Obj               hObj;

    WM_HWIN                         hParentWin;
    WM_PID_STATE_CHANGED_INFO *     pPidState;

    hWin = pMsg->hWin;
    WM_GetUserData(hWin, &hObj, sizeof(TP_MESSAGEBOX_Obj));

    WM_GetWindowRectEx(hWin, &winRect);
    GUI_MoveRect(&winRect, -winRect.x0, -winRect.y0);
    WM_GetWindowRectEx(hWin, &textRect);
    GUI_MoveRect(&textRect, -textRect.x0, -textRect.y0);

    textRect.x0 = winRect.x0 + hObj.textMargin;
    textRect.x1 = winRect.x1 - hObj.textMargin;
    textRect.y1 = winRect.y1 - hObj.buttonHeight;

    switch (pMsg->MsgId)
    {
    case WM_PID_STATE_CHANGED:
        pPidState   = (WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p);
        hParentWin  = WM_GetParent(pMsg->hWin);
        if (NULL != pPidState)
        {
            if (pPidState->State)
            {
                WM_NotifyParent(hParentWin, WM_NOTIFICATION_CLICKED);

                if (hObj.focussable)
                {
                    WM_SetFocus(pMsg->hWin);
                }
            }
            else
            {
                WM_NotifyParent(hParentWin, WM_NOTIFICATION_RELEASED);
            }
        }
        WM_InvalidateWindow(pMsg->hWin);
        break;

    case WM_PAINT:
        /* 填充背景 */
        GUI_SetColor(hObj.bkColor);
        GUI_FillRectEx(&winRect);

        /* 绘制文本 */
        if (NULL != hObj.pText)
        {
            GUI_SetTextMode(GUI_TEXTMODE_TRANS);
            /* 没有指定颜色，使用默认颜色 */
            GUI_SetColor(hObj.textColor ? hObj.textColor : TP_MessageBox_Default.textColor);
            /* 没有指定字体，使用默认字体 */
            GUI_SetFont(hObj.pTextFont ? hObj.pTextFont : TP_MessageBox_Default.pTextFont);
            /* 在指定的矩形区域显示文本，并可自动换行 */
            GUI_DispStringInRectWrap(hObj.pText, &textRect, hObj.textAlign ? hObj.textAlign : TP_MessageBox_Default.textAlign, GUI_WRAPMODE_WORD);

            /* 绘制图标 */
            if (NULL != hObj.pBkBitmap)
            {
                /* 紧挨着文本绘制 */
                GUI_DrawBitmap(hObj.pBkBitmap, textRect.x0-hObj.pBkBitmap->XSize, GUI_GetDispPosY()-((GUI_GetFontSizeY()+hObj.pBkBitmap->YSize) >> 1));
            }
        }
        break;

    case WM_SET_FOCUS:
        if (pMsg->Data.v && hObj.focussable)
        {
            pMsg->Data.v = 0;
            WM_InvalidateWindow(hWin);
        }
        break;

    case WM_DELETE:
        if (NULL != hObj.pText)
        {
            TP_FREE(hObj.pText);
        }
        WM_SetUserData(hWin, &hObj, sizeof(TP_MESSAGEBOX_Obj));
        if (NULL != TP_MessageBox_Default.pText)
        {
            TP_FREE(TP_MessageBox_Default.pText);
        }
        break;

    case WM_GET_ID:
        pMsg->Data.v = hObj.id;
        break;

    case WM_SET_ID:
        hObj.id = pMsg->Data.v;
        WM_SetUserData(hWin, &hObj, sizeof(TP_MESSAGEBOX_Obj));
        break;

    default:
        WM_DefaultProc(pMsg);
    }
}

int TP_MessageBox_SetUserData(TP_MESSAGEBOX_Handle hObj, void * pSrc, int size)
{
    TP_MESSAGEBOX_Obj   obj;
    U8 *            pExtraBytes;
    int             numBytes;

    if (size <= 0)
    {
        return RET_FAIL;
    }

    WM_GetUserData(hObj, &obj, sizeof(TP_MESSAGEBOX_Obj));

    pExtraBytes = (U8 *)TP_MALLOC(sizeof(TP_MESSAGEBOX_Obj) + obj.numExtraBytes);
    if (NULL != pExtraBytes)
    {
        WM_GetUserData(hObj, pExtraBytes, sizeof(TP_MESSAGEBOX_Obj) + obj.numExtraBytes);
        if (size >= obj.numExtraBytes)
        {
            numBytes = obj.numExtraBytes;
        }
        else
        {
            numBytes = size;
        }
        TP_MEMCPY(pExtraBytes + sizeof(TP_MESSAGEBOX_Obj), pSrc, numBytes);
        WM_SetUserData(hObj, pExtraBytes, sizeof(TP_MESSAGEBOX_Obj) + obj.numExtraBytes);
        TP_FREE(pExtraBytes);

        return RET_SUCCESS;
    }

    return RET_FAIL;
}

int TP_MessageBox_GetUserData(TP_MESSAGEBOX_Handle hObj, void * pDest, int size)
{
    TP_MESSAGEBOX_Obj   obj;
    U8 *            pExtraBytes;
    int             numBytes;

    if (size <= 0)
    {
        return 0;
    }

    WM_GetUserData(hObj, &obj, sizeof(TP_MESSAGEBOX_Obj));
    pExtraBytes = (U8 *)TP_MALLOC(sizeof(TP_MESSAGEBOX_Obj) + obj.numExtraBytes);
    if (pExtraBytes)
    {
        WM_GetUserData(hObj, pExtraBytes, sizeof(TP_MESSAGEBOX_Obj) + obj.numExtraBytes);
        if (size >= obj.numExtraBytes)
        {
            numBytes = obj.numExtraBytes;
        }
        else
        {
            numBytes = size;
        }
        TP_MEMCPY(pDest, pExtraBytes + sizeof(TP_MESSAGEBOX_Obj), numBytes);
        TP_FREE(pExtraBytes);

        return numBytes;
    }
    return 0;
}

TP_MESSAGEBOX_Handle TP_MessageBox_Create(int x0, int y0, int xSize, int ySize, WM_HWIN hParent,
        U8 style, WM_CALLBACK * cb, int numExtraBytes, int id, int buttonNum, int buttonHeight)
{
    WM_CALLBACK *           pUsed;
    TP_MESSAGEBOX_Obj       obj;
    TP_MESSAGEBOX_Handle    hWin;
    U8                      borderSize1[TP_BUTTON_BORDER_NUM] = {1, 0, 1, 1};
    U8                      borderSize2[TP_BUTTON_BORDER_NUM] = {1, 0, 0, 1};

    obj = TP_MessageBox_Default;
    obj.numExtraBytes = numExtraBytes;
    obj.id = id;

    if ((buttonNum > 0) && (buttonNum <= TP_MESSAGEBOX_MAX_BUTTON))
    {
        obj.buttonNum = buttonNum;
    }

    if ((buttonHeight > 0) && (buttonHeight < ySize))
    {
        obj.buttonHeight = buttonHeight;
    }

    if (cb)
    {
        pUsed = cb;
    }
    else
    {
        pUsed = TP_MessageBox_Callback;
    }

    hWin = WM_CreateWindowAsChild(x0, y0, xSize, ySize, hParent,
            style | WM_CF_SHOW | WM_CF_MEMDEV | WM_CF_HASTRANS, pUsed,
            sizeof(TP_MESSAGEBOX_Obj) + numExtraBytes);

    if (1 == obj.buttonNum)
    {
        obj.aHButton[0] = TP_Button_Create(x0, ySize-obj.buttonHeight, xSize, obj.buttonHeight, hWin, style, NULL, 0, 1);
        obj.aHButton[1] = WM_HMEM_NULL;

        if (obj.aHButton[0])
        {
            TP_Button_SetRadius(obj.aHButton[0], 0);
            TP_Button_SetBkColor(obj.aHButton[0], TP_BUTTON_UNPRESSED, GUI_WHITE);
            TP_Button_SetBkColor(obj.aHButton[0], TP_BUTTON_PRESSED, GUI_WHITE);
            TP_Button_SetBorderColor(obj.aHButton[0], TP_BUTTON_UNPRESSED, GUI_GRAY);
            TP_Button_SetBorderColor(obj.aHButton[0], TP_BUTTON_PRESSED, GUI_GRAY);
            TP_Button_SetBorderSizeEx(obj.aHButton[0], borderSize1);
        }
    }
    else if (2 == obj.buttonNum)
    {
        obj.aHButton[0] = TP_Button_Create(x0, ySize-obj.buttonHeight, xSize >> 1 , obj.buttonHeight, hWin, style, NULL, 0, 1);
        obj.aHButton[1] = TP_Button_Create(xSize >> 1, ySize-obj.buttonHeight, xSize >> 1, obj.buttonHeight, hWin, style, NULL, 0, 1);

        if (obj.aHButton[0] && obj.aHButton[1])
        {
            TP_Button_SetRadius(obj.aHButton[0], 0);
            TP_Button_SetBkColor(obj.aHButton[0], TP_BUTTON_UNPRESSED, GUI_WHITE);
            TP_Button_SetBkColor(obj.aHButton[0], TP_BUTTON_PRESSED, GUI_WHITE);
            TP_Button_SetBorderColor(obj.aHButton[0], TP_BUTTON_UNPRESSED, GUI_GRAY);
            TP_Button_SetBorderColor(obj.aHButton[0], TP_BUTTON_PRESSED, GUI_GRAY);
            TP_Button_SetBorderSizeEx(obj.aHButton[0], borderSize1);
            TP_Button_SetRadius(obj.aHButton[1], 0);
            TP_Button_SetBkColor(obj.aHButton[1], TP_BUTTON_UNPRESSED, GUI_WHITE);
            TP_Button_SetBkColor(obj.aHButton[1], TP_BUTTON_PRESSED, GUI_WHITE);
            TP_Button_SetBorderColor(obj.aHButton[1], TP_BUTTON_UNPRESSED, GUI_GRAY);
            TP_Button_SetBorderColor(obj.aHButton[1], TP_BUTTON_PRESSED, GUI_GRAY);
            TP_Button_SetBorderSizeEx(obj.aHButton[1], borderSize2);
        }
    }

    if (hWin)
    {
        WM_SetUserData(hWin, &obj, sizeof(TP_MESSAGEBOX_Obj));
    }

    return hWin;
}
