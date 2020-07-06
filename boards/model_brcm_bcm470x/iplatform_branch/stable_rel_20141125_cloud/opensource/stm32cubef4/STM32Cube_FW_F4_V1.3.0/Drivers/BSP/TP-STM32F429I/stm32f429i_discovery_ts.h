
#ifndef __STM32F429I_DISCOVERY_TS_H
#define __STM32F429I_DISCOVERY_TS_H

#include "stm32f429i_discovery.h"

#include "gsl1680.h"


typedef struct
{
  uint16_t TouchDetected;
  uint16_t X;
  uint16_t Y;
  uint16_t Z;
}TS_StateTypeDef;

uint8_t BSP_TS_Init(uint16_t XSize, uint16_t YSize);
void    BSP_TS_GetState(TS_StateTypeDef *TsState);
uint8_t BSP_TS_ITConfig(void);
uint8_t BSP_TS_ITGetStatus(void);
void    BSP_TS_ITClear(void);


#endif 
