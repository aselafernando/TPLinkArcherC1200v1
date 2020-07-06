/*! Copyright (c) 2014 Shenzhen TP-LINK Technologies Co.Ltd.
 *
 *\file     TP_MessageBox.h
 *\brief    TP_MessageBox控件的定义与函数原型.
 *
 *\version  1.0.0
 *\date     11Nov14
 */
#ifndef TP_MESSAGEBOX_H__
#define TP_MESSAGEBOX_H__

#include "TP_Global.h"
#include "TP_Button.h"

/// TP_MessageBox按钮最大数目：2
#define TP_MESSAGEBOX_MAX_BUTTON                    2

/// TP_MessageBox默认ID：73
#define TP_MESSAGEBOX_DEFAULT_ID                    73
/// TP_MessageBox默认接收输入焦点：否
#define TP_MESSAGEBOX_DEFAULT_FOCUSSABLE            0
/// TP_MessageBox默认接收输入焦点颜色：白
#define TP_MESSAGEBOX_DEFAULT_FOCUSCOLOR            GUI_WHITE
/// TP_MessageBox默认文本颜色：白，蓝，黑
#define TP_MESSAGEBOX_DEFAULT_TEXTCOLOR             GUI_BLACK
/// TP_MessageBox默认文本字体：冬青黑
#define TP_MESSAGEBOX_DEFAULT_PTEXTFONT             &GUI_FontF16DqW3
/// TP_MessageBox默认文本对齐方法：正中间
#define TP_MESSAGEBOX_DEFAULT_TEXTALIGN             TP_TA_HCENTER_VCENTER
/// TP_MessageBox默认文本页边空白宽度：110p
#define TP_MESSAGEBOX_DEFAULT_TEXTMARGIN            110
/// TP_MessageBox默认文本内容：无文本
#define TP_MESSAGEBOX_DEFAULT_PTEXT                 NULL
/// TP_MessageBox默认消息包含图片：无
#define TP_MESSAGEBOX_DEFAULT_PBKBITMAP             NULL
/// TP_MessageBox默认背景颜色：白
#define TP_MESSAGEBOX_DEFAULT_BKCOLOR               GUI_WHITE
/// TP_MessageBox默认按钮个数：2
#define TP_MESSAGEBOX_DEFAULT_BUTTONNUM             2
/// TP_MessageBox默认按钮高度：30p
#define TP_MESSAGEBOX_DEFAULT_BUTTONHEIGHT          30
/// TP_MessageBox默认按钮句柄：空
#define TP_MESSAGEBOX_DEFAULT_AHBUTTON              {WM_HMEM_NULL, WM_HMEM_NULL}
/// TP_MessageBox默认额外空间：0
#define TP_MESSAGEBOX_DEFAULT_NUMEXTRABYTES         0

typedef struct
{
    /// MessageBox ID
    int                 id;
    /// 是否接收输入焦点
	int                 focussable;
    /// 输入焦点颜色
	GUI_COLOR           focusColor;
    /// MessageBox控件显示文本颜色
    GUI_COLOR           textColor;
    /// MessageBox控件显示文本字体
    const GUI_FONT *    pTextFont;
    /// MessageBox控件显示文本对齐方式
    int                 textAlign;
    /// MessageBox控件文本页边空白宽度
    int                 textMargin;
    /// MessageBox控件显示文本消息
    char *              pText;
    /// MessageBox控件显示位图
    /*
     * \attention 此位图为消息中待显示位图，非控件背景位图。
     */
    const GUI_BITMAP *  pBkBitmap;
    /// MessageBox控件背景颜色
    /*
     * \attention MessageBox控件背景颜色不包含按钮区域背景颜色。
     */
    GUI_COLOR           bkColor;
    /// MessageBox控件按钮个数
    int                 buttonNum;
    /// MessageBox控件按钮高度
    int                 buttonHeight;
    /// Button控件句柄
    TP_BUTTON_Handle    aHButton[TP_MESSAGEBOX_MAX_BUTTON];
    /// 额外存储空间
    int                 numExtraBytes;
} TP_MESSAGEBOX_Obj;

/**
 * \brief TP_MESSAGEBOX_Handle对象。
 *
 * 便于用户区分控件类型。
 */
typedef WM_HMEM TP_MESSAGEBOX_Handle;

/*******************
 * 属性Get/Set方法 *
 *******************/
/**
 * \brief        返回给定TP_MESSAGEBOX控件的Button句柄。
 * \param        hObj    控件的句柄。
 * \param        index   按钮的索引，大于等于0，小于TP_MESSAGEBOB_MAX_BUTTON。
 * \return       成功时返回给定控件的Button句柄，失败时返回WM_HMEM_NULL。
 */
TP_BUTTON_Handle TP_MessageBox_GetButtonHandle(TP_MESSAGEBOX_Handle hObj, unsigned int index);

/**
 * \brief        设置给定TP_MESSAGEBOX控件的Button句柄。
 * \param        hObj    控件的句柄。
 * \param        index   按钮的索引，大于等于0，小于TP_MESSAGEBOB_MAX_BUTTON。
 * \param        hButton 按钮的句柄。
 */
void TP_MessageBox_SetButtonHandle(TP_MESSAGEBOX_Handle hObj, unsigned int index, TP_BUTTON_Handle hButton);

/**
 * \brief        返回给定TP_MESSAGEBOX控件的背景颜色。
 * \param        hObj    控件的句柄。
 * \return       成功时返回给定控件背景颜色，失败时返回GUI_INVALID_COLOR。
 */
GUI_COLOR TP_MessageBox_GetBkColor(TP_MESSAGEBOX_Handle hObj);

/**
 * \brief        设置给定TP_MESSAGEBOX控件的背景颜色。
 * \param        hObj    控件的句柄。
 * \param        color   背景颜色。
 */
void TP_MessageBox_SetBkColor(TP_MESSAGEBOX_Handle hObj, GUI_COLOR color);

/**
 * \brief        返回给定TP_MESSAGEBOX控件的背景图片。
 * \param        hObj    控件的句柄。
 * \return       成功时返回给定控件背景图片，失败时返回NULL。
 */
const GUI_BITMAP * TP_MessageBox_GetBkBitmap(TP_MESSAGEBOX_Handle hObj);

/**
 * \brief        设置给定TP_MESSAGEBOX控件的背景图片。
 * \param        hObj    控件的句柄。
 * \param        bitmap  背景图片。
 * \attention    调用此方法绘制背景图前需先设置好显示文本，因为背景图根据文本显示结果进行绘制。
 */
void TP_MessageBox_SetBkBitmap(TP_MESSAGEBOX_Handle hObj, const GUI_BITMAP * bitmap);

/**
 * \brief        检索指定TP_MESSAGEBOX控件的文本。
 * \param        hObj    控件的句柄。
 * \param        pBuffer 指向缓冲区的指针。
 * \param        maxLen  缓冲区的大小。
 * \attention    当缓冲区空间小于控件文本长度，截断处理，只能检索到部分控件文本。
 * \return       成功时返回检索到的文本长度，失败时返回RET_FAIL(-1)。
 */
int TP_Message_GetText(TP_MESSAGEBOX_Handle hObj, char * pBuffer, int maxLen);

/**
 * \brief        设置显示在给定TP_MESSAGEBOX控件的文本。
 * \param        hObj    控件的句柄。
 * \param        pText   待设置的文本。
 */
int TP_MessageBox_SetText(TP_MESSAGEBOX_Handle hObj, char * pText);

/**
 * \brief        返回给定TP_MESSAGEBOX控件的文本页边空白宽度。
 * \param        hObj    控件的句柄。
 * \return       成功时返回给定控件文本页边空白宽度，失败时返回RET_FAIL(-1)。
 */
int TP_MessageBox_GetTextMargin(TP_MESSAGEBOX_Handle hObj);

/**
 * \brief        设置给定TP_MESSAGEBOX控件的文本布边空白宽度。
 * \param        hObj    控件的句柄。
 * \param        margin  文本布边空白宽度，单位：像素。
 */
void TP_MessageBox_SetTextMargin(TP_MESSAGEBOX_Handle hObj, int margin);

/**
 * \brief        返回给定TP_MESSAGEBOX控件的文本对齐方式。
 * \param        hObj    控件的句柄。
 * \return       成功时返回给定控件文本对齐方式，失败时返回RET_FAIL(-1)。
 */
int TP_MessageBox_GetTextAlign(TP_MESSAGEBOX_Handle hObj);

/**
 * \brief        设置给定TP_MESSAGEBOX控件的文本对齐方式。
 * \param        hObj    控件的句柄。
 * \param        align   文本对齐方式。
 */
void TP_MessageBox_SetTextAlign(TP_MESSAGEBOX_Handle hObj, int align);

/**
 * \brief        返回给定TP_MESSAGEBOX控件的文本字体。
 * \param        hObj    控件的句柄。
 * \return       成功时返回给定控件文本字体，失败时返回NULL。
 */
const GUI_FONT * TP_MessageBox_GetTextFont(TP_MESSAGEBOX_Handle hObj);

/**
 * \brief        设置给定TP_MESSAGEBOX控件的文本字体。
 * \param        hObj    控件的句柄。
 * \param        font    文本字体。
 */
void TP_MessageBox_SetTextFont(TP_MESSAGEBOX_Handle hObj, const GUI_FONT * font);

/**
 * \brief        返回给定TP_MESSAGEBOX控件的文本颜色。
 * \param        hObj    控件的句柄。
 * \return       成功时返回给定控件文本颜色，失败时返回GUI_INVALID_COLOR。
 */
GUI_COLOR TP_MessageBox_GetTextColor(TP_MESSAGEBOX_Handle hObj);

/**
 * \brief        设置给定TP_MESSAGEBOX控件的文本颜色。
 * \param        hObj    控件的句柄。
 * \param        color   文本颜色。
 */
void TP_MessageBox_SetTextColor(TP_MESSAGEBOX_Handle hObj, GUI_COLOR color);

/**
 * \brief        返回给定TP_MESSAGEBOX控件接受输入焦点时的颜色。
 * \param        hObj    控件的句柄。
 * \return       成功时返回给定TP_MESSAGEBOX控件接受输入焦点时的颜色，失败时返回GUI_INVALID_COLOR。
 */
GUI_COLOR TP_MessageBox_GetFocusColor(TP_MESSAGEBOX_Handle hObj);

/**
 * \brief        设置给定TP_MESSAGEBOX控件接受输入焦点时的颜色。
 * \param        hObj    控件的句柄。
 * \param        color   待设置接受输入焦点时的颜色。
 */
void TP_MessageBox_SetFocusColor(TP_MESSAGEBOX_Handle hObj, GUI_COLOR color);

/**
 * \brief        返回给定TP_MESSAGEBOX控件是否接受输入焦点。
 * \param        hObj    控件的句柄。
 * \return       成功时返回给定TP_MESSAGEBOX控件是否接受输入焦点，失败时返回RET_FAIL(-1)。
 */
int TP_MessageBox_GetFocussable(TP_MESSAGEBOX_Handle hObj);

/**
 * \brief        设置给定TP_MESSAGEBOX控件是否接受输入焦点。
 * \param        hObj    控件的句柄。
 * \param        state   是否接受输入焦点。
 */
void TP_MessageBox_SetFocussable(TP_MESSAGEBOX_Handle hObj, int state);

/***********************
 * 默认属性Get/Set方法 *
 ***********************/
/**
 * \brief        返回给定TP_MESSAGEBOX控件的默认Button句柄。
 * \param        index   按钮的索引，大于等于0，小于TP_MESSAGEBOB_MAX_BUTTON。
 * \return       成功时返回给定控件的默认Button句柄，失败时返回WM_HMEM_NULL。
 */
TP_BUTTON_Handle TP_MessageBox_GetDefaultButtonHandle(unsigned int index);

/**
 * \brief        设置给定TP_MESSAGEBOX控件的默认Button句柄。
 * \param        index   按钮的索引，大于等于0，小于TP_MESSAGEBOB_MAX_BUTTON。
 * \param        hButton 按钮的句柄。
 */
void TP_MessageBox_SetDefaultButtonHandle(unsigned int index, TP_BUTTON_Handle hButton);

/**
 * \brief        返回给定TP_MESSAGEBOX控件的默认背景颜色。
 * \return       成功时返回给定控件默认背景颜色，失败时返回GUI_INVALID_COLOR。
 */
GUI_COLOR TP_MessageBox_GetDefaultBkColor();

/**
 * \brief        设置给定TP_MESSAGEBOX控件的默认背景颜色。
 * \param        color   默认背景颜色。
 */
void TP_MessageBox_SetDefaultBkColor(GUI_COLOR color);

/**
 * \brief        返回给定TP_MESSAGEBOX控件的默认背景图片。
 * \return       成功时返回给定控件背景图片，失败时返回NULL。
 */
const GUI_BITMAP * TP_MessageBox_GetDefaultBkBitmap();

/**
 * \brief        设置给定TP_MESSAGEBOX控件的默认背景图片。
 * \param        bitmap  默认背景图片。
 */
void TP_MessageBox_SetDefaultBkBitmap(const GUI_BITMAP * bitmap);

/**
 * \brief        检索指定TP_MESSAGEBOX控件的默认文本。
 * \param        pBuffer 指向缓冲区的指针。
 * \param        maxLen  缓冲区的大小。
 * \attention    当缓冲区空间小于控件文本长度，截断处理，只能检索到部分控件文本。
 * \return       成功时返回检索到的文本长度，失败时返回RET_FAIL(-1)。
 */
int TP_Message_GetDefaultText(char * pBuffer, int maxLen);

/**
 * \brief        设置显示在给定TP_MESSAGEBOX控件的默认文本。
 * \param        pText   待设置的文本。
 */
int TP_MessageBox_SetDefaultText(char * pText);

/**
 * \brief        返回给定TP_MESSAGEBOX控件的默认文本页边空白宽度。
 * \return       成功时返回给定控件默认文本页边空白宽度，失败时返回RET_FAIL(-1)。
 */
int TP_MessageBox_GetDefaultTextMargin();

/**
 * \brief        设置给定TP_MESSAGEBOX控件的文本布边空白宽度。
 * \param        margin  默认文本布边空白宽度，单位：像素。
 */
void TP_MessageBox_SetDefaultTextMargin(int margin);

/**
 * \brief        返回给定TP_MESSAGEBOX控件的默认文本对齐方式。
 * \return       成功时返回给定控件默认文本对齐方式，失败时返回RET_FAIL(-1)。
 */
int TP_MessageBox_GetDefaultTextAlign();

/**
 * \brief        设置给定TP_MESSAGEBOX控件的默认文本对齐方式。
 * \param        align   文本对齐方式。
 */
void TP_MessageBox_SetDefaultTextAlign(int align);

/**
 * \brief        返回给定TP_MESSAGEBOX控件的默认文本字体。
 * \return       成功时返回给定控件默认文本字体，失败时返回NULL。
 */
const GUI_FONT * TP_MessageBox_GetDefaultTextFont();

/**
 * \brief        设置给定TP_MESSAGEBOX控件的默认文本字体。
 * \param        font    默认文本字体。
 */
void TP_MessageBox_SetDefaultTextFont(GUI_FONT * font);

/**
 * \brief        返回给定TP_MESSAGEBOX控件的默认文本颜色。
 * \return       成功时返回给定控件默认文本颜色，失败时返回GUI_INVALID_COLOR。
 */
GUI_COLOR TP_MessageBox_GetDefaultTextColor();

/**
 * \brief        设置给定TP_MESSAGEBOX控件的默认文本颜色。
 * \param        color   默认文本颜色。
 */
void TP_MessageBox_SetDefaultTextColor(GUI_COLOR color);

/**
 * \brief        返回给定TP_MESSAGEBOX控件接受输入焦点时的默认颜色。
 * \return       成功时返回给定TP_MESSAGEBOX控件接受输入焦点时的默认颜色，失败时返回GUI_INVALID_COLOR。
 */
GUI_COLOR TP_MessageBox_GetDefaultFocusColor();

/**
 * \brief        设置给定TP_MESSAGEBOX控件接受输入焦点时的默认颜色。
 * \param        color   待设置接受输入焦点时的默认颜色。
 */
void TP_MessageBox_SetDefaultFocusColor(GUI_COLOR color);

/**
 * \brief        返回给定TP_MESSAGEBOX控件默认是否接受输入焦点。
 * \return       成功时返回给定TP_MESSAGEBOX控件默认是否接受输入焦点，失败时返回RET_FAIL(-1)。
 */
int TP_MessageBox_GetDefaultFocussable();

/**
 * \brief        设置给定TP_MESSAGEBOX控件默认是否接受输入焦点。
 * \param        state   默认是否接受输入焦点。
 */
void TP_MessageBox_SetDefaultFocussable(int state);

/********************
 * 窗口处理相关方法 *
 ********************/
/**
 * \brief        设置用户私有数据。
 *
 * \param        hObj       控件的句柄。
 * \param        pDest      指向缓冲区的指针。
 * \param        numBytes   缓冲区的大小。
 * \return       成功时返回RET_SUCCESS(0)，失败时返回RET_FAIL(-1)。
 */
int TP_MessageBox_SetUserData(TP_BUTTON_Handle hObj, void * pDest, int numBytes);

/**
 * \brief        返回用户私有数据。
 *
 * \param        hObj       控件的句柄。
 * \param        pDest      指向缓冲区的指针。
 * \param        numBytes   缓冲区的大小。
 * \return       成功时返回用户私有数据的长度，失败时返回0。
 */
int TP_MessageBox_GetUserData(TP_BUTTON_Handle hObj, void * pDest, int numBytes);

/**
 * \brief        创建控件窗口。
 *
 * \param        x0             MessageBox的最左像素（在父坐标中）。
 * \param        y0             MessageBox的最上像素（在父坐标中）。
 * \param        xSize          MessageBox的水平尺寸（单位：像素）。
 * \param        ySize          MessageBox的垂直尺寸（单位：像素）。
 * \param        hParent        父窗口的句柄。如果为0,则为桌面（顶级窗口）的子项。
 * \param        style          窗口创建标记。
 * \param        cb             窗口回调函数，用于消息处理。
 * \param        numExtraBytes  用户私有数据的长度。
 * \param        id             MessageBox的ID。
 * \param        buttonNum      按钮个数，未输入则按默认处理。
 * \param        buttonHeight   按钮高度，未输入则按默认处理。
 * \return       MessageBox的句柄。
 */
TP_MESSAGEBOX_Handle TP_MessageBox_Create(int x0, int y0, int xSize, int ySize, WM_HWIN hParent,
        U8 style, WM_CALLBACK * cb, int numExtraBytes, int id, int buttonNum, int buttonHeight);

#endif /* TP_MESSAGEBOX_H__ */
