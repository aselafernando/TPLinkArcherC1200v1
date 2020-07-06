/*! Copyright (c) 2014 Shenzhen TP-LINK Technologies Co.Ltd.
 *
 * \file     TP_Text.c
 * \brief    TP_TEXT控件实现。
 */

#include "TP_Text.h"

TP_TEXT_Handle TP_Text_CreateEx(int x0,             int y0,
                                int xSize,          int ySize,
                                WM_HWIN hParent,    int WinFlags,
                                int ExFlags,        int Id,
                                const char * pText)
{
	return TEXT_CreateEx(x0, y0, xSize, ySize, hParent, WinFlags, ExFlags, Id, pText);
}

const GUI_FONT * TP_Text_GetDefaultFont(void)
{
    return TEXT_GetDefaultFont();
}

int TP_Text_GetNumLines(TP_TEXT_Handle hObj)
{
    return TEXT_GetNumLines(hObj);
}

void TP_Text_SetBkColor(TP_TEXT_Handle hObj, GUI_COLOR Color)
{
	TEXT_SetBkColor(hObj, Color);
}

void TP_Text_SetDefaultFont(const GUI_FONT * pFont)
{
    TEXT_SetDefaultFont(pFont);
}

void TP_Text_SetDefaultTextColor(GUI_COLOR Color)
{
    TEXT_SetDefaultTextColor(Color);
}

GUI_WRAPMODE TP_Text_SetDefaultWrapMode(GUI_WRAPMODE WrapMode)
{
    return TEXT_SetDefaultWrapMode(WrapMode);
}

void TP_Text_SetFont(TP_TEXT_Handle hObj, const GUI_FONT * pFont)
{
	TEXT_SetFont(hObj, pFont);
}

void TP_Text_SetText(TP_TEXT_Handle hObj, const char * s)
{
	TEXT_SetText(hObj, s);
}

void TP_Text_SetTextAlign(TP_TEXT_Handle hObj, int align)
{
	TEXT_SetTextAlign(hObj, align);
}

void TP_Text_SetTextColor(TP_TEXT_Handle hObj, GUI_COLOR color)
{
	TEXT_SetTextColor(hObj, color);
}

void TP_Text_SetWrapMode(TP_TEXT_Handle hObj, GUI_WRAPMODE WrapMode)
{
    TEXT_SetWrapMode(hObj, WrapMode);
}

