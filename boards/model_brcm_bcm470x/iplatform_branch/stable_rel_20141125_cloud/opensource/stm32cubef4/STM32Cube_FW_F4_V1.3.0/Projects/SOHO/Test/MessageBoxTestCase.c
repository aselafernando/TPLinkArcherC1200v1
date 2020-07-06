#include <stdio.h>
#include "TP_Widget.h"
#include "TP_ScrollBar.h"
#include "TP_MessageBox.h"
#include "Bsp.h"
#include "cmsis_os.h"

extern GUI_CONST_STORAGE GUI_BITMAP bmSWITCH_On;

typedef struct
{
    int xSizeWin, ySizeWin;
    int xSizeLCD, ySizeLCD;
    int xPosWin,  yPosWin;
    WM_HWIN hWinScroll;
} PARA;

PARA TP_Para = {0};

static void _cbWin(WM_MESSAGE *pMsg)
{
    WM_MESSAGE      Message;
    WM_HWIN         hWin;
    PARA *          pPara;
    int             xPosWinNew;
    int             yPosWinNew;

    hWin = pMsg->hWin;
    WM_GetUserData(hWin, &pPara, sizeof(PARA *));
    switch (pMsg->MsgId)
    {
    case WM_MOVE:
        xPosWinNew = WM_GetWindowOrgX(hWin);
        yPosWinNew = WM_GetWindowOrgY(hWin);

        if (pPara->yPosWin != yPosWinNew)
        {
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
        break;

    case WM_PAINT:
        /* 水平滚动栏测试用 */
        // GUI_DrawGradientH(0, 0, pPara->xSizeWin, pPara->ySizeWin, 0xBB9B07, 0xF7D00A);
        /* 垂直滚动栏测试用 */
        GUI_DrawGradientV(0, 0, pPara->xSizeWin, pPara->ySizeWin, 0xBB9B07, 0xF7D00A);
        break;

    default:
        WM_DefaultProc(pMsg);
    }
}

static void Start_Thread(void const *argument)
{
    while(1)
    {
        osDelay(100);
    }
}

static void GUI_Thread(void const *argument)
{
    WM_HWIN hWin;
    PARA *pPara = &TP_Para;

    /* Just for test */
    GUI_COLOR               aButtonTextColor[TP_BUTTON_STATE_COUNT] = {0xBB9B07, GUI_RED, 0xBB9B07};
    TP_MESSAGEBOX_Handle    hMessageBox;

    WM_MOTION_Enable(1);
    pPara->xSizeLCD = LCD_GetXSize();
    pPara->ySizeLCD = LCD_GetYSize();
    pPara->xSizeWin = pPara->xSizeLCD;
    pPara->ySizeWin = pPara->ySizeLCD << 1;
    pPara->xPosWin  = 0;
    pPara->yPosWin  = 0;

    WM_SetSize(WM_HBKWIN, pPara->xSizeLCD, pPara->ySizeLCD);

    hWin = WM_CreateWindowAsChild(pPara->xPosWin, 0, pPara->xSizeWin, pPara->ySizeWin, WM_HBKWIN,
            WM_CF_SHOW | WM_CF_MEMDEV | WM_CF_MOTION_Y | WM_CF_MOTION_X, _cbWin, sizeof(PARA *));
    WM_SetUserData(hWin, &pPara, sizeof(PARA *));

    /* 水平滚动栏 */
    // pPara->hWinScroll = TP_Scrollbar_Create_X(0, 0, pPara->xSizeLCD,
    //         pPara->ySizeLCD, pPara->xSizeWin, pPara->ySizeWin, WM_HBKWIN, 0, NULL, 0, 0);

    /* 垂直滚动栏 */
    pPara->hWinScroll = TP_Scrollbar_Create_Y(0, 0, pPara->xSizeLCD, pPara->ySizeLCD,
            pPara->xSizeWin, pPara->ySizeWin, WM_HBKWIN, 0, NULL, 0, 0);

    /* Just for test */
    /* 创建双按钮消息框 */
    hMessageBox = TP_MessageBox_Create(0, 0, pPara->xSizeLCD, pPara->ySizeLCD,
            hWin, WM_CF_SHOW, NULL, 0, 0, 132, 40, 2);
    /* 设置按钮1文字 */
    TP_Button_SetTextColorEx(TP_MessageBox_GetButtonHandle(hMessageBox, 0), aButtonTextColor);
    TP_Button_SetText(TP_MessageBox_GetButtonHandle(hMessageBox, 0), "No");
    /* 设置按钮2文字 */
    TP_Button_SetTextColorEx(TP_MessageBox_GetButtonHandle(hMessageBox, 1), aButtonTextColor);
    TP_Button_SetText(TP_MessageBox_GetButtonHandle(hMessageBox, 1), "Yes");
    /* 设置消息 */
    TP_MessageBox_SetText(hMessageBox, "Are you sure you want to reboot the device?");
    /* 设置消息中图片 */
    TP_MessageBox_SetBkBitmap(hMessageBox, &bmSWITCH_On);

#if 0
    /* 创建单按钮消息框 */
    hMessageBox = TP_MessageBox_Create(0, 0, pPara->xSizeLCD, pPara->ySizeLCD,
            hWin, WM_CF_SHOW, NULL, 0, 0, 132, 40, 1);
    /* 设置按钮文字 */
    TP_Button_SetTextColorEx(TP_MessageBox_GetButtonHandle(hMessageBox, 0), aButtonTextColor);
    TP_Button_SetText(TP_MessageBox_GetButtonHandle(hMessageBox, 0), "Ok");
    /* 设置消息 */
    TP_MessageBox_SetText(hMessageBox, "The device has been reboot successs.");
    /* 设置消息中图片 */
    TP_MessageBox_SetBkBitmap(hMessageBox, &bmSWITCH_On);
#endif

    while(1)
    {
      GUI_Exec();
      osDelay(10);
    }
}

int main(void)
{
    BspInit();
    osThreadDef(t1, Start_Thread, osPriorityNormal, 0, 15 * configMINIMAL_STACK_SIZE);
    osThreadDef(t2, GUI_Thread, osPriorityHigh, 0, 2048);

    osThreadCreate (osThread(t1), NULL);
    osThreadCreate (osThread(t2), NULL);

    osKernelStart (NULL, NULL);

    for(;;);
}
