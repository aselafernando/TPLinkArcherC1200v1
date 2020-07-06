/******************************************************************************
*
* Copyright (c) 2010 TP-LINK Technologies CO.,LTD.
* All rights reserved.
*
* FILE NAME  :   stack.h
* VERSION    :   1.0
* DESCRIPTION:    µœ÷∂—’ª.
*
* AUTHOR     :   Huangwenzhong <Huangwenzhong@tp-link.net>
* CREATE DATE:   11/14/2014
*
* HISTORY    :
* 01   11/14/2014  Huangwenzhong     Create.
*
******************************************************************************/

#ifndef _STACK_H__
#define _STACK_H__

#include "TP_Type.h"

#define STACK_MAX_SIZE		(32)

typedef struct
{
	I32 array[STACK_MAX_SIZE];
	U32 maxNum;
	U32 count;
}STACK;

void stackInit(STACK *pStack, U32 maxNum);
STATUS stackPush(STACK *pStack, I32 data);
STATUS stackPop(STACK *pStack, I32 *pData);
BOOL stackIsEmpty(const STACK *pStack);
BOOL stackIsFull(const STACK *pStack);

#endif

