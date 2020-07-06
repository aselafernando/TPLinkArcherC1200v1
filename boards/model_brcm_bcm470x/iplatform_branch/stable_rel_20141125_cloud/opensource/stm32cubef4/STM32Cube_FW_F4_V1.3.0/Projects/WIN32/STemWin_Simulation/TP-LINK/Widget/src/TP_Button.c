/*! Copyright (c) 2014 Shenzhen TP-LINK Technologies Co.Ltd.
 *
 * \file     TP_Button.c
 * \brief    TP_BUTTON控件实现。
 */

#include "TP_Button.h"

/**
 * \breif TP_BUTTON控件默认属性值
 */
TP_BUTTON_Obj TP_Button_Default =
{
    TP_BUTTON_DEFAULT_ID,
    TP_BUTTON_DEFAULT_ISPRESSED,
    TP_BUTTON_DEFAULT_FOCUSSABLE,
    TP_BUTTON_DEFAULT_FOCUSCOLOR,
    TP_BUTTON_DEFAULT_PBKBITMAP,
    TP_BUTTON_DEFAULT_ABKCOLOR,
    TP_BUTTON_DEFAULT_BKCOLORALPHA,
    TP_BUTTON_DEFAULT_PTEXT,
    TP_BUTTON_DEFAULT_ATEXTCOLOR,
    TP_BUTTON_DEFAULT_PFONT,
    TP_BUTTON_DEFAULT_TEXTALIGN,
    TP_BUTTON_DEFAULT_TEXTALPHA,
    TP_BUTTON_DEFAULT_RADIUS,
    TP_BUTTON_DEFAULT_ABORDERCOLOR,
    TP_BUTTON_DEFAULT_ABORDERSIZE,
    TP_BUTTON_DEFAULT_NUMEXTRABYTES
};

/*********************************************************************
*
* 属性值Get/Set方法
*/
int TP_Button_GetBorderSize(TP_BUTTON_Handle hObj, unsigned int index)
{
    TP_BUTTON_Obj   obj;

    if (hObj && (index < TP_BUTTON_BORDER_NUM))
    {
        TP_GUI_LOCK();
        WM_GetUserData(hObj, &obj, sizeof(TP_BUTTON_Obj));
        TP_GUI_UNLOCK();

        return obj.aBorderSize[index];
    }

    return RET_FAIL;
}

void TP_Button_SetBorderSizeEx(TP_BUTTON_Handle hObj, U8 * aSize)
{
    TP_BUTTON_Obj   obj;
    unsigned int    index;

    if (hObj)
    {
        TP_GUI_LOCK();
        WM_GetUserData(hObj, &obj, sizeof(TP_BUTTON_Obj));
        for(index=0; index<TP_BUTTON_BORDER_NUM; index++)
        {
            obj.aBorderSize[index] = aSize[index];
        }
        WM_SetUserData(hObj, &obj, sizeof(TP_BUTTON_Obj));
        TP_GUI_UNLOCK();
    }
}

void TP_Button_SetBorderSize(TP_BUTTON_Handle hObj, unsigned int index, U8 size)
{
    TP_BUTTON_Obj   obj;

    if (hObj)
    {
        TP_GUI_LOCK();
        WM_GetUserData(hObj, &obj, sizeof(TP_BUTTON_Obj));
        obj.aBorderSize[index] = size;
        WM_SetUserData(hObj, &obj, sizeof(TP_BUTTON_Obj));
        TP_GUI_UNLOCK();
    }
}

GUI_COLOR TP_Button_GetBorderColor(TP_BUTTON_Handle hObj, unsigned int index)
{
    TP_BUTTON_Obj   obj;
    GUI_COLOR       color = GUI_INVALID_COLOR;

    if (hObj && (index < TP_BUTTON_STATE_COUNT))
    {
        TP_GUI_LOCK();
        WM_GetUserData(hObj, &obj, sizeof(TP_BUTTON_Obj));
        TP_GUI_UNLOCK();

        color = obj.aBorderColor[index];
    }

    return color;
}

void TP_Button_SetBorderColorEx(TP_BUTTON_Handle hObj, GUI_COLOR * aColor)
{
    TP_Button_SetBorderColor(hObj, TP_BUTTON_UNPRESSED, aColor[TP_BUTTON_UNPRESSED]);
    TP_Button_SetBorderColor(hObj, TP_BUTTON_PRESSED, aColor[TP_BUTTON_PRESSED]);
    TP_Button_SetBorderColor(hObj, TP_BUTTON_DISABLED, aColor[TP_BUTTON_DISABLED]);
}

void TP_Button_SetBorderColor(TP_BUTTON_Handle hObj, unsigned int index, GUI_COLOR color)
{
    TP_BUTTON_Obj obj;

    if (hObj && (index < TP_BUTTON_STATE_COUNT))
    {
        TP_GUI_LOCK();
        WM_GetUserData(hObj, &obj, sizeof(TP_BUTTON_Obj));
        obj.aBorderColor[index] = color;
        WM_SetUserData(hObj, &obj, sizeof(TP_BUTTON_Obj));
        TP_GUI_UNLOCK();
    }
}

int TP_Button_GetRadius(TP_BUTTON_Handle hObj)
{
    TP_BUTTON_Obj obj;

    if (hObj)
    {
        TP_GUI_LOCK();
        WM_GetUserData(hObj, &obj, sizeof(TP_BUTTON_Obj));
        TP_GUI_UNLOCK();

        return obj.radius;
    }

    return RET_FAIL;
}

void TP_Button_SetRadius(TP_BUTTON_Handle hObj, int radius)
{
    TP_BUTTON_Obj obj;

    if (hObj)
    {
        TP_GUI_LOCK();
        WM_GetUserData(hObj, &obj, sizeof(TP_BUTTON_Obj));
        obj.radius = radius;
        WM_SetUserData(hObj, &obj, sizeof(TP_BUTTON_Obj));
        TP_GUI_UNLOCK();
    }
}

int TP_Button_GetTextAlpha(TP_BUTTON_Handle hObj)
{
    TP_BUTTON_Obj obj;

    if (hObj)
    {
        TP_GUI_LOCK();
        WM_GetUserData(hObj, &obj, sizeof(TP_BUTTON_Obj));
        TP_GUI_UNLOCK();

        return obj.textAlpha;
    }

    return RET_FAIL;
}

void TP_Button_SetTextAlpha(TP_BUTTON_Handle hObj, int alpha)
{
    TP_BUTTON_Obj obj;

    if (hObj && (alpha >= TP_BUTTON_ALPHA_MIN) && (alpha <= TP_BUTTON_ALPHA_MAX))
    {
        TP_GUI_LOCK();
        WM_GetUserData(hObj, &obj, sizeof(TP_BUTTON_Obj));
        obj.textAlpha = alpha;
        WM_SetUserData(hObj, &obj, sizeof(TP_BUTTON_Obj));
        TP_GUI_UNLOCK();
    }
}

int TP_Button_GetTextAlign(TP_BUTTON_Handle hObj)
{
    TP_BUTTON_Obj obj;

    if (hObj)
    {
        TP_GUI_LOCK();
        WM_GetUserData(hObj, &obj, sizeof(TP_BUTTON_Obj));
        TP_GUI_UNLOCK();

        return obj.textAlign;
    }

    return RET_FAIL;
}

void TP_Button_SetTextAlign(TP_BUTTON_Handle hObj, int align)
{
    TP_BUTTON_Obj obj;

    if (hObj)
    {
        TP_GUI_LOCK();
        WM_GetUserData(hObj, &obj, sizeof(TP_BUTTON_Obj));
        obj.textAlign = align;
        WM_SetUserData(hObj, &obj, sizeof(TP_BUTTON_Obj));
        TP_GUI_UNLOCK();
    }
}

const GUI_FONT * TP_Button_GetTextFont(TP_BUTTON_Handle hObj)
{
    TP_BUTTON_Obj obj;

    if (hObj)
    {
        TP_GUI_LOCK();
        WM_GetUserData(hObj, &obj, sizeof(TP_BUTTON_Obj));
        TP_GUI_UNLOCK();

        return obj.pFont;
    }

    return NULL;
}

void TP_Button_SetTextFont(TP_BUTTON_Handle hObj, const GUI_FONT * pFont)
{
    TP_BUTTON_Obj obj;

    if (hObj && (NULL != pFont))
    {
        TP_GUI_LOCK();
        WM_GetUserData(hObj, &obj, sizeof(TP_BUTTON_Obj));
        obj.pFont = pFont;
        WM_SetUserData(hObj, &obj, sizeof(TP_BUTTON_Obj));
        TP_GUI_UNLOCK();
    }
}

GUI_COLOR TP_Button_GetTextColor(TP_BUTTON_Handle hObj, unsigned int index)
{
    TP_BUTTON_Obj   obj;
    GUI_COLOR       color = GUI_INVALID_COLOR;

    if (hObj && (index < TP_BUTTON_STATE_COUNT))
    {
        TP_GUI_LOCK();
        WM_GetUserData(hObj, &obj, sizeof(TP_BUTTON_Obj));
        TP_GUI_UNLOCK();
        color = obj.aTextColor[index];
    }

    return color;
}

void TP_Button_SetTextColorEx(TP_BUTTON_Handle hObj, GUI_COLOR * aColor)
{
    TP_Button_SetTextColor(hObj, TP_BUTTON_UNPRESSED, aColor[TP_BUTTON_UNPRESSED]);
    TP_Button_SetTextColor(hObj, TP_BUTTON_PRESSED, aColor[TP_BUTTON_PRESSED]);
    TP_Button_SetTextColor(hObj, TP_BUTTON_DISABLED, aColor[TP_BUTTON_DISABLED]);
}

void TP_Button_SetTextColor(TP_BUTTON_Handle hObj, unsigned int index, GUI_COLOR color)
{
    TP_BUTTON_Obj obj;

    if (hObj && (index < TP_BUTTON_STATE_COUNT))
    {
        TP_GUI_LOCK();
        WM_GetUserData(hObj, &obj, sizeof(TP_BUTTON_Obj));
        obj.aTextColor[index] = color;
        WM_SetUserData(hObj, &obj, sizeof(TP_BUTTON_Obj));
        TP_GUI_UNLOCK();
    }
}

int TP_Button_GetText(TP_BUTTON_Handle hObj, char * pBuffer, int maxLen)
{
    int             len = 0;
    TP_BUTTON_Obj   obj;

    if (hObj && (NULL != pBuffer))
    {
        TP_GUI_LOCK();
        WM_GetUserData(hObj, &obj, sizeof(TP_BUTTON_Obj));
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

int TP_Button_SetText(TP_BUTTON_Handle hObj, char * pText)
{
    int             len = 0;
    TP_BUTTON_Obj   obj;

    if (hObj && (NULL != pText))
    {
        TP_GUI_LOCK();
        WM_GetUserData(hObj, &obj, sizeof(TP_BUTTON_Obj));
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
            WM_SetUserData(hObj, &obj, sizeof(TP_BUTTON_Obj));
            TP_GUI_UNLOCK();

            return RET_SUCCESS;
        }
    }

    return RET_FAIL;
}

int TP_Button_GetBkColorAlpha(TP_BUTTON_Handle hObj)
{
    TP_BUTTON_Obj obj;

    if (hObj)
    {
        TP_GUI_LOCK();
        WM_GetUserData(hObj, &obj, sizeof(TP_BUTTON_Obj));
        TP_GUI_UNLOCK();

        return obj.bkColorAlpha;
    }

    return RET_FAIL;
}

void TP_Button_SetBkColorAlpha(TP_BUTTON_Handle hObj, int alpha)
{
    TP_BUTTON_Obj obj;

    if (hObj && (alpha >= TP_BUTTON_ALPHA_MIN) && (alpha <= TP_BUTTON_ALPHA_MAX))
    {
        TP_GUI_LOCK();
        WM_GetUserData(hObj, &obj, sizeof(TP_BUTTON_Obj));
        obj.bkColorAlpha = alpha;
        WM_SetUserData(hObj, &obj, sizeof(TP_BUTTON_Obj));
        TP_GUI_UNLOCK();
    }
}

GUI_COLOR TP_Button_GetBkColor(TP_BUTTON_Handle hObj, unsigned int index)
{
    TP_BUTTON_Obj   obj;
    GUI_COLOR       color = GUI_INVALID_COLOR;

    if (hObj && (index < TP_BUTTON_STATE_COUNT))
    {
        TP_GUI_LOCK();
        WM_GetUserData(hObj, &obj, sizeof(TP_BUTTON_Obj));
        TP_GUI_UNLOCK();
        color = obj.aBkColor[index];
    }

    return color;
}

void TP_Button_SetBkColorEx(TP_BUTTON_Handle hObj, GUI_COLOR * aColor)
{
    TP_Button_SetBkColor(hObj, TP_BUTTON_UNPRESSED, aColor[TP_BUTTON_UNPRESSED]);
    TP_Button_SetBkColor(hObj, TP_BUTTON_PRESSED, aColor[TP_BUTTON_PRESSED]);
    TP_Button_SetBkColor(hObj, TP_BUTTON_DISABLED, aColor[TP_BUTTON_DISABLED]);
}

void TP_Button_SetBkColor(TP_BUTTON_Handle hObj, unsigned int index, GUI_COLOR color)
{
    TP_BUTTON_Obj obj;

    if (hObj && (index < TP_BUTTON_STATE_COUNT))
    {
        TP_GUI_LOCK();
        WM_GetUserData(hObj, &obj, sizeof(TP_BUTTON_Obj));
        obj.aBkColor[index] = color;
        WM_SetUserData(hObj, &obj, sizeof(TP_BUTTON_Obj));
        TP_GUI_UNLOCK();
    }
}

const GUI_BITMAP * TP_Button_GetBitmap(TP_BUTTON_Handle hObj, unsigned int index)
{
    TP_BUTTON_Obj obj;

    if (hObj && (index < TP_BUTTON_STATE_COUNT))
    {
        TP_GUI_LOCK();
        WM_GetUserData(hObj, &obj, sizeof(TP_BUTTON_Obj));
        TP_GUI_UNLOCK();

        return obj.pBkBitmap[index];
    }

    return NULL;
}

void TP_Button_SetBitmap(TP_BUTTON_Handle hObj, unsigned int index, const GUI_BITMAP* pBitmap)
{
    TP_BUTTON_Obj obj;

    if (hObj && (index < TP_BUTTON_STATE_COUNT) && (NULL != pBitmap))
    {
        TP_GUI_LOCK();
        WM_GetUserData(hObj, &obj, sizeof(TP_BUTTON_Obj));
        obj.pBkBitmap[index] = pBitmap;
        WM_SetUserData(hObj, &obj, sizeof(TP_BUTTON_Obj));
        TP_GUI_UNLOCK();
    }
}

GUI_COLOR TP_Button_GetFocusColor(TP_BUTTON_Handle hObj)
{
    TP_BUTTON_Obj   obj;
    GUI_COLOR       color = GUI_INVALID_COLOR;

    if (hObj)
    {
        TP_GUI_LOCK();
        WM_GetUserData(hObj, &obj, sizeof(TP_BUTTON_Obj));
        TP_GUI_UNLOCK();
        color = obj.focusColor;
    }

    return color;
}

void TP_Button_SetFocusColor(TP_BUTTON_Handle hObj, GUI_COLOR color)
{
    TP_BUTTON_Obj   obj;

    if (hObj)
    {
        TP_GUI_LOCK();
        WM_GetUserData(hObj, &obj, sizeof(TP_BUTTON_Obj));
        obj.focusColor = color;
        WM_SetUserData(hObj, &obj, sizeof(TP_BUTTON_Obj));
        TP_GUI_UNLOCK();
    }
}

int TP_Button_GetFocussable(TP_BUTTON_Handle hObj)
{
    TP_BUTTON_Obj obj;

    if (hObj)
    {
        TP_GUI_LOCK();
        WM_GetUserData(hObj, &obj, sizeof(TP_BUTTON_Obj));
        TP_GUI_UNLOCK();
        return obj.focussable;
    }

    return RET_FAIL;
}

void TP_Button_SetFocussable(TP_BUTTON_Handle hObj, int state)
{
    TP_BUTTON_Obj obj;

    if (hObj)
    {
        TP_GUI_LOCK();
        WM_GetUserData(hObj, &obj, sizeof(TP_BUTTON_Obj));
        obj.focussable = state;
        WM_SetUserData(hObj, &obj, sizeof(TP_BUTTON_Obj));
        TP_GUI_UNLOCK();
    }
}

int TP_Button_IsPressed(TP_BUTTON_Handle hObj)
{
    TP_BUTTON_Obj obj;

    if (hObj)
    {
        TP_GUI_LOCK();
        WM_GetUserData(hObj, &obj, sizeof(TP_BUTTON_Obj));
        TP_GUI_UNLOCK();

        return obj.isPressed;
    }

    return RET_FAIL;
}

void TP_Button_SetPressed(TP_BUTTON_Handle hObj, int state)
{
    TP_BUTTON_Obj obj;

    if (hObj)
    {
        TP_GUI_LOCK();
        WM_GetUserData(hObj, &obj, sizeof(TP_BUTTON_Obj));
        obj.isPressed = state;
        WM_SetUserData(hObj, &obj, sizeof(TP_BUTTON_Obj));
        TP_GUI_UNLOCK();
    }
}

int TP_Button_GetId(TP_BUTTON_Handle hObj)
{
    TP_BUTTON_Obj obj;

    if (hObj)
    {
        TP_GUI_LOCK();
        WM_GetUserData(hObj, &obj, sizeof(TP_BUTTON_Obj));
        TP_GUI_UNLOCK();

        return obj.id;
    }

    return RET_FAIL;
}

void TP_Button_SetId(TP_BUTTON_Handle hObj, int id)
{
    TP_BUTTON_Obj obj;

    if (hObj)
    {
        TP_GUI_LOCK();
        WM_GetUserData(hObj, &obj, sizeof(TP_BUTTON_Obj));
        obj.id = id;
        WM_SetUserData(hObj, &obj, sizeof(TP_BUTTON_Obj));
        TP_GUI_UNLOCK();
    }
}

/*********************************************************************
*
* 默认属性值Get/Set方法
*/
U8 TP_Button_GetDefaultBorderSize(unsigned int index)
{
    return TP_Button_Default.aBorderSize[index];
}

void TP_Button_SetDefaultBorderSize(unsigned int index, U8 size)
{
    TP_Button_Default.aBorderSize[index] = size;
}

GUI_COLOR TP_Button_GetDefaultBorderColor(unsigned int index)
{
    return TP_Button_Default.aBorderColor[index];
}

void TP_Button_SetDefaultBorderColor(unsigned int index, GUI_COLOR color)
{
    TP_Button_Default.aBorderColor[index] = color;
}

int TP_Button_GetDefaultRadius()
{
    return TP_Button_Default.radius;
}

void TP_Button_SetDefaultRadius(int radius)
{
    TP_Button_Default.radius = radius;
}

int TP_Button_GetDefaultTextAlpha()
{
    return TP_Button_Default.textAlpha;
}

void TP_Button_SetDefaultTextAlpha(int alpha)
{
    TP_Button_Default.textAlpha = alpha;
}

int TP_Button_GetDefaultTextAlign()
{
    return TP_Button_Default.textAlign;
}

void TP_Button_SetDefaultTextAlign(int align)
{
    TP_Button_Default.textAlign = align;
}

const GUI_FONT * TP_Button_GetDefaultTextFont()
{
    return TP_Button_Default.pFont;
}

void TP_Button_SetDefaultTextFont(const GUI_FONT * pFont)
{
    TP_Button_Default.pFont = pFont;
}

GUI_COLOR TP_Button_GetDefaultTextColor(unsigned int index)
{
    GUI_COLOR color = GUI_INVALID_COLOR;
    if (index < TP_BUTTON_STATE_COUNT)
    {
        color = TP_Button_Default.aTextColor[index];
    }

    return color;
}

void TP_Button_SetDefaultTextColor(unsigned int index, GUI_COLOR color)
{
    if (index < TP_BUTTON_STATE_COUNT)
    {
        TP_Button_Default.aTextColor[index] = color;
    }
}

int TP_Button_GetDefaultText(char * pBuffer, int maxLen)
{
    int len = 0;

    if ((NULL != TP_Button_Default.pText) && (NULL != pBuffer))
    {
        len = strlen(TP_Button_Default.pText);

        if (len > maxLen)
        {
            /* FIXME: 用户分配空间过小，暂时截断处理 */
            TP_MEMCPY(pBuffer, TP_Button_Default.pText, maxLen);
            pBuffer[maxLen - 1] = '\0';

            return maxLen;
        }
        else
        {
            TP_MEMCPY(pBuffer, TP_Button_Default.pText, len);

            return len;
        }
    }

    return RET_FAIL;
}

int TP_Button_SetDefaultText(char * pText)
{
    int len = 0;

    if (NULL != pText)
    {
        if (NULL != TP_Button_Default.pText)
        {
            TP_FREE(TP_Button_Default.pText);
        }

        len = strlen(pText) + 1;
        TP_Button_Default.pText = TP_MALLOC(sizeof(char) * len);
        if (NULL != TP_Button_Default.pText)
        {
            TP_MEMSET(TP_Button_Default.pText, 0, len);
            TP_MEMCPY(TP_Button_Default.pText, pText, len);

            return RET_SUCCESS;
        }
    }

    return RET_FAIL;
}

int TP_Button_GetDefaultBkColorAlpha()
{
    return TP_Button_Default.bkColorAlpha;
}

void TP_Button_SetDefaultBkColorAlpha(int alpha)
{
    TP_Button_Default.bkColorAlpha = alpha;
}

GUI_COLOR TP_Button_GetDefaultBkColor(unsigned int index)
{
    GUI_COLOR color = GUI_INVALID_COLOR;

    if (index < TP_BUTTON_STATE_COUNT)
    {
        color = TP_Button_Default.aBkColor[index];
    }
    return color;
}

void TP_Button_SetDefaultBkColor(unsigned int index, GUI_COLOR color)
{
    if (index < TP_BUTTON_STATE_COUNT)
    {
        TP_Button_Default.aBkColor[index] = color;
    }
}

GUI_COLOR TP_Button_GetDefaultFocusColor()
{
    return TP_Button_Default.focusColor;
}

void TP_Button_SetDefaultFocusColor(GUI_COLOR color)
{
    TP_Button_Default.focusColor = color;
}

int TP_Button_GetDefaultFocussable()
{
    return TP_Button_Default.focussable;
}

void TP_Button_SetDefaultFocussable(int state)
{
    TP_Button_Default.focussable = state;
}

int TP_Button_IsDefaultPressed()
{
    return TP_Button_Default.isPressed;
}

void TP_Button_SetDefaultPressed(int state)
{
    TP_Button_Default.isPressed= state;
}

int TP_Button_GetDefaultId()
{
    return TP_Button_Default.id;
}

void TP_Button_SetDefaultId(int id)
{
    TP_Button_Default.id = id;
}

/*********************************************************************
*
* 创建控件相关方法
*/
void TP_Button_Callback(WM_MESSAGE * pMsg)
{
    TP_BUTTON_Handle            hWin;
    GUI_RECT                    winRect;
    TP_BUTTON_Obj               hObj;
    int                         colorIndex;
    int                         bmpIndex;
    GUI_PID_STATE *             pState;
    int                         pressed;
    U8                          oldPenSize;

    WM_HWIN                     hParentWin;
    WM_PID_STATE_CHANGED_INFO * pPidState;

    hWin = pMsg->hWin;
    WM_GetWindowRectEx(hWin, &winRect);
    GUI_MoveRect(&winRect, -winRect.x0, -winRect.y0);
    WM_GetUserData(hWin, &hObj, sizeof(TP_BUTTON_Obj));

    switch (pMsg->MsgId)
    {
    case WM_PAINT:
        if (WM_IsEnabled(hWin))
        {
            if (hObj.isPressed)
            {
                colorIndex = TP_BUTTON_PRESSED;
            }
            else
            {
                colorIndex = TP_BUTTON_UNPRESSED;
            }
        }
        else
        {
            colorIndex = TP_BUTTON_DISABLED;
        }

        bmpIndex = colorIndex;
        /* 图与颜色都设置时，背景图优先，都没有时使用默认颜色 */
        if (NULL != hObj.pBkBitmap[bmpIndex])
        {
            GUI_DrawBitmap(hObj.pBkBitmap[bmpIndex], winRect.x0, winRect.y0);
        }
        else
        {
            /* 设置背景透明度 */
            if (0 != hObj.bkColorAlpha)
            {
                GUI_EnableAlpha(1);
                GUI_SetAlpha(hObj.bkColorAlpha);
            }

            /* 绘制指定背景颜色，若没有指定背景颜色，则绘制默认背景颜色 */
            GUI_SetColor(hObj.aBkColor[colorIndex] ? hObj.aBkColor[colorIndex] : TP_Button_Default.aBkColor[colorIndex]);
            if (0 == hObj.radius)
            {
                GUI_FillRectEx(&winRect);

                /* 指定边框颜色的话进行绘制 */
                oldPenSize = GUI_GetPenSize();
                GUI_SetColor(hObj.aBorderColor[colorIndex] ? hObj.aBorderColor[colorIndex] : TP_Button_Default.aBorderColor[colorIndex]);
                if (hObj.aBorderSize[TP_BUTTON_BORDER_TOP] > 0)
                {
                    GUI_SetPenSize(hObj.aBorderSize[TP_BUTTON_BORDER_TOP]);
                    GUI_DrawHLine(winRect.y0, winRect.x0, winRect.x1);
                }
                if (hObj.aBorderSize[TP_BUTTON_BORDER_BOTTOM] > 0)
                {
                    GUI_SetPenSize(hObj.aBorderSize[TP_BUTTON_BORDER_BOTTOM]);
                    GUI_DrawHLine(winRect.y1, winRect.x0, winRect.x1);
                }
                if (hObj.aBorderSize[TP_BUTTON_BORDER_LEFT] > 0)
                {
                    GUI_SetPenSize(hObj.aBorderSize[TP_BUTTON_BORDER_LEFT]);
                    GUI_DrawVLine(winRect.x0, winRect.y0, winRect.y1);
                }
                if (hObj.aBorderSize[TP_BUTTON_BORDER_RIGHT] > 0)
                {
                    GUI_SetPenSize(hObj.aBorderSize[TP_BUTTON_BORDER_RIGHT]);
                    GUI_DrawVLine(winRect.x1, winRect.y0, winRect.y1);
                }
                GUI_SetPenSize(oldPenSize);
            }
            else
            {
                /* 抗锯齿圆角矩形 */
                TP_AA_FillRoundedRect(winRect.x0, winRect.y0, winRect.x1, winRect.y1, hObj.radius);
            }

            /* Alpha值设回0 */
            if (0 != hObj.bkColorAlpha)
            {
                GUI_SetAlpha(0);
            }
        }

        /* 绘制文本 */
        if (NULL != hObj.pText)
        {
            /* 设置文本透明度 */
            if (0 != hObj.textAlpha)
            {
                GUI_EnableAlpha(1);
                GUI_SetAlpha(hObj.textAlpha);
            }

            /* 没有指定颜色，使用默认颜色 */
            GUI_SetColor(hObj.aTextColor[colorIndex] ? hObj.aTextColor[colorIndex] : TP_Button_Default.aTextColor[colorIndex]);
            GUI_SetTextMode(GUI_TEXTMODE_TRANS);
            /* 没有指定字体，使用默认字体 */
            GUI_SetFont(hObj.pFont ? hObj.pFont : TP_Button_Default.pFont);

            /* 文本过长，截断尾部，以...代替 */
            if (GUI_GetStringDistX(hObj.pText) > (winRect.x1 - winRect.x0))
            {
                int len = strlen(hObj.pText) + 1;
                double dist = 1.0 * GUI_GetStringDistX(hObj.pText) / strlen(hObj.pText);
                char * dispStr = NULL;

                dispStr = TP_MALLOC(sizeof(char) * len);
                if (NULL != dispStr)
                {
                    TP_MEMSET(dispStr, 0, len);
                    TP_MEMCPY(dispStr, hObj.pText, len);

                    /* 替换位置 */
                    len = (int)((winRect.x1 - winRect.x0) / dist) - strlen(REPLACE);
                    strncpy(dispStr+len, REPLACE, strlen(REPLACE)+1);
                    GUI_DispStringInRect(dispStr, &winRect, hObj.textAlign ? hObj.textAlign : TP_Button_Default.textAlign);

                    TP_FREE(dispStr);
                }
            }
            else
            {
                GUI_DispStringInRect(hObj.pText, &winRect, hObj.textAlign ? hObj.textAlign : TP_Button_Default.textAlign);
            }

            /* Alpha值设回默认值0 */
            if (0 != hObj.textAlpha)
            {
                GUI_SetAlpha(0);
            }
        }
        break;

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

    case WM_TOUCH:
        pState      = (GUI_PID_STATE *)pMsg->Data.p;
        hParentWin  = WM_GetParent(pMsg->hWin);

        if (NULL != pState)
        {
            pressed = pState->Pressed;
        }
        else
        {
            pressed = TP_BUTTON_UNPRESSED;
        }

        if (hObj.isPressed != pressed)
        {
            hObj.isPressed = pressed;
            WM_SetUserData(hWin, &hObj, sizeof(TP_BUTTON_Obj));
            if (hObj.isPressed && hObj.focussable)
            {
                WM_SetFocus(hWin);
            }
            WM_InvalidateWindow(hWin);
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
        WM_SetUserData(hWin, &hObj, sizeof(TP_BUTTON_Obj));
        if (NULL != TP_Button_Default.pText)
        {
            TP_FREE(TP_Button_Default.pText);
        }
        break;

    case WM_GET_ID:
        pMsg->Data.v = hObj.id;
        break;

    case WM_SET_ID:
        hObj.id = pMsg->Data.v;
        WM_SetUserData(hWin, &hObj, sizeof(TP_BUTTON_Obj));
        break;

    default:
        WM_DefaultProc(pMsg);
    }
}

int TP_Button_SetUserData(TP_BUTTON_Handle hObj, void * pSrc, int size)
{
    TP_BUTTON_Obj   obj;
    U8 *            pExtraBytes;
    int             numBytes;

    if (size <= 0)
    {
        return RET_FAIL;
    }

    WM_GetUserData(hObj, &obj, sizeof(TP_BUTTON_Obj));

    pExtraBytes = (U8 *)TP_MALLOC(sizeof(TP_BUTTON_Obj) + obj.numExtraBytes);
    if (NULL != pExtraBytes)
    {
        WM_GetUserData(hObj, pExtraBytes, sizeof(TP_BUTTON_Obj) + obj.numExtraBytes);
        if (size >= obj.numExtraBytes)
        {
            numBytes = obj.numExtraBytes;
        }
        else
        {
            numBytes = size;
        }
        TP_MEMCPY(pExtraBytes + sizeof(TP_BUTTON_Obj), pSrc, numBytes);
        WM_SetUserData(hObj, pExtraBytes, sizeof(TP_BUTTON_Obj) + obj.numExtraBytes);
        TP_FREE(pExtraBytes);

        return RET_SUCCESS;
    }

    return RET_FAIL;
}

int TP_Button_GetUserData(TP_BUTTON_Handle hObj, void * pDest, int size)
{
    TP_BUTTON_Obj   obj;
    U8 *            pExtraBytes;
    int             numBytes;

    if (size <= 0)
    {
        return 0;
    }

    WM_GetUserData(hObj, &obj, sizeof(TP_BUTTON_Obj));
    pExtraBytes = (U8 *)TP_MALLOC(sizeof(TP_BUTTON_Obj) + obj.numExtraBytes);
    if (pExtraBytes)
    {
        WM_GetUserData(hObj, pExtraBytes, sizeof(TP_BUTTON_Obj) + obj.numExtraBytes);
        if (size >= obj.numExtraBytes)
        {
            numBytes = obj.numExtraBytes;
        }
        else
        {
            numBytes = size;
        }
        TP_MEMCPY(pDest, pExtraBytes + sizeof(TP_BUTTON_Obj), numBytes);
        TP_FREE(pExtraBytes);

        return numBytes;
    }
    return 0;
}

TP_BUTTON_Handle TP_Button_Create(int x0, int y0, int xSize, int ySize, WM_HWIN hParent,
        U8 style, WM_CALLBACK * cb, int numExtraBytes, int id)
{
    WM_CALLBACK *       pUsed;
    TP_BUTTON_Obj       obj;
    TP_BUTTON_Handle    hWin;

    obj = TP_Button_Default;
    obj.numExtraBytes = numExtraBytes;
    obj.id = id;

    if (cb)
    {
        pUsed = cb;
    }
    else
    {
        pUsed = TP_Button_Callback;
    }

    hWin = WM_CreateWindowAsChild(x0, y0, xSize, ySize, hParent,
    style | WM_CF_SHOW | WM_CF_MEMDEV | WM_CF_HASTRANS, pUsed,
    sizeof(TP_BUTTON_Obj) + numExtraBytes);

    if (hWin)
    {
        WM_SetUserData(hWin, &obj, sizeof(TP_BUTTON_Obj));
    }

    return hWin;
}
