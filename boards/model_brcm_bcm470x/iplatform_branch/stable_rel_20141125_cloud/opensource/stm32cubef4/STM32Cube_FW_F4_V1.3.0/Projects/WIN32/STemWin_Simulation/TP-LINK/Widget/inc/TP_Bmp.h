#ifndef TP_BMP_H
#define TP_BMP_H

#include <string.h>
#include <stdlib.h>

#include "GUI.h"

#include "WM.h"

typedef WM_HMEM TP_BMP_Handle;

typedef struct 
{
	int id;
	GUI_COLOR bkColor;
	const GUI_BITMAP * pBkBitmap;
} TP_BMP_Obj;

void TP_BMP_Callback(WM_MESSAGE * pMsg);
TP_BMP_Handle TP_BMP_Create(int x0, int y0, int xSize, int ySize, 
	WM_HWIN hParent, U8 style, WM_CALLBACK * cb, int numExtraBytes, int id);

void TP_BMP_SetBkColor(TP_BMP_Handle hObj, GUI_COLOR color);
void TP_BMP_SetBkBitmap(TP_BMP_Handle hObj, const GUI_BITMAP * pBitmap);

GUI_COLOR TP_BMP_GetDefaultBkColor();
void TP_BMP_SetDefaultBkColor(GUI_COLOR color);

const GUI_BITMAP * TP_BMP_GetDefaultBkBitmap();
void TP_BMP_SetDefaultBkBitmap(const GUI_BITMAP * pBitmap);
int TP_BMP_GetId(TP_BMP_Handle hObj);
#endif