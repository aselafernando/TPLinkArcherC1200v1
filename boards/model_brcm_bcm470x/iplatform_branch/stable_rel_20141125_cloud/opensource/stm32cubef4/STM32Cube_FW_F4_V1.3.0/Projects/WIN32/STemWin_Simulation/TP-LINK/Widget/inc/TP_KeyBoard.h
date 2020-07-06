#ifndef __TP_KEYBOARD_H__
#define __TP_KEYBOARD_H__

#include "GUI.h"
#include "WM.h"

typedef WM_HMEM TP_KEYBOARD_Handle;

#define TP_KEYBOARD_SHOW_LOWER_LETTER		(0)

typedef enum
{
	KEY_PAD_MIN_INDEX = 0,
	KEY_PAD_LOWER_INDEX = 0,
	KEY_PAD_UPPER_INDEX,
	KEY_PAD_NUM_INDEX,
	KEY_PAD_SPECHAR_INDEX,
	KEY_PAD_MAX_INDEX
}KEY_PAD_INDEX;

typedef enum
{
	TP_KB_BMP_MIN_INDEX = 0,
	TP_KB_BMP_NORMAL_INDEX	=	0,
	TP_KB_BMP_CLICKED_INDEX, 
	TP_KB_BMP_MAX_INDEX
}TP_KB_BMP_INDEX;

typedef struct
{
	int xGap;/* 描述每个键盘中x方向的gap */
	int yGap;/* 描述每个键盘中y方向的gap */
}TP_KB_KEY_GAP;

typedef struct
{
	int xSize;
	int ySize;
}KEY_SIZE;

typedef struct
{
	int          xPos;
	int          yPos;
	const CHAR*  pLabel;
	KEY_SIZE		(*pfGetKeySize)();  
	const GUI_BITMAP* (*pfGetBitmap)(int index);
	const GUI_FONT* (*pfGetFont)();
	void (*pfWmCallback)(TP_KEYBOARD_Handle hObj);
} KEY_DATA;

#define TP_KB_MAX_KEY_LINE	(4)

typedef struct
{
	int			lineNum;/* 有多少行按钮 */
	TP_KB_KEY_GAP		keyGap[KEY_PAD_MAX_INDEX];
	GUI_COLOR	bgColor;
	GUI_COLOR	color;/* 文本的颜色 */
	const GUI_FONT *pLetterFont;
	const GUI_FONT *pNum123Font;
	const GUI_FONT *pSpaceFont;
	const GUI_FONT *pDoneFont;
	const GUI_BITMAP *pBgBmp;
	const GUI_BITMAP *pDelBmp[TP_KB_BMP_MAX_INDEX];
	const GUI_BITMAP *pDoneBmp[TP_KB_BMP_MAX_INDEX];
	const GUI_BITMAP *pLetterBmp[TP_KB_BMP_MAX_INDEX];
	const GUI_BITMAP *pNumBmp[TP_KB_BMP_MAX_INDEX];
	const GUI_BITMAP *pNum123Bmp[TP_KB_BMP_MAX_INDEX];
	const GUI_BITMAP *pShiftBmp[TP_KB_BMP_MAX_INDEX];
	const GUI_BITMAP *pSpaceBmp[TP_KB_BMP_MAX_INDEX];
	const GUI_BITMAP *pSpecharBmp[TP_KB_BMP_MAX_INDEX];
	const GUI_BITMAP *pCharGuideBmp[TP_KB_BMP_MAX_INDEX];
} TP_KEYBOARD_Obj;

TP_KEYBOARD_Handle TP_KeyBoard_Create(WM_HWIN hParent);
void TP_KeyBoard_Show();
void TP_KeyBoard_Hide();

const GUI_FONT* TP_KeyBoard_GetDoneKeyFont();

void TP_KeyBoard_SetDoneKeyFont(const GUI_FONT* pFont);

const GUI_FONT* TP_KeyBoard_GetLetterKeyFont();


void TP_KeyBoard_SetLetterKeyFont(const GUI_FONT* pFont);

const GUI_FONT* TP_KeyBoard_GetSpaceKeyFont();


void TP_KeyBoard_SetSpaceKeyFont(const GUI_FONT* pFont);

const GUI_FONT* TP_KeyBoard_GetNum123KeyFont();

void TP_KeyBoard_SetNum123KeyFont(const GUI_FONT* pFont);


#endif/* __TP_KEYBOARD_H__ */

