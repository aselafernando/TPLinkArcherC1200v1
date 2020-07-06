/*! Copyright (c) 2014 Shenzhen TP-LINK Technologies Co.Ltd.
 *
 *\file     TP_Scrollbar.h
 *\brief    TP_SCROLLBAR�ؼ��Ķ����뺯��ԭ�͡�
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
 * \brief ��ʾ��Ļ�Ŀ��
 */
#define TP_SCROLLBAR_LCD_WIDTH              480
/**
 * \brief ��ʾ��Ļ�ĸ߶�
 */
#define TP_SCROLLBAR_LCD_HEIGHT             272

/// TP_SCROLLBAR��Ĭ��ID
#define TP_SCROLLBAR_DEFAULT_ID             72
/// TP_SCROLLBAR������ʾ�����Ĭ��ˮƽ�ߴ磬ͨ��Ϊ������ʾ��
#define TP_SCROLLBAR_DEFAULT_XSIZEWIN       0
/// TP_SCROLLBAR������ʾ�����Ĭ�ϴ�ֱ�ߴ磬ͨ��Ϊ������ʾ��
#define TP_SCROLLBAR_DEFAULT_YSIZEWIN       0
/// TP_SCROLLBAR�����ڵ�Ĭ��ˮƽ�ߴ�
#define TP_SCROLLBAR_DEFAULT_XSIZELCD       TP_SCROLLBAR_LCD_WIDTH
/// TP_SCROLLBAR�����ڵ�Ĭ�ϴ�ֱ�ߴ�
#define TP_SCROLLBAR_DEFAULT_YSIZELCD       TP_SCROLLBAR_LCD_HEIGHT
/// TP_SCROLLBAR���ڵ�Ĭ��ˮƽ�ߴ�
#define TP_SCROLLBAR_DEFAULT_WIDTH          3
/// TP_SCROLLBAR���ڵ�Ĭ�ϴ�ֱ�ߴ�
#define TP_SCROLLBAR_DEFAULT_HEIGHT         2
/// ��ֱ����TP_SCROLLBAR��ʾ���ֶ����ڸ������е�Ĭ��ˮƽλ��
#define TP_SCROLLBAR_DEFAULT_XPOS           0
/// ��ֱ����TP_SCROLLBAR��ʾ���ֶ����ڸ������е�Ĭ�ϴ�ֱλ��
#define TP_SCROLLBAR_DEFAULT_YPOS           0
/// TP_SCROLLBAR�ƶ��򻬶�ʱ��ʾ��Ĭ����ɫ��
#define TP_SCROLLBAR_DEFAULT_BKCOLOR        0xB2B2B2
/// TP_SCROLLBAR���������Ƿ������ƶ���
#define TP_SCROLLBAR_DEFAULT_ISMOVING       0
/// TP_SCROLLBARֹͣ�ƶ������ʧʱ�䣬��λ�����룩��
#define TP_SCROLLBAR_DEFAULT_TIMEOUT        1000
/// TP_SCROLLBAR��Ĭ�϶���洢�ռ�
#define TP_SCROLLBAR_DEFAULT_NUMEXTRABYTES  0

/**
 * \brief TP_SCROLLBAR����
 *
 * ���ڴ洢TP_SCROLLBAR������ֵ��
 */
typedef struct
{
    /// Scrollbar ID
    int                 id;
    /// ��ʾ���ڵ�ˮƽ�ߴ磬ͨ����ʾ��Ϊ������ʾ����
    int                 xSizeWin;
    /// ��ʾ���ڵĴ�ֱ�ߴ磬ͨ����ʾ��Ϊ������ʾ����
    int                 ySizeWin;
    /// �����ڵ�ˮƽ�ߴ硣
    int                 xSizeLCD;
    /// �����ڵĴ�ֱ�ߴ硣
    int                 ySizeLCD;
    /// TP_Scrollbar��ˮƽ�ߴ硣
    int                 width;
    /// TP_Scrollbar�Ĵ�ֱ�ߴ硣
    int                 height;
    /// ˮƽ����TP_Scrollbar��ʾ���ֶ����ڸ������е�ˮƽλ�á�
    int                 xPos;
    /// ��ֱ����TP_Scrollbar��ʾ���ֶ����ڸ������еĴ�ֱλ�á�
    int                 yPos;
    /// �ƶ��򻬶�ʱ��ʾ����ɫ��
    GUI_COLOR           bkColor;
    /// �������Ƿ������ƶ���
    int                 isMoving;
    /// ֹͣ�ƶ�����ʧʱ�䡣
    int                 timeout;
    /// ����洢�ռ�
    int                 numExtraBytes;
} TP_SCROLLBAR_Obj;

/**
 * \brief TP_SCROLLBAR_Handle����
 *
 * �����û����ֿؼ����͡�
 */
typedef WM_HMEM TP_SCROLLBAR_Handle;

/**
 * \brief        ��ֱ����TP_Scrollbar�ؼ��Ļص�������
 *
 * �ص��������ؼ��д�����PAINT��TP_MSG_SCROLLBAR_Y����Ϣ��
 * \param        pMsg    ������Ϣ��
 */
static void TP_Scrollbar_Callback_Y(WM_MESSAGE * pMsg);

/**
 * \brief        ˮƽ����TP_Scrollbar�ؼ��Ļص�������
 *
 * �ص��������ؼ��д�����PAINT��TP_MSG_SCROLLBAR_X��Ϣ��
 * \param        pMsg    ������Ϣ��
 */
static void TP_Scrollbar_Callback_Y(WM_MESSAGE * pMsg);

/**
 * \brief        ������ֱ�����TP_SCROLLBAR�ؼ����ڡ�
 *
 * \param        x0             �����ڵ��������ء�
 * \param        y0             �����ڵ��������ء�
 * \param        xSizeLCD       �����ڵ�ˮƽ�ߴ磨��λ�����أ���
 * \param        ySizeLCD       �����ڵĴ�ֱ�ߴ磨��λ�����أ���
 * \param        xSizeWin       �������ڣ��������ڣ���ˮƽ�ߴ磨��λ�����أ���
 * \param        ySizeLCD       �������ڵĴ�ֱ�ߴ磨��λ�����أ���
 * \param        hParent        �����ڣ�TP_Scrollbar�����뻬������ͬ��һ�������ڣ��ľ����
 *                              ���Ϊ0,��Ϊ���棨�������ڣ������
 * \param        style          ���ڴ�����ǡ�
 * \param        cb             ���ڻص�������������Ϣ����
 * \param        numExtraBytes  �û�˽�����ݵĳ��ȡ�
 * \param        id             TP_Scrollbar��ID��
 * \return       TP_Scrollbar�ľ����
 */
TP_SCROLLBAR_Handle TP_Scrollbar_Create_Y(int x0, int y0,
        int xSizeLCD, int ySizeLCD,
        int xSizeWin, int ySizeWin,
        WM_HWIN hParent, U8 style,
        WM_CALLBACK * cb, int numExtraBytes, int id);

/**
 * \brief        ����ˮƽ�����TP_SCROLLBAR�ؼ����ڡ�
 *
 * \param        x0             �����ڵ��������ء�
 * \param        y0             �����ڵ��������ء�
 * \param        xSizeLCD       �����ڵ�ˮƽ�ߴ磨��λ�����أ���
 * \param        ySizeLCD       �����ڵĴ�ֱ�ߴ磨��λ�����أ���
 * \param        xSizeWin       �������ڣ��������ڣ���ˮƽ�ߴ磨��λ�����أ���
 * \param        ySizeLCD       �������ڵĴ�ֱ�ߴ磨��λ�����أ���
 * \param        hParent        �����ڣ�TP_Scrollbar�����뻬������ͬ��һ�������ڣ��ľ����
 *                              ���Ϊ0,��Ϊ���棨�������ڣ������
 * \param        style          ���ڴ�����ǡ�
 * \param        cb             ���ڻص�������������Ϣ����
 * \param        numExtraBytes  �û�˽�����ݵĳ��ȡ�
 * \param        id             TP_Scrollbar��ID��
 * \return       TP_Scrollbar�ľ����
 */
TP_SCROLLBAR_Handle TP_Scrollbar_Create_X(int x0, int y0,
        int xSizeLCD, int ySizeLCD,
        int xSizeWin, int ySizeWin,
        WM_HWIN hParent, U8 style,
        WM_CALLBACK * cb, int numExtraBytes, int id);

#endif /* TP_SCROLLBAR_H__ */
