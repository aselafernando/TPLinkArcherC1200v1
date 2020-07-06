#ifndef _TP_GLOBAL_H
#define _TP_GLOBAL_H

#include "TP_Type.h"
#include "GUI.h"

#ifndef GUI_CONST_STORAGE
	#define GUI_CONST_STORAGE const
#endif


#define RET_SUCCESS			(0)
#define RET_FAIL			(-1)


#define WM_PID_STATE_PRESSED	(1)
#define WM_PID_STATE_UNPRESSED	(0)

/* For ActionBar */
#define TP_AB_BASE_ID	GUI_ID_USER
#define TP_AB_HANDLE_ID	(TP_AB_BASE_ID + 0x01)
#define TP_AB_UP_ID		(TP_AB_BASE_ID + 0x02)
#define TP_AB_HOME_ID	(TP_AB_BASE_ID + 0x03)
#define TP_AB_TITLE_ID	(TP_AB_BASE_ID + 0x04)

#define TP_AB_BUTTON_ID_0 (TP_AB_BASE_ID + 0x10)
#define TP_AB_BUTTON_ID_1 (TP_AB_BASE_ID + 0x11)
#define TP_AB_BUTTON_ID_2 (TP_AB_BASE_ID + 0x12)
#define TP_AB_BUTTON_ID_3 (TP_AB_BASE_ID + 0x13)
#define TP_AB_BUTTON_ID_4 (TP_AB_BASE_ID + 0x14)

/* For Keyboard */
#define TP_KB_BASE_ID		(GUI_ID_USER + 32)
#define TP_KB_LOWER_ID		(TP_KB_BASE_ID)
#define TP_KB_UPPER_ID		(TP_KB_BASE_ID + 40)
#define TP_KB_NUM_ID		(TP_KB_BASE_ID + 80)
#define TP_KB_CHAR_ID		(TP_KB_BASE_ID + 120)

/* For WM Message */
#define WM_TP_MENU (WM_USER + 0x21)
#define WM_TP_ACTION_BAR	(WM_USER + 0x22)
/* For Scrollbar message */
#define TP_MSG_SCROLLBAR_TIMEOUT            (WM_USER+0x23)
#define TP_MSG_SCROLLBAR_X                  (WM_USER+0x24)
#define TP_MSG_SCROLLBAR_Y                  (WM_USER+0x25)

#define TP_MALLOC	malloc
#define TP_MEMSET	memset
#define TP_MEMCPY	GUI_MEMCPY
#define TP_FREE		free

#define TP_GUI_LOCK()       WM_LOCK()
#define TP_GUI_UNLOCK()     WM_UNLOCK()

/* 默认字体 */
extern GUI_CONST_STORAGE GUI_FONT GUI_FontF16DqW3;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontF12DqW3;

/* 文本对齐方式 */
/* 左上方 */
#define TP_TA_LEFT_TOP           (GUI_TA_LEFT | GUI_TA_TOP)
/* 正上方 */
#define TP_TA_HCENTER_TOP        (GUI_TA_HCENTER | GUI_TA_TOP)
/* 右上方 */
#define TP_TA_RIGHT_TOP          (GUI_TA_RIGHT | GUI_TA_TOP)
/* 正左边 */
#define TP_TA_LEFT_VCENTER       (GUI_TA_LEFT | GUI_TA_VCENTER)
/* 正中间 */
#define TP_TA_HCENTER_VCENTER    (GUI_TA_HCENTER | GUI_TA_VCENTER)
/* 正右边 */
#define TP_TA_RIGHT_VCENTER      (GUI_TA_RIGHT | GUI_TA_VCENTER)
/* 左下方 */
#define TP_TA_LEFT_BOTTOM        (GUI_TA_LEFT | GUI_TA_BOTTOM)
/* 正下方 */
#define TP_TA_HCENTER_BOTTOM     (GUI_TA_HCENTER | GUI_TA_BOTTOM)
/* 右下方 */
#define TP_TA_RIGHT_BOTTOM       (GUI_TA_RIGHT | GUI_TA_BOTTOM)

/* 返回0说明两个矩形区域一致 */
static int TP_CompareRect(GUI_RECT rect1, GUI_RECT rect2)
{
	if ((rect1.x0 == rect2.x0) && (rect1.x1 == rect2.x1)
		&& (rect1.y0 == rect2.y0) && (rect1.y1 == rect2.y1))
	{
		return 0;
	}

	return 1;
}

/* 填充抗锯齿的圆角矩形 */
static void TP_AA_FillRoundedRect(int x0, int y0, int x1, int y1, int r)
{
    GUI_AA_SetFactor(4);
    GUI_AA_FillCircle(x0 + r, y0 + r, r);
    GUI_AA_FillCircle(x1 - r, y0 + r, r);
    GUI_AA_FillCircle(x0 + r, y1 - r, r);
    GUI_AA_FillCircle(x1 - r, y1 - r, r);
    GUI_AA_SetFactor(1);
    GUI_FillRect(x0 + r, y0, x1 - r, y0 + r);
    GUI_FillRect(x0, y0 + r, x1, y1 - r);
    GUI_FillRect(x0 + r, y1 - r, x1 - r, y1);
}

#endif
