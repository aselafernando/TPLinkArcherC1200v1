#include "TP_FakeList.h"

void fakelistList_init(fakelistItemList * list)
{
	list->head = NULL;
	list->tail = NULL;
	list->count = 0;
}

int fakelistList_Clear(fakelistItemList * list)
{
	fakelistListNode * head = list->head;
	fakelistListNode * preNode;
	
	if (list->count == 0)
	{
		return 1;
	}

	while (head != NULL)
	{
		preNode = head;
		head = head->next;
		if (preNode->item.pTextL)
		{
			free(preNode->item.pTextL);
		}

		if (preNode->item.pTextR)
		{
			free(preNode->item.pTextR);
		}
		free(preNode);
		preNode = NULL;
	}

	list = NULL;
	return 1;
}

void fakelistList_addItem(fakelistItemList * list, fakelistListNode * node)
{
	list->count++;
	node->next = NULL;

	if (list->head == NULL)
	{
		list->head = node;
		list->tail = node;
	}
	else
	{
		list->tail->next = node;
		list->tail = node;
	}
}

int fakelistList_delItem(fakelistItemList * list, fakelistListNode * node)
{
	fakelistListNode * head = list->head;
	fakelistListNode * preNode;

	if (head == NULL)
	{
		return 0;
	}

	if (node == head)
	{
		list->count--;
		if (list->tail == node)
		{
			list->head = NULL;
			list->tail = NULL;
		}
		else
		{
			list->head = head->next;
		}

		return 1;
	}

	while (head != NULL)
	{
		if (node == head)
		{
			break;
		}
		preNode = head;
		head = preNode->next;
	}

	if (head == NULL)
	{
		return 0;
	}
	else
	{
		preNode->next = head->next;
		if (list->tail == head)
		{
			list->tail = preNode;
		}
		list->count--;
		return 1;
	}
}

fakelistListNode * newFakelistListNode(int id, int index, GUI_RECT rect, char * sTextL, const GUI_BITMAP * pBitmapL, char * sTextR, const GUI_BITMAP * pBitmapR, pHandleFun func)
{
	char * pTextL = NULL;
	char * pTextR = NULL;
	fakelistListNode * node = (fakelistListNode*)TP_MALLOC(sizeof(fakelistListNode));

	if (node)
	{
		TP_MEMSET(node, 0, sizeof(fakelistListNode));
		
		if (sTextL && strlen(sTextL) > 0)
		{
			pTextL = (char *)TP_MALLOC(strlen(sTextL) + 1);
		}

		if (sTextR && strlen(sTextR) > 0)
		{
			pTextR = (char *)TP_MALLOC(strlen(sTextR) + 1);
		}
		node->item.id = id;
		node->item.index = index;
		node->item.rect = rect;
		if (pTextL)
		{
			TP_MEMSET(pTextL, 0, strlen(sTextL) + 1);
			TP_MEMCPY(pTextL, sTextL, strlen(sTextL));
			node->item.pTextL = pTextL;
		}
		if (pBitmapL)
		{
			node->item.pBitmapL = pBitmapL;
		}

		if (pTextR)
		{
			TP_MEMSET(pTextR, 0, strlen(sTextR) + 1);
			TP_MEMCPY(pTextR, sTextR, strlen(sTextR));
			node->item.pTextR = pTextR;
		}

		if (pBitmapR)
		{
			node->item.pBitmapR = pBitmapR;
		}

		if (func)
		{
			node->item.handle = func;
		}
		
		node->next = NULL;

		return node;
	}
	else
	{
		return NULL;
	}
}

fakelistListNode * findFakelistNodeByPos(fakelistItemList * list, int x, int y)
{
	fakelistItemList * tmpList = list;
	fakelistListNode * head;
	GUI_RECT rClient;

	if (tmpList->count == 0)
	{
		return NULL;
	}

	head = tmpList->head;
	while (head)
	{
		rClient = head->item.rect;
		if ((x >= rClient.x0) && (x <= rClient.x1) && 
			(y >= rClient.y0) && (y <= rClient.y1))
		{
				return head;
		}
		head = head->next;
	}

	return NULL;
}

fakelistListNode * findFakelistNodeById(fakelistItemList * list, int id)
{
	fakelistItemList * tmpList = list;
	fakelistListNode * head;

	if (tmpList->count == 0)
	{
		return NULL;
	}

	head = tmpList->head;
	while (head)
	{
		if (head->item.id == id)
		{
				return head;
		}
		head = head->next;
	}

	return NULL;
}

fakelistListNode * findFakelistNodeByIndex(fakelistItemList * list, int index)
{
	fakelistItemList * tmpList = list;
	fakelistListNode * head;

	if (tmpList->count == 0)
	{
		return NULL;
	}

	head = tmpList->head;
	while (head)
	{
		if (head->item.index == index)
		{
				return head;
		}
		head = head->next;
	}

	return NULL;
}

int updateFakelistNodeBitmapByIndex(fakelistItemList * list, int index, const GUI_BITMAP * pBitmap, TP_FAKELIST_BITMAP_POS flag)
{
	fakelistItemList * tmpList = list;
	fakelistListNode * head;

	if (tmpList->count == 0)
	{
		return -1;
	}

	head = tmpList->head;
	while (head)
	{
		if (head->item.index == index)
		{
			if (flag == POS_LEFT)
			{
				head->item.pBitmapL = pBitmap;
			}
			else
			{
				head->item.pBitmapR = pBitmap;
			}
			return 0;
		}
		head = head->next;
	}

	return -1;
}
