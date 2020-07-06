/*! Copyright (c) 2014 Shenzhen TP-LINK Technologies Co.Ltd.
 *
 * \file     TP_Button.h
 * \brief    TP_BUTTON控件的定义与函数原型。
 *
 * \version  1.0.0
 * \date     30Oct14
 */
#ifndef TP_BUTTON_H__
#define TP_BUTTON_H__

#include <string.h>
#include <stdlib.h>
#include "TP_Widget.h"

/// TP_Button状态最小值
#define TP_BUTTON_STATE_MIN             0
/// TP_Button非按下状态值
#define TP_BUTTON_UNPRESSED             0
/// TP_Button按下状态值
#define TP_BUTTON_PRESSED               1
/// TP_Button禁用状态值
#define TP_BUTTON_DISABLED              2
/// TP_Button状态
#define TP_BUTTON_STATE_COUNT           3
/// 检查Index是否合法
#define CHECK_INDEX(index)              (index < TP_BUTTON_STATE_COUNT)

/// 边框位置，上边
#define TP_BUTTON_BORDER_TOP            0
/// 边框位置，下边
#define TP_BUTTON_BORDER_BOTTOM         1
/// 边框位置，左边
#define TP_BUTTON_BORDER_LEFT           2
/// 边框位置，右边
#define TP_BUTTON_BORDER_RIGHT          3
/// 边框个数
#define TP_BUTTON_BORDER_NUM            4


/// 透明度最小值0，表示完全不透明
#define TP_BUTTON_ALPHA_MIN             0
/// 透明度最大值255，表示完全透明
#define TP_BUTTON_ALPHA_MAX             255

/// 显示文本超出窗口时替换符号
#define REPLACE                         "..."

/// TP_Button默认ID：71
#define TP_BUTTON_DEFAULT_ID            71
/// TP_Button默认状态：非按下状态
#define TP_BUTTON_DEFAULT_ISPRESSED     TP_BUTTON_UNPRESSED
/// TP_Button默认接收输入焦点：否
#define TP_BUTTON_DEFAULT_FOCUSSABLE    0
/// TP_Button默认输入焦点颜色：白色
#define TP_BUTTON_DEFAULT_FOCUSCOLOR    GUI_WHITE
/// TP_Button默认背景图片：无，无，无
#define TP_BUTTON_DEFAULT_APBKBITMAP    {NULL, NULL, NULL}
/// TP_Button默认背景颜色：绿，红，白
#define TP_BUTTON_DEFAULT_ABKCOLOR      {0x97cc58, GUI_RED, GUI_INVALID_COLOR}
/// TP_Button默认背景颜色透明度：完全不透明
#define TP_BUTTON_DEFAULT_BKCOLORALPHA  0
/// TP_Button默认文本内容：无文本
#define TP_BUTTON_DEFAULT_PTEXT         NULL
/// TP_Button默认文本颜色：白，蓝，黑
#define TP_BUTTON_DEFAULT_ATEXTCOLOR    {0xf8ffe4, GUI_BLUE, GUI_INVALID_COLOR}
/// TP_Button默认文本字体：冬青黑
#define TP_BUTTON_DEFAULT_PFONT         &GUI_FontF16DqW3
/// TP_Button默认文本对齐方法：正中间
#define TP_BUTTON_DEFAULT_TEXTALIGN     TP_TA_HCENTER_VCENTER
/// TP_Button默认文本透明度：完全不透明
#define TP_BUTTON_DEFAULT_TEXTALPHA     0
/// TP_Button默认矩形圆角半径：无圆角
#define TP_BUTTON_DEFAULT_RADIUS        0
/// TP_Button默认边框颜色：无，无，无
#define TP_BUTTON_DEFAULT_ABORDERCOLOR  {GUI_INVALID_COLOR, GUI_INVALID_COLOR, GUI_INVALID_COLOR}
/// TP_Button默认边框宽度：0, 0, 0, 0
#define TP_BUTTON_DEFAULT_ABORDERSIZE   {0, 0, 0, 0}
/// TP_Button默认额外存储空间：0
#define TP_BUTTON_DEFAULT_NUMEXTRABYTES 0

#define TP_BUTTON_DEFAULT_BK_COLOR_ON	0

/**
 * \brief TP_BUTTON对象。
 *
 * 用于存储TP_BUTTON各属性值。
 */
typedef struct
{
    /// Button ID
    int                 id;
    /// Button状态
    int                 isPressed;
    /// 是否接收输入焦点
    int                 focussable;
    /// 输入焦点颜色
    GUI_COLOR           focusColor;
    /// 背景图片
    const GUI_BITMAP *  aPBkBitmap[TP_BUTTON_STATE_COUNT];
    /// 背景颜色
    GUI_COLOR           aBkColor[TP_BUTTON_STATE_COUNT];
    /// 背景透明度
    int                 bkColorAlpha;
    /// 文本内容
    char *              pText;
    /// 文本颜色
    GUI_COLOR           aTextColor[TP_BUTTON_STATE_COUNT];
    /// 文本字体
    const GUI_FONT *    pFont;
    /// 文本对齐方式
    int                 textAlign;
    /// 文本透明度
    int                 textAlpha;
    /// 矩形圆角半径
    int                 radius;
    /// 边框颜色
    GUI_COLOR           aBorderColor[TP_BUTTON_STATE_COUNT];
    /// 边框宽度
    U8                  aBorderSize[TP_BUTTON_BORDER_NUM];
    /// 额外存储空间
    int                 numExtraBytes;
	/// 是否使用背景颜色
	U8					bkColorOn;
} TP_BUTTON_Obj;

/**
 * \brief TP_BUTTON_Handle对象。
 *
 * 便于用户区分控件类型。
 */
typedef WM_HMEM TP_BUTTON_Handle;

/*********************************************************************
*
* 属性值Get/Set方法
*/
/**
 * \brief        返回给定TP_BUTTON控件的边框宽度。
 * \param        hObj    控件的句柄。
 * \param        index   边框的索引，大于等于0，小于TP_BUTTON_BORDER_NUM。
 * \return       成功时返回给定TP_BUTTON控件的边框宽度，失败时返回RET_FAIL(-1)。
 */
int TP_Button_GetBorderSize(TP_BUTTON_Handle hObj, unsigned int index);

/**
 * \brief        设置给定TP_BUTTON控件的边框宽度。
 * \param        hObj    控件的句柄。
 * \param        aSize   待设置边框宽度。
 */
void TP_Button_SetBorderSizeEx(TP_BUTTON_Handle hObj, U8 * aSize);

/**
 * \brief        设置给定TP_BUTTON控件的边框宽度。
 * \param        hObj    控件的句柄。
 * \param        size    待设置边框。
 * \param        size    待设置边框宽度。
 */
void TP_Button_SetBorderSize(TP_BUTTON_Handle hObj, unsigned int index, U8 size);

/**
 * \brief        返回给定TP_BUTTON控件的边框颜色。
 * \param        hObj    控件的句柄。
 * \param        index   边框文本颜色的索引，大于等于0，小于TP_BUTTON_STATE_COUNT。
 * \return       成功时返回给定TP_BUTTON控件的边框颜色，失败时返回GUI_INVALID_COLOR。
 */
GUI_COLOR TP_Button_GetBorderColor(TP_BUTTON_Handle hObj, unsigned int index);

/**
 * \brief        设置给定TP_BUTTON控件的边框颜色。
 * \param        hObj    控件的句柄。
 * \param        aColor  待设置边框颜色。
 */
void TP_Button_SetBorderColorEx(TP_BUTTON_Handle hObj, GUI_COLOR * aColor);

/**
 * \brief        设置给定TP_BUTTON控件的边框颜色。
 * \param        hObj    控件的句柄。
 * \param        index   边框颜色的索引，大于等于0，小于TP_BUTTON_STATE_COUNT。
 * \param        color   待设置边框颜色。
 */
void TP_Button_SetBorderColor(TP_BUTTON_Handle hObj, unsigned int index, GUI_COLOR color);

/**
 * \brief        返回给定TP_BUTTON控件的矩形圆角半径。
 * \param        hObj    控件的句柄。
 * \return       成功时返回给定TP_BUTTON控件的矩形圆角半径，失败时返回RET_FAIL(-1)。
 */
int TP_Button_GetRadius(TP_BUTTON_Handle hObj);

/**
 * \brief        设置给定TP_BUTTON控件的矩形圆角半径。
 * \param        hObj    控件的句柄。
 * \param        alpha   待设置矩形圆角半径。
 */
void TP_Button_SetRadius(TP_BUTTON_Handle hObj, int radius);

/**
 * \brief        返回给定TP_BUTTON控件的文本透明度。
 * \param        hObj    控件的句柄。
 * \return       成功时返回给定TP_BUTTON控件的文本透明度，失败时返回RET_FAIL(-1)。
 */
int TP_Button_GetTextAlpha(TP_BUTTON_Handle hObj);

/**
 * \brief        设置给定TP_BUTTON控件的文本透明度。
 * \param        hObj    控件的句柄。
 * \param        alpha   待设置文本的透明度。
 */
void TP_Button_SetTextAlpha(TP_BUTTON_Handle hObj, int alpha);

/**
 * \brief        返回给定TP_BUTTON控件的文本对齐方式。
 * \param        hObj    控件的句柄。
 * \return       成功时返回给定TP_BUTTON控件的文本对齐方式，失败时返回RET_FAIL(-1)。
 */
int TP_Button_GetTextAlign(TP_BUTTON_Handle hObj);

/**
 * \brief        设置给定TP_BUTTON控件的文本对齐方式。
 * \param        hObj    控件的句柄。
 * \param        align   待设置文本的对齐方式。
 */
void TP_Button_SetTextAlign(TP_BUTTON_Handle hObj, int align);

/**
 * \brief        返回给定TP_BUTTON控件的文本字体指针。
 * \param        hObj    控件的句柄。
 * \return       成功时返回给定TP_BUTTON控件的文本字体指针，失败时返回空指针。
 */
const GUI_FONT * TP_Button_GetTextFont(TP_BUTTON_Handle hObj);

/**
 * \brief        设置给定TP_BUTTON控件的文本字体。
 * \param        hObj    控件的句柄。
 * \param        pFont   待设置文本的字体。
 */
void TP_Button_SetTextFont(TP_BUTTON_Handle hObj, const GUI_FONT * pFont);

/**
 * \brief        返回给定TP_BUTTON控件的文本颜色。
 * \param        hObj    控件的句柄。
 * \param        index   文本颜色的索引，大于等于0，小于TP_BUTTON_STATE_COUNT。
 * \return       成功时返回给定TP_BUTTON控件的文本颜色，失败时返回GUI_INVALID_COLOR。
 */
GUI_COLOR TP_Button_GetTextColor(TP_BUTTON_Handle hObj, unsigned int index);

/**
 * \brief        设置给定TP_BUTTON控件的文本颜色。
 * \param        hObj    控件的句柄。
 * \param        aColor  待设置文本的颜色。
 */
void TP_Button_SetTextColorEx(TP_BUTTON_Handle hObj, GUI_COLOR * aColor);

/**
 * \brief        设置给定TP_BUTTON控件的文本颜色。
 * \param        hObj    控件的句柄。
 * \param        index   待设置文本颜色的索引，大于等于0，小于TP_BUTTON_STATE_COUNT。
 * \param        color   待设置文本的颜色。
 */
void TP_Button_SetTextColor(TP_BUTTON_Handle hObj, unsigned int index, GUI_COLOR color);

/**
 * \brief        检索指定TP_BUTTON控件的文本。
 * \param        hObj    控件的句柄。
 * \param        pBuffer 指向缓冲区的指针。
 * \param        maxLen  缓冲区的大小。
 * \attention    当缓冲区空间小于控件文本长度，截断处理，只能检索到部分控件文本。
 * \return       成功时返回检索到的文本长度，失败时返回RET_FAIL(-1)。
 */
int TP_Button_GetText(TP_BUTTON_Handle hObj, char * pBuffer, int maxLen);

/**
 * \brief        设置显示在给定TP_BUTTON控件的文本。
 * \param        hObj    控件的句柄。
 * \param        pText   待设置的文本。
 */
int TP_Button_SetText(TP_BUTTON_Handle hObj, char * pText);

/**
 * \brief        返回给定TP_BUTTON控件的背景颜色透明度。
 * \param        hObj    控件的句柄。
 * \return       成功时返回给定TP_BUTTON控件的背景颜色透明度，失败时返回RET_FAIL(-1)。
 */
int TP_Button_GetBkColorAlpha(TP_BUTTON_Handle hObj);

/**
 * \brief        设置给定TP_BUTTON控件背景颜色的透明度。
 * \param        hObj    控件的句柄。
 * \param        alpha   待设置背景颜色的透明度。
 */
void TP_Button_SetBkColorAlpha(TP_BUTTON_Handle hObj, int alpha);

/**
 * \brief        返回给定TP_BUTTON控件的背景颜色。
 * \param        hObj    控件的句柄。
 * \param        index   背景颜色的索引，大于等于0，小于TP_BUTTON_STATE_COUNT。
 * \return       成功时返回给定TP_BUTTON控件的背景颜色，失败时返回GUI_INVALID_COLOR。
 */
GUI_COLOR TP_Button_GetBkColor(TP_BUTTON_Handle hObj, unsigned int index);

/**
 * \brief        设置给定TP_BUTTON控件的背景颜色。
 * \param        hObj    控件的句柄。
 * \param        aColor  待设置背景颜色。
 */
void TP_Button_SetBkColorEx(TP_BUTTON_Handle hObj, GUI_COLOR * aColor);

/**
 * \brief        设置给定TP_BUTTON控件的背景颜色。
 * \param        hObj    控件的句柄。
 * \param        index   背景颜色的索引。
 * \param        color   待设置背景颜色。
 */
void TP_Button_SetBkColor(TP_BUTTON_Handle hObj, unsigned int index, GUI_COLOR color);

/**
 * \brief        设置给定TP_BUTTON控件的背景颜色是否需要绘制。
 * \param        hObj    控件的句柄。
 * \return        返回对应的开关值。
 */
U8 TP_Button_GetBkColorOn(TP_BUTTON_Handle hObj);

/**
 * \brief        设置给定TP_BUTTON控件的背景颜色是否需要绘制。
 * \param        hObj    控件的句柄。
 * \param        onff   待设置的值。
 */
void TP_Button_SetBkColorOn(TP_BUTTON_Handle hObj, U8 onff);


/**
 * \brief        返回给定TP_BUTTON控件的背景位图指针。
 * \param        hObj    控件的句柄。
 * \param        index   所需背景位图的索引，大于等于0，小于TP_BUTTON_STATE_COUNT。
 * \return       成功时返回给定TP_BUTTON控件的背景位图指针，失败时返回空指针。
 */
const GUI_BITMAP* TP_Button_GetBitmap(TP_BUTTON_Handle hObj, unsigned int index);

/**
 * \brief        设置给定TP_BUTTON控件的背景位图。
 * \param        hObj    控件的句柄。
 * \param        pBitmap 指向背景位图的指针数组。
 */
void TP_Button_SetBitmapEx(TP_BUTTON_Handle hObj,
        const GUI_BITMAP * aPBitmap[TP_BUTTON_STATE_COUNT]);

/**
 * \brief        设置给定TP_BUTTON控件的背景位图。
 * \param        hObj    控件的句柄。
 * \param        index   所需背景位图的索引，大于等于0，小于TP_BUTTON_STATE_COUNT。
 * \param        pBitmap 指向背景位图的指针。
 */
void TP_Button_SetBitmap(TP_BUTTON_Handle hObj, unsigned int index, const GUI_BITMAP * pBitmap);

/**
 * \brief        返回给定TP_BUTTON控件接受输入焦点时的颜色。
 * \param        hObj    控件的句柄。
 * \return       成功时返回给定TP_BUTTON控件接受输入焦点时的颜色，失败时返回GUI_INVALID_COLOR。
 */
GUI_COLOR TP_Button_GetFocusColor(TP_BUTTON_Handle hObj);

/**
 * \brief        设置给定TP_BUTTON控件接受输入焦点时的颜色。
 * \param        hObj    控件的句柄。
 * \param        color   待设置接受输入焦点时的颜色。
 */
void TP_Button_SetFocusColor(TP_BUTTON_Handle hObj, GUI_COLOR color);

/**
 * \brief        返回给定TP_BUTTON控件是否接受输入焦点。
 * \param        hObj    控件的句柄。
 * \return       成功时返回给定TP_BUTTON控件是否接受输入焦点，失败时返回RET_FAIL(-1)。
 */
int TP_Button_GetFocussable(TP_BUTTON_Handle hObj);

/**
 * \brief        设置给定TP_BUTTON控件是否接受输入焦点。
 * \param        hObj    控件的句柄。
 * \param        state   是否接受输入焦点。
 */
void TP_Button_SetFocussable(TP_BUTTON_Handle hObj, int state);

/**
 * \brief        返回给定TP_BUTTON控件是否处于按下状态。
 * \param        hObj    控件的句柄。
 * \return       成功时返回给定TP_BUTTON控件是否处于按下状态，失败时返回RET_FAIL(-1)。
 */
int TP_Button_IsPressed(TP_BUTTON_Handle hObj);

/**
 * \brief        设置给定TP_BUTTON控件是否已经按下。
 * \param        hObj    控件的句柄。
 * \param        state   是否已经按下。
 */
void TP_Button_SetPressed(TP_BUTTON_Handle hObj, int state);

/**
 * \brief        返回给定TP_BUTTON控件的ID。
 * \param        hObj    控件的句柄。
 * \return       成功时返回给定TP_BUTTON控件的ID，失败时返回RET_FAIL(-1)。
 */
int TP_Button_GetId(TP_BUTTON_Handle hObj);

/**
 * \brief        设置给定TP_BUTTON控件的ID。
 * \param        hObj    控件的句柄。
 * \param        id      控件的ID。
 */
void TP_Button_SetId(TP_BUTTON_Handle hObj, int id);

/*********************************************************************
*
* 默认属性值Get/Set方法
*/
/**
 * \brief        返回给定TP_BUTTON控件的默认边框宽度。
 * \param        index   默认边框宽度的索引，大于等于0，小于TP_BUTTON_BORDER_NUM。
 * \return       给定TP_BUTTON控件的默认边框宽度。
 */
U8 TP_Button_GetDefaultBorderSize(unsigned int index);

/**
 * \brief        设置给定TP_BUTTON控件的默认边框颜色。
 * \param        index   默认边框宽度的索引，大于等于0，小于TP_BUTTON_BORDER_NUM。
 * \param        size    待设置默认边框宽度。
 */
void TP_Button_SetDefaultBorderSize(unsigned int index, U8 size);

/**
 * \brief        返回给定TP_BUTTON控件的默认边框颜色。
 * \param        index   边框默认颜色的索引，大于等于0，小于TP_BUTTON_STATE_COUNT。
 * \return       给定TP_BUTTON控件的默认边框颜色。
 */
GUI_COLOR TP_Button_GetDefaultBorderColor(unsigned int index);

/**
 * \brief        设置给定TP_BUTTON控件的默认边框颜色。
 * \param        index   边框默认颜色的索引，大于等于0，小于TP_BUTTON_STATE_COUNT。
 * \param        color   待设置默认边框颜色。
 */
void TP_Button_SetDefaultBorderColor(unsigned int index, GUI_COLOR color);

/**
 * \brief        返回给定TP_BUTTON控件的文本默认透明度。
 * \return       给定TP_BUTTON控件的文本默认透明度。
 */
int TP_Button_GetDefaultRadius();

/**
 * \brief        设置给定TP_BUTTON控件的默认矩形圆角半径。
 * \param        alpha   待设置默认矩形圆角半径。
 */
void TP_Button_SetDefaultRadius(int radius);

/**
 * \brief        返回给定TP_BUTTON控件的文本默认透明度。
 * \return       给定TP_BUTTON控件的文本默认透明度。
 */
int TP_Button_GetDefaultTextAlpha();

/**
 * \brief        设置给定TP_BUTTON控件的文本默认透明度。
 * \param        alpha   待设置文本的默认透明度。
 */
void TP_Button_SetDefaultTextAlpha(int alpha);

/**
 * \brief        返回给定TP_BUTTON控件的文本默认对齐方式。
 * \return       给定TP_BUTTON控件的文本默认对齐方式。
 */
int TP_Button_GetDefaultTextAlign();

/**
 * \brief        设置给定TP_BUTTON控件的文本默认对齐方式。
 * \param        align   待设置文本默认对齐方式。
 */
void TP_Button_SetDefaultTextAlign(int align);

/**
 * \brief        返回给定TP_BUTTON控件的文本默认字体指针。
 * \return       给定TP_BUTTON控件的文本默认字体指针。
 */
const GUI_FONT * TP_Button_GetDefaultTextFont();

/**
 * \brief        设置给定TP_BUTTON控件的文本默认字体。
 * \param        pFont   待设置文本默认字体。
 */
void TP_Button_SetDefaultTextFont(const GUI_FONT * pFont);

/**
 * \brief        返回给定TP_BUTTON控件的文本默认颜色。
 * \param        index   文本默认颜色的索引，大于等于0，小于TP_BUTTON_STATE_COUNT。
 * \return       给定TP_BUTTON控件的文本默认颜色。
 */
GUI_COLOR TP_Button_GetDefaultTextColor(unsigned int index);

/**
 * \brief        设置给定TP_BUTTON控件的文本默认颜色。
 * \param        index   待设置文本默认颜色的索引，大于等于0，小于TP_BUTTON_STATE_COUNT。
 * \param        color   待设置文本默认颜色。
 */
void TP_Button_SetDefaultTextColor(unsigned int index, GUI_COLOR color);

/**
 * \brief        检索指定TP_BUTTON控件的默认文本。
 * \param        pBuffer 指向缓冲区的指针。
 * \param        maxLen  缓冲区的大小。
 * \attention    当缓冲区空间小于控件默认文本长度，截断处理，只能检索到部分控件默认文本。
 * \return       成功时返回检索到的默认文本长度，失败时返回RET_FAIL(-1)。
 */
int TP_Button_GetDefaultText(char * pBuffer, int maxLen);

/**
 * \brief        设置显示在给定TP_BUTTON控件的默认文本。
 * \param        pText   待设置的默认文本。
 */
int TP_Button_SetDefaultText(char * pText);

/**
 * \brief        返回给定TP_BUTTON控件的背景颜色默认透明度。
 * \return       给定TP_BUTTON控件的背景颜色默认透明度。
 */
int TP_Button_GetDefaultBkColorAlpha();

/**
 * \brief        设置给定TP_BUTTON控件背景颜色的默认透明度。
 * \param        alpha   待设置背景颜色的默认透明度。
 */
void TP_Button_SetDefaultBkColorAlpha(int alpha);

/**
 * \brief        返回给定TP_BUTTON控件的默认背景颜色。
 * \param        index   默认背景颜色的索引，大于等于0，小于TP_BUTTON_STATE_COUNT。
 * \return       给定TP_BUTTON控件的默认背景颜色。
 */
GUI_COLOR TP_Button_GetDefaultBkColor(unsigned int index);

/**
 * \brief        设置给定TP_BUTTON控件的默认背景颜色。
 * \param        index   默认背景颜色的索引，大于等于0，小于TP_BUTTON_STATE_COUNT。
 * \param        color   待设置默认背景颜色。
 */
void TP_Button_SetDefaultBkColor(unsigned int index, GUI_COLOR color);

/**
 * \brief        返回TP_BUTTON控件的默认的背景开关的值。
 * \return       给定TP_BUTTON控件的默认的背景开关的值。
 */
U8 TP_Button_GetDefaultBkColorOn();

/**
 * \brief        设置TP_BUTTON控件的默认的背景开关的值。
 * \param      要设定的值。
 */
void TP_Button_SetDefaultBkColorOn(U8 onff);


/**
 * \brief        返回给定TP_BUTTON控件的默认背景位图指针。
 * \param        index   默认背景位图的索引，大于等于0，小于TP_BUTTON_STATE_COUNT。
 * \return       给定TP_BUTTON控件的默认背景位图指针。
 */
const GUI_BITMAP* TP_Button_GetDefaultBitmap(unsigned int index);

/**
 * \brief        设置给定TP_BUTTON控件的默认背景位图。
 * \param        index   默认背景位图的索引，大于等于0，小于TP_BUTTON_STATE_COUNT。
 * \param        pBitmap 指向默认背景位图的指针。
 */
void TP_Button_SetDefaultBitmap(unsigned int index, const GUI_BITMAP * pBitmap);

/**
 * \brief        返回给定TP_BUTTON控件接受输入焦点时的默认颜色。
 * \return       给定TP_BUTTON控件接受输入焦点时的默认颜色。
 */
GUI_COLOR TP_Button_GetDefaultFocusColor();

/**
 * \brief        设置给定TP_BUTTON控件接受输入焦点时的默认颜色。
 * \param        color   待设置接受输入焦点时的默认颜色。
 */
void TP_Button_SetDefaultFocusColor(GUI_COLOR color);

/**
 * \brief        返回给定TP_BUTTON控件默认是否接受输入焦点。
 * \return       给定TP_BUTTON控件默认是否接受输入焦点。
 */
int TP_Button_GetDefaultFocussable();

/**
 * \brief        设置给定TP_BUTTON控件默认是否接受输入焦点。
 * \param        state   默认是否接受输入焦点。
 */
void TP_Button_SetDefaultFocussable(int state);

/**
 * \brief        返回给定TP_BUTTON控件默认是否处于按下状态。
 * \return       给定TP_BUTTON控件默认是否处于按下状态。
 */
int TP_Button_IsDefaultPressed();

/**
 * \brief        设置给定TP_BUTTON控件默认是否按下。
 * \param        state   默认是否按下。
 */
void TP_Button_SetDefaultPressed(int state);

/**
 * \brief        返回给定TP_BUTTON控件的默认ID。
 * \return       给定TP_BUTTON控件的默认ID。
 */
int TP_Button_GetDefaultId();

/**
 * \brief        设置给定TP_BUTTON控件的默认ID。
 * \param        id      待设置默认ID。
 */
void TP_Button_SetDefaultId(int id);

/*********************************************************************
*
* 创建控件相关方法
*/

/**
 * \brief        控件的回调函数。
 *
 * 允许用户自定义回调函数。控件中处理了WM_PAINT，WM_PID_STATE_CHANGED，WM_TOUCH，
 * WM_SET_FOCUS，WM_DELETE，WM_GET_ID，WM_SET_ID等消息。
 * \param        pMsg    窗口消息。
 */
void TP_Button_Callback(WM_MESSAGE * pMsg);

/**
 * \brief        设置用户私有数据。
 *
 * \param        hObj       控件的句柄。
 * \param        pDest      指向缓冲区的指针。
 * \param        numBytes   缓冲区的大小。
 * \return       成功时返回RET_SUCCESS(0)，失败时返回RET_FAIL(-1)。
 */
int TP_Button_SetUserData(TP_BUTTON_Handle hObj, void * pDest, int numBytes);

/**
 * \brief        返回用户私有数据。
 *
 * \param        hObj       控件的句柄。
 * \param        pDest      指向缓冲区的指针。
 * \param        numBytes   缓冲区的大小。
 * \return       成功时返回用户私有数据的长度，失败时返回0。
 */
int TP_Button_GetUserData(TP_BUTTON_Handle hObj, void * pDest, int numBytes);

/**
 * \brief        创建控件窗口。
 *
 * \param        x0             Button的最左像素（在父坐标中）。
 * \param        y0             Button的最上像素（在父坐标中）。
 * \param        xSize          Button的水平尺寸（单位：像素）。
 * \param        ySize          Button的垂直尺寸（单位：像素）。
 * \param        hParent        父窗口的句柄。如果为0,则为桌面（顶级窗口）的子项。
 * \param        style          窗口创建标记。
 * \param        cb             窗口回调函数，用于消息处理。
 * \param        numExtraBytes  用户私有数据的长度。
 * \param        id             Button的ID。
 * \return       Button的句柄。
 */
TP_BUTTON_Handle TP_Button_Create(int x0, int y0, int xSize, int ySize, WM_HWIN hParent,
        U8 style, WM_CALLBACK * cb, int numExtraBytes, int id);

#endif /* TP_BUTTON_H__ */
