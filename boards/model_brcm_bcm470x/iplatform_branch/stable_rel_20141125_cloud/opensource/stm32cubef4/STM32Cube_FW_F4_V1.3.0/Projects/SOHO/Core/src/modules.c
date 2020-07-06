/******************************************************************************
*
* Copyright (c) 2010 TP-LINK Technologies CO.,LTD.
* All rights reserved.
*
* FILE NAME  :   modules.c
* VERSION    :   1.0
* DESCRIPTION:   APP管理模块.
*
* AUTHOR     :   Huangwenzhong <Huangwenzhong@tp-link.net>
* CREATE DATE:   11/14/2014
*
* HISTORY    :
* 01   11/14/2014  Huangwenzhong     Create.
*
******************************************************************************/

#include "modules.h"
#include "Common.h"

static const MODULE_ENTRY *modules[MODULE_MAX_NUM];
static U32 moduleNum = 0;


/******************************************************************************
* FUNCTION      : modulesInit()
* AUTHOR        : Huangwenzhong <Huangwenzhong@tp-link.net>
* DESCRIPTION   : 初始化模块表等参数 
* INPUT         : N/A
*
* OUTPUT        : N/A
* RETURN        : N/A
* OTHERS        :
******************************************************************************/
void modulesInit()
{
	I32 i = 0;

	moduleNum = 0;
	for (i = 0; i < MODULE_MAX_NUM; i++)
	{
		modules[i] = NULL;		
	}
}

/******************************************************************************
* FUNCTION      : modulesRegister()
* AUTHOR        : Huangwenzhong <Huangwenzhong@tp-link.net>
* DESCRIPTION   : 注册一个新的模块 
* INPUT         :	pModule - 需要注册的新的模块结构体
*
* OUTPUT        : N/A
* RETURN        : 返回成功或者失败
* OTHERS        :
******************************************************************************/
STATUS modulesRegister(const MODULE_ENTRY *pModule)
{
	if ((pModule == NULL) || (moduleNum >= MODULE_MAX_NUM)
		|| (modulesIsRegistered(pModule->id)))
	{
		return RET_FAIL;
	}

	modules[moduleNum] = pModule;
	moduleNum++;
	return RET_SUCCESS;
}

/******************************************************************************
* FUNCTION      : modulesRemoveALL()
* AUTHOR        : Huangwenzhong <Huangwenzhong@tp-link.net>
* DESCRIPTION   : 移出所有注册的模块 
* INPUT         : N/A
*
* OUTPUT        : N/A
* RETURN        : N/A
* OTHERS        :
******************************************************************************/
void modulesRemoveALL()
{
	I32 i = 0;

	for (i = 0; i < MODULE_MAX_NUM; i++)
	{
		modules[i] = NULL;
	}

	moduleNum = 0;
}

/******************************************************************************
* FUNCTION      : modulesGetNum()
* AUTHOR        : Huangwenzhong <Huangwenzhong@tp-link.net>
* DESCRIPTION   : 返回已经注册的模块的数量 
* INPUT         : N/A
*
* OUTPUT        : N/A
* RETURN        : 返回已经注册的模块的数量
* OTHERS        :
******************************************************************************/
U32 modulesGetNum()
{
	return moduleNum;
}

/******************************************************************************
* FUNCTION      : modulesIsRegistered()
* AUTHOR        : Huangwenzhong <Huangwenzhong@tp-link.net>
* DESCRIPTION   : 判断某个模块是否已经注册 
* INPUT         : id - 指定模块的ID
*
* OUTPUT        : N/A
* RETURN        : 返回是否已经注册
* OTHERS        :
******************************************************************************/
BOOL modulesIsRegistered(U8 id)
{
	I32 i = 0;
	
	for (i = 0; i < MODULE_MAX_NUM; i++)
	{
		if ((modules[i]) && (modules[i]->id == id))
		{
			return TRUE;
		}
	}

	return FALSE;
}

/******************************************************************************
* FUNCTION      : modulesGetModInfo()
* AUTHOR        : Huangwenzhong <Huangwenzhong@tp-link.net>
* DESCRIPTION   : 获取对应模块的结构体信息 
* INPUT         : 	id - 指定模块的ID
*
* OUTPUT        :  pModule - 返回模块的信息
* RETURN        : 返回获取是否成功
* OTHERS        :
******************************************************************************/
BOOL modulesGetModInfo(U8 id, const MODULE_ENTRY **pModule)
{
	I32 i = 0;
	
	for (i = 0; i < MODULE_MAX_NUM; i++)
	{
		if ((modules[i]) && (modules[i]->id == id))
		{
			*pModule = modules[i];
			return TRUE;
		}
	}

	return FALSE;
}

