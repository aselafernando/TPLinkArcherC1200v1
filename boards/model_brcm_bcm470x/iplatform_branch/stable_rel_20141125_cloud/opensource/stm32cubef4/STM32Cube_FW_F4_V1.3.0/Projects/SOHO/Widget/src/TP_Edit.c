#include "TP_Edit.h"

#include "GUI_Private.h"

static int _textOffset = 3;

extern GUI_CONST_STORAGE GUI_FONT GUI_FontF16ASCIICourier;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontF16ASCIIHNPMD;

extern GUI_CONST_STORAGE GUI_BITMAP bmEDIT_DeleteBmp;
extern GUI_CONST_STORAGE GUI_BITMAP bmEDIT_SoliderBmp;

TP_EDIT_Obj TP_EDIT_Default = 
{
	0,
	0,
	{TP_EDIT_BK_NORMAL_COLOR, TP_EDIT_BK_FOCUS_COLOR, TP_EDIT_BK_DISABLED_COLOR},
	{GUI_BLACK, GUI_GRAY},
	//&GUI_FontHelveticaNeueLTPro65Md16,
	&GUI_FontF16ASCIICourier,
	&bmEDIT_DeleteBmp,
	"TP_LINK-GUEST_5289_5GXXXX",
	//"ABCDEFGHILMN",
	255,
	{21, 21},
	0,
	0,
	{14, 5, 10, 5},
	0,
	0,
	{
		2,
		GUI_BLUE,
		TP_EDIT_CURSOR_BLINK_OFF,
		0,
		1800
	},
	{
		0, 
		0
	},
	1,
	1,
	0,
	0,
	GUI_BLACK
};

TP_EDIT_SLIDER_Obj TP_EDIT_SLIDER_Default =
{
	0,
	0,
	0,
	0,
	0,
	0,
	&bmEDIT_SoliderBmp,
	0,
	0,
	0,
	0
};

void TP_EDIT_SetDefaultDelIconBitmap(const GUI_BITMAP * pBitmap)
{
	TP_EDIT_Default.pDelBitmap = pBitmap;
}

void TP_EDIT_SetMaxAvaliableSeenCharNum(int index, int num)
{
	TP_EDIT_Default.maxAvaliableSeenCharNum[index] = num;
}

void TP_EDIT_SetDefaultMaxInputCharNum(int num)
{
	TP_EDIT_Default.maxInputCharNum = num;
}

void TP_EDIT_SetDefaultBorderColor(GUI_COLOR color)
{
	TP_EDIT_Default.borderColor = color;
}

void TP_EDIT_SetDefaultBkColor(int index, GUI_COLOR color)
{
	TP_EDIT_Default.aBkColor[index] = color;
}

GUI_COLOR TP_EDIT_GetDefaultBkColor(int index)
{
	return TP_EDIT_Default.aBkColor[index];
}

void TP_EDIT_SetDefaultTextColor(int index, GUI_COLOR color)
{
	TP_EDIT_Default.aTextColor[index] = color;
}

GUI_COLOR TP_EDIT_GetDefaultTextColor(int index)
{
	return TP_EDIT_Default.aTextColor[index];
}

void TP_EDIT_SetDefaultCurosorWidth(int width)
{
	if (width > 0)
	{
		TP_EDIT_Default.cursorProps.cursorWidth = width;
	}
}

int TP_EDIT_GetDefaultCursorWidth()
{
	return TP_EDIT_Default.cursorProps.cursorWidth;
}

void TP_EDIT_SetDefaultFont(const GUI_FONT * pFont)
{
	TP_EDIT_Default.pFont = pFont;
}

const GUI_FONT * TP_EDIT_GetDefaultFont()
{
	return TP_EDIT_Default.pFont;
}

void TP_EDIT_SetBorderColor(TP_EDIT_Handle hWin, GUI_COLOR color)
{
	if (hWin)
	{
		TP_EDIT_Obj hObj;
		WM_GetUserData(hWin, &hObj, sizeof(TP_EDIT_Obj));
		hObj.borderColor = color;
		WM_SetUserData(hWin, &hObj, sizeof(TP_EDIT_Obj));
		WM_InvalidateWindow(hWin);
	}
}

void TP_EDIT_SetBkColor(TP_EDIT_Handle hWin, int index, GUI_COLOR color)
{
	if (hWin)
	{
		TP_EDIT_Obj hObj;
		WM_GetUserData(hWin, &hObj, sizeof(TP_EDIT_Obj));
		hObj.aBkColor[index] = color;
		WM_SetUserData(hWin, &hObj, sizeof(TP_EDIT_Obj));
		WM_InvalidateWindow(hWin);
	}
}

void TP_EDIT_SetText(TP_EDIT_Handle hWin, const char * sText)
{
	if (hWin)
	{
		TP_EDIT_Obj hObj;
		int len;
		WM_GetUserData(hWin, &hObj, sizeof(TP_EDIT_Obj));
		len = strlen(sText);
                if (len > TP_MAX_STR_LEN)
                {
                      len = TP_MAX_STR_LEN;
                }
		memset(hObj.aText, 0, strlen(hObj.aText));
		strncpy(hObj.aText, sText, len);
		WM_SetUserData(hWin, &hObj, sizeof(TP_EDIT_Obj));
		WM_InvalidateWindow(hWin);
	}
}

void TP_EDIT_GetText(TP_EDIT_Handle hWin, char * sDest, int maxLen)
{
	if (hWin)
	{
		TP_EDIT_Obj hObj;
		char * pText;
		int numChars;
		WM_GetUserData(hWin, &hObj, sizeof(TP_EDIT_Obj));
		pText = hObj.aText;
		numChars = GUI__GetNumChars(pText);
		if (numChars > maxLen)
		{
			numChars = maxLen;
		}
		strncpy(sDest, pText, numChars);
		sDest[numChars] = '\0';
	}
}

void TP_EDIT_SetTextColor(TP_EDIT_Handle hWin, int index, GUI_COLOR color)
{
	if (hWin)
	{
		TP_EDIT_Obj hObj;
		WM_GetUserData(hWin, &hObj, sizeof(TP_EDIT_Obj));
		hObj.aTextColor[index] = color;
		WM_SetUserData(hWin, &hObj, sizeof(TP_EDIT_Obj));
		WM_InvalidateWindow(hWin);
	}
}

void TP_EDIT_SetMaxInputNumChars(TP_EDIT_Handle hWin, int num)
{
	if (hWin)
	{
		TP_EDIT_Obj hObj;
		WM_GetUserData(hWin, &hObj, sizeof(TP_EDIT_Obj));
		hObj.maxInputCharNum = num;
		WM_SetUserData(hWin, &hObj, sizeof(TP_EDIT_Obj));
		WM_InvalidateWindow(hWin);
	}
}

int TP_EDIT_GetInputNumChars(TP_EDIT_Handle hWin)
{
	int numChars = 0;
	if (hWin)
	{
		TP_EDIT_Obj hObj;
		char * pText;
		WM_GetUserData(hWin, &hObj, sizeof(TP_EDIT_Obj));
		pText = hObj.aText;
		numChars = GUI__GetNumChars(pText);
		pText = NULL;
	}

	return numChars;
}

void TP_EDIT_SetFont(TP_EDIT_Handle hWin, const GUI_FONT * pFont)
{
	if (hWin)
	{
		TP_EDIT_Obj hObj;
		WM_GetUserData(hWin, &hObj, sizeof(TP_EDIT_Obj));
		hObj.pFont = pFont;
		WM_SetUserData(hWin, &hObj, sizeof(TP_EDIT_Obj));
		WM_InvalidateWindow(hWin);
	}
}

void TP_EDIT_SetFocussable(TP_EDIT_Handle hWin, int state)
{
	if (hWin)
	{
		TP_EDIT_Obj hObj;
		WM_GetUserData(hWin, &hObj, sizeof(TP_EDIT_Obj));
		hObj.isFocussable = state;
		WM_SetUserData(hWin, &hObj, sizeof(TP_EDIT_Obj));
		WM_InvalidateWindow(hWin);
	}
}

void TP_EDIT_SetCursorPosBySlider(TP_EDIT_Handle hWin, TP_EDIT_Obj * pObj, int pos)
{
	if (hWin)
	{
		//TP_EDIT_Obj hObj;
		//WM_GetUserData(hWin, &hObj, sizeof(TP_EDIT_Obj));
		if (strlen(pObj->aText) > 0)
		{
			int relativePos = 0;
			int absolutePos = 0;
			int numChars = 0;
			
			numChars = GUI__GetNumChars(pObj->aText);
			relativePos = GUI__GetCursorPosChar(pObj->aText + pObj->rightMoveTimes, pos, numChars - pObj->rightMoveTimes + 1);
			absolutePos = relativePos + pObj->rightMoveTimes;
			if (pObj->cursorPos > absolutePos)
			{
				int tmp = absolutePos;
				while (pObj->cursorPos > tmp)
				{
					WM_SendMessageNoPara(hWin, TP_MSG_EDIT_AUTO_LEFT);
					tmp++;
				}
			}
			else if (pObj->cursorPos < absolutePos)
			{
				int tmp = absolutePos;
				while (pObj->cursorPos < tmp)
				{
					WM_SendMessageNoPara(hWin, TP_MSG_EDIT_AUTO_RIGHT);
					tmp--;
				}
			}
			
			//TP_EDIT_SetCursorPos(pObj, absolutePos, CLICKOP);

			//WM_SetUserData(hWin, pObj, sizeof(TP_EDIT_Obj));
			//WM_InvalidateWindow(hWin);
		}
	}
}

void TP_EDIT_SetCursorAtPixelPos(TP_EDIT_Handle hWin, TP_EDIT_Obj * pObj, int pos)
{
	if (hWin)
	{
                int len = strlen(pObj->aText);
		if (len >= 0)
		{
			int textWidth;
			int numChars;
			int absolutePos = 0;
			int relativePos = 0;
			int _offset = 0;

			GUI_SetFont(pObj->pFont);
			textWidth = GUI_GetStringDistX(pObj->aText);
			numChars = GUI__GetNumChars(pObj->aText);

			if (pos < 0)
			{
				TP_EDIT_SetCursorPos(pObj, 0, CLICKOP);

				if (pObj->isFirstClick == 1)
				{
					_offset = 0;
				}
			}
			else if (pos > (textWidth + _textOffset))
			{
				if (numChars <= pObj->maxAvaliableSeenCharNum[pObj->isClick])
				{
					TP_EDIT_SetCursorPos(pObj, numChars, CLICKOP);
					
					if (pObj->isFirstClick == 1)
					{
						_offset = textWidth;
					}
					_offset = textWidth;
				}
				else
				{
					pObj->rightMoveTimes = numChars - pObj->maxAvaliableSeenCharNum[pObj->isClick];
					
					if (pObj->isFirstClick == 1)
					{
						absolutePos = numChars;
					}
					else
					{
						relativePos = GUI__GetCursorPosChar(pObj->aText + pObj->rightMoveTimes, pos, numChars - pObj->rightMoveTimes + 1);
						absolutePos = relativePos + pObj->rightMoveTimes;
						_offset = relativePos * GUI_GetCharDistX('A');
					}
					TP_EDIT_SetCursorPos(pObj, absolutePos, CLICKOP);
					if (pObj->isFirstClick == 1)
					{
						_offset = GUI_GetStringDistX(pObj->aText + pObj->rightMoveTimes);
					}
				}
			}
#if 0
			else if (pos == 0)
			{
				TP_EDIT_SetCursorPos(pObj, pObj->cursorPos - 1, MOVELEFTOP);
			}
#endif
			else
			{
				if (numChars <= pObj->maxAvaliableSeenCharNum[pObj->isClick])
				{
					absolutePos = GUI__GetCursorPosChar(pObj->aText, pos, numChars);
					_offset = absolutePos * GUI_GetCharDistX('A');
				}
				else
				{
					pObj->rightMoveTimes = numChars - pObj->maxAvaliableSeenCharNum[pObj->isClick];
					relativePos = GUI__GetCursorPosChar(pObj->aText + pObj->rightMoveTimes, pos, numChars - pObj->rightMoveTimes + 1);
					absolutePos = relativePos + pObj->rightMoveTimes;
					_offset = relativePos * GUI_GetCharDistX('A');
				}
				
				if (pObj->isFirstClick == 1)
				{
					absolutePos = numChars;
					_offset = GUI_GetStringDistX(pObj->aText + pObj->rightMoveTimes);
				}

				TP_EDIT_SetCursorPos(pObj, absolutePos, CLICKOP);
			}

			//if (pObj->isFirstClick == 1)
			{
				pObj->sliderOffset = _offset;
			}
		}

		WM_SetUserData(hWin, pObj, sizeof(TP_EDIT_Obj));
		WM_InvalidateWindow(hWin);
	}
}

void TP_EDIT_SetCursorPos(TP_EDIT_Obj * pObj, int pos, CURSOR_OP op)
{
	int numChars;
	int maxAvaliableSeenCharNum;

	numChars = GUI__GetNumChars(pObj->aText);

	maxAvaliableSeenCharNum = pObj->maxAvaliableSeenCharNum[pObj->isClick];

	if (pos < 0)
	{
		pos = 0;
		return;
	}

	if (pos > numChars)
	{
		pos = numChars;
		return;
	}

	if (op == ADDOP)
	{
		if (pos > maxAvaliableSeenCharNum)
		{
			pObj->rightMoveTimes = numChars - maxAvaliableSeenCharNum;
		}
	}

	if (op == DELOP)
	{
		if (pObj->rightMoveTimes > 0)
		{
			pObj->rightMoveTimes--;
		}
	}

	if (op == MOVERIGHTOP)
	{
		if (pos > (pObj->rightMoveTimes + maxAvaliableSeenCharNum))
		{
			pObj->rightMoveTimes++;
		}
	}

	if (op == MOVELEFTOP)
	{
		if (pos < pObj->rightMoveTimes)
		{
			if (pObj->rightMoveTimes > 0)
			{
				pObj->rightMoveTimes--;
			}
		}
	}

	if (pObj->cursorPos != pos)
	{
		pObj->cursorPos = pos;
	}
}

int TP_EDIT_IsInsertAvaliable(TP_EDIT_Obj * pObj)
{
	int numChars;
	if (strlen(pObj->aText) > 0)
	{
		numChars = GUI__GetNumChars(pObj->aText);
		if ((numChars + 1) > pObj->maxInputCharNum)
		{
			return 0;
		}
	}
	return 1;
}

int TP_EDIT_InsertChar(TP_EDIT_Handle hWin, TP_EDIT_Obj * pObj, U16 key)
{
	if (TP_EDIT_IsInsertAvaliable(pObj))
	{
		int curPos = 0;
		int tmpPos = 0;
		int numChars = 0;
		
		numChars = GUI__GetNumChars(pObj->aText);
		curPos = pObj->cursorPos;
		if (curPos == 0)
		{
			tmpPos = numChars;
			while (tmpPos)
			{
				pObj->aText[tmpPos] = pObj->aText[tmpPos - 1];
				tmpPos--;
			}
			pObj->aText[tmpPos] = key;
		}
		else if (curPos == numChars)
		{
			pObj->aText[numChars] = key;
			pObj->aText[numChars + 1] = '\0';
		}
		else
		{
			tmpPos = numChars;
			while (tmpPos > curPos)
			{
				pObj->aText[tmpPos] = pObj->aText[tmpPos - 1];
				tmpPos--;
			}
			pObj->aText[curPos] = key;
		}
		pObj->aText[numChars + 1] = '\0';
		
		WM_NotifyParent(hWin, WM_NOTIFICATION_VALUE_CHANGED);

		return 1;
	}
	return 0;
}

int TP_EDIT_SetUserData(TP_EDIT_Handle hWin, void * pSrc, int size)
{
	TP_EDIT_Obj hObj;
	U8 * pExtraBytes;
	int numBytes;

	WM_LOCK();
	if (size <= 0)
	{
		return 0;
	}
	WM_GetUserData(hWin, &hObj, sizeof(TP_EDIT_Obj));

	pExtraBytes = (U8 *)malloc(sizeof(TP_EDIT_Obj) + hObj.numExtraBytes);
	if (pExtraBytes)
	{
		WM_GetUserData(hWin, pExtraBytes, sizeof(TP_EDIT_Obj) + hObj.numExtraBytes);
		if (size >= hObj.numExtraBytes)
		{
			numBytes = hObj.numExtraBytes;
		}
		else
		{
			numBytes = size;
		}
		GUI_MEMCPY(pExtraBytes + sizeof(TP_EDIT_Obj), pSrc, numBytes);
		WM_SetUserData(hWin, pExtraBytes, sizeof(TP_EDIT_Obj) + hObj.numExtraBytes);
		free(pExtraBytes);
		return numBytes;
	}
	return 0;
	WM_UNLOCK();
}

int TP_EDIT_GetUserData(TP_EDIT_Handle hWin, void * pDest, int size)
{
	TP_EDIT_Obj hObj;
	U8 * pExtraBytes;
	int numBytes;

	WM_LOCK();
	if (size <= 0)
	{
		return 0;
	}
	WM_GetUserData(hWin, &hObj, sizeof(TP_EDIT_Obj));
	pExtraBytes = (U8 *)malloc(sizeof(TP_EDIT_Obj) + hObj.numExtraBytes);
	if (pExtraBytes)
	{
		WM_GetUserData(hWin, pExtraBytes, sizeof(TP_EDIT_Obj) + hObj.numExtraBytes);
		if (size >= hObj.numExtraBytes)
		{
			numBytes = hObj.numExtraBytes;
		}
		else
		{
			numBytes = size;
		}
		GUI_MEMCPY(pDest, pExtraBytes + sizeof(TP_EDIT_Obj), numBytes);
		free(pExtraBytes);
		return numBytes;
	}
	return 0;
	WM_UNLOCK();
}

void TP_EDIT_DeleteChar(TP_EDIT_Obj * pObj)
{
	int len = strlen(pObj->aText);
	int cursorPos = pObj->cursorPos;
	if (cursorPos == 0)
	{
		return;
	}

	if (len > 0)
	{
		while (cursorPos <= (len - 1))
		{
			int indexPos;

			indexPos = cursorPos - 1;

			pObj->aText[indexPos] = pObj->aText[indexPos + 1];
			cursorPos++;

		}
		pObj->aText[len - 1] = '\0';
	}
}

void TP_EDIT_EnableBlink(TP_EDIT_Handle hWin, int period, int Onff)
{
	if (hWin)
	{
		TP_EDIT_Obj hObj;
		WM_GetUserData(hWin, &hObj, sizeof(TP_EDIT_Obj));
		hObj.cursorProps.isBlink = Onff;
		hObj.cursorProps.blinkPeriod = period;
		WM_SetUserData(hWin, &hObj, sizeof(TP_EDIT_Obj));
		WM_InvalidateWindow(hWin);
	}
}

void TP_EDIT_SetCursorColor(TP_EDIT_Handle hWin, GUI_COLOR color)
{
	if (hWin)
	{
		TP_EDIT_Obj hObj;
		WM_GetUserData(hWin, &hObj, sizeof(TP_EDIT_Obj));
		hObj.cursorProps.cursorColor = color;
		WM_SetUserData(hWin, &hObj, sizeof(TP_EDIT_Obj));
		WM_InvalidateWindow(hWin);
	}
}

void TP_EDIT_ClearText(TP_EDIT_Handle hWin)
{
	if (hWin)
	{
		TP_EDIT_Obj hObj;
		WM_GetUserData(hWin, &hObj, sizeof(TP_EDIT_Obj));
		memset(hObj.aText, 0, strlen(hObj.aText));
		hObj.cursorPos = 0;
		hObj.rightMoveTimes = 0;
		WM_SetUserData(hWin, &hObj, sizeof(TP_EDIT_Obj));
		WM_NotifyParent(hWin, WM_NOTIFICATION_VALUE_CHANGED);
		WM_InvalidateWindow(hWin);
	}
}

void TP_EDIT_EraseText(TP_EDIT_Handle hWin, TP_EDIT_Obj * pObj)
{
	if (hWin)
	{
		memset(pObj->aText, 0, strlen(pObj->aText));
		pObj->cursorPos = 0;
		pObj->rightMoveTimes = 0;
		WM_SetUserData(hWin, pObj, sizeof(TP_EDIT_Obj));
		WM_NotifyParent(hWin, WM_NOTIFICATION_VALUE_CHANGED);
		WM_InvalidateWindow(hWin);
	}
}

int TP_EDIT_IsTouchClearIcon(TP_EDIT_Obj * pObj, GUI_RECT rect, int xPos)
{
	int circleStartXPos = (rect.x1 - rect.x0) - (pObj->aBorderSize[TP_BORDER_RIGHT] + pObj->pDelBitmap->XSize);
	int re = 0;
	if (xPos >= circleStartXPos)
	{
		re = 1;
	}

	return re;
}

void TP_EDIT_OnPaint(TP_EDIT_Handle hWin, TP_EDIT_Obj * pObj)
{
	int cursorWidth;
	int numChars;
	GUI_RECT rClient;
	GUI_RECT rInside;
	GUI_RECT rInvert;
	char pText[TP_MAX_STR_LEN];
	int bkColorIndex;
	int textColorIndex;
	int fontHeight;
        int len = 0;

	memset(pText, 0, sizeof(pText));

	if (WM_IsEnabled(hWin))
	{
		textColorIndex = TP_EDIT_FONT_ENABLED;
		if (WM_HasFocus(hWin))
		{
			bkColorIndex = TP_EDIT_BK_FOCUS;
		}
		else
		{
			bkColorIndex = TP_EDIT_BK_NORMAL;
		}

		if (pObj->cursorProps.isBlink == TP_EDIT_CURSOR_BLINK_ON)
		{
			pObj->cursorProps.blinkTimer = WM_CreateTimer(hWin, 0, pObj->cursorProps.blinkPeriod, 0);
		}
		else
		{
			if (pObj->cursorProps.blinkTimer != 0)
			{
				WM_DeleteTimer(pObj->cursorProps.blinkTimer);
			}
		}
	}
	else
	{
		textColorIndex = TP_EDIT_FONT_DISABLED;
		bkColorIndex = TP_EDIT_BK_DISABLED;
	}
	GUI_GetClientRect(&rClient);
	rInside = rClient;
	GUI_SetPenSize(1);
	GUI_SetColor(pObj->borderColor);
	GUI_DrawRectEx(&rClient);

	rInside.x0 += 1;
	rInside.y0 += 1;
	rInside.x1 -= 1;
	rInside.y1 -= 1;
	WM_SetUserClipRect(&rInside);
	GUI_SetBkColor(pObj->aBkColor[bkColorIndex]);
	GUI_Clear();

	GUI_SetColor(pObj->aTextColor[textColorIndex]);
	GUI_SetFont(pObj->pFont);
	numChars = GUI__GetNumChars(pObj->aText);
	fontHeight = GUI_GetFontSizeY();

	cursorWidth = pObj->cursorProps.cursorWidth;
	rInvert = rInside;

	len = strlen(pObj->aText);
	if (len >= 0)
	{
		int _off_ = GUI__GetCursorPosX(pObj->aText + pObj->rightMoveTimes, pObj->cursorPos - pObj->rightMoveTimes, pObj->maxAvaliableSeenCharNum[pObj->isClick]);
		rInvert.x1 = rInvert.x0 + _off_;
		rInvert.y0 += (((rInvert.y1 - rInvert.y0) - fontHeight) / 2);
		rInvert.y1 -= (((rInvert.y1 - rInvert.y0) - fontHeight) / 2);
		if (pObj->isClick == 1)
		{
			strncpy(pText, pObj->aText + pObj->rightMoveTimes, pObj->maxAvaliableSeenCharNum[pObj->isClick]);
			pText[pObj->maxAvaliableSeenCharNum[pObj->isClick]] = '\0';
		}
		else
		{
			if (numChars > pObj->maxAvaliableSeenCharNum[pObj->isClick])
			{
#if 0
				strncpy(pText, pObj->aText + numChars - pObj->maxAvaliableSeenCharNum[pObj->isClick], pObj->maxAvaliableSeenCharNum[pObj->isClick]);
#endif
				strncpy(pText, pObj->aText, pObj->maxAvaliableSeenCharNum[pObj->isClick] - 3);
				strncpy(pText + pObj->maxAvaliableSeenCharNum[pObj->isClick] - 3, "...", 3);
				pText[pObj->maxAvaliableSeenCharNum[pObj->isClick]] = '\0';
			}
			else
			{
				strncpy(pText, pObj->aText, numChars);
				pText[numChars] = '\0';
			}
		}
		
		GUI_SetColor(pObj->aTextColor[0]);
		//GUI_DispStringAt(pText, rClient.x0, rInvert.y0);
		//GUI_DispStringAt(pText, rInside.x0 + 2, rInvert.y0);
		GUI_DispStringAt(pText, rClient.x0 + _textOffset, rInvert.y0);


		if (pObj->isClick == 1)
		{
			if (pObj->cursorProps.isBlink == TP_EDIT_CURSOR_BLINK_OFF)
			{
				GUI_SetColor(pObj->cursorProps.cursorColor);
				GUI_FillRect(rInvert.x1, rInvert.y0, rInvert.x1 + cursorWidth - 1, rInvert.y1);
			}
			else if (pObj->cursorProps.isBlink == TP_EDIT_CURSOR_BLINK_ON)
			{
				if (pObj->blinkFlag == TP_EDIT_CURSOR_BLINK_FLAG_ON)
				{
					GUI_SetColor(pObj->cursorProps.cursorColor);
					GUI_FillRect(rInvert.x1 + 1, rInvert.y0, rInvert.x1 + 1 + cursorWidth - 1, rInvert.y1);
				}
			}
		}

		if (strlen(pObj->aText))
		{
			int x0 = rClient.x1 - pObj->pDelBitmap->XSize - pObj->aBorderSize[TP_BORDER_RIGHT];
			int y0 = rClient.y0 + pObj->aBorderSize[TP_BORDER_TOP];
			GUI_DrawBitmap(pObj->pDelBitmap, x0, y0);
		}

#if 0
		//if (strlen(pObj->aText))
		if (pObj->isClick == 1)
		{
			int circleCenterPosX;
			int circleCenterPosY;
			int circleRadius;
			int border = 5;
			int x0;
			int y0;
			int x1;
			int y1;

			circleRadius = (rClient.y1 - rClient.y0 - (border * 2)) / 2;
			circleCenterPosX = (rClient.x1 - border) - circleRadius;
			circleCenterPosY = border + circleRadius;

			GUI_SetColor(GUI_GRAY);
			GUI_DrawCircle(circleCenterPosX, circleCenterPosY, circleRadius);
			GUI_SetColor(GUI_GRAY);
			GUI_FillCircle(circleCenterPosX, circleCenterPosY, circleRadius);

			GUI_SetColor(GUI_WHITE);
			x0 = circleCenterPosX - (circleRadius / 2);
			y0 = circleCenterPosY - (circleRadius / 2);
			x1 = circleCenterPosX + (circleRadius / 2);
			y1 = circleCenterPosY + (circleRadius / 2);

			GUI_DrawLine(x0, y0, x1, y1);

			y0 = circleCenterPosY + (circleRadius / 2);
			y1 = circleCenterPosY - (circleRadius / 2);

			GUI_DrawLine(x0, y0, x1, y1); 
		}
#endif
	}
	
	WM_SetUserClipRect(NULL);
}

void TP_EDIT_Callback(WM_MESSAGE * pMsg)
{
	WM_HWIN hWin;
	TP_EDIT_Obj hObj;
	WM_MESSAGE msgToSlider;
	WM_KEY_INFO * pKeyInfo;
	GUI_PID_STATE * pState;
	GUI_RECT rClient;

	hWin = pMsg->hWin;
	WM_GetUserData(hWin, &hObj, sizeof(TP_EDIT_Obj));
	WM_GetWindowRectEx(hWin, &rClient);

	switch (pMsg->MsgId)
	{
	case WM_PAINT:
		TP_EDIT_OnPaint(hWin, &hObj);
		break;
	
	case WM_TOUCH:
		pState = (GUI_PID_STATE *)(pMsg->Data.p);
		if (pState)
		{
			if (pState->Pressed)
			{
				WM_SetFocus(hWin);
				hObj.isClick = 1;
				if (TP_EDIT_IsTouchClearIcon(&hObj, rClient, pState->x))
				{
					TP_EDIT_EraseText(hWin, &hObj);
					WM_HideWindow(hObj.hSliderWin);
				}
				else
				{
					TP_EDIT_SetCursorAtPixelPos(hWin, &hObj, pState->x);

					if (hObj.isFirstClick == 1)
					{
						msgToSlider.MsgId = TP_MSG_EDIT_TO_SLIDER_FIRST;
						msgToSlider.Data.v = hObj.sliderOffset;
						hObj.sliderOffset = 0;
						hObj.isFirstClick = 0;
					}
					else
					{
						msgToSlider.MsgId = TP_MSG_EDIT_TO_SLIDER;
						msgToSlider.Data.v = hObj.sliderOffset;
					}
					#if 1
					if (strlen(hObj.aText) > 0)
					{
						WM_SendMessage(hObj.hSliderWin, &msgToSlider);
						WM_ShowWindow(hObj.hSliderWin);
					}
					#endif
				}
				WM_SetUserData(hWin, &hObj, sizeof(TP_EDIT_Obj));
				WM_InvalidateWindow(hWin);
				WM_NotifyParent(hWin, WM_NOTIFICATION_CLICKED);
			}
			else
			{
				//hObj.isClick = 0;
				WM_SetUserData(hWin, &hObj, sizeof(TP_EDIT_Obj));
				WM_InvalidateWindow(hWin);
				WM_NotifyParent(hWin, WM_NOTIFICATION_RELEASED);
			}
		}
		else
		{
			hObj.isClick = 0;
			hObj.isFirstClick = 1;
			hObj.sliderOffset = 0;
			WM_SetUserData(hWin, &hObj, sizeof(TP_EDIT_Obj));
			WM_InvalidateWindow(hWin);
			WM_NotifyParent(hWin, WM_NOTIFICATION_MOVED_OUT);
		}
		break;

	case WM_DELETE:
		if (hObj.cursorProps.blinkTimer != 0)
		{
			WM_DeleteTimer(hObj.cursorProps.blinkTimer);
		}
		break;

	case WM_SET_FOCUS:
		if (pMsg->Data.v && (hObj.isFocussable))
		{
			pMsg->Data.v = 0;
		}
		WM_InvalidateWindow(hWin);

	case WM_SET_ID:
		hObj.id = pMsg->Data.v;
		WM_SetUserData(hWin, &hObj, sizeof(TP_EDIT_Obj));
		WM_InvalidateWindow(hWin);
		break;

	case WM_GET_ID:
		pMsg->Data.v = hObj.id;
		break;
		
	case WM_TIMER:
		if (hObj.blinkFlag == TP_EDIT_CURSOR_BLINK_FLAG_ON)
		{
			hObj.blinkFlag = TP_EDIT_CURSOR_BLINK_FLAG_OFF;
		}
		else
		{
			hObj.blinkFlag = TP_EDIT_CURSOR_BLINK_FLAG_ON;
		}
		WM_SetUserData(hWin, &hObj, sizeof(TP_EDIT_Obj));
		WM_InvalidateWindow(hWin);
		break;

	case TP_MSG_SLIDER_TO_EDIT:
		TP_EDIT_SetCursorPosBySlider(hWin, &hObj, pMsg->Data.v);
		break;

	case TP_MSG_EDIT_AUTO_LEFT:
		TP_EDIT_SetCursorPos(&hObj, hObj.cursorPos - 1, MOVELEFTOP);
		WM_SetUserData(hWin, &hObj, sizeof(TP_EDIT_Obj));
		WM_InvalidateWindow(hWin);
		break;

	case TP_MSG_EDIT_AUTO_RIGHT:
		TP_EDIT_SetCursorPos(&hObj, hObj.cursorPos + 1, MOVERIGHTOP);
		WM_SetUserData(hWin, &hObj, sizeof(TP_EDIT_Obj));
		WM_InvalidateWindow(hWin);
		break;

	case WM_KEY:
		pKeyInfo = (WM_KEY_INFO *)(pMsg->Data.p);
		if (pKeyInfo->PressedCnt > 0)
		{
			int key = pKeyInfo->Key;
			switch (key)
			{
			case GUI_KEY_ENTER:
			//case GUI_KEY_SPACE:
			case GUI_KEY_TAB:
			//case GUI_KEY_BACKTAB:
				break;

			case GUI_KEY_RIGHT:
				TP_EDIT_SetCursorPos(&hObj, hObj.cursorPos + 1, MOVERIGHTOP);
				WM_SetUserData(hWin, &hObj, sizeof(TP_EDIT_Obj));
				WM_InvalidateWindow(hWin);
				break;

			case GUI_KEY_LEFT:
				TP_EDIT_SetCursorPos(&hObj, hObj.cursorPos - 1, MOVELEFTOP);
				WM_SetUserData(hWin, &hObj, sizeof(TP_EDIT_Obj));
				WM_InvalidateWindow(hWin);
				break;

			case GUI_KEY_BACKSPACE:
				TP_EDIT_DeleteChar(&hObj);
				TP_EDIT_SetCursorPos(&hObj, hObj.cursorPos - 1, DELOP);
				WM_SetUserData(hWin, &hObj, sizeof(TP_EDIT_Obj));
				WM_NotifyParent(hWin, WM_NOTIFICATION_VALUE_CHANGED);
				WM_InvalidateWindow(hWin);
				break;

			case GUI_KEY_DELETE:
				break;

			default:
				if (key >= 0x20)
				{
					if (TP_EDIT_InsertChar(hWin, &hObj, key)) {
						TP_EDIT_SetCursorPos(&hObj, hObj.cursorPos + 1, ADDOP);
					}
				}
				WM_SetUserData(hWin, &hObj, sizeof(TP_EDIT_Obj));
				WM_InvalidateWindow(hWin);
				break;
			}
		}
		break;

	default:
		WM_DefaultProc(pMsg);
	}
}

void TP_EDIT_SLIDER_SetPos(WM_HWIN hWin, TP_EDIT_SLIDER_Obj * hObj, int offset)
{
	if ((hObj->orgXPos + offset) <= hObj->sliderMaxX)
	{
		WM_MoveTo(hWin, hObj->orgXPos + offset, hObj->orgYPos);
		WM_InvalidateWindow(hWin);
	}
	else if ((hObj->orgXPos + offset) > hObj->sliderMaxX)
	{
		WM_MoveTo(hWin, hObj->sliderMaxX, hObj->orgYPos);
		WM_InvalidateWindow(hWin);
	}
}

void TP_EDIT_SLIDER_Move(WM_HWIN hWin, TP_EDIT_SLIDER_Obj * hObj, int xPos)
{
	WM_MESSAGE msgToEdit;
	int offset;
	int maxXPos = 0;
	TP_EDIT_Obj hEditObj;
	int wrapSize = 0;

	WM_GetUserData(hObj->hEditWin, &hEditObj, sizeof(TP_EDIT_Obj));
	GUI_SetFont(hEditObj.pFont);
	if (strlen(hEditObj.aText) > 0)
	{
		char aTmp[TP_MAX_STR_LEN];
		memset(aTmp, 0, TP_MAX_STR_LEN);
		strncpy(aTmp, hEditObj.aText + hEditObj.rightMoveTimes, hEditObj.maxAvaliableSeenCharNum[1]);
		maxXPos = hObj->orgXPos + GUI_GetStringDistX(aTmp);
	}
	offset = xPos - hObj->orgXPos;

	if (maxXPos > 0)
	{
		hObj->sliderMaxX = maxXPos;
	}
	
	if ((xPos > (hObj->sliderMaxX + wrapSize)) || (xPos < hObj->sliderMinX))
	//if ((xPos > (hObj->sliderMaxX + 0)) || (xPos < hObj->sliderMinX))
	{
		return;
	}

	msgToEdit.MsgId = TP_MSG_SLIDER_TO_EDIT;
	msgToEdit.Data.v = offset;
	WM_SendMessage(hObj->hEditWin, &msgToEdit);
	WM_SetUserData(hWin, hObj, sizeof(TP_EDIT_SLIDER_Obj));
	WM_MoveTo(hWin, xPos, hObj->orgYPos);
}

void TP_EDIT_SLIDER_Callback(WM_MESSAGE * pMsg)
{
	TP_EDIT_SLIDER_Obj hObj;
	WM_HWIN hWin;
	GUI_PID_STATE * pState;
	WM_KEY_INFO * pKeyInfo;
	int keyOp;
	GUI_RECT rClient;
	int curXPos = 0;
	int charWidth = 0;
	static int s_moveFlag = -1;

	hWin = pMsg->hWin;
	WM_GetUserData(hWin, &hObj, sizeof(TP_EDIT_SLIDER_Obj));
	WM_GetWindowRectEx(hWin, &rClient);
	charWidth = hObj.charWidth;
	switch (pMsg->MsgId)
	{
	case WM_PAINT:
		GUI_SetBkColor(TP_EDIT_BK_NORMAL_COLOR);
		GUI_Clear();
		if (hObj.pSoliderBitmap)
		{
			GUI_DrawBitmap(hObj.pSoliderBitmap, 0, 0);
		}
		break;

	case WM_TOUCH:
		pState = (GUI_PID_STATE *)(pMsg->Data.p);
		if (pState)
		{
			if (pState->Pressed)
			{
				int xPos;
				xPos = pState->x;
				hObj.isClick = 1;
				curXPos = WM_GetWindowOrgX(hWin);
				s_moveFlag = -1;

				{
					if ((curXPos <= hObj.orgXPos) || (curXPos >= hObj.sliderMaxX))
					{
						if (curXPos <= hObj.orgXPos)
						{
							s_moveFlag = 1;
						}
						else
						{
							s_moveFlag = 2;
						}
						if (hObj.hMoveTimer)
						{
							WM_RestartTimer(hObj.hMoveTimer, 2000);
						}
						else
						{
							hObj.hMoveTimer = WM_CreateTimer(hWin, TP_TIMER_MOVE, 2000, 0);
						}
					}
				}
				
				if (1)
				{
					WM_SetCapture(hWin, 1);
				}
				else
				{
					WM_SetCapture(hWin, 0);
				}
				
				if ((rClient.x0 + xPos) > 0)
				{
				#if 1
					int _offset_x_s = (rClient.x0 + xPos) - hObj.orgXPos;
					if ((_offset_x_s < charWidth) && (_offset_x_s > 0))
					{}
					else if (((_offset_x_s % charWidth) == 0) || (_offset_x_s == 0))
				#endif
					{
						TP_EDIT_SLIDER_Move(hWin, &hObj, rClient.x0 + xPos);
					}
				}
			}
			else
			{
				hObj.isClick = 0;
				s_moveFlag = -1;
			}
		}
		WM_SetUserData(hWin, &hObj, sizeof(TP_EDIT_SLIDER_Obj));
		WM_InvalidateWindow(hWin);
		break;

	case WM_TIMER:
		if (hObj.isClick == 0)
		{
			WM_HideWindow(hWin);
		}
		else
		{	
			if (s_moveFlag == 1)
			{
				WM_MESSAGE _toEdit;
				_toEdit.MsgId = TP_MSG_EDIT_AUTO_LEFT;
				WM_SendMessage(hObj.hEditWin, &_toEdit);
				hObj.hMoveTimer = WM_CreateTimer(hWin, TP_TIMER_MOVE, 2000, 0);
			}
			else if (s_moveFlag == 2)
			{
				WM_MESSAGE _toEdit;
				_toEdit.MsgId = TP_MSG_EDIT_AUTO_RIGHT;
				WM_SendMessage(hObj.hEditWin, &_toEdit);
				hObj.hMoveTimer = WM_CreateTimer(hWin, TP_TIMER_MOVE, 2000, 0);
			}
		}

		WM_SetUserData(hWin, &hObj, sizeof(TP_EDIT_SLIDER_Obj));
		WM_InvalidateWindow(hWin);
		break;

	case TP_MSG_EDIT_TO_SLIDER_FIRST:
		if (hObj.hShowTimer)
		{
			WM_RestartTimer(hObj.hShowTimer, 3000);
		}
		else
		{
			hObj.hShowTimer = WM_CreateTimer(hWin, TP_TIMER_SHOW, 3000, 0);
		}
		if (hObj.sliderMaxX > (hObj.sliderMinX + pMsg->Data.v))
		{
			hObj.sliderMaxX = hObj.sliderMinX + pMsg->Data.v;
		}
		TP_EDIT_SLIDER_SetPos(hWin, &hObj, pMsg->Data.v);
		WM_SetUserData(hWin, &hObj, sizeof(TP_EDIT_SLIDER_Obj));
		WM_InvalidateWindow(hWin);
		break;
		
	case TP_MSG_EDIT_TO_SLIDER:
		if (hObj.hShowTimer)
		{
			WM_RestartTimer(hObj.hShowTimer, 3000);
		}
		else
		{
			hObj.hShowTimer = WM_CreateTimer(hWin, TP_TIMER_SHOW, 3000, 0);
		}
		TP_EDIT_SLIDER_SetPos(hWin, &hObj, pMsg->Data.v);
		WM_SetUserData(hWin, &hObj, sizeof(TP_EDIT_SLIDER_Obj));
		WM_InvalidateWindow(hWin);
		break;

		
	case WM_KEY:
		pKeyInfo = (WM_KEY_INFO *)(pMsg->Data.p);
		keyOp = pKeyInfo->Key;
		if (pKeyInfo->PressedCnt > 0)
		{
			switch (keyOp)
			{
			case GUI_KEY_RIGHT:
				break;

			case GUI_KEY_LEFT:
				break;
			}
		}
		break;

	default:
		WM_DefaultProc(pMsg);
	}
}

TP_EDIT_Handle TP_EDIT_Create(int x0, int y0, int xSize, int ySize, WM_HWIN hParent, U8 style, WM_CALLBACK * cb, int xSizeSlider, int ySizeSlider, int numExtraBytes, int id)
{
	TP_EDIT_Handle hWin;
	TP_EDIT_Obj hObj;
	TP_EDIT_SLIDER_Obj hObjSlider;
	WM_CALLBACK * pUsed;
	GUI_RECT rClient;

	hObj = TP_EDIT_Default;
	hObjSlider = TP_EDIT_SLIDER_Default;
	hObj.id = id;
	hObj.numExtraBytes = numExtraBytes;

	if (cb)
	{
		pUsed = cb;
	}
	else
	{
		pUsed = TP_EDIT_Callback;
	}

	hWin = WM_CreateWindowAsChild(x0, y0, xSize, ySize, hParent, style | WM_CF_MEMDEV, pUsed, sizeof(TP_EDIT_Obj) + numExtraBytes);
	WM_GetWindowRectEx(hWin, &rClient);
	if (hWin)
	{
		if ((xSizeSlider != 0) && (ySizeSlider != 0))
		{
			hObj.sliderProps.sliderXSize = xSizeSlider;
			hObj.sliderProps.sliderYSize = ySizeSlider;
		}
		else if (hObjSlider.pSoliderBitmap)
		{
			hObj.sliderProps.sliderXSize = hObjSlider.pSoliderBitmap->XSize;
			hObj.sliderProps.sliderYSize = hObjSlider.pSoliderBitmap->YSize;
		}
		x0 = x0 - hObj.sliderProps.sliderXSize / 2;
		hObj.hSliderWin = WM_CreateWindowAsChild(x0 + _textOffset , y0 + ySize - 3, hObj.sliderProps.sliderXSize, hObj.sliderProps.sliderYSize, hParent, WM_CF_HIDE | WM_CF_MEMDEV | WM_CF_STAYONTOP, TP_EDIT_SLIDER_Callback, sizeof(TP_EDIT_SLIDER_Obj));
		WM_GetWindowRectEx(hObj.hSliderWin , &rClient);
		GUI_SetFont(hObj.pFont);
		hObjSlider.charWidth = GUI_GetCharDistX('A');
		hObjSlider.orgXPos = rClient.x0;
		hObjSlider.orgYPos = rClient.y0;
		hObjSlider.sliderMaxX = rClient.x0 + (hObj.maxAvaliableSeenCharNum[1] * GUI_GetCharDistX('W'));;
		hObjSlider.sliderMinX = rClient.x0;
		hObjSlider.hEditWin = hWin;

		WM_SetUserData(hWin, &hObj, sizeof(TP_EDIT_Obj));
		if (hObj.hSliderWin)
		{
			WM_SetUserData(hObj.hSliderWin, &hObjSlider, sizeof(TP_EDIT_SLIDER_Obj));
		}
	}

	return hWin;
}

