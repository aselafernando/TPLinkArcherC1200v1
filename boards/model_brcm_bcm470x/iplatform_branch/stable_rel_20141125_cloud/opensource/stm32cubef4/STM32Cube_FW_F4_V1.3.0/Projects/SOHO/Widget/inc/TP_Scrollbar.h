/*! Copyright (c) 2014 Shenzhen TP-LINK Technologies Co.Ltd.
 *
 *\file     TP_Scrollbar.h
 *\brief    TP_SCROLLBAR控件的定义与函数原型。
 *
 *\version  1.0.0
 *\date     05Nov14
 */
#ifndef TP_SCROLLBAR_H__
#define TP_SCROLLBAR_H__

#include <string.h>
#include <stdlib.h>
#include "TP_Widget.h"

/**
 * \brief 显示屏幕的宽度
 */
#define TP_SCROLLBAR_LCD_WIDTH              480
/**
 * \brief 显示屏幕的高度
 */
#define TP_SCROLLBAR_LCD_HEIGHT             272

/// TP_SCROLLBAR的默认ID
#define TP_SCROLLBAR_DEFAULT_ID             72
/// TP_SCROLLBAR所在显示区域的默认水平尺寸，通常为整个显示屏
#define TP_SCROLLBAR_DEFAULT_XSIZEWIN       0
/// TP_SCROLLBAR所在显示区域的默认垂直尺寸，通常为整个显示屏
#define TP_SCROLLBAR_DEFAULT_YSIZEWIN       0
/// TP_SCROLLBAR父窗口的默认水平尺寸
#define TP_SCROLLBAR_DEFAULT_XSIZELCD       TP_SCROLLBAR_LCD_WIDTH
/// TP_SCROLLBAR父窗口的默认垂直尺寸
#define TP_SCROLLBAR_DEFAULT_YSIZELCD       TP_SCROLLBAR_LCD_HEIGHT
/// TP_SCROLLBAR窗口的默认水平尺寸
#define TP_SCROLLBAR_DEFAULT_WIDTH          3
/// TP_SCROLLBAR窗口的默认垂直尺寸
#define TP_SCROLLBAR_DEFAULT_HEIGHT         2
/// 垂直方向TP_SCROLLBAR显示部分顶点在父窗口中的默认水平位置
#define TP_SCROLLBAR_DEFAULT_XPOS           0
/// 垂直方向TP_SCROLLBAR显示部分顶点在父窗口中的默认垂直位置
#define TP_SCROLLBAR_DEFAULT_YPOS           0
/// TP_SCROLLBAR移动或滑动时显示的默认颜色。
#define TP_SCROLLBAR_DEFAULT_BKCOLOR        0xB2B2B2
/// TP_SCROLLBAR的主窗口是否正在移动。
#define TP_SCROLLBAR_DEFAULT_ISMOVING       0
/// TP_SCROLLBAR停止移动后的消失时间，单位（毫秒）。
#define TP_SCROLLBAR_DEFAULT_TIMEOUT        1000
/// TP_SCROLLBAR的默认额外存储空间
#define TP_SCROLLBAR_DEFAULT_NUMEXTRABYTES  0

/**
 * \brief TP_SCROLLBAR对象。
 *
 * 用于存储TP_SCROLLBAR各属性值。
 */
typedef struct
{
    /// Scrollbar ID
    int                 id;
    /// 显示窗口的水平尺寸，通常显示区为整个显示屏。
    int                 xSizeWin;
    /// 显示窗口的垂直尺寸，通常显示区为整个显示屏。
    int                 ySizeWin;
    /// 父窗口的水平尺寸。
    int                 xSizeLCD;
    /// 父窗口的垂直尺寸。
    int                 ySizeLCD;
    /// TP_Scrollbar的水平尺寸。
    int                 width;
    /// TP_Scrollbar的垂直尺寸。
    int                 height;
    /// 水平方向TP_Scrollbar显示部分顶点在父窗口中的水平位置。
    int                 xPos;
    /// 垂直方向TP_Scrollbar显示部分顶点在父窗口中的垂直位置。
    int                 yPos;
    /// 移动或滑动时显示的颜色。
    GUI_COLOR           bkColor;
    /// 主窗口是否正在移动。
    int                 isMoving;
    /// 停止移动后消失时间。
    int                 timeout;
    /// 额外存储空间
    int                 numExtraBytes;
} TP_SCROLLBAR_Obj;

/**
 * \brief TP_SCROLLBAR_Handle对象。
 *
 * 便于用户区分控件类型。
 */
typedef WM_HMEM TP_SCROLLBAR_Handle;

/**
 * \brief        垂直方向TP_Scrollbar控件的回调函数。
 *
 * 回调函数。控件中处理了PAINT、TP_MSG_SCROLLBAR_Y等消息。
 * \param        pMsg    窗口消息。
 */
static void TP_Scrollbar_Callback_Y(WM_MESSAGE * pMsg);

/**
 * \brief        水平方向TP_Scrollbar控件的回调函数。
 *
 * 回调函数。控件中处理了PAINT、TP_MSG_SCROLLBAR_X消息。
 * \param        pMsg    窗口消息。
 */
static void TP_Scrollbar_Callback_Y(WM_MESSAGE * pMsg);

/**
 * \brief        创建垂直方向的TP_SCROLLBAR控件窗口。
 *
 * \param        x0             父窗口的最左像素。
 * \param        y0             父窗口的最上像素。
 * \param        xSizeLCD       父窗口的水平尺寸（单位：像素）。
 * \param        ySizeLCD       父窗口的垂直尺寸（单位：像素）。
 * \param        xSizeWin       滑动窗口（整个窗口）的水平尺寸（单位：像素）。
 * \param        ySizeLCD       滑动窗口的垂直尺寸（单位：像素）。
 * \param        hParent        父窗口（TP_Scrollbar窗口与滑动窗口同属一个父窗口）的句柄。
 *                              如果为0,则为桌面（顶级窗口）的子项。
 * \param        style          窗口创建标记。
 * \param        cb             窗口回调函数，用于消息处理。
 * \param        numExtraBytes  用户私有数据的长度。
 * \param        id             TP_Scrollbar的ID。
 * \return       TP_Scrollbar的句柄。
 */
TP_SCROLLBAR_Handle TP_Scrollbar_Create_Y(int x0, int y0,
        int xSizeLCD, int ySizeLCD,
        int xSizeWin, int ySizeWin,
        WM_HWIN hParent, U8 style,
        WM_CALLBACK * cb, int numExtraBytes, int id);

/**
 * \brief        创建水平方向的TP_SCROLLBAR控件窗口。
 *
 * \param        x0             父窗口的最左像素。
 * \param        y0             父窗口的最上像素。
 * \param        xSizeLCD       父窗口的水平尺寸（单位：像素）。
 * \param        ySizeLCD       父窗口的垂直尺寸（单位：像素）。
 * \param        xSizeWin       滑动窗口（整个窗口）的水平尺寸（单位：像素）。
 * \param        ySizeLCD       滑动窗口的垂直尺寸（单位：像素）。
 * \param        hParent        父窗口（TP_Scrollbar窗口与滑动窗口同属一个父窗口）的句柄。
 *                              如果为0,则为桌面（顶级窗口）的子项。
 * \param        style          窗口创建标记。
 * \param        cb             窗口回调函数，用于消息处理。
 * \param        numExtraBytes  用户私有数据的长度。
 * \param        id             TP_Scrollbar的ID。
 * \return       TP_Scrollbar的句柄。
 */
TP_SCROLLBAR_Handle TP_Scrollbar_Create_X(int x0, int y0,
        int xSizeLCD, int ySizeLCD,
        int xSizeWin, int ySizeWin,
        WM_HWIN hParent, U8 style,
        WM_CALLBACK * cb, int numExtraBytes, int id);

#endif /* TP_SCROLLBAR_H__ */
