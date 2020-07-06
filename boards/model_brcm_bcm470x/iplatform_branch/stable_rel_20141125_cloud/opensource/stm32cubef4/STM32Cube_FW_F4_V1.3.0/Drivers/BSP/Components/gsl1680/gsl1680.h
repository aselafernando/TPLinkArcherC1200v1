
#ifndef __TpGsl1680_H
#define __TpGsl1680_H

#include "..\Common\ts.h"
#include "Types.h"

#define TP_I2C_SCL_PIN						GPIO_PIN_8
#define TP_I2C_SCL_GPIO_PORT				GPIOA
#define TpI2cSclClkEnable()					__GPIOA_CLK_ENABLE()
#define TP_I2C_SDA_PIN						GPIO_PIN_9
#define TP_I2C_SDA_GPIO_PORT				GPIOC
#define TpI2cSdaClkEnable()					__GPIOC_CLK_ENABLE()
#define TP_INT_GPIO_PORT					GPIOA
#define TP_INT_PIN							GPIO_PIN_15
#define TpI2cIntClkEnable()					__GPIOA_CLK_ENABLE()

void     TpGsl1680_Init(WORD DeviceAddr);
void     TpGsl1680_Reset(WORD DeviceAddr);
WORD TpGsl1680_ReadID(WORD DeviceAddr);
void     TpGsl1680_TS_Start(WORD DeviceAddr);
BYTE  TpGsl1680_TS_DetectTouch(WORD DeviceAddr);
void     TpGsl1680_TS_GetXY(WORD DeviceAddr, WORD *X, WORD *Y);
void     TpGsl1680_TS_EnableIT(WORD DeviceAddr);
void     TpGsl1680_TS_DisableIT(WORD DeviceAddr);
BYTE  TpGsl1680_TS_ITStatus (WORD DeviceAddr);
void     TpGsl1680_TS_ClearIT (WORD DeviceAddr);

extern TS_DrvTypeDef TpGsl1680_ts_drv;

#endif

