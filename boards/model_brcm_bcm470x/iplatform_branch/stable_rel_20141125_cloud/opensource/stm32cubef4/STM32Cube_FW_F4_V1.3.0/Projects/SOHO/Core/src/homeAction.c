/******************************************************************************
*
* Copyright (c) 2010 TP-LINK Technologies CO.,LTD.
* All rights reserved.
*
* FILE NAME  :   homeAction.c
* VERSION    :   1.0
* DESCRIPTION:   ʵ��Home���Ĵ���.
*
* AUTHOR     :   Huangwenzhong <Huangwenzhong@tp-link.net>
* CREATE DATE:   11/17/2014
*
* HISTORY    :
* 01   11/17/2014  Huangwenzhong     Create.
*
******************************************************************************/

#include "homeAction.h"
#include "modules.h"
#include "stack.h"
#include "Common.h"

#define HOME_ACTION_STACK_MAX_SIZE	(MODULE_MAX_NUM)

static STACK haStack;

void homeActionInit()
{
	stackInit(&haStack, HOME_ACTION_STACK_MAX_SIZE);

	/* TODO, ȱ�ٶ�HOME���жϴ�������ע�� */
}

STATUS homeActionPushApp(I32 appId)
{
	return stackPush(&haStack, appId);
}

STATUS homeActionPopApp(I32 appId)
{
	return stackPop(&haStack, &appId);
}

void homeActionHandle()
{
	const MODULE_ENTRY *pModule = NULL;
	I32 appId;
	
	while (!stackIsEmpty(&haStack))
	{
		stackPop(&haStack, &appId);

		if (modulesGetModInfo((U8)appId, &pModule) && pModule)
		{
			pModule->shutdown();
		}
	}
}

