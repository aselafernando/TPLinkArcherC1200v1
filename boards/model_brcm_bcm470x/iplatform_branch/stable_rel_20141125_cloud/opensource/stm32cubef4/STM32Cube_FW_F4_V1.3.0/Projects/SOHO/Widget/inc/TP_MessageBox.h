/*! Copyright (c) 2014 Shenzhen TP-LINK Technologies Co.Ltd.
 *
 *\file     TP_MessageBox.h
 *\brief    TP_MESSAGEBOX�ؼ��Ķ����뺯��ԭ��.
 *
 *\version  1.0.0
 *\date     11Nov14
 */
#ifndef TP_MESSAGEBOX_H__
#define TP_MESSAGEBOX_H__

#include "Common.h"
#include "TP_Button.h"

/// TP_MessageBox��ť�����Ŀ��2
#define TP_MESSAGEBOX_MAX_BUTTON                    2

/// TP_MessageBoxĬ��ID��73
#define TP_MESSAGEBOX_DEFAULT_ID                    73
/// TP_MessageBoxĬ�Ͻ������뽹�㣺��
#define TP_MESSAGEBOX_DEFAULT_FOCUSSABLE            0
/// TP_MessageBoxĬ�Ͻ������뽹����ɫ����
#define TP_MESSAGEBOX_DEFAULT_FOCUSCOLOR            GUI_WHITE
/// TP_MessageBoxĬ���ı���ɫ���ף�������
#define TP_MESSAGEBOX_DEFAULT_TEXTCOLOR             GUI_BLACK
/// TP_MessageBoxĬ���ı����壺�����
#define TP_MESSAGEBOX_DEFAULT_PTEXTFONT             &GUI_FontF16DqW3
/// TP_MessageBoxĬ���ı����뷽�������м�
#define TP_MESSAGEBOX_DEFAULT_TEXTALIGN             TP_TA_HCENTER_VCENTER
/// TP_MessageBoxĬ���ı�ҳ�߿հ׿�ȣ�110p
#define TP_MESSAGEBOX_DEFAULT_TEXTMARGIN            110
/// TP_MessageBoxĬ���ı����ݣ����ı�
#define TP_MESSAGEBOX_DEFAULT_PTEXT                 NULL
/// TP_MessageBoxĬ����Ϣ����ͼƬ����
#define TP_MESSAGEBOX_DEFAULT_PBKBITMAP             NULL
/// TP_MessageBoxĬ�ϱ�����ɫ����
#define TP_MESSAGEBOX_DEFAULT_BKCOLOR               GUI_WHITE
/// TP_MessageBoxĬ��͸���ȣ�227
#define TP_MESSAGEBOX_DEFAULT_ALPHA                 227
/// TP_MessageBoxĬ����Ϣ�߶ȣ�142p
#define TP_MESSAGEBOX_DEFAULT_MESSAGEHEIGHT         142
/// TP_MessageBoxĬ�ϰ�ť������2
#define TP_MESSAGEBOX_DEFAULT_BUTTONNUM             2
/// TP_MessageBoxĬ�ϰ�ť�߶ȣ�30p
#define TP_MESSAGEBOX_DEFAULT_BUTTONHEIGHT          30
/// TP_MessageBoxĬ�ϰ�ť�������
#define TP_MESSAGEBOX_DEFAULT_AHBUTTON              {WM_HMEM_NULL, WM_HMEM_NULL}
/// TP_MessageBoxĬ�϶���ռ䣺0
#define TP_MESSAGEBOX_DEFAULT_NUMEXTRABYTES         0

typedef struct
{
    /// MessageBox ID
    int                 id;
    /// �Ƿ�������뽹��
    int                 focussable;
    /// ���뽹����ɫ
    GUI_COLOR           focusColor;
    /// MessageBox�ؼ���ʾ�ı���ɫ
    GUI_COLOR           textColor;
    /// MessageBox�ؼ���ʾ�ı�����
    const GUI_FONT *    pTextFont;
    /// MessageBox�ؼ���ʾ�ı����뷽ʽ
    int                 textAlign;
    /// MessageBox�ؼ��ı�ҳ�߿հ׿��
    int                 textMargin;
    /// MessageBox�ؼ���ʾ�ı���Ϣ
    char *              pText;
    /// MessageBox�ؼ���ʾλͼ
    /*
     * \attention ��λͼΪ��Ϣ�д���ʾλͼ���ǿؼ�����λͼ��
     */
    const GUI_BITMAP *  pBkBitmap;
    /// MessageBox�ؼ�������ɫ
    /*
     * \attention MessageBox�ؼ�������ɫ��������ť���򱳾���ɫ��
     */
    GUI_COLOR           bkColor;
    /// MessageBox�ؼ�͸������͸����
    int                 alpha;
    /// MessageBox�ؼ���Ϣ�߶�
    int                 messageHeight;
    /// MessageBox�ؼ���ť����
    int                 buttonNum;
    /// MessageBox�ؼ���ť�߶�
    int                 buttonHeight;
    /// Button�ؼ����
    TP_BUTTON_Handle    aHButton[TP_MESSAGEBOX_MAX_BUTTON];
    /// ����洢�ռ�
    int                 numExtraBytes;
} TP_MESSAGEBOX_Obj;

/**
 * \brief TP_MESSAGEBOX_Handle����
 *
 * �����û����ֿؼ����͡�
 */
typedef WM_HMEM TP_MESSAGEBOX_Handle;

/*******************
 * ����Get/Set���� *
 *******************/
/**
 * \brief        ���ظ���TP_MESSAGEBOX�ؼ���Button�����
 * \param        hObj    �ؼ��ľ����
 * \param        index   ��ť�����������ڵ���0��С��TP_MESSAGEBOB_MAX_BUTTON��
 * \return       �ɹ�ʱ���ظ����ؼ���Button�����ʧ��ʱ����WM_HMEM_NULL��
 */
TP_BUTTON_Handle TP_MessageBox_GetButtonHandle(TP_MESSAGEBOX_Handle hObj, unsigned int index);

/**
 * \brief        ���ø���TP_MESSAGEBOX�ؼ���Button�����
 * \param        hObj    �ؼ��ľ����
 * \param        index   ��ť�����������ڵ���0��С��TP_MESSAGEBOB_MAX_BUTTON��
 * \param        hButton ��ť�ľ����
 */
void TP_MessageBox_SetButtonHandle(TP_MESSAGEBOX_Handle hObj, unsigned int index, TP_BUTTON_Handle hButton);

/**
 * \brief        ���ظ���TP_MESSAGEBOX�ؼ��ı�����ɫ��
 * \param        hObj    �ؼ��ľ����
 * \return       �ɹ�ʱ���ظ����ؼ�������ɫ��ʧ��ʱ����GUI_INVALID_COLOR��
 */
GUI_COLOR TP_MessageBox_GetBkColor(TP_MESSAGEBOX_Handle hObj);

/**
 * \brief        ���ø���TP_MESSAGEBOX�ؼ��ı�����ɫ��
 * \param        hObj    �ؼ��ľ����
 * \param        color   ������ɫ��
 */
void TP_MessageBox_SetBkColor(TP_MESSAGEBOX_Handle hObj, GUI_COLOR color);

/**
 * \brief        ���ظ���TP_MESSAGEBOX�ؼ��ı���ͼƬ��
 * \param        hObj    �ؼ��ľ����
 * \return       �ɹ�ʱ���ظ����ؼ�����ͼƬ��ʧ��ʱ����NULL��
 */
const GUI_BITMAP * TP_MessageBox_GetBkBitmap(TP_MESSAGEBOX_Handle hObj);

/**
 * \brief        ���ø���TP_MESSAGEBOX�ؼ��ı���ͼƬ��
 * \param        hObj    �ؼ��ľ����
 * \param        bitmap  ����ͼƬ��
 * \attention    ���ô˷������Ʊ���ͼǰ�������ú���ʾ�ı�����Ϊ����ͼ�����ı���ʾ������л��ơ�
 */
void TP_MessageBox_SetBkBitmap(TP_MESSAGEBOX_Handle hObj, const GUI_BITMAP * bitmap);

/**
 * \brief        ����ָ��TP_MESSAGEBOX�ؼ����ı���
 * \param        hObj    �ؼ��ľ����
 * \param        pBuffer ָ�򻺳�����ָ�롣
 * \param        maxLen  �������Ĵ�С��
 * \attention    ���������ռ�С�ڿؼ��ı����ȣ��ضϴ���ֻ�ܼ��������ֿؼ��ı���
 * \return       �ɹ�ʱ���ؼ��������ı����ȣ�ʧ��ʱ����RET_FAIL(-1)��
 */
int TP_Message_GetText(TP_MESSAGEBOX_Handle hObj, char * pBuffer, int maxLen);

/**
 * \brief        ������ʾ�ڸ���TP_MESSAGEBOX�ؼ����ı���
 * \param        hObj    �ؼ��ľ����
 * \param        pText   �����õ��ı���
 */
int TP_MessageBox_SetText(TP_MESSAGEBOX_Handle hObj, char * pText);

/**
 * \brief        ���ظ���TP_MESSAGEBOX�ؼ����ı�ҳ�߿հ׿�ȡ�
 * \param        hObj    �ؼ��ľ����
 * \return       �ɹ�ʱ���ظ����ؼ��ı�ҳ�߿հ׿�ȣ�ʧ��ʱ����RET_FAIL(-1)��
 */
int TP_MessageBox_GetTextMargin(TP_MESSAGEBOX_Handle hObj);

/**
 * \brief        ���ø���TP_MESSAGEBOX�ؼ����ı����߿հ׿�ȡ�
 * \param        hObj    �ؼ��ľ����
 * \param        margin  �ı����߿հ׿�ȣ���λ�����ء�
 */
void TP_MessageBox_SetTextMargin(TP_MESSAGEBOX_Handle hObj, int margin);

/**
 * \brief        ���ظ���TP_MESSAGEBOX�ؼ����ı����뷽ʽ��
 * \param        hObj    �ؼ��ľ����
 * \return       �ɹ�ʱ���ظ����ؼ��ı����뷽ʽ��ʧ��ʱ����RET_FAIL(-1)��
 */
int TP_MessageBox_GetTextAlign(TP_MESSAGEBOX_Handle hObj);

/**
 * \brief        ���ø���TP_MESSAGEBOX�ؼ����ı����뷽ʽ��
 * \param        hObj    �ؼ��ľ����
 * \param        align   �ı����뷽ʽ��
 */
void TP_MessageBox_SetTextAlign(TP_MESSAGEBOX_Handle hObj, int align);

/**
 * \brief        ���ظ���TP_MESSAGEBOX�ؼ����ı����塣
 * \param        hObj    �ؼ��ľ����
 * \return       �ɹ�ʱ���ظ����ؼ��ı����壬ʧ��ʱ����NULL��
 */
const GUI_FONT * TP_MessageBox_GetTextFont(TP_MESSAGEBOX_Handle hObj);

/**
 * \brief        ���ø���TP_MESSAGEBOX�ؼ����ı����塣
 * \param        hObj    �ؼ��ľ����
 * \param        font    �ı����塣
 */
void TP_MessageBox_SetTextFont(TP_MESSAGEBOX_Handle hObj, const GUI_FONT * font);

/**
 * \brief        ���ظ���TP_MESSAGEBOX�ؼ����ı���ɫ��
 * \param        hObj    �ؼ��ľ����
 * \return       �ɹ�ʱ���ظ����ؼ��ı���ɫ��ʧ��ʱ����GUI_INVALID_COLOR��
 */
GUI_COLOR TP_MessageBox_GetTextColor(TP_MESSAGEBOX_Handle hObj);

/**
 * \brief        ���ø���TP_MESSAGEBOX�ؼ����ı���ɫ��
 * \param        hObj    �ؼ��ľ����
 * \param        color   �ı���ɫ��
 */
void TP_MessageBox_SetTextColor(TP_MESSAGEBOX_Handle hObj, GUI_COLOR color);

/**
 * \brief        ���ظ���TP_MESSAGEBOX�ؼ��������뽹��ʱ����ɫ��
 * \param        hObj    �ؼ��ľ����
 * \return       �ɹ�ʱ���ظ���TP_MESSAGEBOX�ؼ��������뽹��ʱ����ɫ��ʧ��ʱ����GUI_INVALID_COLOR��
 */
GUI_COLOR TP_MessageBox_GetFocusColor(TP_MESSAGEBOX_Handle hObj);

/**
 * \brief        ���ø���TP_MESSAGEBOX�ؼ��������뽹��ʱ����ɫ��
 * \param        hObj    �ؼ��ľ����
 * \param        color   �����ý������뽹��ʱ����ɫ��
 */
void TP_MessageBox_SetFocusColor(TP_MESSAGEBOX_Handle hObj, GUI_COLOR color);

/**
 * \brief        ���ظ���TP_MESSAGEBOX�ؼ��Ƿ�������뽹�㡣
 * \param        hObj    �ؼ��ľ����
 * \return       �ɹ�ʱ���ظ���TP_MESSAGEBOX�ؼ��Ƿ�������뽹�㣬ʧ��ʱ����RET_FAIL(-1)��
 */
int TP_MessageBox_GetFocussable(TP_MESSAGEBOX_Handle hObj);

/**
 * \brief        ���ø���TP_MESSAGEBOX�ؼ��Ƿ�������뽹�㡣
 * \param        hObj    �ؼ��ľ����
 * \param        state   �Ƿ�������뽹�㡣
 */
void TP_MessageBox_SetFocussable(TP_MESSAGEBOX_Handle hObj, int state);

/***********************
 * Ĭ������Get/Set���� *
 ***********************/
/**
 * \brief        ���ظ���TP_MESSAGEBOX�ؼ���Ĭ��Button�����
 * \param        index   ��ť�����������ڵ���0��С��TP_MESSAGEBOB_MAX_BUTTON��
 * \return       �ɹ�ʱ���ظ����ؼ���Ĭ��Button�����ʧ��ʱ����WM_HMEM_NULL��
 */
TP_BUTTON_Handle TP_MessageBox_GetDefaultButtonHandle(unsigned int index);

/**
 * \brief        ���ø���TP_MESSAGEBOX�ؼ���Ĭ��Button�����
 * \param        index   ��ť�����������ڵ���0��С��TP_MESSAGEBOB_MAX_BUTTON��
 * \param        hButton ��ť�ľ����
 */
void TP_MessageBox_SetDefaultButtonHandle(unsigned int index, TP_BUTTON_Handle hButton);

/**
 * \brief        ���ظ���TP_MESSAGEBOX�ؼ���Ĭ�ϱ�����ɫ��
 * \return       �ɹ�ʱ���ظ����ؼ�Ĭ�ϱ�����ɫ��ʧ��ʱ����GUI_INVALID_COLOR��
 */
GUI_COLOR TP_MessageBox_GetDefaultBkColor();

/**
 * \brief        ���ø���TP_MESSAGEBOX�ؼ���Ĭ�ϱ�����ɫ��
 * \param        color   Ĭ�ϱ�����ɫ��
 */
void TP_MessageBox_SetDefaultBkColor(GUI_COLOR color);

/**
 * \brief        ���ظ���TP_MESSAGEBOX�ؼ���Ĭ�ϱ���ͼƬ��
 * \return       �ɹ�ʱ���ظ����ؼ�����ͼƬ��ʧ��ʱ����NULL��
 */
const GUI_BITMAP * TP_MessageBox_GetDefaultBkBitmap();

/**
 * \brief        ���ø���TP_MESSAGEBOX�ؼ���Ĭ�ϱ���ͼƬ��
 * \param        bitmap  Ĭ�ϱ���ͼƬ��
 */
void TP_MessageBox_SetDefaultBkBitmap(const GUI_BITMAP * bitmap);

/**
 * \brief        ����ָ��TP_MESSAGEBOX�ؼ���Ĭ���ı���
 * \param        pBuffer ָ�򻺳�����ָ�롣
 * \param        maxLen  �������Ĵ�С��
 * \attention    ���������ռ�С�ڿؼ��ı����ȣ��ضϴ���ֻ�ܼ��������ֿؼ��ı���
 * \return       �ɹ�ʱ���ؼ��������ı����ȣ�ʧ��ʱ����RET_FAIL(-1)��
 */
int TP_Message_GetDefaultText(char * pBuffer, int maxLen);

/**
 * \brief        ������ʾ�ڸ���TP_MESSAGEBOX�ؼ���Ĭ���ı���
 * \param        pText   �����õ��ı���
 */
int TP_MessageBox_SetDefaultText(char * pText);

/**
 * \brief        ���ظ���TP_MESSAGEBOX�ؼ���Ĭ���ı�ҳ�߿հ׿�ȡ�
 * \return       �ɹ�ʱ���ظ����ؼ�Ĭ���ı�ҳ�߿հ׿�ȣ�ʧ��ʱ����RET_FAIL(-1)��
 */
int TP_MessageBox_GetDefaultTextMargin();

/**
 * \brief        ���ø���TP_MESSAGEBOX�ؼ����ı����߿հ׿�ȡ�
 * \param        margin  Ĭ���ı����߿հ׿�ȣ���λ�����ء�
 */
void TP_MessageBox_SetDefaultTextMargin(int margin);

/**
 * \brief        ���ظ���TP_MESSAGEBOX�ؼ���Ĭ���ı����뷽ʽ��
 * \return       �ɹ�ʱ���ظ����ؼ�Ĭ���ı����뷽ʽ��ʧ��ʱ����RET_FAIL(-1)��
 */
int TP_MessageBox_GetDefaultTextAlign();

/**
 * \brief        ���ø���TP_MESSAGEBOX�ؼ���Ĭ���ı����뷽ʽ��
 * \param        align   �ı����뷽ʽ��
 */
void TP_MessageBox_SetDefaultTextAlign(int align);

/**
 * \brief        ���ظ���TP_MESSAGEBOX�ؼ���Ĭ���ı����塣
 * \return       �ɹ�ʱ���ظ����ؼ�Ĭ���ı����壬ʧ��ʱ����NULL��
 */
const GUI_FONT * TP_MessageBox_GetDefaultTextFont();

/**
 * \brief        ���ø���TP_MESSAGEBOX�ؼ���Ĭ���ı����塣
 * \param        font    Ĭ���ı����塣
 */
void TP_MessageBox_SetDefaultTextFont(GUI_FONT * font);

/**
 * \brief        ���ظ���TP_MESSAGEBOX�ؼ���Ĭ���ı���ɫ��
 * \return       �ɹ�ʱ���ظ����ؼ�Ĭ���ı���ɫ��ʧ��ʱ����GUI_INVALID_COLOR��
 */
GUI_COLOR TP_MessageBox_GetDefaultTextColor();

/**
 * \brief        ���ø���TP_MESSAGEBOX�ؼ���Ĭ���ı���ɫ��
 * \param        color   Ĭ���ı���ɫ��
 */
void TP_MessageBox_SetDefaultTextColor(GUI_COLOR color);

/**
 * \brief        ���ظ���TP_MESSAGEBOX�ؼ��������뽹��ʱ��Ĭ����ɫ��
 * \return       �ɹ�ʱ���ظ���TP_MESSAGEBOX�ؼ��������뽹��ʱ��Ĭ����ɫ��ʧ��ʱ����GUI_INVALID_COLOR��
 */
GUI_COLOR TP_MessageBox_GetDefaultFocusColor();

/**
 * \brief        ���ø���TP_MESSAGEBOX�ؼ��������뽹��ʱ��Ĭ����ɫ��
 * \param        color   �����ý������뽹��ʱ��Ĭ����ɫ��
 */
void TP_MessageBox_SetDefaultFocusColor(GUI_COLOR color);

/**
 * \brief        ���ظ���TP_MESSAGEBOX�ؼ�Ĭ���Ƿ�������뽹�㡣
 * \return       �ɹ�ʱ���ظ���TP_MESSAGEBOX�ؼ�Ĭ���Ƿ�������뽹�㣬ʧ��ʱ����RET_FAIL(-1)��
 */
int TP_MessageBox_GetDefaultFocussable();

/**
 * \brief        ���ø���TP_MESSAGEBOX�ؼ�Ĭ���Ƿ�������뽹�㡣
 * \param        state   Ĭ���Ƿ�������뽹�㡣
 */
void TP_MessageBox_SetDefaultFocussable(int state);

/********************
 * ���ڴ�����ط��� *
 ********************/
/**
 * \brief        �����û�˽�����ݡ�
 *
 * \param        hObj       �ؼ��ľ����
 * \param        pDest      ָ�򻺳�����ָ�롣
 * \param        numBytes   �������Ĵ�С��
 * \return       �ɹ�ʱ����RET_SUCCESS(0)��ʧ��ʱ����RET_FAIL(-1)��
 */
int TP_MessageBox_SetUserData(TP_BUTTON_Handle hObj, void * pDest, int numBytes);

/**
 * \brief        �����û�˽�����ݡ�
 *
 * \param        hObj       �ؼ��ľ����
 * \param        pDest      ָ�򻺳�����ָ�롣
 * \param        numBytes   �������Ĵ�С��
 * \return       �ɹ�ʱ�����û�˽�����ݵĳ��ȣ�ʧ��ʱ����0��
 */
int TP_MessageBox_GetUserData(TP_BUTTON_Handle hObj, void * pDest, int numBytes);

/**
 * \brief        �����ؼ����ڡ�
 *
 * \param        x0             MessageBox���������أ��ڸ������У���ͨ��Ϊ0��
 * \param        y0             MessageBox���������أ��ڸ������У���ͨ��Ϊ0��
 * \param        xSize          MessageBox��ˮƽ�ߴ磨��λ�����أ���
 * \param        ySize          MessageBox�Ĵ�ֱ�ߴ磨��λ�����أ���
 * \param        hParent        �����ڵľ�������Ϊ0,��Ϊ���棨�������ڣ������
 * \param        style          ���ڴ�����ǡ�
 * \param        cb             ���ڻص�������������Ϣ����
 * \param        numExtraBytes  �û�˽�����ݵĳ��ȡ�
 * \param        id             MessageBox��ID��
 * \param        messageHeight  ��Ϣ��ʾ�߶ȣ�δ������Ĭ�ϴ���
 * \param        buttonHeight   ��ť�߶ȣ�δ������Ĭ�ϴ���
 * \param        buttonNum      ��ť������δ������Ĭ�ϴ���
 * \return       MessageBox�ľ����
 */
TP_MESSAGEBOX_Handle TP_MessageBox_Create(int x0, int y0, int xSize, int ySize, WM_HWIN hParent,
        U8 style, WM_CALLBACK * cb, int numExtraBytes, int id,
        int messageHeight, int buttonHeight, int buttonNum);

#endif /* TP_MESSAGEBOX_H__ */
