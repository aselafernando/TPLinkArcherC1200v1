/******************************************************************************
*
* Copyright (c) 2010 TP-LINK Technologies CO.,LTD.
* All rights reserved.
*
* FILE NAME  :   modules.h
* VERSION    :   1.0
* DESCRIPTION:   模块核心文件，提供统一的模块管理方法.
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
	U8 id;/* ID从1开始排 */
	const CHAR *pName;/* 模块名称，也是显示在菜单图标上的名字 */
	const GUI_BITMAP *pIcon;/* 图标图片 */
	void (*init)(void);/* 模块的初始化函数 */
	void (*unInit)(void);
	void (*startup)(WM_HWIN hWin, I32 x, I32 y);/* 模块的启动函数 */
	void (*shutdown)();/* 模块的关闭函数 */
}MODULE_ENTRY;

void modulesInit();
STATUS modulesRegister(const MODULE_ENTRY *pModule);
void modulesRemoveALL();
U32 modulesGetNum();
BOOL modulesIsRegistered(U8 id);
BOOL modulesGetModInfo(U8 id, const MODULE_ENTRY **pModule);

#endif

