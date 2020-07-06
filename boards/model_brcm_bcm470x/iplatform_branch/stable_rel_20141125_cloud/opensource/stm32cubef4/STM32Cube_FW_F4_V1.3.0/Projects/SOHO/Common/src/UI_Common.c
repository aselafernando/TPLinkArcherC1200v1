/******************************************************************************
*
* Copyright (c) 2010 TP-LINK Technologies CO.,LTD.
* All rights reserved.
*
* FILE NAME  :   UI_Common.h
* VERSION    :   1.0
* DESCRIPTION:   定义UI相关的共用的信息.
*
* AUTHOR     :   Huangwenzhong <Huangwenzhong@tp-link.net>
* CREATE DATE:   11/17/2014
*
* HISTORY    :
* 01   11/17/2014  Huangwenzhong     Create.
*
******************************************************************************/


#include "UI_Common.h"


/* 返回0说明两个矩形区域一致 */
I32 TP_CompareRect(GUI_RECT rect1, GUI_RECT rect2)
{
	if ((rect1.x0 == rect2.x0) && (rect1.x1 == rect2.x1)
		&& (rect1.y0 == rect2.y0) && (rect1.y1 == rect2.y1))
	{
		return 0;
	}

	return 1;
}

/* 填充抗锯齿的圆角矩形 */
void TP_AA_FillRoundedRect(I32 x0, I32 y0, I32 x1, I32 y1, I32 r)
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

