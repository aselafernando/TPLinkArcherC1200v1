/*! Copyright (c) 2014 Shenzhen TP-LINK Technologies Co.Ltd.
 *
 * \file     TP_Button.h
 * \brief    TP_BUTTON�ؼ��Ķ����뺯��ԭ�͡�
 *
 * \version  1.0.0
 * \date     30Oct14
 */
#ifndef TP_BUTTON_H__
#define TP_BUTTON_H__

#include <string.h>
#include <stdlib.h>
#include "TP_Widget.h"

/// TP_Button״̬��Сֵ
#define TP_BUTTON_STATE_MIN             0
/// TP_Button�ǰ���״ֵ̬
#define TP_BUTTON_UNPRESSED             0
/// TP_Button����״ֵ̬
#define TP_BUTTON_PRESSED               1
/// TP_Button����״ֵ̬
#define TP_BUTTON_DISABLED              2
/// TP_Button״̬
#define TP_BUTTON_STATE_COUNT           3
/// ���Index�Ƿ�Ϸ�
#define CHECK_INDEX(index)              (index < TP_BUTTON_STATE_COUNT)

/// �߿�λ�ã��ϱ�
#define TP_BUTTON_BORDER_TOP            0
/// �߿�λ�ã��±�
#define TP_BUTTON_BORDER_BOTTOM         1
/// �߿�λ�ã����
#define TP_BUTTON_BORDER_LEFT           2
/// �߿�λ�ã��ұ�
#define TP_BUTTON_BORDER_RIGHT          3
/// �߿����
#define TP_BUTTON_BORDER_NUM            4


/// ͸������Сֵ0����ʾ��ȫ��͸��
#define TP_BUTTON_ALPHA_MIN             0
/// ͸�������ֵ255����ʾ��ȫ͸��
#define TP_BUTTON_ALPHA_MAX             255

/// ��ʾ�ı���������ʱ�滻����
#define REPLACE                         "..."

/// TP_ButtonĬ��ID��71
#define TP_BUTTON_DEFAULT_ID            71
/// TP_ButtonĬ��״̬���ǰ���״̬
#define TP_BUTTON_DEFAULT_ISPRESSED     TP_BUTTON_UNPRESSED
/// TP_ButtonĬ�Ͻ������뽹�㣺��
#define TP_BUTTON_DEFAULT_FOCUSSABLE    0
/// TP_ButtonĬ�����뽹����ɫ����ɫ
#define TP_BUTTON_DEFAULT_FOCUSCOLOR    GUI_WHITE
/// TP_ButtonĬ�ϱ���ͼƬ���ޣ��ޣ���
#define TP_BUTTON_DEFAULT_APBKBITMAP    {NULL, NULL, NULL}
/// TP_ButtonĬ�ϱ�����ɫ���̣��죬��
#define TP_BUTTON_DEFAULT_ABKCOLOR      {0x97cc58, GUI_RED, GUI_INVALID_COLOR}
/// TP_ButtonĬ�ϱ�����ɫ͸���ȣ���ȫ��͸��
#define TP_BUTTON_DEFAULT_BKCOLORALPHA  0
/// TP_ButtonĬ���ı����ݣ����ı�
#define TP_BUTTON_DEFAULT_PTEXT         NULL
/// TP_ButtonĬ���ı���ɫ���ף�������
#define TP_BUTTON_DEFAULT_ATEXTCOLOR    {0xf8ffe4, GUI_BLUE, GUI_INVALID_COLOR}
/// TP_ButtonĬ���ı����壺�����
#define TP_BUTTON_DEFAULT_PFONT         &GUI_FontF16DqW3
/// TP_ButtonĬ���ı����뷽�������м�
#define TP_BUTTON_DEFAULT_TEXTALIGN     TP_TA_HCENTER_VCENTER
/// TP_ButtonĬ���ı�͸���ȣ���ȫ��͸��
#define TP_BUTTON_DEFAULT_TEXTALPHA     0
/// TP_ButtonĬ�Ͼ���Բ�ǰ뾶����Բ��
#define TP_BUTTON_DEFAULT_RADIUS        0
/// TP_ButtonĬ�ϱ߿���ɫ���ޣ��ޣ���
#define TP_BUTTON_DEFAULT_ABORDERCOLOR  {GUI_INVALID_COLOR, GUI_INVALID_COLOR, GUI_INVALID_COLOR}
/// TP_ButtonĬ�ϱ߿��ȣ�0, 0, 0, 0
#define TP_BUTTON_DEFAULT_ABORDERSIZE   {0, 0, 0, 0}
/// TP_ButtonĬ�϶���洢�ռ䣺0
#define TP_BUTTON_DEFAULT_NUMEXTRABYTES 0

#define TP_BUTTON_DEFAULT_BK_COLOR_ON	0

/**
 * \brief TP_BUTTON����
 *
 * ���ڴ洢TP_BUTTON������ֵ��
 */
typedef struct
{
    /// Button ID
    int                 id;
    /// Button״̬
    int                 isPressed;
    /// �Ƿ�������뽹��
    int                 focussable;
    /// ���뽹����ɫ
    GUI_COLOR           focusColor;
    /// ����ͼƬ
    const GUI_BITMAP *  aPBkBitmap[TP_BUTTON_STATE_COUNT];
    /// ������ɫ
    GUI_COLOR           aBkColor[TP_BUTTON_STATE_COUNT];
    /// ����͸����
    int                 bkColorAlpha;
    /// �ı�����
    char *              pText;
    /// �ı���ɫ
    GUI_COLOR           aTextColor[TP_BUTTON_STATE_COUNT];
    /// �ı�����
    const GUI_FONT *    pFont;
    /// �ı����뷽ʽ
    int                 textAlign;
    /// �ı�͸����
    int                 textAlpha;
    /// ����Բ�ǰ뾶
    int                 radius;
    /// �߿���ɫ
    GUI_COLOR           aBorderColor[TP_BUTTON_STATE_COUNT];
    /// �߿���
    U8                  aBorderSize[TP_BUTTON_BORDER_NUM];
    /// ����洢�ռ�
    int                 numExtraBytes;
	/// �Ƿ�ʹ�ñ�����ɫ
	U8					bkColorOn;
} TP_BUTTON_Obj;

/**
 * \brief TP_BUTTON_Handle����
 *
 * �����û����ֿؼ����͡�
 */
typedef WM_HMEM TP_BUTTON_Handle;

/*********************************************************************
*
* ����ֵGet/Set����
*/
/**
 * \brief        ���ظ���TP_BUTTON�ؼ��ı߿��ȡ�
 * \param        hObj    �ؼ��ľ����
 * \param        index   �߿�����������ڵ���0��С��TP_BUTTON_BORDER_NUM��
 * \return       �ɹ�ʱ���ظ���TP_BUTTON�ؼ��ı߿��ȣ�ʧ��ʱ����RET_FAIL(-1)��
 */
int TP_Button_GetBorderSize(TP_BUTTON_Handle hObj, unsigned int index);

/**
 * \brief        ���ø���TP_BUTTON�ؼ��ı߿��ȡ�
 * \param        hObj    �ؼ��ľ����
 * \param        aSize   �����ñ߿��ȡ�
 */
void TP_Button_SetBorderSizeEx(TP_BUTTON_Handle hObj, U8 * aSize);

/**
 * \brief        ���ø���TP_BUTTON�ؼ��ı߿��ȡ�
 * \param        hObj    �ؼ��ľ����
 * \param        size    �����ñ߿�
 * \param        size    �����ñ߿��ȡ�
 */
void TP_Button_SetBorderSize(TP_BUTTON_Handle hObj, unsigned int index, U8 size);

/**
 * \brief        ���ظ���TP_BUTTON�ؼ��ı߿���ɫ��
 * \param        hObj    �ؼ��ľ����
 * \param        index   �߿��ı���ɫ�����������ڵ���0��С��TP_BUTTON_STATE_COUNT��
 * \return       �ɹ�ʱ���ظ���TP_BUTTON�ؼ��ı߿���ɫ��ʧ��ʱ����GUI_INVALID_COLOR��
 */
GUI_COLOR TP_Button_GetBorderColor(TP_BUTTON_Handle hObj, unsigned int index);

/**
 * \brief        ���ø���TP_BUTTON�ؼ��ı߿���ɫ��
 * \param        hObj    �ؼ��ľ����
 * \param        aColor  �����ñ߿���ɫ��
 */
void TP_Button_SetBorderColorEx(TP_BUTTON_Handle hObj, GUI_COLOR * aColor);

/**
 * \brief        ���ø���TP_BUTTON�ؼ��ı߿���ɫ��
 * \param        hObj    �ؼ��ľ����
 * \param        index   �߿���ɫ�����������ڵ���0��С��TP_BUTTON_STATE_COUNT��
 * \param        color   �����ñ߿���ɫ��
 */
void TP_Button_SetBorderColor(TP_BUTTON_Handle hObj, unsigned int index, GUI_COLOR color);

/**
 * \brief        ���ظ���TP_BUTTON�ؼ��ľ���Բ�ǰ뾶��
 * \param        hObj    �ؼ��ľ����
 * \return       �ɹ�ʱ���ظ���TP_BUTTON�ؼ��ľ���Բ�ǰ뾶��ʧ��ʱ����RET_FAIL(-1)��
 */
int TP_Button_GetRadius(TP_BUTTON_Handle hObj);

/**
 * \brief        ���ø���TP_BUTTON�ؼ��ľ���Բ�ǰ뾶��
 * \param        hObj    �ؼ��ľ����
 * \param        alpha   �����þ���Բ�ǰ뾶��
 */
void TP_Button_SetRadius(TP_BUTTON_Handle hObj, int radius);

/**
 * \brief        ���ظ���TP_BUTTON�ؼ����ı�͸���ȡ�
 * \param        hObj    �ؼ��ľ����
 * \return       �ɹ�ʱ���ظ���TP_BUTTON�ؼ����ı�͸���ȣ�ʧ��ʱ����RET_FAIL(-1)��
 */
int TP_Button_GetTextAlpha(TP_BUTTON_Handle hObj);

/**
 * \brief        ���ø���TP_BUTTON�ؼ����ı�͸���ȡ�
 * \param        hObj    �ؼ��ľ����
 * \param        alpha   �������ı���͸���ȡ�
 */
void TP_Button_SetTextAlpha(TP_BUTTON_Handle hObj, int alpha);

/**
 * \brief        ���ظ���TP_BUTTON�ؼ����ı����뷽ʽ��
 * \param        hObj    �ؼ��ľ����
 * \return       �ɹ�ʱ���ظ���TP_BUTTON�ؼ����ı����뷽ʽ��ʧ��ʱ����RET_FAIL(-1)��
 */
int TP_Button_GetTextAlign(TP_BUTTON_Handle hObj);

/**
 * \brief        ���ø���TP_BUTTON�ؼ����ı����뷽ʽ��
 * \param        hObj    �ؼ��ľ����
 * \param        align   �������ı��Ķ��뷽ʽ��
 */
void TP_Button_SetTextAlign(TP_BUTTON_Handle hObj, int align);

/**
 * \brief        ���ظ���TP_BUTTON�ؼ����ı�����ָ�롣
 * \param        hObj    �ؼ��ľ����
 * \return       �ɹ�ʱ���ظ���TP_BUTTON�ؼ����ı�����ָ�룬ʧ��ʱ���ؿ�ָ�롣
 */
const GUI_FONT * TP_Button_GetTextFont(TP_BUTTON_Handle hObj);

/**
 * \brief        ���ø���TP_BUTTON�ؼ����ı����塣
 * \param        hObj    �ؼ��ľ����
 * \param        pFont   �������ı������塣
 */
void TP_Button_SetTextFont(TP_BUTTON_Handle hObj, const GUI_FONT * pFont);

/**
 * \brief        ���ظ���TP_BUTTON�ؼ����ı���ɫ��
 * \param        hObj    �ؼ��ľ����
 * \param        index   �ı���ɫ�����������ڵ���0��С��TP_BUTTON_STATE_COUNT��
 * \return       �ɹ�ʱ���ظ���TP_BUTTON�ؼ����ı���ɫ��ʧ��ʱ����GUI_INVALID_COLOR��
 */
GUI_COLOR TP_Button_GetTextColor(TP_BUTTON_Handle hObj, unsigned int index);

/**
 * \brief        ���ø���TP_BUTTON�ؼ����ı���ɫ��
 * \param        hObj    �ؼ��ľ����
 * \param        aColor  �������ı�����ɫ��
 */
void TP_Button_SetTextColorEx(TP_BUTTON_Handle hObj, GUI_COLOR * aColor);

/**
 * \brief        ���ø���TP_BUTTON�ؼ����ı���ɫ��
 * \param        hObj    �ؼ��ľ����
 * \param        index   �������ı���ɫ�����������ڵ���0��С��TP_BUTTON_STATE_COUNT��
 * \param        color   �������ı�����ɫ��
 */
void TP_Button_SetTextColor(TP_BUTTON_Handle hObj, unsigned int index, GUI_COLOR color);

/**
 * \brief        ����ָ��TP_BUTTON�ؼ����ı���
 * \param        hObj    �ؼ��ľ����
 * \param        pBuffer ָ�򻺳�����ָ�롣
 * \param        maxLen  �������Ĵ�С��
 * \attention    ���������ռ�С�ڿؼ��ı����ȣ��ضϴ���ֻ�ܼ��������ֿؼ��ı���
 * \return       �ɹ�ʱ���ؼ��������ı����ȣ�ʧ��ʱ����RET_FAIL(-1)��
 */
int TP_Button_GetText(TP_BUTTON_Handle hObj, char * pBuffer, int maxLen);

/**
 * \brief        ������ʾ�ڸ���TP_BUTTON�ؼ����ı���
 * \param        hObj    �ؼ��ľ����
 * \param        pText   �����õ��ı���
 */
int TP_Button_SetText(TP_BUTTON_Handle hObj, char * pText);

/**
 * \brief        ���ظ���TP_BUTTON�ؼ��ı�����ɫ͸���ȡ�
 * \param        hObj    �ؼ��ľ����
 * \return       �ɹ�ʱ���ظ���TP_BUTTON�ؼ��ı�����ɫ͸���ȣ�ʧ��ʱ����RET_FAIL(-1)��
 */
int TP_Button_GetBkColorAlpha(TP_BUTTON_Handle hObj);

/**
 * \brief        ���ø���TP_BUTTON�ؼ�������ɫ��͸���ȡ�
 * \param        hObj    �ؼ��ľ����
 * \param        alpha   �����ñ�����ɫ��͸���ȡ�
 */
void TP_Button_SetBkColorAlpha(TP_BUTTON_Handle hObj, int alpha);

/**
 * \brief        ���ظ���TP_BUTTON�ؼ��ı�����ɫ��
 * \param        hObj    �ؼ��ľ����
 * \param        index   ������ɫ�����������ڵ���0��С��TP_BUTTON_STATE_COUNT��
 * \return       �ɹ�ʱ���ظ���TP_BUTTON�ؼ��ı�����ɫ��ʧ��ʱ����GUI_INVALID_COLOR��
 */
GUI_COLOR TP_Button_GetBkColor(TP_BUTTON_Handle hObj, unsigned int index);

/**
 * \brief        ���ø���TP_BUTTON�ؼ��ı�����ɫ��
 * \param        hObj    �ؼ��ľ����
 * \param        aColor  �����ñ�����ɫ��
 */
void TP_Button_SetBkColorEx(TP_BUTTON_Handle hObj, GUI_COLOR * aColor);

/**
 * \brief        ���ø���TP_BUTTON�ؼ��ı�����ɫ��
 * \param        hObj    �ؼ��ľ����
 * \param        index   ������ɫ��������
 * \param        color   �����ñ�����ɫ��
 */
void TP_Button_SetBkColor(TP_BUTTON_Handle hObj, unsigned int index, GUI_COLOR color);

/**
 * \brief        ���ø���TP_BUTTON�ؼ��ı�����ɫ�Ƿ���Ҫ���ơ�
 * \param        hObj    �ؼ��ľ����
 * \return        ���ض�Ӧ�Ŀ���ֵ��
 */
U8 TP_Button_GetBkColorOn(TP_BUTTON_Handle hObj);

/**
 * \brief        ���ø���TP_BUTTON�ؼ��ı�����ɫ�Ƿ���Ҫ���ơ�
 * \param        hObj    �ؼ��ľ����
 * \param        onff   �����õ�ֵ��
 */
void TP_Button_SetBkColorOn(TP_BUTTON_Handle hObj, U8 onff);


/**
 * \brief        ���ظ���TP_BUTTON�ؼ��ı���λͼָ�롣
 * \param        hObj    �ؼ��ľ����
 * \param        index   ���豳��λͼ�����������ڵ���0��С��TP_BUTTON_STATE_COUNT��
 * \return       �ɹ�ʱ���ظ���TP_BUTTON�ؼ��ı���λͼָ�룬ʧ��ʱ���ؿ�ָ�롣
 */
const GUI_BITMAP* TP_Button_GetBitmap(TP_BUTTON_Handle hObj, unsigned int index);

/**
 * \brief        ���ø���TP_BUTTON�ؼ��ı���λͼ��
 * \param        hObj    �ؼ��ľ����
 * \param        pBitmap ָ�򱳾�λͼ��ָ�����顣
 */
void TP_Button_SetBitmapEx(TP_BUTTON_Handle hObj,
        const GUI_BITMAP * aPBitmap[TP_BUTTON_STATE_COUNT]);

/**
 * \brief        ���ø���TP_BUTTON�ؼ��ı���λͼ��
 * \param        hObj    �ؼ��ľ����
 * \param        index   ���豳��λͼ�����������ڵ���0��С��TP_BUTTON_STATE_COUNT��
 * \param        pBitmap ָ�򱳾�λͼ��ָ�롣
 */
void TP_Button_SetBitmap(TP_BUTTON_Handle hObj, unsigned int index, const GUI_BITMAP * pBitmap);

/**
 * \brief        ���ظ���TP_BUTTON�ؼ��������뽹��ʱ����ɫ��
 * \param        hObj    �ؼ��ľ����
 * \return       �ɹ�ʱ���ظ���TP_BUTTON�ؼ��������뽹��ʱ����ɫ��ʧ��ʱ����GUI_INVALID_COLOR��
 */
GUI_COLOR TP_Button_GetFocusColor(TP_BUTTON_Handle hObj);

/**
 * \brief        ���ø���TP_BUTTON�ؼ��������뽹��ʱ����ɫ��
 * \param        hObj    �ؼ��ľ����
 * \param        color   �����ý������뽹��ʱ����ɫ��
 */
void TP_Button_SetFocusColor(TP_BUTTON_Handle hObj, GUI_COLOR color);

/**
 * \brief        ���ظ���TP_BUTTON�ؼ��Ƿ�������뽹�㡣
 * \param        hObj    �ؼ��ľ����
 * \return       �ɹ�ʱ���ظ���TP_BUTTON�ؼ��Ƿ�������뽹�㣬ʧ��ʱ����RET_FAIL(-1)��
 */
int TP_Button_GetFocussable(TP_BUTTON_Handle hObj);

/**
 * \brief        ���ø���TP_BUTTON�ؼ��Ƿ�������뽹�㡣
 * \param        hObj    �ؼ��ľ����
 * \param        state   �Ƿ�������뽹�㡣
 */
void TP_Button_SetFocussable(TP_BUTTON_Handle hObj, int state);

/**
 * \brief        ���ظ���TP_BUTTON�ؼ��Ƿ��ڰ���״̬��
 * \param        hObj    �ؼ��ľ����
 * \return       �ɹ�ʱ���ظ���TP_BUTTON�ؼ��Ƿ��ڰ���״̬��ʧ��ʱ����RET_FAIL(-1)��
 */
int TP_Button_IsPressed(TP_BUTTON_Handle hObj);

/**
 * \brief        ���ø���TP_BUTTON�ؼ��Ƿ��Ѿ����¡�
 * \param        hObj    �ؼ��ľ����
 * \param        state   �Ƿ��Ѿ����¡�
 */
void TP_Button_SetPressed(TP_BUTTON_Handle hObj, int state);

/**
 * \brief        ���ظ���TP_BUTTON�ؼ���ID��
 * \param        hObj    �ؼ��ľ����
 * \return       �ɹ�ʱ���ظ���TP_BUTTON�ؼ���ID��ʧ��ʱ����RET_FAIL(-1)��
 */
int TP_Button_GetId(TP_BUTTON_Handle hObj);

/**
 * \brief        ���ø���TP_BUTTON�ؼ���ID��
 * \param        hObj    �ؼ��ľ����
 * \param        id      �ؼ���ID��
 */
void TP_Button_SetId(TP_BUTTON_Handle hObj, int id);

/*********************************************************************
*
* Ĭ������ֵGet/Set����
*/
/**
 * \brief        ���ظ���TP_BUTTON�ؼ���Ĭ�ϱ߿��ȡ�
 * \param        index   Ĭ�ϱ߿��ȵ����������ڵ���0��С��TP_BUTTON_BORDER_NUM��
 * \return       ����TP_BUTTON�ؼ���Ĭ�ϱ߿��ȡ�
 */
U8 TP_Button_GetDefaultBorderSize(unsigned int index);

/**
 * \brief        ���ø���TP_BUTTON�ؼ���Ĭ�ϱ߿���ɫ��
 * \param        index   Ĭ�ϱ߿��ȵ����������ڵ���0��С��TP_BUTTON_BORDER_NUM��
 * \param        size    ������Ĭ�ϱ߿��ȡ�
 */
void TP_Button_SetDefaultBorderSize(unsigned int index, U8 size);

/**
 * \brief        ���ظ���TP_BUTTON�ؼ���Ĭ�ϱ߿���ɫ��
 * \param        index   �߿�Ĭ����ɫ�����������ڵ���0��С��TP_BUTTON_STATE_COUNT��
 * \return       ����TP_BUTTON�ؼ���Ĭ�ϱ߿���ɫ��
 */
GUI_COLOR TP_Button_GetDefaultBorderColor(unsigned int index);

/**
 * \brief        ���ø���TP_BUTTON�ؼ���Ĭ�ϱ߿���ɫ��
 * \param        index   �߿�Ĭ����ɫ�����������ڵ���0��С��TP_BUTTON_STATE_COUNT��
 * \param        color   ������Ĭ�ϱ߿���ɫ��
 */
void TP_Button_SetDefaultBorderColor(unsigned int index, GUI_COLOR color);

/**
 * \brief        ���ظ���TP_BUTTON�ؼ����ı�Ĭ��͸���ȡ�
 * \return       ����TP_BUTTON�ؼ����ı�Ĭ��͸���ȡ�
 */
int TP_Button_GetDefaultRadius();

/**
 * \brief        ���ø���TP_BUTTON�ؼ���Ĭ�Ͼ���Բ�ǰ뾶��
 * \param        alpha   ������Ĭ�Ͼ���Բ�ǰ뾶��
 */
void TP_Button_SetDefaultRadius(int radius);

/**
 * \brief        ���ظ���TP_BUTTON�ؼ����ı�Ĭ��͸���ȡ�
 * \return       ����TP_BUTTON�ؼ����ı�Ĭ��͸���ȡ�
 */
int TP_Button_GetDefaultTextAlpha();

/**
 * \brief        ���ø���TP_BUTTON�ؼ����ı�Ĭ��͸���ȡ�
 * \param        alpha   �������ı���Ĭ��͸���ȡ�
 */
void TP_Button_SetDefaultTextAlpha(int alpha);

/**
 * \brief        ���ظ���TP_BUTTON�ؼ����ı�Ĭ�϶��뷽ʽ��
 * \return       ����TP_BUTTON�ؼ����ı�Ĭ�϶��뷽ʽ��
 */
int TP_Button_GetDefaultTextAlign();

/**
 * \brief        ���ø���TP_BUTTON�ؼ����ı�Ĭ�϶��뷽ʽ��
 * \param        align   �������ı�Ĭ�϶��뷽ʽ��
 */
void TP_Button_SetDefaultTextAlign(int align);

/**
 * \brief        ���ظ���TP_BUTTON�ؼ����ı�Ĭ������ָ�롣
 * \return       ����TP_BUTTON�ؼ����ı�Ĭ������ָ�롣
 */
const GUI_FONT * TP_Button_GetDefaultTextFont();

/**
 * \brief        ���ø���TP_BUTTON�ؼ����ı�Ĭ�����塣
 * \param        pFont   �������ı�Ĭ�����塣
 */
void TP_Button_SetDefaultTextFont(const GUI_FONT * pFont);

/**
 * \brief        ���ظ���TP_BUTTON�ؼ����ı�Ĭ����ɫ��
 * \param        index   �ı�Ĭ����ɫ�����������ڵ���0��С��TP_BUTTON_STATE_COUNT��
 * \return       ����TP_BUTTON�ؼ����ı�Ĭ����ɫ��
 */
GUI_COLOR TP_Button_GetDefaultTextColor(unsigned int index);

/**
 * \brief        ���ø���TP_BUTTON�ؼ����ı�Ĭ����ɫ��
 * \param        index   �������ı�Ĭ����ɫ�����������ڵ���0��С��TP_BUTTON_STATE_COUNT��
 * \param        color   �������ı�Ĭ����ɫ��
 */
void TP_Button_SetDefaultTextColor(unsigned int index, GUI_COLOR color);

/**
 * \brief        ����ָ��TP_BUTTON�ؼ���Ĭ���ı���
 * \param        pBuffer ָ�򻺳�����ָ�롣
 * \param        maxLen  �������Ĵ�С��
 * \attention    ���������ռ�С�ڿؼ�Ĭ���ı����ȣ��ضϴ���ֻ�ܼ��������ֿؼ�Ĭ���ı���
 * \return       �ɹ�ʱ���ؼ�������Ĭ���ı����ȣ�ʧ��ʱ����RET_FAIL(-1)��
 */
int TP_Button_GetDefaultText(char * pBuffer, int maxLen);

/**
 * \brief        ������ʾ�ڸ���TP_BUTTON�ؼ���Ĭ���ı���
 * \param        pText   �����õ�Ĭ���ı���
 */
int TP_Button_SetDefaultText(char * pText);

/**
 * \brief        ���ظ���TP_BUTTON�ؼ��ı�����ɫĬ��͸���ȡ�
 * \return       ����TP_BUTTON�ؼ��ı�����ɫĬ��͸���ȡ�
 */
int TP_Button_GetDefaultBkColorAlpha();

/**
 * \brief        ���ø���TP_BUTTON�ؼ�������ɫ��Ĭ��͸���ȡ�
 * \param        alpha   �����ñ�����ɫ��Ĭ��͸���ȡ�
 */
void TP_Button_SetDefaultBkColorAlpha(int alpha);

/**
 * \brief        ���ظ���TP_BUTTON�ؼ���Ĭ�ϱ�����ɫ��
 * \param        index   Ĭ�ϱ�����ɫ�����������ڵ���0��С��TP_BUTTON_STATE_COUNT��
 * \return       ����TP_BUTTON�ؼ���Ĭ�ϱ�����ɫ��
 */
GUI_COLOR TP_Button_GetDefaultBkColor(unsigned int index);

/**
 * \brief        ���ø���TP_BUTTON�ؼ���Ĭ�ϱ�����ɫ��
 * \param        index   Ĭ�ϱ�����ɫ�����������ڵ���0��С��TP_BUTTON_STATE_COUNT��
 * \param        color   ������Ĭ�ϱ�����ɫ��
 */
void TP_Button_SetDefaultBkColor(unsigned int index, GUI_COLOR color);

/**
 * \brief        ����TP_BUTTON�ؼ���Ĭ�ϵı������ص�ֵ��
 * \return       ����TP_BUTTON�ؼ���Ĭ�ϵı������ص�ֵ��
 */
U8 TP_Button_GetDefaultBkColorOn();

/**
 * \brief        ����TP_BUTTON�ؼ���Ĭ�ϵı������ص�ֵ��
 * \param      Ҫ�趨��ֵ��
 */
void TP_Button_SetDefaultBkColorOn(U8 onff);


/**
 * \brief        ���ظ���TP_BUTTON�ؼ���Ĭ�ϱ���λͼָ�롣
 * \param        index   Ĭ�ϱ���λͼ�����������ڵ���0��С��TP_BUTTON_STATE_COUNT��
 * \return       ����TP_BUTTON�ؼ���Ĭ�ϱ���λͼָ�롣
 */
const GUI_BITMAP* TP_Button_GetDefaultBitmap(unsigned int index);

/**
 * \brief        ���ø���TP_BUTTON�ؼ���Ĭ�ϱ���λͼ��
 * \param        index   Ĭ�ϱ���λͼ�����������ڵ���0��С��TP_BUTTON_STATE_COUNT��
 * \param        pBitmap ָ��Ĭ�ϱ���λͼ��ָ�롣
 */
void TP_Button_SetDefaultBitmap(unsigned int index, const GUI_BITMAP * pBitmap);

/**
 * \brief        ���ظ���TP_BUTTON�ؼ��������뽹��ʱ��Ĭ����ɫ��
 * \return       ����TP_BUTTON�ؼ��������뽹��ʱ��Ĭ����ɫ��
 */
GUI_COLOR TP_Button_GetDefaultFocusColor();

/**
 * \brief        ���ø���TP_BUTTON�ؼ��������뽹��ʱ��Ĭ����ɫ��
 * \param        color   �����ý������뽹��ʱ��Ĭ����ɫ��
 */
void TP_Button_SetDefaultFocusColor(GUI_COLOR color);

/**
 * \brief        ���ظ���TP_BUTTON�ؼ�Ĭ���Ƿ�������뽹�㡣
 * \return       ����TP_BUTTON�ؼ�Ĭ���Ƿ�������뽹�㡣
 */
int TP_Button_GetDefaultFocussable();

/**
 * \brief        ���ø���TP_BUTTON�ؼ�Ĭ���Ƿ�������뽹�㡣
 * \param        state   Ĭ���Ƿ�������뽹�㡣
 */
void TP_Button_SetDefaultFocussable(int state);

/**
 * \brief        ���ظ���TP_BUTTON�ؼ�Ĭ���Ƿ��ڰ���״̬��
 * \return       ����TP_BUTTON�ؼ�Ĭ���Ƿ��ڰ���״̬��
 */
int TP_Button_IsDefaultPressed();

/**
 * \brief        ���ø���TP_BUTTON�ؼ�Ĭ���Ƿ��¡�
 * \param        state   Ĭ���Ƿ��¡�
 */
void TP_Button_SetDefaultPressed(int state);

/**
 * \brief        ���ظ���TP_BUTTON�ؼ���Ĭ��ID��
 * \return       ����TP_BUTTON�ؼ���Ĭ��ID��
 */
int TP_Button_GetDefaultId();

/**
 * \brief        ���ø���TP_BUTTON�ؼ���Ĭ��ID��
 * \param        id      ������Ĭ��ID��
 */
void TP_Button_SetDefaultId(int id);

/*********************************************************************
*
* �����ؼ���ط���
*/

/**
 * \brief        �ؼ��Ļص�������
 *
 * �����û��Զ���ص��������ؼ��д�����WM_PAINT��WM_PID_STATE_CHANGED��WM_TOUCH��
 * WM_SET_FOCUS��WM_DELETE��WM_GET_ID��WM_SET_ID����Ϣ��
 * \param        pMsg    ������Ϣ��
 */
void TP_Button_Callback(WM_MESSAGE * pMsg);

/**
 * \brief        �����û�˽�����ݡ�
 *
 * \param        hObj       �ؼ��ľ����
 * \param        pDest      ָ�򻺳�����ָ�롣
 * \param        numBytes   �������Ĵ�С��
 * \return       �ɹ�ʱ����RET_SUCCESS(0)��ʧ��ʱ����RET_FAIL(-1)��
 */
int TP_Button_SetUserData(TP_BUTTON_Handle hObj, void * pDest, int numBytes);

/**
 * \brief        �����û�˽�����ݡ�
 *
 * \param        hObj       �ؼ��ľ����
 * \param        pDest      ָ�򻺳�����ָ�롣
 * \param        numBytes   �������Ĵ�С��
 * \return       �ɹ�ʱ�����û�˽�����ݵĳ��ȣ�ʧ��ʱ����0��
 */
int TP_Button_GetUserData(TP_BUTTON_Handle hObj, void * pDest, int numBytes);

/**
 * \brief        �����ؼ����ڡ�
 *
 * \param        x0             Button���������أ��ڸ������У���
 * \param        y0             Button���������أ��ڸ������У���
 * \param        xSize          Button��ˮƽ�ߴ磨��λ�����أ���
 * \param        ySize          Button�Ĵ�ֱ�ߴ磨��λ�����أ���
 * \param        hParent        �����ڵľ�������Ϊ0,��Ϊ���棨�������ڣ������
 * \param        style          ���ڴ�����ǡ�
 * \param        cb             ���ڻص�������������Ϣ����
 * \param        numExtraBytes  �û�˽�����ݵĳ��ȡ�
 * \param        id             Button��ID��
 * \return       Button�ľ����
 */
TP_BUTTON_Handle TP_Button_Create(int x0, int y0, int xSize, int ySize, WM_HWIN hParent,
        U8 style, WM_CALLBACK * cb, int numExtraBytes, int id);

#endif /* TP_BUTTON_H__ */
