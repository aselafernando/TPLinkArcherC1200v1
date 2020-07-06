#include "TP_Widget.h"
#include "TP_Button.h"
#include "Bsp.h"
#include "cmsis_os.h"

extern GUI_CONST_STORAGE GUI_BITMAP bmSWITCH_On;

typedef struct
{
    int                 id;
    int                 x0;
    int                 y0;
    int                 width;
    int                 height;
    WM_HWIN             hParent;
    U8                  style;
    WM_CALLBACK *       cb;
    const GUI_BITMAP *  aPBitmaps[TP_BUTTON_STATE_COUNT];
    GUI_COLOR           aColors[TP_BUTTON_STATE_COUNT];
    char                text[32];
    GUI_COLOR           aTextColors[TP_BUTTON_STATE_COUNT];
    const GUI_FONT *    pFont;
    int                 align;
    U8                  colorAlpha;
    U8                  textAlpha;
    int                 isPressed;
} TP_BUTTON;

TP_BUTTON button0 =
{
    0,
    30,
    20,
    100,
    50,
    WM_HMEM_NULL,
    WM_CF_SHOW,
    NULL,
    {NULL, NULL, NULL},
    {GUI_INVALID_COLOR, GUI_INVALID_COLOR, GUI_INVALID_COLOR},
    "",
    {GUI_INVALID_COLOR, GUI_INVALID_COLOR, GUI_INVALID_COLOR},
    NULL,
    TP_TA_HCENTER_VCENTER,
    0,
    0,
    0
};

TP_BUTTON button1 =
{
    1,
    190,
    20,
    100,
    50,
    0,
    WM_CF_SHOW,
    NULL,
    {NULL, NULL, NULL},
    {GUI_CYAN, GUI_WHITE, GUI_GRAY},
    "Disabled",
    {GUI_DARKBLUE, GUI_GREEN, GUI_WHITE},
    &GUI_Font16_1,
    TP_TA_HCENTER_VCENTER,
    0,
    0,
    TP_BUTTON_DISABLED
};

TP_BUTTON button2 =
{
    2,
    350,
    20,
    100,
    50,
    0,
    WM_CF_SHOW,
    NULL,
    {&bmSWITCH_On, &bmSWITCH_On, &bmSWITCH_On},
    {GUI_CYAN, GUI_WHITE, GUI_GRAY},
    "TestBkBitmap",
    {GUI_BLUE, GUI_RED, GUI_GREEN},
    &GUI_Font16_1,
    TP_TA_RIGHT_BOTTOM,
    255,
    0,
    TP_BUTTON_UNPRESSED
};

TP_BUTTON button3 =
{
    3,
    30,
    110,
    100,
    50,
    0,
    WM_CF_SHOW,
    NULL,
    {NULL, NULL, NULL},
    {0x96cc56, GUI_GREEN, GUI_GRAY},
    "Normal",
    {GUI_WHITE, GUI_RED, GUI_GREEN},
    &GUI_Font32_1,
    TP_TA_HCENTER_VCENTER,
    0,
    0,
    TP_BUTTON_UNPRESSED
};

TP_BUTTON button4 =
{
    4,
    190,
    110,
    100,
    50,
    0,
    WM_CF_SHOW,
    NULL,
    {NULL, NULL, NULL},
    {0x96cc56, GUI_GREEN, GUI_BLACK},
    "Alpha",
    {GUI_WHITE, GUI_RED, GUI_GREEN},
    &GUI_Font32_1,
    TP_TA_HCENTER_VCENTER,
    175,
    0,
    TP_BUTTON_UNPRESSED
};

TP_BUTTON button5 =
{
    5,
    350,
    110,
    100,
    50,
    0,
    WM_CF_SHOW,
    NULL,
    {NULL, NULL, NULL},
    {0x96cc56, GUI_GREEN, GUI_GRAY},
    "LongTextTest",
    {GUI_WHITE, GUI_RED, GUI_GREEN},
    &GUI_Font32_1,
    TP_TA_HCENTER_VCENTER,
    0,
    0,
    TP_BUTTON_UNPRESSED
};

void cb(WM_MESSAGE * pMsg)
{
    GUI_RECT WinRect;
    char     acText[20] = "Custom CB";

    switch (pMsg->MsgId)
    {
    case WM_PAINT:
        GUI_SetColor(GUI_WHITE);
        GUI_SetTextMode(GUI_TM_TRANS);
        WM_GetWindowRectEx(pMsg->hWin, &WinRect);
        GUI_MoveRect(&WinRect, -WinRect.x0, -WinRect.y0);
        GUI_DispStringInRect(acText, &WinRect, GUI_TA_HCENTER | GUI_TA_VCENTER);
        break;

    default:
        TP_Button_Callback(pMsg);
    }
}

void
callback(WM_MESSAGE * pMsg)
{
    TP_BUTTON_Handle hWin;
    GUI_RECT winRect;

    hWin = pMsg->hWin;
    WM_GetWindowRectEx(hWin, &winRect);

    switch (pMsg->MsgId)
    {
    case WM_PAINT:
        GUI_SetBkColor(GUI_DARKMAGENTA);
        GUI_Clear();
        break;

    default:
        WM_DefaultProc(pMsg);
    }
}

TP_BUTTON_Handle
Create_Button(WM_HWIN hWin, TP_BUTTON button)
{
    TP_BUTTON_Handle hTP_Button;
    hTP_Button = TP_Button_Create(button.x0, button.y0, button.width, button.height, hWin,
            button.style, button.cb, strlen(button.text), button.id);

    /* Disable◊¥Ã¨≤‚ ‘ */
    if (TP_BUTTON_DISABLED == button.isPressed)
    {
        WM_DisableWindow(hTP_Button);
    }

    /* ±≥æ∞…´≤‚ ‘ */
    TP_Button_SetBkColorEx(hTP_Button, button.aColors);

    /* ±≥æ∞Õº≤‚ ‘ */
    TP_Button_SetBitmapEx(hTP_Button, button.aPBitmaps);

    /* Œƒ±æ≤‚ ‘ */
    TP_Button_SetText(hTP_Button, button.text);
    TP_Button_SetTextAlign(hTP_Button, button.align);
    TP_Button_SetTextFont(hTP_Button, button.pFont);
    TP_Button_SetTextColorEx(hTP_Button, button.aTextColors);

    /* Õ∏√˜∂»≤‚ ‘ */
    TP_Button_SetBkColorAlpha(hTP_Button, button.colorAlpha);
    TP_Button_SetTextAlpha(hTP_Button, button.textAlpha);

    return hTP_Button;
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

    hWin = WM_CreateWindowAsChild(0, 0, LCD_GetXSize(), LCD_GetYSize(), WM_HBKWIN,
            WM_CF_SHOW | WM_CF_MEMDEV, &callback, 0);

    Create_Button(hWin, button0);
    Create_Button(hWin, button1);
    Create_Button(hWin, button2);
    Create_Button(hWin, button3);
    Create_Button(hWin, button4);
    Create_Button(hWin, button5);

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
