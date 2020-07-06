/******************************************************************************
*
* Copyright (c) 2010 TP-LINK Technologies CO.,LTD.
* All rights reserved.
*
* FILE NAME  :   guiQuickSetup.h
* VERSION    :   1.0
* DESCRIPTION:   处理Quick Setup的流程.
*
* AUTHOR     :   Huangwenzhong <Huangwenzhong@tp-link.net>
* CREATE DATE:   11/19/2014
*
* HISTORY    :
* 01   11/19/2014  Huangwenzhong     Create.
*
******************************************************************************/

#include "GUI_QuickSetup.h"

/* 头结点，不指向实际内容 */
static QS_STEP_HEAD qsHeadStep = {NULL};

static QS_STEP_ITEM *pCurStep = NULL;

void GUI_QsInit()
{
	pCurStep = qsHeadStep.pStepList;
}

BOOL GUI_QsAddStep(QS_STEP_ID id, QS_STEP_LEVEL level, BOOL flag)
{
	QS_STEP_ITEM *pNewItem = NULL;
	QS_STEP_ITEM *pStepItem = NULL;

	if ((id < QS_STEP_START) || (id > QS_STEP_END))
	{
		return FALSE;
	}

	pNewItem = (QS_STEP_ITEM*)malloc(sizeof(QS_STEP_ITEM));
	if (pNewItem == NULL)
	{
		return FALSE;
	}

	pNewItem->id = id;
	pNewItem->flag = flag;
	pNewItem->level = level;
	pNewItem->next = NULL;

	pStepItem = qsHeadStep.pStepList;
	while (pNewItem->next != NULL)
	{
		pNewItem = pNewItem->next;
	}

	pStepItem->next = pNewItem;
	pNewItem->prev = pStepItem;

	return TRUE;
}

BOOL GUI_QsFindNext(QS_STEP_ITEM **pStepData)
{
	QS_STEP_ITEM *pStepItem = NULL;

	if ((pCurStep == NULL) || (pStepData == NULL))
	{
		return FALSE;
	}

	for (pStepItem = pCurStep->next; pStepItem != NULL; pStepItem = pStepItem->next)
	{
		if (pStepItem && (pStepItem->flag == TRUE))
		{
			*pStepData = pStepItem;
			pCurStep = pStepItem;
			break;
		}
	}

	if (pStepItem == NULL)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL GUI_QsFindPrev(QS_STEP_ITEM **pStepData)
{
	QS_STEP_ITEM *pStepItem = NULL;

	if ((pCurStep == NULL) || (pStepData == NULL))
	{
		return FALSE;
	}

	for (pStepItem = pCurStep->prev; pStepItem != NULL; pStepItem = pStepItem->prev)
	{
		if (pStepItem && (pStepItem->flag == TRUE))
		{
			*pStepData = pStepItem;
			pCurStep = pStepItem;
			break;
		}
	}

	if (pStepItem == NULL)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL GUI_QsActiveBranchStep(QS_STEP_ID id)
{
	QS_STEP_ITEM *pStepItem = NULL;
	QS_STEP_LEVEL level;

	if ((id < QS_STEP_START) || (id > QS_STEP_END))
	{
		return FALSE;
	}

	for (pStepItem = qsHeadStep.pStepList; pStepItem != NULL; pStepItem = pStepItem->next)
	{
		if (pStepItem && (pStepItem->id == id))
		{
			break;
		}
	}

	if ((pStepItem == NULL) || (pStepItem->level == QS_STEP_TRUNK))
	{
		return FALSE;
	}

	level = pStepItem->level;
	for (pStepItem = qsHeadStep.pStepList; pStepItem != NULL; pStepItem = pStepItem->next)
	{
		if (pStepItem && (pStepItem->level == level))
		{
			if (pStepItem->id == id)
			{
				pStepItem->flag = TRUE;
			}
			else
			{
				pStepItem->flag = FALSE;
			}
		}
	}

	return TRUE;
}

