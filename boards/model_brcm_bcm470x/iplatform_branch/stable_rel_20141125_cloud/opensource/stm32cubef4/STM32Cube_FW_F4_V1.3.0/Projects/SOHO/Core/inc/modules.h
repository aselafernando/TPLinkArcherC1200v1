/******************************************************************************
*
* Copyright (c) 2010 TP-LINK Technologies CO.,LTD.
* All rights reserved.
*
* FILE NAME  :   modules.h
* VERSION    :   1.0
* DESCRIPTION:   ģ������ļ����ṩͳһ��ģ�������.
*
* AUTHOR     :   Huangwenzhong <Huangwenzhong@tp-link.net>
* CREATE DATE:   11/13/2014
*
* HISTORY    :
* 01   11/13/2014  Huangwenzhong     Create.
*
******************************************************************************/
#ifndef _MODULES__H_
#define _MODULES__H_
#include "Common.h"
#include "GUI.h"

#define MODULE_MAX_NUM		(32)

typedef struct
{
	U8 id;/* ID��1��ʼ�� */
	const CHAR *pName;/* ģ�����ƣ�Ҳ����ʾ�ڲ˵�ͼ���ϵ����� */
	const GUI_BITMAP *pIcon;/* ͼ��ͼƬ */
	void (*init)(void);/* ģ��ĳ�ʼ������ */
	void (*unInit)(void);
	void (*startup)(WM_HWIN hWin, I32 x, I32 y);/* ģ����������� */
	void (*shutdown)();/* ģ��Ĺرպ��� */
}MODULE_ENTRY;

void modulesInit();
STATUS modulesRegister(const MODULE_ENTRY *pModule);
void modulesRemoveALL();
U32 modulesGetNum();
BOOL modulesIsRegistered(U8 id);
BOOL modulesGetModInfo(U8 id, const MODULE_ENTRY **pModule);

#endif

