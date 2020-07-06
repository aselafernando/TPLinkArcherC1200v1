/******************************************************************************
*
* Copyright (c) 2010 TP-LINK Technologies CO.,LTD.
* All rights reserved.
*
* FILE NAME  :   modules.c
* VERSION    :   1.0
* DESCRIPTION:   APP����ģ��.
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
* DESCRIPTION   : ��ʼ��ģ���Ȳ��� 
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
* DESCRIPTION   : ע��һ���µ�ģ�� 
* INPUT         :	pModule - ��Ҫע����µ�ģ��ṹ��
*
* OUTPUT        : N/A
* RETURN        : ���سɹ�����ʧ��
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
* DESCRIPTION   : �Ƴ�����ע���ģ�� 
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
* DESCRIPTION   : �����Ѿ�ע���ģ������� 
* INPUT         : N/A
*
* OUTPUT        : N/A
* RETURN        : �����Ѿ�ע���ģ�������
* OTHERS        :
******************************************************************************/
U32 modulesGetNum()
{
	return moduleNum;
}

/******************************************************************************
* FUNCTION      : modulesIsRegistered()
* AUTHOR        : Huangwenzhong <Huangwenzhong@tp-link.net>
* DESCRIPTION   : �ж�ĳ��ģ���Ƿ��Ѿ�ע�� 
* INPUT         : id - ָ��ģ���ID
*
* OUTPUT        : N/A
* RETURN        : �����Ƿ��Ѿ�ע��
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
* DESCRIPTION   : ��ȡ��Ӧģ��Ľṹ����Ϣ 
* INPUT         : 	id - ָ��ģ���ID
*
* OUTPUT        :  pModule - ����ģ�����Ϣ
* RETURN        : ���ػ�ȡ�Ƿ�ɹ�
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

