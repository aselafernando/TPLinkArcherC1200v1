#include "stm32f4xx_hal.h"
#include "kd43g20.h"

LCD_DrvTypeDef LcdKd43g20_drv= 
{
	Kd43g20_Init,
	Kd43g20_ReadID,
	Kd43g20_DisplayOn,
	Kd43g20_DisplayOff,
	0,
	0,
	0,
	0,
	0,
	0,
	Kd43g20_GetLcdPixelWidth,
	Kd43g20_GetLcdPixelHeight,
	0,
	0,    
};

void Kd43g20_Init(void){
	GPIO_InitTypeDef  GPIO_InitStruct;
	LcdBklClkEnable();
	GPIO_InitStruct.Pin = LCD_BKL_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FAST;  
	HAL_GPIO_Init(LCD_BKL_PORT, &GPIO_InitStruct);  
	HAL_GPIO_WritePin(LCD_BKL_PORT, LCD_BKL_PIN, GPIO_PIN_SET);
}

/**
  * @brief  Disables the Display.
  * @param  None
  * @retval LCD Register Value.
  */
uint16_t Kd43g20_ReadID(void)
{
  return 0;
}

/**
  * @brief  Enables the Display.
  * @param  None
  * @retval None
  */
void Kd43g20_DisplayOn(void)
{
	HAL_GPIO_WritePin(LCD_BKL_PORT, LCD_BKL_PIN, GPIO_PIN_SET);
}

/**
  * @brief  Disables the Display.
  * @param  None
  * @retval None
  */
void Kd43g20_DisplayOff(void)
{
  /* Display Off */
  HAL_GPIO_WritePin(LCD_BKL_PORT, LCD_BKL_PIN, GPIO_PIN_RESET);
}

/**
  * @brief  Writes  to the selected LCD register.
  * @param  LCD_Reg: address of the selected register.
  * @retval None
  */
void Kd43g20_WriteReg(uint8_t LCD_Reg)
{
 
}

/**
  * @brief  Writes data to the selected LCD register.
  * @param  LCD_Reg: address of the selected register.
  * @retval None
  */
void Kd43g20_WriteData(uint16_t RegValue)
{
 
}

/**
  * @brief  Reads the selected LCD Register.
  * @param  RegValue: Address of the register to read
  * @param  ReadSize: Number of bytes to read
  * @retval LCD Register Value.
  */
uint32_t Kd43g20_ReadData(uint16_t RegValue, uint8_t ReadSize)
{
  /* Read a max of 4 bytes */
  return 0;
}

/**
  * @brief  Get LCD PIXEL WIDTH.
  * @param  None
  * @retval LCD PIXEL WIDTH.
  */
uint16_t Kd43g20_GetLcdPixelWidth(void)
{
  return 480;
}

/**
  * @brief  Get LCD PIXEL HEIGHT.
  * @param  None
  * @retval LCD PIXEL HEIGHT.
  */
uint16_t Kd43g20_GetLcdPixelHeight(void)
{
  return 272;
}

