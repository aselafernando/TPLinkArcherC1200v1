#include "TP_Dropdown.h"

void dropdownList_init(dropdownItemList * list)
{
	list->head = NULL;
	list->tail = NULL;
	list->count = 0;
}

int dropdownList_Clear(dropdownItemList * list)
{
	dropdownListNode * head = list->head;
	dropdownListNode * preNode;
	
	if (list->count == 0)
	{
		return 1;
	}

	while (head != NULL)
	{
		preNode = head;
		head = head->next;
		if (preNode->item.pText)
		{
			free(preNode->item.pText);
		}
		free(preNode);
		preNode = NULL;
	}

	list = NULL;
	return 1;
}

void dropdownList_addItem(dropdownItemList * list, dropdownListNode * node)
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

int dropdownList_delItem(dropdownItemList * list, dropdownListNode * node)
{
	dropdownListNode * head = list->head;
	dropdownListNode * preNode;

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

dropdownListNode * newDropdownListNode(int id, int index, GUI_RECT rect, char * sText)
{
	dropdownListNode * node = (dropdownListNode*)TP_MALLOC(sizeof(dropdownListNode));
	char *pText = (char *)TP_MALLOC(strlen(sText) + 1);
	node->item.id = id;
	node->item.index = index;
	node->item.rect = rect;
	if (pText)
	{
		TP_MEMSET(pText, 0, strlen(sText) + 1);
		TP_MEMCPY(pText, sText, strlen(sText));
		node->item.pText = pText;
	}
	node->next = NULL;

	return node;
}

dropdownListNode * findDropdownNodeByPos(dropdownItemList * list, int x, int y)
{
	dropdownItemList * tmpList = list;
	dropdownListNode * head;
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

dropdownListNode * findDropdownNodeById(dropdownItemList * list, int id)
{
	dropdownItemList * tmpList = list;
	dropdownListNode * head;

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

dropdownListNode * findDropdownNodeByIndex(dropdownItemList * list, int index)
{
	dropdownItemList * tmpList = list;
	dropdownListNode * head;

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
