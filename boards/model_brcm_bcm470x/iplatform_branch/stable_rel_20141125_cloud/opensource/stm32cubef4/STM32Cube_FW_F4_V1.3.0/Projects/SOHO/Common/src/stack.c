/******************************************************************************
*
* Copyright (c) 2010 TP-LINK Technologies CO.,LTD.
* All rights reserved.
*
* FILE NAME  :   stack.c
* VERSION    :   1.0
* DESCRIPTION:   实现整型的通用堆栈.
*
* AUTHOR     :   Huangwenzhong <Huangwenzhong@tp-link.net>
* CREATE DATE:   11/14/2014
*
* HISTORY    :
* 01   11/14/2014  Huangwenzhong     Create.
*
******************************************************************************/
#include <string.h>

#include "stack.h"
#include "Common.h"

void stackInit(STACK *pStack, U32 maxNum)
{
	if (pStack)
	{
		TP_MEMSET(pStack, 0, sizeof(STACK));
		pStack->count = 0;
		pStack->maxNum =  (maxNum > 0);
	}
}

STATUS stackPush(STACK *pStack, I32 data)
{
	if ((pStack == NULL) || (pStack->count >= pStack->maxNum))
	{
		return RET_FAIL;
	}

	pStack->array[pStack->count] = data;
	pStack->count++;

	return RET_SUCCESS;
}

STATUS stackPop(STACK *pStack, I32 *pData)
{
	if ((pStack == NULL) || (pStack->count <= 0))
	{
		return RET_FAIL;
	}

	*pData = pStack->array[pStack->count - 1];
	pStack->count--;

	return RET_SUCCESS;
}

BOOL stackIsEmpty(const STACK *pStack)
{
	if (pStack)
	{
		return (pStack->count <= 0);
	}

	return TRUE;
}

BOOL stackIsFull(const STACK *pStack)
{
	if (pStack)
	{
		return (pStack->count >= pStack->maxNum);
	}

	return TRUE;
}

