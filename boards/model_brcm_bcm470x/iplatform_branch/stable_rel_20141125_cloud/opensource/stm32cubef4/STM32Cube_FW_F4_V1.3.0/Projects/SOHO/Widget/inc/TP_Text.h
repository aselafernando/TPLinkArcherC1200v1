/*! Copyright (c) 2014 Shenzhen TP-LINK Technologies Co.Ltd.
 *
 * \file     TP_Text.h
 * \brief    TP_TEXT�ؼ��Ķ����뺯��ԭ��.
 *
 * \version  1.0.0
 * \date     03Nov14
 */
#ifndef TP_TEXT_H__
#define TP_TEXT_H__

#include "WM.h"
#include "TEXT.h"

/**
 * \brief TP_TEXT_Handle����
 *
 * �����û����ֿؼ����͡�
 */
typedef WM_HMEM TP_TEXT_Handle;

/**
 * \brief       ��ָ��λ�ô���ָ����С��TP_TEXT�ؼ���
 * \param       x0          TP_TEXT�ؼ����������أ��ڸ������У���
 * \param       y0          TP_TEXT�ؼ���˵����أ��ڸ������У���
 * \param       xSize       TP_TEXT�ؼ���ˮƽ�ߴ磨������Ϊ��λ����
 * \param       ySize       TP_TEXT�ؼ�����ֱ�ߴ磨������Ϊ��λ����
 * \param       hParent     �����ڵľ�������Ϊ0���򴴽��Ŀؼ���Ϊ���棨���㴰�ڣ����Ӵ��ڡ�
 * \param       WinFlags    ���ڴ�����ǣ�Ĭ�Ͽ���WM_CF_SHOW��WM_CF_MEMDEV�� WM_CF_HASTRANS��
 * \param       ExFlags     �ı��Ķ������ԡ�
 * \param       Id          TEXT�ؼ��Ĵ���ID��
 * \param       pText       Ҫ��ʾ���ı���ָ�롣
 * \return      ��������TP_TEXT�ؼ��ľ�����������ִ��ʧ�ܣ��򷵻�ֵΪ0��
 */
TP_TEXT_Handle TP_Text_CreateEx(int x0,             int y0,
                                int xSize,          int ySize,
                                WM_HWIN hParent,    int WinFlags,
                                int ExFlags,        int Id,
                                const char * pText);

/**
 * \brief       ����TP_TEXT�ؼ���ʹ�õ�Ĭ�����塣
 * \return      TP_TEXT�ؼ���ʹ�õ�Ĭ������ָ�롣
 */
const GUI_FONT * TP_Text_GetDefaultFont(void);

/**
 * \brief       ���ص�ǰ��TP_TEXT�ؼ�������ʾ����������
 * \param       hObj        TP_TEXT�ؼ��ľ����
 * \return      ��������
 */
int TP_Text_GetNumLines(TP_TEXT_Handle hObj);

/**
 * \brief       ����TP_TEXT�ؼ��ı�����ɫ��
 * \param       hObj        TP_TEXT�ؼ��ľ����
 * \param       Color       �����ñ�����ɫ����Χ0x000000��0xFFFFFF��,GUI_INVALID_COLOR��ʹ����͸����
 */
void TP_Text_SetBkColor(TP_TEXT_Handle hObj, GUI_COLOR Color);

/**
 * \brief       ����TP_TEXT�ؼ���Ĭ�����塣
 * \param       pFont       Ҫ���ó�Ĭ��ֵ������ָ�롣
 */
void TP_Text_SetDefaultFont(const GUI_FONT * pFont);

/**
 * \brief       ����TP_TEXT�ؼ���Ĭ���ı���ɫ��
 * \param       Color       Ҫʹ�õ���ɫ��
 */
void TP_Text_SetDefaultColor(GUI_COLOR Color);

/**
 * \brief       ����TP_TEXT�ؼ���Ĭ���ı�����ģʽ��
 * \param       Color       Ҫʹ�õ�Ĭ���ı�����ģʽ����������ֵ��
 *
 * GUI_WRAPMODE_NONE    δִ���κθ��ǡ�
 * GUI_WRAPMODE_WORD    �ı����ַ�ʽ���ǡ�
 * GUI_WRAPMODE_CHAR    �ı����ַ���ʽ���ǡ�
 */
GUI_WRAPMODE TP_Text_SetDefaultWrapMode(GUI_WRAPMODE WrapMode);

/**
 * \brief       ����TP_TEXT�ؼ������塣
 * \param       hObj        TP_TEXT�ؼ��ľ����
 * \param       pFont       Ҫʹ�õ�����ָ�롣
 */
void TP_Text_SetFont(TP_TEXT_Handle hObj, const GUI_FONT * pFont);

/**
 * \brief       ����TP_TEXT�ؼ����ı���
 * \param       hObj        TP_TEXT�ؼ��ľ����
 * \param       s           Ҫ��ʾ���ı���
 */
void TP_Text_SetText(TP_TEXT_Handle hObj, const char * s);

/**
 * \brief       ����TP_TEXT�ؼ����ı����뷽ʽ��
 * \param       hObj        TP_TEXT�ؼ��ľ����
 * \param       Align       �ı����뷽ʽ��
 */
void TP_Text_SetTextAlign(TP_TEXT_Handle hObj, int align);

/**
 * \brief       ����TP_TEXT�ؼ����ı���ɫ��
 * \param       hObj        TP_TEXT�ؼ��ľ����
 * \param       color       ���ı���ɫ��
 */
void TP_Text_SetTextColor(TP_TEXT_Handle hObj, GUI_COLOR color);

/**
 * \brief       ����TP_TEXT�ؼ����ı�����ģʽ��
 * \param       Color       Ҫʹ�õ��ı�����ģʽ����������ֵ��
 *
 * GUI_WRAPMODE_NONE    δִ���κθ��ǡ�
 * GUI_WRAPMODE_WORD    �ı����ַ�ʽ���ǡ�
 * GUI_WRAPMODE_CHAR    �ı����ַ���ʽ���ǡ�
 */
void TP_Text_SetWrapMode(TP_TEXT_Handle hObj, GUI_WRAPMODE WrapMode);

#endif /* TP_TEXT_H__ */
