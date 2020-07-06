#include <string.h> // Definition of NULL
#include <stdlib.h> // malloc() and free()

#include "TP_Global.h"
#include "TP_KeyBoard.h"
#include "TP_Button.h"

#if TP_KEYBOARD_SHOW_LOWER_LETTER
static TP_KEYBOARD_Handle lowerPadHandle;
#endif

static TP_KEYBOARD_Handle upperPadHandle;
static TP_KEYBOARD_Handle numPadHandle;
static TP_KEYBOARD_Handle charPadHandle;
static TP_KEYBOARD_Handle keyBoardHandle;

extern GUI_CONST_STORAGE GUI_BITMAP bmKB_123Bmp;
extern GUI_CONST_STORAGE GUI_BITMAP bmKB_123PressedBmp;

extern GUI_CONST_STORAGE GUI_BITMAP bmKB_BgBmp;

extern GUI_CONST_STORAGE GUI_BITMAP bmKB_DelBmp;
extern GUI_CONST_STORAGE GUI_BITMAP bmKB_DelPressedBmp;

extern GUI_CONST_STORAGE GUI_BITMAP bmKB_DoneBmp;
extern GUI_CONST_STORAGE GUI_BITMAP bmKB_DonePressedBmp;

extern GUI_CONST_STORAGE GUI_BITMAP bmKB_LetterBmp;
extern GUI_CONST_STORAGE GUI_BITMAP bmKB_LetterPressedBmp;

extern GUI_CONST_STORAGE GUI_BITMAP bmKB_NumBmp;
extern GUI_CONST_STORAGE GUI_BITMAP bmKB_NumPressedBmp;

extern GUI_CONST_STORAGE GUI_BITMAP bmKB_ShiftBmp;
extern GUI_CONST_STORAGE GUI_BITMAP bmKB_ShiftPressedBmp;

extern GUI_CONST_STORAGE GUI_BITMAP bmKB_SpaceBmp;
extern GUI_CONST_STORAGE GUI_BITMAP bmKB_SpacePressedBmp;

extern GUI_CONST_STORAGE GUI_BITMAP bmKB_SpecharBmp;
extern GUI_CONST_STORAGE GUI_BITMAP bmKB_SpecharPressedBmp;

extern GUI_CONST_STORAGE GUI_BITMAP bmKB_CharGuidePressedBmp;
extern GUI_CONST_STORAGE GUI_BITMAP bmKB_CharGuideBmp;


#define TP_KB_DEFAULT_X_GAP		(6)
#define TP_KB_DEFAULT_Y_GAP		(7)

#if 1
extern GUI_CONST_STORAGE GUI_FONT GUI_FontF12DqW3;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontF16DqW3;

#define TP_KB_DEFAULT_LETTER_FONT	&GUI_FontF16DqW3
#define TP_KB_DEFAULT_NUM123_FONT	&GUI_FontF12DqW3
#define TP_KB_DEFAULT_SPACE_FONT	&GUI_FontF12DqW3
#define TP_KB_DEFAULT_DONE_FONT		&GUI_FontF12DqW3

#else

#define TP_KB_DEFAULT_LETTER_FONT	GUI_DEFAULT_FONT
#define TP_KB_DEFAULT_NUM123_FONT	GUI_DEFAULT_FONT
#define TP_KB_DEFAULT_SPACE_FONT	GUI_DEFAULT_FONT
#define TP_KB_DEFAULT_DONE_FONT		GUI_DEFAULT_FONT

#endif

#define TP_KB_DEFAULT_BG_BMP				&bmKB_BgBmp

#define TP_KB_DEFAULT_DEL_BMP				&bmKB_DelBmp
#define TP_KB_DEFAULT_DEL_PRESSED_BMP		&bmKB_DelPressedBmp

#define TP_KB_DEFAULT_DONE_BMP				&bmKB_DoneBmp
#define TP_KB_DEFAULT_DONE_PRESSED_BMP		&bmKB_DonePressedBmp

#define TP_KB_DEFAULT_LETTER_BMP			&bmKB_LetterBmp
#define TP_KB_DEFAULT_LETTER_PRESSED_BMP	&bmKB_LetterPressedBmp

#define TP_KB_DEFAULT_NUM_BMP				&bmKB_NumBmp
#define TP_KB_DEFAULT_NUM_PRESSED_BMP		&bmKB_NumPressedBmp

#define TP_KB_DEFAULT_NUM123_BMP			&bmKB_123Bmp
#define TP_KB_DEFAULT_NUM123_PRESSED_BMP	&bmKB_123PressedBmp

#define TP_KB_DEFAULT_SHIFT_BMP				&bmKB_ShiftBmp
#define TP_KB_DEFAULT_SHIFT_PRESSED_BMP		&bmKB_ShiftPressedBmp

#define TP_KB_DEFAULT_SPACE_BMP				&bmKB_SpaceBmp
#define TP_KB_DEFAULT_SPACE_PRESSED_BMP		&bmKB_SpacePressedBmp

#define TP_KB_DEFAULT_SPECHAR_BMP			&bmKB_SpecharBmp
#define TP_KB_DEFAULT_SPECHAR_PRESSED_BMP	&bmKB_SpecharPressedBmp

#define TP_KB_CHAR_GUIDE_BMP				&bmKB_CharGuideBmp
#define TP_KB_CHAR_GUIDE_PRESSED_BMP		&bmKB_CharGuidePressedBmp


#define TP_KB_DEFAULT_TEXT_COLOR			GUI_BLACK
#define TP_KB_DEFAULT_BG_COLOR				GUI_GRAY

#if !TP_KEYBOARD_SHOW_LOWER_LETTER
#define TP_KB_UPPER_CASE		(1)
#define TP_KB_LOWER_CASE		(0)
static int letterCase = TP_KB_UPPER_CASE;
#endif

static TP_KEYBOARD_Obj TP_KeyBoard_default =
{
	4,	/* line number */
	{	/* x gap and y gap for every keypad */
		{TP_KB_DEFAULT_X_GAP, TP_KB_DEFAULT_Y_GAP},
		{TP_KB_DEFAULT_X_GAP, TP_KB_DEFAULT_Y_GAP},
		{TP_KB_DEFAULT_X_GAP, TP_KB_DEFAULT_Y_GAP},
		{TP_KB_DEFAULT_X_GAP, TP_KB_DEFAULT_Y_GAP},
	},
	TP_KB_DEFAULT_BG_COLOR,		/* default background color */
	TP_KB_DEFAULT_TEXT_COLOR,	/* default text color */
	TP_KB_DEFAULT_LETTER_FONT,	/* default letter button font */
	TP_KB_DEFAULT_NUM123_FONT,	/* default 123 button font */
	TP_KB_DEFAULT_SPACE_FONT,	/* default space button font */
	TP_KB_DEFAULT_DONE_FONT,	/* default done button font */
	TP_KB_DEFAULT_BG_BMP,		/* default background image */
	{TP_KB_DEFAULT_DEL_BMP, TP_KB_DEFAULT_DEL_PRESSED_BMP},			/* default del button image */
	{TP_KB_DEFAULT_DONE_BMP, TP_KB_DEFAULT_DONE_PRESSED_BMP},		/* default done button image */
	{TP_KB_DEFAULT_LETTER_BMP, TP_KB_DEFAULT_LETTER_PRESSED_BMP},	/* default letter button image */
	{TP_KB_DEFAULT_NUM_BMP, TP_KB_DEFAULT_NUM_PRESSED_BMP},			/* default number button image */
  	{TP_KB_DEFAULT_NUM123_BMP, TP_KB_DEFAULT_NUM123_PRESSED_BMP},	/* default 123 button image */
	{TP_KB_DEFAULT_SHIFT_BMP, TP_KB_DEFAULT_SHIFT_PRESSED_BMP},		/* default shift button image */
	{TP_KB_DEFAULT_SPACE_BMP, TP_KB_DEFAULT_SPACE_PRESSED_BMP},		/* default space button image */
	{TP_KB_DEFAULT_SPECHAR_BMP, TP_KB_DEFAULT_SPECHAR_PRESSED_BMP},	/* default spechar button image */
	{TP_KB_CHAR_GUIDE_BMP, TP_KB_CHAR_GUIDE_PRESSED_BMP},	
};


static KEY_SIZE	TP_KeyBoard_GetDelKeySize()
{
	KEY_SIZE	size;
	const GUI_BITMAP *pBitmap = TP_KeyBoard_default.pDelBmp[0];

	size.xSize = pBitmap->XSize;
	size.ySize = pBitmap->YSize;

	return size;
}

static KEY_SIZE TP_KeyBoard_GetDoneKeySize()
{
	KEY_SIZE	size;
	const GUI_BITMAP *pBitmap = TP_KeyBoard_default.pDoneBmp[0];

	size.xSize = pBitmap->XSize;
	size.ySize = pBitmap->YSize;

	return size;
}

static KEY_SIZE TP_KeyBoard_GetLetterKeySize()
{
	KEY_SIZE	size;
	const GUI_BITMAP *pBitmap = TP_KeyBoard_default.pLetterBmp[0];

	size.xSize = pBitmap->XSize;
	size.ySize = pBitmap->YSize;

	return size;
}

static KEY_SIZE TP_KeyBoard_GetNumKeySize()
{
	KEY_SIZE	size;
	const GUI_BITMAP *pBitmap = TP_KeyBoard_default.pNumBmp[0];

	size.xSize = pBitmap->XSize;
	size.ySize = pBitmap->YSize;
	return size;
}

static KEY_SIZE TP_KeyBoard_GetNum123KeySize()
{
	KEY_SIZE	size;
	const GUI_BITMAP *pBitmap = TP_KeyBoard_default.pNum123Bmp[0];

	size.xSize = pBitmap->XSize;
	size.ySize = pBitmap->YSize;

	return size;
}

#if 0
/* 暂时用123的图片，因为大小一样 */
static KEY_SIZE TP_KeyBoard_GetABCKeySize()
{
	KEY_SIZE	size;
	const GUI_BITMAP *pBitmap = TP_KeyBoard_default.pNum123Bmp[0];

	size.xSize = pBitmap->XSize;
	size.ySize = pBitmap->YSize;

	return size;
}
#endif

static KEY_SIZE TP_KeyBoard_GetShiftKeySize()
{
	KEY_SIZE	size;
	const GUI_BITMAP *pBitmap = TP_KeyBoard_default.pShiftBmp[0];

	size.xSize = pBitmap->XSize;
	size.ySize = pBitmap->YSize;

	return size;
}

static KEY_SIZE TP_KeyBoard_GetCharGuideKeySize()
{
	KEY_SIZE	size;
	const GUI_BITMAP *pBitmap = TP_KeyBoard_default.pCharGuideBmp[0];

	size.xSize = pBitmap->XSize;
	size.ySize = pBitmap->YSize;

	return size;
}

static KEY_SIZE TP_KeyBoard_GetSpaceKeySize()
{
	KEY_SIZE	size;
	const GUI_BITMAP *pBitmap = TP_KeyBoard_default.pSpaceBmp[0];

	size.xSize = pBitmap->XSize;
	size.ySize = pBitmap->YSize;
	return size;
}

static KEY_SIZE TP_KeyBoard_GetSpecharKeySize()
{
	KEY_SIZE	size;
	const GUI_BITMAP *pBitmap = TP_KeyBoard_default.pSpecharBmp[0];

	size.xSize = pBitmap->XSize;
	size.ySize = pBitmap->YSize;

	return size;
}

static const GUI_BITMAP* TP_KeyBoard_GetDelKeyBmp(int index)
{

	if (index < TP_KB_BMP_MIN_INDEX || (index >= TP_KB_BMP_MAX_INDEX))
	{
		return NULL;
	}
	
	return TP_KeyBoard_default.pDelBmp[index];
}

static const GUI_BITMAP* TP_KeyBoard_GetDoneKeyBmp(int index)
{
	if (index < TP_KB_BMP_MIN_INDEX || (index >= TP_KB_BMP_MAX_INDEX))
	{
		return NULL;
	}
	
	return TP_KeyBoard_default.pDoneBmp[index];
}

static const GUI_BITMAP* TP_KeyBoard_GetLetterKeyBmp(int index)
{
	if (index < TP_KB_BMP_MIN_INDEX || (index >= TP_KB_BMP_MAX_INDEX))
	{
		return NULL;
	}
	
	return TP_KeyBoard_default.pLetterBmp[index];
}

static const GUI_BITMAP* TP_KeyBoard_GetNumKeyBmp(int index)
{
	if (index < TP_KB_BMP_MIN_INDEX || (index >= TP_KB_BMP_MAX_INDEX))
	{
		return NULL;
	}
	
	return TP_KeyBoard_default.pNumBmp[index];
}

static const GUI_BITMAP* TP_KeyBoard_GetNum123KeyBmp(int index)
{
	if (index < TP_KB_BMP_MIN_INDEX || (index >= TP_KB_BMP_MAX_INDEX))
	{
		return NULL;
	}
	
	return TP_KeyBoard_default.pNum123Bmp[index];
}

#if 0
static const GUI_BITMAP* TP_KeyBoard_GetABCKeyBmp(int index)
{
	if (index < TP_KB_BMP_MIN_INDEX || (index >= TP_KB_BMP_MAX_INDEX))
	{
		return NULL;
	}
	
	return TP_KeyBoard_default.pNum123Bmp[index];
}
#endif

static const GUI_BITMAP* TP_KeyBoard_GetShiftKeyBmp(int index)
{
	if (index < TP_KB_BMP_MIN_INDEX || (index >= TP_KB_BMP_MAX_INDEX))
	{
		return NULL;
	}
	
	return TP_KeyBoard_default.pShiftBmp[index];
}


static const GUI_BITMAP* TP_KeyBoard_GetCharGuideKeyBmp(int index)
{
	if (index < TP_KB_BMP_MIN_INDEX || (index >= TP_KB_BMP_MAX_INDEX))
	{
		return NULL;
	}
	
	return TP_KeyBoard_default.pCharGuideBmp[index];
}

static const GUI_BITMAP* TP_KeyBoard_GetSpaceKeyBmp(int index)
{
	if (index < TP_KB_BMP_MIN_INDEX || (index >= TP_KB_BMP_MAX_INDEX))
	{
		return NULL;
	}
	
	return TP_KeyBoard_default.pSpaceBmp[index];
}

static const GUI_BITMAP* TP_KeyBoard_GetSpecharKeyBmp(int index)
{
	if (index < TP_KB_BMP_MIN_INDEX || (index >= TP_KB_BMP_MAX_INDEX))
	{
		return NULL;
	}
	
	return TP_KeyBoard_default.pSpecharBmp[index];
}

const GUI_FONT* TP_KeyBoard_GetDoneKeyFont()
{
	
	return TP_KeyBoard_default.pDoneFont;
}

void TP_KeyBoard_SetDoneKeyFont(const GUI_FONT* pFont)
{
	
	TP_KeyBoard_default.pDoneFont = pFont;
}

const GUI_FONT* TP_KeyBoard_GetLetterKeyFont()
{
	return TP_KeyBoard_default.pLetterFont;
}


void TP_KeyBoard_SetLetterKeyFont(const GUI_FONT* pFont)
{
	TP_KeyBoard_default.pLetterFont = pFont;
}

const GUI_FONT* TP_KeyBoard_GetSpaceKeyFont()
{
	return TP_KeyBoard_default.pSpaceFont;
}


void TP_KeyBoard_SetSpaceKeyFont(const GUI_FONT* pFont)
{
	TP_KeyBoard_default.pSpaceFont  = pFont;
}

const GUI_FONT* TP_KeyBoard_GetNum123KeyFont()
{
	return TP_KeyBoard_default.pNum123Font;
}


void TP_KeyBoard_SetNum123KeyFont(const GUI_FONT* pFont)
{
	TP_KeyBoard_default.pNum123Font = pFont;
}


static void TP_KeyBoard_ShiftKeyCallback(TP_KEYBOARD_Handle hObj)
{
#if TP_KEYBOARD_SHOW_LOWER_LETTER
	if (hObj == lowerPadHandle)
	{
		WM_BringToTop(upperPadHandle);
	}
	else if (hObj == upperPadHandle)
	{
		WM_BringToTop(lowerPadHandle);
	}
#else
	letterCase ^= 1;
#endif
}

static void TP_KeyBoard_DelKeyCallback(TP_KEYBOARD_Handle hObj)
{
#define DEL_KEY_ASCII	(0x8)
	GUI_StoreKeyMsg(DEL_KEY_ASCII, 1);
	GUI_StoreKeyMsg(DEL_KEY_ASCII, 0);
}

static void TP_KeyBoard_DoneKeyCallback(TP_KEYBOARD_Handle hObj)
{
	TP_KeyBoard_Hide();
}

static void TP_KeyBoard_SpaceKeyCallback(TP_KEYBOARD_Handle hObj)
{
#define SPACE_KEY_ASCII	(0x20)
	GUI_StoreKeyMsg(SPACE_KEY_ASCII, 1);
	GUI_StoreKeyMsg(SPACE_KEY_ASCII, 0);
}

static void TP_KeyBoard_Num123KeyCallback(TP_KEYBOARD_Handle hObj)
{
	WM_BringToTop(numPadHandle);
}

static void TP_KeyBoard_CharGuideKeyCallback(TP_KEYBOARD_Handle hObj)
{
	WM_BringToTop(charPadHandle);
}

static void TP_KeyBoard_ABCKeyCallback(TP_KEYBOARD_Handle hObj)
{
#if TP_KEYBOARD_SHOW_LOWER_LETTER 
	WM_BringToTop(lowerPadHandle);
#else
	letterCase = TP_KB_UPPER_CASE;
	WM_BringToTop(upperPadHandle);
#endif
}

//#define TP_KB_LETTER_SIZE_FUNC	TP_KeyBoard_GetLetterKeySize
#if TP_KEYBOARD_SHOW_LOWER_LETTER
static KEY_DATA lowerKeyPad[] =
{
	{-1, -1, "q", TP_KeyBoard_GetLetterKeySize, TP_KeyBoard_GetLetterKeyBmp, TP_KeyBoard_GetLetterKeyFont, NULL},
	{-1, -1, "w", TP_KeyBoard_GetLetterKeySize, TP_KeyBoard_GetLetterKeyBmp, TP_KeyBoard_GetLetterKeyFont, NULL},
	{-1, -1, "e", TP_KeyBoard_GetLetterKeySize, TP_KeyBoard_GetLetterKeyBmp, TP_KeyBoard_GetLetterKeyFont, NULL},
	{-1, -1, "r", TP_KeyBoard_GetLetterKeySize, TP_KeyBoard_GetLetterKeyBmp, TP_KeyBoard_GetLetterKeyFont, NULL},
	{-1, -1, "t", TP_KeyBoard_GetLetterKeySize, TP_KeyBoard_GetLetterKeyBmp, TP_KeyBoard_GetLetterKeyFont, NULL},
	{-1, -1, "y", TP_KeyBoard_GetLetterKeySize, TP_KeyBoard_GetLetterKeyBmp, TP_KeyBoard_GetLetterKeyFont, NULL},
	{-1, -1, "u", TP_KeyBoard_GetLetterKeySize, TP_KeyBoard_GetLetterKeyBmp, TP_KeyBoard_GetLetterKeyFont, NULL},
	{-1, -1, "i", TP_KeyBoard_GetLetterKeySize, TP_KeyBoard_GetLetterKeyBmp, TP_KeyBoard_GetLetterKeyFont, NULL},
	{-1, -1, "o", TP_KeyBoard_GetLetterKeySize, TP_KeyBoard_GetLetterKeyBmp, TP_KeyBoard_GetLetterKeyFont, NULL},
	{-1, -1, "p", TP_KeyBoard_GetLetterKeySize, TP_KeyBoard_GetLetterKeyBmp, TP_KeyBoard_GetLetterKeyFont, NULL},
	{-1, -1, NULL, NULL, NULL, NULL, NULL},/* 一行的结束 */

	{-1, -1, "a", TP_KeyBoard_GetLetterKeySize, TP_KeyBoard_GetLetterKeyBmp, TP_KeyBoard_GetLetterKeyFont, NULL},
	{-1, -1, "s", TP_KeyBoard_GetLetterKeySize, TP_KeyBoard_GetLetterKeyBmp, TP_KeyBoard_GetLetterKeyFont, NULL},
	{-1, -1, "d", TP_KeyBoard_GetLetterKeySize, TP_KeyBoard_GetLetterKeyBmp, TP_KeyBoard_GetLetterKeyFont, NULL},
	{-1, -1, "f", TP_KeyBoard_GetLetterKeySize, TP_KeyBoard_GetLetterKeyBmp, TP_KeyBoard_GetLetterKeyFont, NULL},
	{-1, -1, "g", TP_KeyBoard_GetLetterKeySize, TP_KeyBoard_GetLetterKeyBmp, TP_KeyBoard_GetLetterKeyFont, NULL},
	{-1, -1, "h", TP_KeyBoard_GetLetterKeySize, TP_KeyBoard_GetLetterKeyBmp, TP_KeyBoard_GetLetterKeyFont, NULL},
	{-1, -1, "j", TP_KeyBoard_GetLetterKeySize, TP_KeyBoard_GetLetterKeyBmp, TP_KeyBoard_GetLetterKeyFont, NULL},
	{-1, -1, "k", TP_KeyBoard_GetLetterKeySize, TP_KeyBoard_GetLetterKeyBmp, TP_KeyBoard_GetLetterKeyFont, NULL},
	{-1, -1, "l", TP_KeyBoard_GetLetterKeySize, TP_KeyBoard_GetLetterKeyBmp, TP_KeyBoard_GetLetterKeyFont, NULL},
	{-1, -1, NULL, NULL, NULL, NULL, NULL},/* 一行的结束 */

	{-1, -1, NULL, TP_KeyBoard_GetShiftKeySize, TP_KeyBoard_GetShiftKeyBmp, NULL, TP_KeyBoard_ShiftKeyCallback},
	{-1, -1, "z", TP_KeyBoard_GetLetterKeySize, TP_KeyBoard_GetLetterKeyBmp, TP_KeyBoard_GetLetterKeyFont, NULL},
	{-1, -1, "x", TP_KeyBoard_GetLetterKeySize, TP_KeyBoard_GetLetterKeyBmp, TP_KeyBoard_GetLetterKeyFont, NULL},
	{-1, -1, "c", TP_KeyBoard_GetLetterKeySize, TP_KeyBoard_GetLetterKeyBmp, TP_KeyBoard_GetLetterKeyFont, NULL},
	{-1, -1, "v", TP_KeyBoard_GetLetterKeySize, TP_KeyBoard_GetLetterKeyBmp, TP_KeyBoard_GetLetterKeyFont, NULL},
	{-1, -1, "b", TP_KeyBoard_GetLetterKeySize, TP_KeyBoard_GetLetterKeyBmp, TP_KeyBoard_GetLetterKeyFont, NULL},
	{-1, -1, "n", TP_KeyBoard_GetLetterKeySize, TP_KeyBoard_GetLetterKeyBmp, TP_KeyBoard_GetLetterKeyFont, NULL},
	{-1, -1, "m", TP_KeyBoard_GetLetterKeySize, TP_KeyBoard_GetLetterKeyBmp, TP_KeyBoard_GetLetterKeyFont, NULL},
	{-1, -1, NULL, TP_KeyBoard_GetDelKeySize, TP_KeyBoard_GetDelKeyBmp, NULL, TP_KeyBoard_DelKeyCallback},
	{-1, -1, NULL, NULL, NULL, NULL, NULL},/* 一行的结束 */
	
	{-1, -1, "123", TP_KeyBoard_GetNum123KeySize, TP_KeyBoard_GetNum123KeyBmp, TP_KeyBoard_GetNum123KeyFont, TP_KeyBoard_Num123KeyCallback},
	{-1, -1, "Space", TP_KeyBoard_GetSpaceKeySize, TP_KeyBoard_GetSpaceKeyBmp, TP_KeyBoard_GetSpaceKeyFont, TP_KeyBoard_SpaceKeyCallback},
	{-1, -1, "Done", TP_KeyBoard_GetDoneKeySize, TP_KeyBoard_GetDoneKeyBmp, TP_KeyBoard_GetDoneKeyFont, TP_KeyBoard_DoneKeyCallback},
	{-1, -1, NULL, NULL, NULL, NULL, NULL},/* 一行的结束 */
};
#endif

static KEY_DATA upperKeyPad[] =
{
	{-1, -1, "Q", TP_KeyBoard_GetLetterKeySize, TP_KeyBoard_GetLetterKeyBmp, TP_KeyBoard_GetLetterKeyFont, NULL},
	{-1, -1, "W", TP_KeyBoard_GetLetterKeySize, TP_KeyBoard_GetLetterKeyBmp, TP_KeyBoard_GetLetterKeyFont, NULL},
	{-1, -1, "E", TP_KeyBoard_GetLetterKeySize, TP_KeyBoard_GetLetterKeyBmp, TP_KeyBoard_GetLetterKeyFont, NULL},
	{-1, -1, "R", TP_KeyBoard_GetLetterKeySize, TP_KeyBoard_GetLetterKeyBmp, TP_KeyBoard_GetLetterKeyFont, NULL},
	{-1, -1, "T", TP_KeyBoard_GetLetterKeySize, TP_KeyBoard_GetLetterKeyBmp, TP_KeyBoard_GetLetterKeyFont, NULL},
	{-1, -1, "Y", TP_KeyBoard_GetLetterKeySize, TP_KeyBoard_GetLetterKeyBmp, TP_KeyBoard_GetLetterKeyFont, NULL},
	{-1, -1, "U", TP_KeyBoard_GetLetterKeySize, TP_KeyBoard_GetLetterKeyBmp, TP_KeyBoard_GetLetterKeyFont, NULL},
	{-1, -1, "I", TP_KeyBoard_GetLetterKeySize, TP_KeyBoard_GetLetterKeyBmp, TP_KeyBoard_GetLetterKeyFont, NULL},
	{-1, -1, "O", TP_KeyBoard_GetLetterKeySize, TP_KeyBoard_GetLetterKeyBmp, TP_KeyBoard_GetLetterKeyFont, NULL},
	{-1, -1, "P", TP_KeyBoard_GetLetterKeySize, TP_KeyBoard_GetLetterKeyBmp, TP_KeyBoard_GetLetterKeyFont, NULL},
	{-1, -1, NULL, NULL, NULL, NULL, NULL},/* 一行的结束 */

	{-1, -1, "A", TP_KeyBoard_GetLetterKeySize, TP_KeyBoard_GetLetterKeyBmp, TP_KeyBoard_GetLetterKeyFont, NULL},
	{-1, -1, "S", TP_KeyBoard_GetLetterKeySize, TP_KeyBoard_GetLetterKeyBmp, TP_KeyBoard_GetLetterKeyFont, NULL},
	{-1, -1, "D", TP_KeyBoard_GetLetterKeySize, TP_KeyBoard_GetLetterKeyBmp, TP_KeyBoard_GetLetterKeyFont, NULL},
	{-1, -1, "F", TP_KeyBoard_GetLetterKeySize, TP_KeyBoard_GetLetterKeyBmp, TP_KeyBoard_GetLetterKeyFont, NULL},
	{-1, -1, "G", TP_KeyBoard_GetLetterKeySize, TP_KeyBoard_GetLetterKeyBmp, TP_KeyBoard_GetLetterKeyFont, NULL},
	{-1, -1, "H", TP_KeyBoard_GetLetterKeySize, TP_KeyBoard_GetLetterKeyBmp, TP_KeyBoard_GetLetterKeyFont, NULL},
	{-1, -1, "J", TP_KeyBoard_GetLetterKeySize, TP_KeyBoard_GetLetterKeyBmp, TP_KeyBoard_GetLetterKeyFont, NULL},
	{-1, -1, "K", TP_KeyBoard_GetLetterKeySize, TP_KeyBoard_GetLetterKeyBmp, TP_KeyBoard_GetLetterKeyFont, NULL},
	{-1, -1, "L", TP_KeyBoard_GetLetterKeySize, TP_KeyBoard_GetLetterKeyBmp, TP_KeyBoard_GetLetterKeyFont, NULL},
	{-1, -1, NULL, NULL, NULL, NULL, NULL},/* 一行的结束 */

	{-1, -1, NULL, TP_KeyBoard_GetShiftKeySize, TP_KeyBoard_GetShiftKeyBmp, NULL, TP_KeyBoard_ShiftKeyCallback},
	{-1, -1, "Z", TP_KeyBoard_GetLetterKeySize, TP_KeyBoard_GetLetterKeyBmp, TP_KeyBoard_GetLetterKeyFont, NULL},
	{-1, -1, "X", TP_KeyBoard_GetLetterKeySize, TP_KeyBoard_GetLetterKeyBmp, TP_KeyBoard_GetLetterKeyFont, NULL},
	{-1, -1, "C", TP_KeyBoard_GetLetterKeySize, TP_KeyBoard_GetLetterKeyBmp, TP_KeyBoard_GetLetterKeyFont, NULL},
	{-1, -1, "V", TP_KeyBoard_GetLetterKeySize, TP_KeyBoard_GetLetterKeyBmp, TP_KeyBoard_GetLetterKeyFont, NULL},
	{-1, -1, "B", TP_KeyBoard_GetLetterKeySize, TP_KeyBoard_GetLetterKeyBmp, TP_KeyBoard_GetLetterKeyFont, NULL},
	{-1, -1, "N", TP_KeyBoard_GetLetterKeySize, TP_KeyBoard_GetLetterKeyBmp, TP_KeyBoard_GetLetterKeyFont, NULL},
	{-1, -1, "M", TP_KeyBoard_GetLetterKeySize, TP_KeyBoard_GetLetterKeyBmp, TP_KeyBoard_GetLetterKeyFont, NULL},
	{-1, -1, NULL, TP_KeyBoard_GetDelKeySize, TP_KeyBoard_GetDelKeyBmp, NULL,TP_KeyBoard_DelKeyCallback},
	{-1, -1, NULL, NULL, NULL, NULL, NULL},/* 一行的结束 */
	
	{-1, -1, "123", TP_KeyBoard_GetNum123KeySize, TP_KeyBoard_GetNum123KeyBmp, TP_KeyBoard_GetNum123KeyFont, TP_KeyBoard_Num123KeyCallback},
	{-1, -1, "Space", TP_KeyBoard_GetSpaceKeySize, TP_KeyBoard_GetSpaceKeyBmp, TP_KeyBoard_GetSpaceKeyFont, TP_KeyBoard_SpaceKeyCallback},
	{-1, -1, "Done", TP_KeyBoard_GetDoneKeySize, TP_KeyBoard_GetDoneKeyBmp, TP_KeyBoard_GetDoneKeyFont, TP_KeyBoard_DoneKeyCallback},
	{-1, -1, NULL, NULL, NULL, NULL, NULL},/* 一行的结束 */
};


static KEY_DATA numKeyPad[] =
{
	{-1, -1, "-", TP_KeyBoard_GetSpecharKeySize, TP_KeyBoard_GetSpecharKeyBmp, TP_KeyBoard_GetLetterKeyFont, NULL},
	{-1, -1, "1", TP_KeyBoard_GetNumKeySize, TP_KeyBoard_GetNumKeyBmp, TP_KeyBoard_GetLetterKeyFont, NULL},
	{-1, -1, "2", TP_KeyBoard_GetNumKeySize, TP_KeyBoard_GetNumKeyBmp, TP_KeyBoard_GetLetterKeyFont, NULL},
	{-1, -1, "3", TP_KeyBoard_GetNumKeySize, TP_KeyBoard_GetNumKeyBmp, TP_KeyBoard_GetLetterKeyFont, NULL},
	{-1, -1, "4", TP_KeyBoard_GetNumKeySize, TP_KeyBoard_GetNumKeyBmp, TP_KeyBoard_GetLetterKeyFont, NULL},
	{-1, -1, ".", TP_KeyBoard_GetSpecharKeySize, TP_KeyBoard_GetSpecharKeyBmp, TP_KeyBoard_GetLetterKeyFont, NULL},
	{-1, -1, NULL, NULL, NULL, NULL, NULL},/* 一行的结束 */

	{-1, -1, "_", TP_KeyBoard_GetSpecharKeySize, TP_KeyBoard_GetSpecharKeyBmp, TP_KeyBoard_GetLetterKeyFont, NULL},
	{-1, -1, "5", TP_KeyBoard_GetNumKeySize, TP_KeyBoard_GetNumKeyBmp, TP_KeyBoard_GetLetterKeyFont, NULL},
	{-1, -1, "6", TP_KeyBoard_GetNumKeySize, TP_KeyBoard_GetNumKeyBmp, TP_KeyBoard_GetLetterKeyFont, NULL},
	{-1, -1, "7", TP_KeyBoard_GetNumKeySize, TP_KeyBoard_GetNumKeyBmp, TP_KeyBoard_GetLetterKeyFont, NULL},
	{-1, -1, "8", TP_KeyBoard_GetNumKeySize, TP_KeyBoard_GetNumKeyBmp, TP_KeyBoard_GetLetterKeyFont, NULL},
	{-1, -1, ":", TP_KeyBoard_GetSpecharKeySize, TP_KeyBoard_GetSpecharKeyBmp, TP_KeyBoard_GetLetterKeyFont, NULL},
	{-1, -1, NULL, NULL, NULL, NULL, NULL},/* 一行的结束 */

	{-1, -1, "#+=", TP_KeyBoard_GetCharGuideKeySize, TP_KeyBoard_GetCharGuideKeyBmp, TP_KeyBoard_GetNum123KeyFont, TP_KeyBoard_CharGuideKeyCallback},
	{-1, -1, "@", TP_KeyBoard_GetNumKeySize, TP_KeyBoard_GetNumKeyBmp, TP_KeyBoard_GetLetterKeyFont, NULL},
	{-1, -1, "9", TP_KeyBoard_GetNumKeySize, TP_KeyBoard_GetNumKeyBmp, TP_KeyBoard_GetLetterKeyFont, NULL},
	{-1, -1, "0", TP_KeyBoard_GetNumKeySize, TP_KeyBoard_GetNumKeyBmp, TP_KeyBoard_GetLetterKeyFont, NULL},
	{-1, -1, "*", TP_KeyBoard_GetNumKeySize, TP_KeyBoard_GetNumKeyBmp, TP_KeyBoard_GetLetterKeyFont, NULL},
	{-1, -1, NULL, TP_KeyBoard_GetDelKeySize, TP_KeyBoard_GetDelKeyBmp, NULL, TP_KeyBoard_DelKeyCallback},
	{-1, -1, NULL, NULL, NULL, NULL, NULL},/* 一行的结束 */
	
	{-1, -1, "ABC", TP_KeyBoard_GetNum123KeySize, TP_KeyBoard_GetNum123KeyBmp, TP_KeyBoard_GetNum123KeyFont, TP_KeyBoard_ABCKeyCallback},
	{-1, -1, "Space", TP_KeyBoard_GetSpaceKeySize, TP_KeyBoard_GetSpaceKeyBmp, TP_KeyBoard_GetSpaceKeyFont, TP_KeyBoard_SpaceKeyCallback},
	{-1, -1, "Done", TP_KeyBoard_GetDoneKeySize, TP_KeyBoard_GetDoneKeyBmp, TP_KeyBoard_GetDoneKeyFont, TP_KeyBoard_DoneKeyCallback},
	{-1, -1, NULL, NULL, NULL, NULL, NULL},/* 一行的结束 */
};

static KEY_DATA specharKeyPad[] =
{
	{-1, -1, ",", TP_KeyBoard_GetLetterKeySize, TP_KeyBoard_GetLetterKeyBmp, TP_KeyBoard_GetLetterKeyFont, NULL},
	{-1, -1, "?", TP_KeyBoard_GetLetterKeySize, TP_KeyBoard_GetLetterKeyBmp, TP_KeyBoard_GetLetterKeyFont, NULL},
	{-1, -1, "/", TP_KeyBoard_GetLetterKeySize, TP_KeyBoard_GetLetterKeyBmp, TP_KeyBoard_GetLetterKeyFont, NULL},
	{-1, -1, "!", TP_KeyBoard_GetLetterKeySize, TP_KeyBoard_GetLetterKeyBmp, TP_KeyBoard_GetLetterKeyFont, NULL},
	{-1, -1, ";", TP_KeyBoard_GetLetterKeySize, TP_KeyBoard_GetLetterKeyBmp, TP_KeyBoard_GetLetterKeyFont, NULL},
	{-1, -1, "(", TP_KeyBoard_GetLetterKeySize, TP_KeyBoard_GetLetterKeyBmp, TP_KeyBoard_GetLetterKeyFont, NULL},
	{-1, -1, ")", TP_KeyBoard_GetLetterKeySize, TP_KeyBoard_GetLetterKeyBmp, TP_KeyBoard_GetLetterKeyFont, NULL},
	{-1, -1, "\\", TP_KeyBoard_GetLetterKeySize, TP_KeyBoard_GetLetterKeyBmp, TP_KeyBoard_GetLetterKeyFont, NULL},
	{-1, -1, "[", TP_KeyBoard_GetLetterKeySize, TP_KeyBoard_GetLetterKeyBmp, TP_KeyBoard_GetLetterKeyFont, NULL},
	{-1, -1, "]", TP_KeyBoard_GetLetterKeySize, TP_KeyBoard_GetLetterKeyBmp, TP_KeyBoard_GetLetterKeyFont, NULL},
	{-1, -1, NULL, NULL, NULL, NULL, NULL},/* 一行的结束 */

	{-1, -1, "`", TP_KeyBoard_GetLetterKeySize, TP_KeyBoard_GetLetterKeyBmp, TP_KeyBoard_GetLetterKeyFont, NULL},
	{-1, -1, "~", TP_KeyBoard_GetLetterKeySize, TP_KeyBoard_GetLetterKeyBmp, TP_KeyBoard_GetLetterKeyFont, NULL},
	{-1, -1, "$", TP_KeyBoard_GetLetterKeySize, TP_KeyBoard_GetLetterKeyBmp, TP_KeyBoard_GetLetterKeyFont, NULL},
	{-1, -1, "%", TP_KeyBoard_GetLetterKeySize, TP_KeyBoard_GetLetterKeyBmp, TP_KeyBoard_GetLetterKeyFont, NULL},
	{-1, -1, "^", TP_KeyBoard_GetLetterKeySize, TP_KeyBoard_GetLetterKeyBmp, TP_KeyBoard_GetLetterKeyFont, NULL},
	{-1, -1, "&", TP_KeyBoard_GetLetterKeySize, TP_KeyBoard_GetLetterKeyBmp, TP_KeyBoard_GetLetterKeyFont, NULL},
	{-1, -1, "'", TP_KeyBoard_GetLetterKeySize, TP_KeyBoard_GetLetterKeyBmp, TP_KeyBoard_GetLetterKeyFont, NULL},
	{-1, -1, "+", TP_KeyBoard_GetLetterKeySize, TP_KeyBoard_GetLetterKeyBmp, TP_KeyBoard_GetLetterKeyFont, NULL},
	{-1, -1, "=", TP_KeyBoard_GetLetterKeySize, TP_KeyBoard_GetLetterKeyBmp, TP_KeyBoard_GetLetterKeyFont, NULL},
	{-1, -1, NULL, NULL, NULL, NULL, NULL},/* 一行的结束 */

	{-1, -1, "123", TP_KeyBoard_GetCharGuideKeySize, TP_KeyBoard_GetCharGuideKeyBmp, TP_KeyBoard_GetNum123KeyFont, TP_KeyBoard_Num123KeyCallback},
	{-1, -1, "{", TP_KeyBoard_GetLetterKeySize, TP_KeyBoard_GetLetterKeyBmp, TP_KeyBoard_GetLetterKeyFont, NULL},
	{-1, -1, "}", TP_KeyBoard_GetLetterKeySize, TP_KeyBoard_GetLetterKeyBmp, TP_KeyBoard_GetLetterKeyFont, NULL},
	{-1, -1, "|", TP_KeyBoard_GetLetterKeySize, TP_KeyBoard_GetLetterKeyBmp, TP_KeyBoard_GetLetterKeyFont, NULL},
	{-1, -1, "#", TP_KeyBoard_GetLetterKeySize, TP_KeyBoard_GetLetterKeyBmp, TP_KeyBoard_GetLetterKeyFont, NULL},
	{-1, -1, "<", TP_KeyBoard_GetLetterKeySize, TP_KeyBoard_GetLetterKeyBmp, TP_KeyBoard_GetLetterKeyFont, NULL},
	{-1, -1, ">", TP_KeyBoard_GetLetterKeySize, TP_KeyBoard_GetLetterKeyBmp, TP_KeyBoard_GetLetterKeyFont, NULL},
	{-1, -1, "\"", TP_KeyBoard_GetLetterKeySize, TP_KeyBoard_GetLetterKeyBmp, TP_KeyBoard_GetLetterKeyFont, NULL},
	{-1, -1, NULL, TP_KeyBoard_GetDelKeySize, TP_KeyBoard_GetDelKeyBmp, NULL, TP_KeyBoard_DelKeyCallback},
	{-1, -1, NULL, NULL, NULL, NULL, NULL},/* 一行的结束 */
	
	{-1, -1, "ABC", TP_KeyBoard_GetNum123KeySize, TP_KeyBoard_GetNum123KeyBmp, TP_KeyBoard_GetNum123KeyFont, TP_KeyBoard_ABCKeyCallback},
	{-1, -1, "Space", TP_KeyBoard_GetSpaceKeySize, TP_KeyBoard_GetSpaceKeyBmp, TP_KeyBoard_GetSpaceKeyFont, TP_KeyBoard_SpaceKeyCallback},
	{-1, -1, "Done", TP_KeyBoard_GetDoneKeySize, TP_KeyBoard_GetDoneKeyBmp, TP_KeyBoard_GetDoneKeyFont, TP_KeyBoard_DoneKeyCallback},
	{-1, -1, NULL, NULL, NULL, NULL, NULL},/* 一行的结束 */
};

static int TP_KeyBoard_GetKeyPadIdxFromId(int baseId, int id, KEY_DATA keyPad[], int len)
{
	int i;
	int idx = 0;

	for (i = 0; i < len; i++)
	{
		if (keyPad[i].pfGetKeySize)
		{
			if ((baseId + idx) == id)
			{
				break;
			}
			idx++;
		}
	}

	return i;
}

static int TP_KeyBoard_CalcKeyPadLines(KEY_DATA keyPad[], int len)
{
	int i = 0;
	int line = 0;

	for (i = 0; i < len; i++)
	{
		if (keyPad[i].pfGetKeySize == NULL)
		{
			line++;
		}
	}

	return line;
}

static int TP_KeyBoard_GetKeyHeight(KEY_DATA keyPad[], int len)
{
	int i = 0;
	KEY_SIZE size = {0, 0};

	for (i = 0; i < len; i++)
	{
		if (keyPad[i].pfGetKeySize)
		{
			size = keyPad[i].pfGetKeySize();
			break;
		}
	}

	return size.ySize;
}


/* 计算每个按钮的位置，这里没有想到很好的算法 */
static void TP_KeyBoard_CalcButtonPos(GUI_RECT rect, KEY_DATA keyPad[], int len, TP_KB_KEY_GAP gap)
{
	int i = 0;
	int start, end, idx;
	int buttonLineLen = 0;/* 每一行button的总长度 */
	KEY_SIZE size;
	int width = rect.x1 - rect.x0 + 1;
	int height = rect.y1 - rect.y0 + 1;
	int keyHeight = 0;
	int lGap = 0, tGap = 0;
	int line;

	line = TP_KeyBoard_CalcKeyPadLines(keyPad, len);
	keyHeight = TP_KeyBoard_GetKeyHeight(keyPad, len);
	tGap = (height - keyHeight * line - (line - 1) * gap.yGap) / 2;
	
	start  = i;
	line = 0;
	for (i = 0; i < len; i++)
	{
		if (keyPad[i].pfGetKeySize)
		{
			size = keyPad[i].pfGetKeySize();
			buttonLineLen += size.xSize;
		}
		else
		{			
			end = i - 1;/* 当前行的最后一个位置 */
			buttonLineLen += (end - start) * gap.xGap;/* 加上间隔长度 */
			lGap = (width - buttonLineLen) / 2;

			for (idx = start; idx <= end; idx ++)
			{
				if (idx == start)
				{
					keyPad[idx].xPos = rect.x0 + lGap;
				}
				else
				{
					size = keyPad[idx - 1].pfGetKeySize();
					keyPad[idx].xPos = keyPad[idx - 1].xPos + size.xSize + gap.xGap;
				}

				keyPad[idx].yPos = rect.y0 + tGap + (keyHeight + gap.yGap) * line;
			}

			start = i + 1;/* 下一行的开始位置 */
			line++;		/* 行数增加 */
			buttonLineLen = 0;
		}
		
	}

	start = 0;
}

static void TP_KeyBoard_DrawKeyPad(TP_KEYBOARD_Handle keyPadHandle, KEY_DATA keyPad[], int len, int baseId)
{
	int i, count;
	const GUI_BITMAP *pBitmap = NULL;
	const GUI_FONT *pFont = NULL;
	TP_BUTTON_Handle hButton;
	KEY_SIZE size;
	
	count = 0;
	for (i = 0; i < len; i++)
	{
		if (keyPad[i].pfGetKeySize)
		{
			size = keyPad[i].pfGetKeySize();
			
			hButton = TP_Button_Create(keyPad[i].xPos, keyPad[i].yPos, size.xSize, size.ySize,
					keyPadHandle, WM_CF_SHOW | WM_CF_HASTRANS | WM_CF_MEMDEV, NULL, 0, baseId + count);

			pBitmap = keyPad[i].pfGetBitmap(TP_KB_BMP_NORMAL_INDEX);
			if (pBitmap)
			{
				TP_Button_SetBitmap(hButton, TP_BUTTON_UNPRESSED, pBitmap);
			}

			pBitmap = keyPad[i].pfGetBitmap(TP_KB_BMP_CLICKED_INDEX);
			if (pBitmap)
			{
				TP_Button_SetBitmap(hButton, TP_BUTTON_PRESSED, pBitmap);
			}

			if (keyPad[i].pLabel)
			{				
				if (keyPad[i].pfGetFont)
				{
					pFont = keyPad[i].pfGetFont();
					if (pFont)
					{
						TP_Button_SetTextFont(hButton, pFont);
					}
				}
				TP_Button_SetText(hButton, (char*)keyPad[i].pLabel);
				TP_Button_SetTextColor(hButton, TP_BUTTON_UNPRESSED, TP_KeyBoard_default.color);
				TP_Button_SetTextColor(hButton, TP_BUTTON_PRESSED, TP_KeyBoard_default.color);
			}
			TP_Button_SetFocussable(hButton, 0);

			count++;
		}
	}
}

static void TP_KeyBoard_keyPadCallback(WM_MESSAGE * pMsg, int baseId, KEY_DATA keypad[], int len, int padIdx) 
{
	TP_KEYBOARD_Handle hWin;
	char c;
	int id;
	int NCode;
	int idx;

	GUI_RECT	winRect;
	const GUI_BITMAP *pBgBmp = TP_KeyBoard_default.pBgBmp;

	hWin = pMsg->hWin;
	WM_GetWindowRectEx(hWin, &winRect);
	GUI_MoveRect(&winRect, -winRect.x0, -winRect.y0);

	switch (pMsg->MsgId) 
	{

	case WM_CREATE:
		//
		// Create the every key button
		//
		TP_KeyBoard_CalcButtonPos(winRect, keypad, 
					len, TP_KeyBoard_default.keyGap[padIdx]);
		TP_KeyBoard_DrawKeyPad(hWin, keypad, len, baseId);	
		break;
	case WM_PAINT:
		//
		// Draw background
		//
		if (pBgBmp)
		{
			GUI_DrawBitmap(pBgBmp, winRect.x0, winRect.y0);
		}
		else
		{
			GUI_SetColor(TP_KeyBoard_default.bgColor);
   			GUI_DrawRect(winRect.x0, winRect.y0, winRect.x1, winRect.y1);
		}
		break;
	case WM_NOTIFY_PARENT:
		//
		// Send key message to currently focused window
		//
		id    = WM_GetId(pMsg->hWinSrc);
		NCode = pMsg->Data.v;
		idx = TP_KeyBoard_GetKeyPadIdxFromId(baseId, id, keypad, len);
		switch (NCode) 
		{
		case WM_NOTIFICATION_RELEASED:
			
			if (keypad[idx].pfWmCallback)
			{
				keypad[idx].pfWmCallback(hWin);
			}
			else if (keypad[idx].pLabel)
			{
				c = keypad[idx].pLabel[0];
#if !TP_KEYBOARD_SHOW_LOWER_LETTER
				if (((c >= 'A') && (c <= 'Z')) && (letterCase == TP_KB_LOWER_CASE))
				{
					c |= 0x20;
				}
#endif

				GUI_StoreKeyMsg(c, 1);
				GUI_StoreKeyMsg(c, 0);
				//WM_SetFocus(focusWin);
				//GUI_SendKeyMsg(c, 1);
				//GUI_SendKeyMsg(c, 0);
			}
			break;
		}
		break;
	}
}

#if TP_KEYBOARD_SHOW_LOWER_LETTER
static void TP_KeyBoard_lowerPadCallback(WM_MESSAGE * pMsg) 
{
	TP_KeyBoard_keyPadCallback(pMsg, TP_KB_LOWER_ID, lowerKeyPad, GUI_COUNTOF(lowerKeyPad), KEY_PAD_LOWER_INDEX);
}
#endif

static void TP_KeyBoard_upperPadCallback(WM_MESSAGE * pMsg) 
{
	TP_KeyBoard_keyPadCallback(pMsg, TP_KB_UPPER_ID, upperKeyPad, GUI_COUNTOF(upperKeyPad), KEY_PAD_UPPER_INDEX);
}

static void TP_KeyBoard_numPadCallback(WM_MESSAGE * pMsg) 
{
	TP_KeyBoard_keyPadCallback(pMsg, TP_KB_NUM_ID, numKeyPad, GUI_COUNTOF(numKeyPad), KEY_PAD_NUM_INDEX);
}

static void TP_KeyBoard_specharPadCallback(WM_MESSAGE * pMsg) 
{
	TP_KeyBoard_keyPadCallback(pMsg, TP_KB_CHAR_ID, specharKeyPad, GUI_COUNTOF(specharKeyPad), KEY_PAD_SPECHAR_INDEX);
}

static void TP_KeyBoard_keyBoardCallback(WM_MESSAGE * pMsg) 
{
	WM_HWIN    hWin;
	GUI_RECT winRect;
	int height, width;
	//const GUI_BITMAP *pBgBmp = TP_KeyBoard_default.pBgBmp;

	hWin = pMsg->hWin;
	WM_GetWindowRectEx(hWin, &winRect);
	GUI_MoveRect(&winRect, -winRect.x0, -winRect.y0);

	width = winRect.x1 - winRect.x0 + 1;
	height = winRect.y1 - winRect.y0 + 1;
	switch (pMsg->MsgId) 
	{
	case WM_CREATE:
		//
		// Create the every keypad
		//
		
		charPadHandle = WM_CreateWindowAsChild(winRect.x0, winRect.y0, width, height, hWin, 
					WM_CF_SHOW | WM_CF_MEMDEV, TP_KeyBoard_specharPadCallback, 0);
		numPadHandle = WM_CreateWindowAsChild(winRect.x0, winRect.y0, width, height, hWin, 
					WM_CF_SHOW | WM_CF_MEMDEV, TP_KeyBoard_numPadCallback, 0);
		upperPadHandle = WM_CreateWindowAsChild(winRect.x0, winRect.y0, width, height, hWin, 
					WM_CF_SHOW | WM_CF_MEMDEV, TP_KeyBoard_upperPadCallback, 0);
		
#if TP_KEYBOARD_SHOW_LOWER_LETTER
		lowerPadHandle = WM_CreateWindowAsChild(winRect.x0, winRect.y0, width, height, hWin, 
					WM_CF_SHOW | WM_CF_MEMDEV, TP_KeyBoard_lowerPadCallback, 0);
#endif
		break;
	case WM_PAINT:
		//
		// Draw background
		//
#if 0
		if (pBgBmp)
		{
			GUI_DrawBitmap(pBgBmp, winRect.x0, winRect.y0);
		}
		else
		{
			GUI_SetColor(TP_KeyBoard_default.bgColor);
   			GUI_DrawRect(winRect.x0, winRect.y0, winRect.x1, winRect.y1);
		}
#endif
		break;
	}
}

TP_KEYBOARD_Handle TP_KeyBoard_Create(WM_HWIN hParent)
{
	int x, y;

	int height;
	int lcdWidth, lcdHeight;
	const GUI_BITMAP *pBgBitmap = TP_KeyBoard_default.pBgBmp;
	TP_KEYBOARD_Handle kbHandle;

	if (pBgBitmap == NULL)
	{
		return 0;
	}

	lcdWidth = LCD_GetXSize();
	lcdHeight = LCD_GetYSize();
	height = pBgBitmap->YSize;

	x = 0;
	y = lcdHeight - height;

	kbHandle = WM_CreateWindowAsChild(x, y, lcdWidth, height, 
				//WM_HBKWIN, WM_CF_HIDE | WM_CF_STAYONTOP | WM_CF_MEMDEV, 
				hParent, WM_CF_SHOW | WM_CF_MEMDEV, 
				TP_KeyBoard_keyBoardCallback, 0);
	keyBoardHandle = kbHandle;
	return kbHandle;
}

void TP_KeyBoard_Show()
{
	GUI_MEMDEV_ShiftInWindow(keyBoardHandle, 500, GUI_MEMDEV_EDGE_BOTTOM);
}

void TP_KeyBoard_Hide()
{
	GUI_MEMDEV_ShiftOutWindow(keyBoardHandle, 500, GUI_MEMDEV_EDGE_BOTTOM);
}

