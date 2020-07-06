
#include "stm32f429i_discovery_ts.h"

static TS_DrvTypeDef     *TsDrv;


/**
  * @brief  Initializes and configures the touch screen functionalities and
  *         configures all necessary hardware resources (GPIOs, clocks..).
  * @param  XSize: The maximum X size of the TS area on LCD
  * @param  YSize: The maximum Y size of the TS area on LCD
  * @retval TS_OK: if all initializations are OK. Other value if error.
  */
uint8_t BSP_TS_Init(uint16_t XSize, uint16_t YSize)
{
	TsDrv = &TpGsl1680_ts_drv;
	TsDrv->Init(0);
	TsDrv->Start(0);
	return 0;
}

/**
  * @brief  Configures and enables the touch screen interrupts.
  * @param  None
  * @retval TS_OK: if ITconfig is OK. Other value if error.
  */
uint8_t BSP_TS_ITConfig(void)
{
  /* Enable the TS ITs */
  TsDrv->EnableIT(0);

  return 0;
}

/**
  * @brief  Gets the TS IT status.
  * @param  None
  * @retval Interrupt status.
  */
uint8_t BSP_TS_ITGetStatus(void)
{
  /* Return the TS IT status */
  return (TsDrv->GetITStatus(0));
}

/**
  * @brief  Returns status and positions of the touch screen.
  * @param  TsState: Pointer to touch screen current state structure
  * @retval None.
  */
void BSP_TS_GetState(TS_StateTypeDef* TsState){
	TsState->TouchDetected = TsDrv->DetectTouch(0);
	if(TsState->TouchDetected){
		TsDrv->GetXY(0, &(TsState->X), &(TsState->Y));
	}
}

/**
  * @brief  Clears all touch screen interrupts.
  * @param  None
  * @retval None
  */
void BSP_TS_ITClear(void)
{
  /* Clear TS IT pending bits */
  TsDrv->ClearIT(0);
}


