#include "TP_Global.h"
#include "TP_Scrollbar.h"

typedef struct PARA PARA;

struct PARA {
  int xSizeWin, ySizeWin;
  int xSizeLCD, ySizeLCD;
  int xPosWin, yPosWin;
  int isMoving, isScroll;
  WM_HWIN hWinScroll;
  WM_HWIN hWinMain;
};

PARA TP_Para = {0};

static void _cbBkWindow(WM_MESSAGE * pMsg)
{
	WM_HWIN hWin;

	hWin = pMsg->hWin;
	switch (pMsg->MsgId) {
	case WM_NOTIFY_PARENT:
        switch (pMsg->Data.v) {
            case TP_MSG_SCROLLBAR_TIMEOUT:
                /* 移动松开后停止继续显示50ms消失 */
                WM_CreateTimer(hWin, 0, 50, 0);
                /* 移动松开后如果有速度则不在50ms后消失 */
                TP_Para.isScroll = 1;
            break;
        }
        break;

	case WM_TIMER:
        if ((TP_Para.isMoving == 1) && (TP_Para.isScroll == 1))
        {
            /* 在此表示移动松开后仍有速度，令滚动条继续显示500ms(粗略估算出的继续滑动时间,FIXME) */
            TP_Para.isScroll = 0;
            WM_CreateTimer(hWin, 0, 500, 0);
        }
        else
        {
            WM_HideWindow(TP_Para.hWinScroll);
        }
        break;
    case WM_PAINT:
        GUI_SetBkColor(GUI_WHITE);
        GUI_Clear();
	default:
        WM_DefaultProc(pMsg);
	}
}

static void _cbWin(WM_MESSAGE *pMsg)
{
    WM_MESSAGE Message;
	WM_HWIN hWin;
	PARA *pPara;
	int xPosWinNew;
	int yPosWinNew;

	hWin = pMsg->hWin;
	WM_GetUserData(hWin, &pPara, sizeof(PARA *));
	switch (pMsg->MsgId) {
	case WM_DELETE:
        break;
	case WM_TOUCH:
        break;
	case WM_PID_STATE_CHANGED:
        if (pPara->isMoving == 1)
        {
            /* 松开后先令isMoving置0，并通知父窗口进行超时处理，
             * 若isMoving被重新置为1，则表示松开后仍有速度，则滚动条继续显示一段时间 */
            pPara->isMoving = 0;
            WM_NotifyParent(hWin, TP_MSG_SCROLLBAR_TIMEOUT);
        }
        break;
	case WM_NOTIFY_PARENT:
        switch (pMsg->Data.v) {
            case WM_NOTIFICATION_RELEASED:
                if (pPara->isMoving == 1)
                {
                    pPara->isMoving = 0;
                    WM_NotifyParent(hWin, TP_MSG_SCROLLBAR_TIMEOUT);
                    /* 此处为移动松开，不进行具体控件操作 */
                }
                else if (pPara->isMoving == 0)
                {
                    /* 此处为点击松开，在此进行具体控件操作 */
                    WM_HideWindow(TP_Para.hWinScroll);
                }
        }
        break;

	case WM_MOVE:
        xPosWinNew = WM_GetWindowOrgX(hWin);
        yPosWinNew = WM_GetWindowOrgY(hWin);

        if (pPara->yPosWin != yPosWinNew) {
            /* 垂直滑动，将滑动条起点纵坐标发给滑动条令其绘制 */
            pPara->yPosWin = yPosWinNew;

            Message.MsgId = TP_MSG_SCROLLBAR_Y;
            Message.Data.p = (void *)(((0-pPara->yPosWin) * pPara->ySizeLCD) / pPara->ySizeWin);
            WM_SendMessage(pPara->hWinScroll, &Message);
            WM_ShowWindow(pPara->hWinScroll);
        }
        else if (pPara->xPosWin != xPosWinNew)
        {
            /* 水平滑动，将滑动条起点横坐标发给滑动条令其绘制 */
            pPara->xPosWin = xPosWinNew;

            Message.MsgId = TP_MSG_SCROLLBAR_X;
            Message.Data.p = (void *)(((0-pPara->xPosWin) * pPara->xSizeLCD) / pPara->xSizeWin);
            WM_SendMessage(pPara->hWinScroll, &Message);
            WM_ShowWindow(pPara->hWinScroll);
        }
        pPara->isMoving = 1;

        break;

	case WM_PAINT:
        /* 水平滚动栏测试用 */
        //GUI_DrawGradientH(0, 0, pPara->xSizeWin, pPara->ySizeWin, 0x0000FF, 0x00FFFF);
        /* 垂直滚动栏测试用 */
        GUI_DrawGradientV(0, 0, pPara->xSizeWin, pPara->ySizeWin, 0x0000FF, 0x00FFFF);
        break;
	default:
        WM_DefaultProc(pMsg);
	}
}

void
MainTask(void)
{
    WM_HWIN hWin;
	PARA *pPara = &TP_Para;

    GUI_Init();

	WM_MOTION_Enable(1);
    pPara->isMoving = 0;
    pPara->isScroll = 0;
	pPara->xSizeLCD = LCD_GetXSize();
	pPara->ySizeLCD = LCD_GetYSize();
	pPara->xSizeWin = pPara->xSizeLCD << 1;
	pPara->ySizeWin = pPara->ySizeLCD << 1;
	pPara->xPosWin  = 0;
	pPara->yPosWin  = 0;

	WM_SetSize(WM_HBKWIN, pPara->xSizeLCD, pPara->ySizeLCD);
	WM_SetCallback(WM_HBKWIN, _cbBkWindow);

    hWin = WM_CreateWindowAsChild(pPara->xPosWin, 0, pPara->xSizeWin, pPara->ySizeWin, WM_HBKWIN,
            WM_CF_SHOW | WM_CF_MEMDEV | WM_CF_MOTION_Y | WM_CF_MOTION_X, _cbWin, sizeof(PARA *));
	WM_SetUserData(hWin, &pPara, sizeof(PARA *));

    /* 水平滚动栏 */
    //pPara->hWinScroll = TP_Scrollbar_Create_X(0, 0, pPara->xSizeLCD, pPara->ySizeLCD, pPara->xSizeWin, pPara->ySizeWin, WM_HBKWIN, 0, NULL, 0, 0);

    /* 垂直滚动栏 */
    pPara->hWinScroll = TP_Scrollbar_Create_Y(0, 0, pPara->xSizeLCD, pPara->ySizeLCD, pPara->xSizeWin, pPara->ySizeWin, WM_HBKWIN, 0, NULL, 0, 0);

    while (1) {
        GUI_Delay(1000);
        GUI_Exec();
    }
}
