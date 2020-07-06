#ifndef _TP_GLOBAL_H_
#define _TP_GLOBAL_H_

#include "TP_Type.h"
#include "GUI.h"
#include "UI_Common.h"
#include "UI_Image.h"
#include "Common.h"
#include "UI_ID.h"

#ifndef GUI_CONST_STORAGE
	#define GUI_CONST_STORAGE const
#endif

#define WM_PID_STATE_PRESSED	(1)
#define WM_PID_STATE_UNPRESSED	(0)

/* For WM Message */
#define WM_TP_MENU (WM_USER + 0x21)
#define WM_TP_ACTION_BAR	(WM_USER + 0x22)

/* For Scrollbar message */
#define TP_MSG_SCROLLBAR_TIMEOUT            (WM_USER + 0x23)
#define TP_MSG_SCROLLBAR_X                  (WM_USER + 0x24)
#define TP_MSG_SCROLLBAR_Y                  (WM_USER + 0x25)

/* for dropdown scroll msg */
#define TP_MSG_DROPDOWN_Y_MOVE (WM_USER + 0x26)

/* for fakelist scorll msg */
#define TP_MSG_FAKELIST_Y_MOVE (WM_USER + 0x27)

/* for edit slider */
#define TP_MSG_SLIDER_TO_EDIT (WM_USER + 0x28)
#define TP_MSG_EDIT_TO_SLIDER_FIRST (WM_USER + 0x29)
#define TP_MSG_EDIT_TO_SLIDER (WM_USER + 0x2a)
#define TP_MSG_SLIDER_MOVE_AUTO_R (WM_USER + 0x2b)
#define TP_MSG_EDIT_AUTO_LEFT (WM_USER + 0x2c)
#define TP_MSG_EDIT_AUTO_RIGHT (WM_USER + 0x2d)

typedef enum _TP_BORDER_INDEX
{
	TP_BORDER_MIN = 0,
	TP_BORDER_TOP = 0,
	TP_BORDER_RIGHT,
	TP_BORDER_BOTTOM,
	TP_BORDER_LEFT,
	TP_BORDER_MAX
}TP_BORDER_INDEX;

typedef enum _TP_SPACE_INDEX
{
	TP_SPACE_MIN = 0,
	TP_SPACE_TB = 0,
	TP_SPACE_LR,
	TP_SPACE_MAX
}TP_SPACE_INDEX;

#endif
