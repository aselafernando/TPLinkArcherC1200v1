/*! Copyright (c) 2014 Shenzhen TP-LINK Technologies Co.Ltd.
 *
 * \file     TP_Text.h
 * \brief    TP_TEXT控件的定义与函数原型.
 *
 * \version  1.0.0
 * \date     03Nov14
 */
#ifndef TP_TEXT_H__
#define TP_TEXT_H__

#include "WM.h"
#include "TEXT.h"

/**
 * \brief TP_TEXT_Handle对象。
 *
 * 便于用户区分控件类型。
 */
typedef WM_HMEM TP_TEXT_Handle;

/**
 * \brief       在指定位置创建指定大小的TP_TEXT控件。
 * \param       x0          TP_TEXT控件最左侧的像素（在父坐标中）。
 * \param       y0          TP_TEXT控件最顶端的像素（在父坐标中）。
 * \param       xSize       TP_TEXT控件的水平尺寸（以像素为单位）。
 * \param       ySize       TP_TEXT控件的竖直尺寸（以像素为单位）。
 * \param       hParent     父窗口的句柄。如果为0，则创建的控件作为桌面（顶层窗口）的子窗口。
 * \param       WinFlags    窗口创建标记，默认开启WM_CF_SHOW，WM_CF_MEMDEV， WM_CF_HASTRANS。
 * \param       ExFlags     文本的对齐属性。
 * \param       Id          TEXT控件的窗口ID。
 * \param       pText       要显示的文本的指针。
 * \return      所创建的TP_TEXT控件的句柄，如果函数执行失败，则返回值为0。
 */
TP_TEXT_Handle TP_Text_CreateEx(int x0,             int y0,
                                int xSize,          int ySize,
                                WM_HWIN hParent,    int WinFlags,
                                int ExFlags,        int Id,
                                const char * pText);

/**
 * \brief       返回TP_TEXT控件所使用的默认字体。
 * \return      TP_TEXT控件所使用的默认字体指针。
 */
const GUI_FONT * TP_Text_GetDefaultFont(void);

/**
 * \brief       返回当前在TP_TEXT控件中所显示的线条数。
 * \param       hObj        TP_TEXT控件的句柄。
 * \return      线条数。
 */
int TP_Text_GetNumLines(TP_TEXT_Handle hObj);

/**
 * \brief       设置TP_TEXT控件的背景颜色。
 * \param       hObj        TP_TEXT控件的句柄。
 * \param       Color       待设置背景颜色（范围0x000000和0xFFFFFF）,GUI_INVALID_COLOR可使背景透明。
 */
void TP_Text_SetBkColor(TP_TEXT_Handle hObj, GUI_COLOR Color);

/**
 * \brief       设置TP_TEXT控件的默认字体。
 * \param       pFont       要设置成默认值的字体指针。
 */
void TP_Text_SetDefaultFont(const GUI_FONT * pFont);

/**
 * \brief       设置TP_TEXT控件的默认文本颜色。
 * \param       Color       要使用的颜色。
 */
void TP_Text_SetDefaultColor(GUI_COLOR Color);

/**
 * \brief       设置TP_TEXT控件的默认文本覆盖模式。
 * \param       Color       要使用的默认文本覆盖模式。允许如下值：
 *
 * GUI_WRAPMODE_NONE    未执行任何覆盖。
 * GUI_WRAPMODE_WORD    文本按字方式覆盖。
 * GUI_WRAPMODE_CHAR    文本按字符方式覆盖。
 */
GUI_WRAPMODE TP_Text_SetDefaultWrapMode(GUI_WRAPMODE WrapMode);

/**
 * \brief       设置TP_TEXT控件的字体。
 * \param       hObj        TP_TEXT控件的句柄。
 * \param       pFont       要使用的字体指针。
 */
void TP_Text_SetFont(TP_TEXT_Handle hObj, const GUI_FONT * pFont);

/**
 * \brief       设置TP_TEXT控件的文本。
 * \param       hObj        TP_TEXT控件的句柄。
 * \param       s           要显示的文本。
 */
void TP_Text_SetText(TP_TEXT_Handle hObj, const char * s);

/**
 * \brief       设置TP_TEXT控件的文本对齐方式。
 * \param       hObj        TP_TEXT控件的句柄。
 * \param       Align       文本对齐方式。
 */
void TP_Text_SetTextAlign(TP_TEXT_Handle hObj, int align);

/**
 * \brief       设置TP_TEXT控件的文本颜色。
 * \param       hObj        TP_TEXT控件的句柄。
 * \param       color       新文本颜色。
 */
void TP_Text_SetTextColor(TP_TEXT_Handle hObj, GUI_COLOR color);

/**
 * \brief       设置TP_TEXT控件的文本覆盖模式。
 * \param       Color       要使用的文本覆盖模式。允许如下值：
 *
 * GUI_WRAPMODE_NONE    未执行任何覆盖。
 * GUI_WRAPMODE_WORD    文本按字方式覆盖。
 * GUI_WRAPMODE_CHAR    文本按字符方式覆盖。
 */
void TP_Text_SetWrapMode(TP_TEXT_Handle hObj, GUI_WRAPMODE WrapMode);

#endif /* TP_TEXT_H__ */
