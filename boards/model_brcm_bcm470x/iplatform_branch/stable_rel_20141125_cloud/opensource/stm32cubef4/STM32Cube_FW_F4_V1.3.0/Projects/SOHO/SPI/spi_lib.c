

#include "stm32f429i_discovery.h"

#include "stm32f4xx_it.h"

#include "defs.h"


#include <stdlib.h>
#include <string.h>
#include <stdio.h>
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* User can use this section to tailor SPIx instance used and associated 
   resources */
/* Definition for SPIx clock resources */
#define SPIx                             SPI4
#define SPIx_CLK_ENABLE()                __SPI4_CLK_ENABLE()
#define SPIx_SCK_GPIO_CLK_ENABLE()       __GPIOE_CLK_ENABLE()
#define SPIx_MISO_GPIO_CLK_ENABLE()      __GPIOE_CLK_ENABLE() 
#define SPIx_MOSI_GPIO_CLK_ENABLE()      __GPIOE_CLK_ENABLE() 

#define SPIx_FORCE_RESET()               __SPI4_FORCE_RESET()
#define SPIx_RELEASE_RESET()             __SPI4_RELEASE_RESET()

/* Definition for SPIx Pins */
#define SPIx_SCK_PIN                     GPIO_PIN_2
#define SPIx_SCK_GPIO_PORT               GPIOE
#define SPIx_SCK_AF                      GPIO_AF5_SPI4
#define SPIx_MISO_PIN                    GPIO_PIN_5
#define SPIx_MISO_GPIO_PORT              GPIOE
#define SPIx_MISO_AF                     GPIO_AF5_SPI4
#define SPIx_MOSI_PIN                    GPIO_PIN_6
#define SPIx_MOSI_GPIO_PORT              GPIOE
#define SPIx_MOSI_AF                     GPIO_AF5_SPI4

/* Definition for SPIx's NVIC */
#define SPIx_IRQn                        SPI4_IRQn
#define SPIx_IRQHandler                  SPI4_IRQHandler

/* Size of buffer */
#define BUFFERSIZE                       (COUNTOF(aTxBuffer) - 1)

/* Exported macro ------------------------------------------------------------*/
#define COUNTOF(__BUFFER__)   (sizeof(__BUFFER__) / sizeof(*(__BUFFER__)))
/* Exported functions ------------------------------------------------------- */
/* Exported functions ------------------------------------------------------- */
void SPIx_IRQHandler(void);



SPI_HandleTypeDef SpiHandle;

 

static int init(void)
{

  SPIx_FORCE_RESET();
  SPIx_RELEASE_RESET();
 
  
  SpiHandle.Instance               = SPIx;
  SpiHandle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
  SpiHandle.Init.Direction         = SPI_DIRECTION_2LINES;
  SpiHandle.Init.CLKPhase          = SPI_PHASE_1EDGE ;
  SpiHandle.Init.CLKPolarity       = SPI_POLARITY_LOW ;
  SpiHandle.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLED;
  SpiHandle.Init.CRCPolynomial     = 7;
  SpiHandle.Init.DataSize          = SPI_DATASIZE_8BIT;
  SpiHandle.Init.FirstBit          =  SPI_FIRSTBIT_MSB;
  SpiHandle.Init.NSS               = SPI_NSS_SOFT;
  SpiHandle.Init.TIMode            =   SPI_TIMODE_DISABLED;
  SpiHandle.Init.Mode = SPI_MODE_SLAVE;

  if(HAL_SPI_Init(&SpiHandle) != HAL_OK)
   {
    /* Initialization Error */
    return -1;
   }
  

  return 0;

}

int spi_raw_send(uint8_t* buf,uint16_t size,uint32_t timeout)
{
  unsigned int count = HAL_GetTick() +timeout;
 
  init();

  HAL_SPI_Transmit_IT(&SpiHandle, buf,size);
  while (HAL_SPI_GetState(&SpiHandle) != HAL_SPI_STATE_READY)
  {
    if (count < HAL_GetTick())
    {
      
      return -1;
    }
    
    HAL_Delay(1);
 
  } 
  

  return 0;
}

int spi_raw_receive(uint8_t* buf,uint16_t size,uint32_t timeout)
{
  unsigned int count = HAL_GetTick() +timeout;
  init();

  HAL_SPI_Receive_IT(&SpiHandle, buf,size);
  while (HAL_SPI_GetState(&SpiHandle) != HAL_SPI_STATE_READY)
  {
     if (count < HAL_GetTick())
    {
      return -1;
    }
    
    HAL_Delay(1);
  } 

  return 0;
}

 




void SPIx_IRQHandler(void)
{
 HAL_SPI_IRQHandler(&SpiHandle);
}




void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
  GPIO_InitTypeDef  GPIO_InitStruct;
  
  /*##-1- Enable peripherals and GPIO Clocks #################################*/
  /* Enable GPIO TX/RX clock */
  SPIx_SCK_GPIO_CLK_ENABLE();
  SPIx_MISO_GPIO_CLK_ENABLE();
  SPIx_MOSI_GPIO_CLK_ENABLE();
  /* Enable SPI clock */
  SPIx_CLK_ENABLE(); 
  
  /*##-2- Configure peripheral GPIO ##########################################*/  
  /* SPI SCK GPIO pin configuration  */
  GPIO_InitStruct.Pin       = SPIx_SCK_PIN;
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull      = GPIO_PULLUP;
  GPIO_InitStruct.Speed     = GPIO_SPEED_FAST;
  GPIO_InitStruct.Alternate = SPIx_SCK_AF;
  
  HAL_GPIO_Init(SPIx_SCK_GPIO_PORT, &GPIO_InitStruct);
    
  /* SPI MISO GPIO pin configuration  */
  GPIO_InitStruct.Pin = SPIx_MISO_PIN;
  GPIO_InitStruct.Alternate = SPIx_MISO_AF;
  
  HAL_GPIO_Init(SPIx_MISO_GPIO_PORT, &GPIO_InitStruct);
  
  /* SPI MOSI GPIO pin configuration  */
  GPIO_InitStruct.Pin = SPIx_MOSI_PIN;
  GPIO_InitStruct.Alternate = SPIx_MOSI_AF;
    
  HAL_GPIO_Init(SPIx_MOSI_GPIO_PORT, &GPIO_InitStruct);

  /*##-3- Configure the NVIC for SPI #########################################*/
  /* NVIC for SPI */
  HAL_NVIC_SetPriority(SPIx_IRQn, 0, 2);
  HAL_NVIC_EnableIRQ(SPIx_IRQn);
}

/**
  * @brief SPI MSP De-Initialization 
  *        This function frees the hardware resources used in this example:
  *          - Disable the Peripheral's clock
  *          - Revert GPIO and NVIC configuration to their default state
  * @param hspi: SPI handle pointer
  * @retval None
  */
void HAL_SPI_MspDeInit(SPI_HandleTypeDef *hspi)
{
  /*##-1- Reset peripherals ##################################################*/
  SPIx_FORCE_RESET();
  SPIx_RELEASE_RESET();

  /*##-2- Disable peripherals and GPIO Clocks ################################*/
  /* Configure SPI SCK as alternate function  */
  HAL_GPIO_DeInit(SPIx_SCK_GPIO_PORT, SPIx_SCK_PIN);
  /* Configure SPI MISO as alternate function  */
  HAL_GPIO_DeInit(SPIx_MISO_GPIO_PORT, SPIx_MISO_PIN);
  /* Configure SPI MOSI as alternate function  */
  HAL_GPIO_DeInit(SPIx_MOSI_GPIO_PORT, SPIx_MOSI_PIN);
  
  /*##-3- Disable the NVIC for SPI ###########################################*/
  HAL_NVIC_DisableIRQ(SPIx_IRQn);
}