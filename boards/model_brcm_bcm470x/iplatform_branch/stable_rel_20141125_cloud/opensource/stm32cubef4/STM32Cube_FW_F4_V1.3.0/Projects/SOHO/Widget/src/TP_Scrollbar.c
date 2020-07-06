/*! Copyright (c) 2014 Shenzhen TP-LINK Technologies Co.Ltd.
 *
 * \file     TP_Scrollbar.c
 * \brief    TP_SCROLLBAR�ؼ���ʵ�֡�
 */

#include "TP_Scrollbar.h"

/**
 * \breif TP_SCROLLBAR�ؼ�Ĭ������ֵ
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
    TP_SCROLLBAR_DEFAULT_ISMOVING,
    TP_SCROLLBAR_DEFAULT_TIMEOUT,
    TP_SCROLLBAR_DEFAULT_NUMEXTRABYTES
};

/********************
 * ���ڴ�����ط��� *
 ********************/
static void TP_Scrollbar_Callback_Y(WM_MESSAGE * pMsg)
{
    WM_HWIN             hWin;
    TP_SCROLLBAR_Obj    hObj;
    int                 leftHeight;

    hWin = pMsg->hWin;
    WM_GetUserData(hWin, &hObj, sizeof(TP_SCROLLBAR_Obj));
    switch (pMsg->MsgId)
    {
    case WM_PAINT:
        leftHeight = hObj.ySizeLCD - hObj.height;
        GUI_SetColor(hObj.bkColor);
        if (hObj.yPos <= leftHeight)
        {
            GUI_FillRect(0, hObj.yPos, hObj.width, hObj.yPos + hObj.height);
        }
        else
        {
            GUI_FillRect(0, leftHeight, hObj.width, hObj.ySizeLCD);
        }
        break;

    case TP_MSG_SCROLLBAR_Y:
        hObj.yPos = (int)pMsg->Data.p;
        hObj.isMoving = 1;
        WM_CreateTimer(hWin, 0, hObj.timeout, 0);
        hObj.isMoving = 0;
        WM_SetUserData(hWin, &hObj, sizeof(TP_SCROLLBAR_Obj));
        break;

    case WM_TIMER:
        if (0 == hObj.isMoving)
        {
            WM_HideWindow(hWin);
        }
        break;

    default:
        WM_DefaultProc(pMsg);
    }
}

static void TP_Scrollbar_Callback_X(WM_MESSAGE * pMsg)
{
    WM_HWIN             hWin;
    TP_SCROLLBAR_Obj    hObj;
    int                 leftWidth;

    hWin = pMsg->hWin;
    WM_GetUserData(hWin, &hObj, sizeof(TP_SCROLLBAR_Obj));
    switch (pMsg->MsgId)
    {
    case WM_PAINT:
        leftWidth = hObj.xSizeLCD - hObj.width;
        GUI_SetColor(hObj.bkColor);
        if (hObj.xPos <= leftWidth)
        {
            GUI_FillRect(hObj.xPos , 0, hObj.xPos + hObj.width, hObj.height);
        }
        else
        {
            GUI_FillRect(leftWidth, 0, hObj.xSizeLCD, hObj.height);
        }
        break;

    case TP_MSG_SCROLLBAR_X:
        hObj.xPos = (int)pMsg->Data.p;
        hObj.isMoving = 1;
        WM_CreateTimer(hWin, 0, hObj.timeout, 0);
        hObj.isMoving = 0;
        WM_SetUserData(hWin, &hObj, sizeof(TP_SCROLLBAR_Obj));
        break;

    case WM_TIMER:
        if (0 == hObj.isMoving)
        {
            WM_HideWindow(hWin);
        }
        break;

    default:
        WM_DefaultProc(pMsg);
    }
}

TP_SCROLLBAR_Handle TP_Scrollbar_Create_Y(int x0, int y0, int xSizeLCD, int ySizeLCD, int xSizeWin, int ySizeWin, WM_HWIN hParent,
        U8 style, WM_CALLBACK * cb, int numExtraBytes, int id)
{
    WM_CALLBACK *       pUsed;
    TP_SCROLLBAR_Obj    obj  = TP_Scrollbar_Default;
    TP_SCROLLBAR_Handle hWin = WM_HMEM_NULL;

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
        /* TP_Scrollbar��ʾ�Ŀ��:TP_Scrollbar���ڣ�����ʾ���ڣ��Ŀ��=Ĭ����ʾ���:��Ļ��� */
        obj.width       = xSizeLCD * TP_SCROLLBAR_DEFAULT_WIDTH / TP_SCROLLBAR_LCD_WIDTH;
        /* TP_Scrollbar��ʾ�ĸ߶�:TP_Scrollbar���ڣ�����ʾ���ڣ��ĸ߶�=�����ڸ߶�:��ʾ���ڸ߶� */
        obj.height      = ySizeLCD * ySizeLCD / ySizeWin;
        hWin = WM_CreateWindowAsChild(x0+xSizeLCD-obj.width, y0, obj.width, ySizeLCD, hParent,
                style | WM_CF_SHOW | WM_CF_MEMDEV | WM_CF_HASTRANS, pUsed,
                sizeof(TP_SCROLLBAR_Obj) + numExtraBytes);

    }

    if (hWin)
    {
        WM_CreateTimer(hWin, 0, obj.timeout, 0);
        WM_SetUserData(hWin, &obj, sizeof(TP_SCROLLBAR_Obj));
    }

    return hWin;
}

TP_SCROLLBAR_Handle TP_Scrollbar_Create_X(int x0, int y0, int xSizeLCD, int ySizeLCD, int xSizeWin, int ySizeWin, WM_HWIN hParent,
        U8 style, WM_CALLBACK * cb, int numExtraBytes, int id)
{
    WM_CALLBACK *       pUsed;
    TP_SCROLLBAR_Obj    obj  = TP_Scrollbar_Default;
    TP_SCROLLBAR_Handle hWin = WM_HMEM_NULL;

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
        /* TP_Scrollbar��ʾ�Ŀ��:TP_Scrollbar���ڣ�����ʾ���ڣ��Ŀ��=�����ڿ��:��ʾ���ڿ�� */
        obj.width       = xSizeLCD * xSizeLCD / xSizeWin;
        /* TP_Scrollbar��ʾ�ĸ߶�:TP_Scrollbar���ڣ�����ʾ���ڣ��ĸ߶�=Ĭ����ʾ�߶�:��Ļ�߶� */
        obj.height      = ySizeLCD * TP_SCROLLBAR_DEFAULT_HEIGHT/ TP_SCROLLBAR_LCD_HEIGHT;
        hWin = WM_CreateWindowAsChild(x0, y0+ySizeLCD-obj.height, xSizeLCD, obj.height, hParent,
                style | WM_CF_SHOW | WM_CF_MEMDEV | WM_CF_HASTRANS, pUsed,
                sizeof(TP_SCROLLBAR_Obj) + numExtraBytes);
    }

    if (hWin)
    {
        WM_CreateTimer(hWin, 0, obj.timeout, 0);
        WM_SetUserData(hWin, &obj, sizeof(TP_SCROLLBAR_Obj));
    }

    return hWin;
}
