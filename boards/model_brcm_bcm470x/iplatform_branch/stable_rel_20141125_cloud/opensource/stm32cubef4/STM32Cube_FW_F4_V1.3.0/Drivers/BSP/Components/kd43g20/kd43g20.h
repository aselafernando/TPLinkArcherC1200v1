
#ifndef __BspKd43g20_H
#define __BspKd43g20_H


#include "..\Common\lcd.h"

#define LCD_BKL_PIN					GPIO_PIN_9
#define LCD_BKL_PORT				GPIOG
#define LcdBklClkEnable()				__GPIOG_CLK_ENABLE()

void     Kd43g20_Init(void);
uint16_t Kd43g20_ReadID(void);
void     Kd43g20_WriteReg(uint8_t LCD_Reg);
void     Kd43g20_WriteData(uint16_t RegValue);
uint32_t Kd43g20_ReadData(uint16_t RegValue, uint8_t ReadSize);
void     Kd43g20_DisplayOn(void);
void     Kd43g20_DisplayOff(void);
uint16_t Kd43g20_GetLcdPixelWidth(void);
uint16_t Kd43g20_GetLcdPixelHeight(void);

extern LCD_DrvTypeDef LcdKd43g20_drv;




#endif

