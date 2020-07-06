/*! Copyright (c) 2014 Shenzhen TP-LINK Technologies Co.Ltd.
 *
 * \file     TP_Scrollbar.c
 * \brief    TP_SCROLLBAR控件的实现。
 */

#include "TP_Scrollbar.h"

/**
 * \breif TP_SCROLLBAR控件默认属性值
 */
TP_SCROLLBAR_Obj TP_Scrollbar_Default =
{
    TP_SCROLLBAR_DEFAULT_ID,
    TP_SCROLLBAR_DEFAULT_XSIZEWIN,
    TP_SCROLLBAR_DEFAULT_YSIZEWIN,
    TP_SCROLLBAR_DEFAULT_XSIZELCD,
    TP_SCROLLBAR_DEFAULT_YSIZELCD,
    TP_SCROLLBAR_DEFAULT_WIDTH,
    TP_SCROLLBAR_DEFAULT_HEIGHT,
    TP_SCROLLBAR_DEFAULT_XPOS,
    TP_SCROLLBAR_DEFAULT_YPOS,
    TP_SCROLLBAR_DEFAULT_BKCOLOR,
    TP_SCROLLBAR_DEFAULT_NUMEXTRABYTES
};

static void TP_Scrollbar_Callback_Y(WM_MESSAGE * pMsg)
{
	WM_HWIN hWin;
    TP_SCROLLBAR_Obj hObj;
    int leftHeight;

	hWin = pMsg->hWin;
	WM_GetUserData(hWin, &hObj, sizeof(TP_SCROLLBAR_Obj));
	switch (pMsg->MsgId) {
	case WM_PAINT:
        leftHeight = hObj.ySizeLCD - hObj.height;
        GUI_SetColor(hObj.bkColor);
        if (hObj.yPos <= leftHeight) {
            GUI_FillRect(0, hObj.yPos, hObj.width, hObj.yPos + hObj.height);
        } else
        {
            GUI_FillRect(0, leftHeight, hObj.width, hObj.ySizeLCD);
        }
        break;
    case TP_MSG_SCROLLBAR_Y:
        hObj.yPos = (int)pMsg->Data.p;
        WM_SetUserData(hWin, &hObj, sizeof(TP_SCROLLBAR_Obj));
	default:
        WM_DefaultProc(pMsg);
	}
}

static void TP_Scrollbar_Callback_X(WM_MESSAGE * pMsg)
{
	WM_HWIN hWin;
    TP_SCROLLBAR_Obj hObj;
    int leftWidth;

	hWin = pMsg->hWin;
	WM_GetUserData(hWin, &hObj, sizeof(TP_SCROLLBAR_Obj));
	switch (pMsg->MsgId) {
	case WM_PAINT:
        leftWidth = hObj.xSizeLCD - hObj.width;
        GUI_SetColor(hObj.bkColor);
        if (hObj.xPos <= leftWidth) {
            GUI_FillRect(hObj.xPos , 0, hObj.xPos + hObj.width, hObj.height);
        } else
        {
            GUI_FillRect(leftWidth, 0, hObj.xSizeLCD, hObj.height);
        }
        break;
    case TP_MSG_SCROLLBAR_X:
        hObj.xPos = (int)pMsg->Data.p;
        WM_SetUserData(hWin, &hObj, sizeof(TP_SCROLLBAR_Obj));
	default:
        WM_DefaultProc(pMsg);
	}
}

TP_SCROLLBAR_Handle TP_Scrollbar_Create_Y(int x0, int y0, int xSizeLCD, int ySizeLCD, int xSizeWin, int ySizeWin, WM_HWIN hParent,
        U8 style, WM_CALLBACK * cb, int numExtraBytes, int id)
{
    WM_CALLBACK *       pUsed;
    TP_SCROLLBAR_Obj    obj;
    TP_SCROLLBAR_Handle hWin;

    obj                 = TP_Scrollbar_Default;
	WM_SetUserData(WM_HBKWIN, &obj, sizeof(TP_SCROLLBAR_Obj));
    obj.id              = id;
    obj.ySizeLCD        = ySizeLCD;
    obj.ySizeWin        = ySizeWin;
    obj.numExtraBytes   = numExtraBytes;

    if (cb)
    {
        pUsed = cb;
    }
    else
    {
        pUsed = TP_Scrollbar_Callback_Y;
    }

    if (ySizeWin > ySizeLCD)
    {
        /* TP_Scrollbar显示的宽度:TP_Scrollbar窗口（即显示窗口）的宽度=默认显示宽度:屏幕宽度 */
        obj.width       = xSizeLCD * TP_SCROLLBAR_DEFAULT_WIDTH / TP_SCROLLBAR_LCD_WIDTH;
        /* TP_Scrollbar显示的高度:TP_Scrollbar窗口（即显示窗口）的高度=父窗口高度:显示窗口高度 */
        obj.height      = ySizeLCD * ySizeLCD / ySizeWin;
        hWin = WM_CreateWindowAsChild(x0+xSizeLCD-obj.width, y0, obj.width, ySizeLCD, hParent,
                style | WM_CF_SHOW | WM_CF_MEMDEV | WM_CF_HASTRANS, pUsed,
                sizeof(TP_SCROLLBAR_Obj) + numExtraBytes);

    }

    if (hWin)
    {
        WM_SetUserData(hWin, &obj, sizeof(TP_SCROLLBAR_Obj));
        WM_HideWindow(hWin);
    }

    return hWin;
}

TP_SCROLLBAR_Handle TP_Scrollbar_Create_X(int x0, int y0, int xSizeLCD, int ySizeLCD, int xSizeWin, int ySizeWin, WM_HWIN hParent,
        U8 style, WM_CALLBACK * cb, int numExtraBytes, int id)
{
    WM_CALLBACK *       pUsed;
    TP_SCROLLBAR_Obj    obj;
    TP_SCROLLBAR_Handle hWin;

    obj                 = TP_Scrollbar_Default;
    obj.id              = id;
    obj.numExtraBytes   = numExtraBytes;
    obj.xSizeLCD        = xSizeLCD;
    obj.xSizeWin        = xSizeWin;

    if (cb)
    {
        pUsed = cb;
    }
    else
    {
        pUsed = TP_Scrollbar_Callback_X;
    }

    if (xSizeWin > xSizeLCD)
    {
        /* TP_Scrollbar显示的宽度:TP_Scrollbar窗口（即显示窗口）的宽度=父窗口宽度:显示窗口宽度 */
        obj.width       = xSizeLCD * xSizeLCD / xSizeWin;
        /* TP_Scrollbar显示的高度:TP_Scrollbar窗口（即显示窗口）的高度=默认显示高度:屏幕高度 */
        obj.height      = ySizeLCD * TP_SCROLLBAR_DEFAULT_HEIGHT/ TP_SCROLLBAR_LCD_HEIGHT;
        hWin = WM_CreateWindowAsChild(x0, y0+ySizeLCD-obj.height, xSizeLCD, obj.height, hParent,
                style | WM_CF_SHOW | WM_CF_MEMDEV | WM_CF_HASTRANS, pUsed,
                sizeof(TP_SCROLLBAR_Obj) + numExtraBytes);
    }

    if (hWin)
    {
        WM_SetUserData(hWin, &obj, sizeof(TP_SCROLLBAR_Obj));
        WM_HideWindow(hWin);
    }

    return hWin;
}
