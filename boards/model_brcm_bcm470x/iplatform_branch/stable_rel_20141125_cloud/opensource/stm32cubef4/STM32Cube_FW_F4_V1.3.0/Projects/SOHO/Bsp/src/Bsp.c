
#include "Types.h"
#include "Bsp.h"
#include "GUI.h"
#include "Wm.h"

uint8_t GUI_Initialized = 0;
TIM_HandleTypeDef    TimHandle;
uint32_t uwPrescalerValue = 0;

static void SystemClockConfig(void){
	RCC_ClkInitTypeDef RCC_ClkInitStruct;
	RCC_OscInitTypeDef RCC_OscInitStruct;
	__PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 8;
	RCC_OscInitStruct.PLL.PLLN = 360;//336;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 7;
	HAL_RCC_OscConfig(&RCC_OscInitStruct);
	HAL_PWREx_ActivateOverDrive();
	RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
	HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);
}



void InitTimForTs()
{
	 /***********************************************************/

	/* Compute the prescaler value to have TIM3 counter clock equal to 10 KHz */
	uwPrescalerValue = (uint32_t) ((SystemCoreClock /2) / 10000) - 1;

	/* Set TIMx instance */
	TimHandle.Instance = TIM3;

	/* Initialize TIM3 peripheral as follows:
	   + Period = 500 - 1
	   + Prescaler = ((SystemCoreClock/2)/10000) - 1
	   + ClockDivision = 0
	   + Counter direction = Up
	*/
	TimHandle.Init.Period = 500 - 1;
	TimHandle.Init.Prescaler = uwPrescalerValue;
	TimHandle.Init.ClockDivision = 0;
	TimHandle.Init.CounterMode = TIM_COUNTERMODE_UP;
	if(HAL_TIM_Base_Init(&TimHandle) != HAL_OK)
	{
		while(1) 
		{
		}
	}

	/*##-2- Start the TIM Base generation in interrupt mode ####################*/
	/* Start Channel1 */
	if(HAL_TIM_Base_Start_IT(&TimHandle) != HAL_OK)
	{
		while(1) 
		{
		}
	}
}

void TouchUpdate(void){
	GUI_PID_STATE TS_State;
	static TS_StateTypeDef prev_state;
	volatile TS_StateTypeDef  ts;
	WORD xDiff, yDiff;
	BSP_TS_GetState((TS_StateTypeDef *)&ts);
	TS_State.Pressed = ts.TouchDetected;
	xDiff = (prev_state.X > ts.X) ? (prev_state.X - ts.X) : (ts.X - prev_state.X);
	yDiff = (prev_state.Y > ts.Y) ? (prev_state.Y - ts.Y) : (ts.Y - prev_state.Y);
	if((prev_state.TouchDetected != ts.TouchDetected )||
		(xDiff > 3 )||
		(yDiff > 3))
	{
		prev_state.TouchDetected = ts.TouchDetected;
		if((ts.X != 0) &&  (ts.Y != 0)){
			prev_state.X = ts.X;
			prev_state.Y = ts.Y;
		}
		TS_State.Layer = 1;
		TS_State.x = prev_state.X;
		TS_State.y = prev_state.Y;
		GUI_TOUCH_StoreStateEx(&TS_State);
	}
}

void TS_Background(void)
{
	/* Capture input event and updade cursor */
	if(GUI_Initialized == 1)
	{
		TouchUpdate();
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{	
	TS_Background();
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
  /*##-1- Enable peripherals and GPIO Clocks #################################*/
  /* TIMx Peripheral clock enable */
  __TIM3_CLK_ENABLE();

  /*##-2- Configure the NVIC for TIMx #########################################*/
  /* Set the TIMx priority */
  HAL_NVIC_SetPriority(TIM3_IRQn, 0, 1);
  
  /* Enable the TIMx global Interrupt */
  HAL_NVIC_EnableIRQ(TIM3_IRQn);
}


void BspInit(void){
	
	HAL_Init();
	SystemClockConfig();
	BSP_SDRAM_Init();
	InitTimForTs();
	BSP_TS_Init(0, 0);//Initialize the Touch screen
	__CRC_CLK_ENABLE();//Enable CRC to Unlock GUI
	 /* Activate the use of memory device feature */
 	//WM_SetCreateFlags(WM_CF_MEMDEV);
	GUI_Init();
	GUI_Initialized = 1;
	WM_MULTIBUF_Enable(1);
	GUI_SelectLayer(1);
}



