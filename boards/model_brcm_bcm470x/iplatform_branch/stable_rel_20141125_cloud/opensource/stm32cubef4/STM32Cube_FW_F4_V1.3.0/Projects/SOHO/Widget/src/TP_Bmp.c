#include "TP_Bmp.h"
#include <stdlib.h>

#include "GUI.h"

static TP_BMP_Obj TP_BMP_Default = 
{
	0,
	GUI_GREEN,
	NULL
};



GUI_COLOR TP_BMP_GetDefaultBkColor() 
{
	GUI_COLOR color = GUI_INVALID_COLOR;
	color = TP_BMP_Default.bkColor;

	return color;
}

void TP_BMP_SetDefaultBkColor(GUI_COLOR color) 
{
	if (color != GUI_INVALID_COLOR) 
	{
		TP_BMP_Default.bkColor = color;
	}
}

const GUI_BITMAP * TP_BMP_GetDefaultBkBitmap() 
{
	return TP_BMP_Default.pBkBitmap;
}
 
void TP_BMP_SetDefaultBkBitmap(const GUI_BITMAP * pBitmap) 
{
	if (pBitmap != NULL) 
	{
		TP_BMP_Default.pBkBitmap = pBitmap;
	}
}

void TP_BMP_SetBkColor(TP_BMP_Handle hObj, GUI_COLOR color) 
{
	if (hObj) 
	{
		TP_BMP_Obj obj;

		WM_LOCK();
		WM_GetUserData(hObj, &obj, sizeof(TP_BMP_Obj));
		obj.bkColor = color;
		WM_SetUserData(hObj, &obj, sizeof(TP_BMP_Obj));
		WM_UNLOCK();
	}
}

int TP_BMP_GetId(TP_BMP_Handle hObj) 
{
	TP_BMP_Obj obj;

	if (hObj) 
	{
		WM_LOCK();
		WM_GetUserData(hObj, &obj, sizeof(TP_BMP_Obj));
		WM_UNLOCK();
		return obj.id;
	}
	return -1;
}

void TP_BMP_SetBkBitmap(TP_BMP_Handle hObj, const GUI_BITMAP * pBitmap) 
{
	TP_BMP_Obj obj;

	if (hObj) 
	{
		WM_LOCK();
		WM_GetUserData(hObj, &obj, sizeof(TP_BMP_Obj));
		obj.pBkBitmap = pBitmap;
		WM_SetUserData(hObj, &obj, sizeof(TP_BMP_Obj));
		WM_UNLOCK();
	}
}

void TP_BMP_Callback(WM_MESSAGE * pMsg) 
{
	TP_BMP_Handle hWin;
	TP_BMP_Obj obj;
	GUI_RECT winRect;
	GUI_RECT rClient;

	hWin = pMsg->hWin;
	WM_GetWindowRectEx(hWin, &winRect);
	WM_GetUserData(hWin, &obj, sizeof(TP_BMP_Obj));

	switch (pMsg->MsgId) 
	{
	case WM_PAINT:
		// Ìî³ä±³¾°É«
		GUI_GetClientRect(&rClient);
		GUI_SetColor(obj.bkColor);
		GUI_FillRectEx(&rClient);

		if (obj.pBkBitmap != NULL) 
		{
			GUI_DrawBitmap(obj.pBkBitmap, rClient.x0, rClient.y0);
		}

		break;
	case WM_GET_ID:
		pMsg->Data.v = obj.id;
		break;
	case WM_SET_ID:
		obj.id = pMsg->Data.v;
		WM_SetUserData(hWin, &obj, sizeof(TP_BMP_Obj));
		break;
	default:
		WM_DefaultProc(pMsg);
	}
}

TP_BMP_Handle TP_BMP_Create(int x0, int y0, int xSize, int ySize, 
	WM_HWIN hParent, U8 style, WM_CALLBACK * cb, int numExtraBytes, int id) 
{
	TP_BMP_Handle hWin;
	WM_CALLBACK * pUsed;
	TP_BMP_Obj obj;

	obj = TP_BMP_Default;
	obj.id = id;

	if (cb) 
	{
		pUsed = cb;
	} 
	else 
	{
		pUsed = TP_BMP_Callback;
	}

	hWin = WM_CreateWindowAsChild(x0, y0, xSize, ySize, hParent, style, pUsed, sizeof(TP_BMP_Obj) + numExtraBytes);
	if (hWin) {
		WM_SetUserData(hWin, &obj, sizeof(TP_BMP_Obj));
	}

	return hWin;
}

