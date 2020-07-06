
#include "stm32f4xx_hal.h"
#include "gsl1680.h"
#include "mGsl1680.h"
#include "mSwI2c.h"

static GPIO_InitTypeDef  gSwI2cGpioStruct= {
	TP_I2C_SCL_PIN, //uint32_t Pin;
	GPIO_MODE_OUTPUT_OD, //uint32_t Mode;
	GPIO_PULLUP, //uint32_t Pull;
	GPIO_SPEED_FAST, //uint32_t Speed;
	0 //uint32_t Alternate;
};

void McuI2CInit(void){
	TpI2cSclClkEnable();
	TpI2cSdaClkEnable();
	TpI2cIntClkEnable();
	gSwI2cGpioStruct.Mode= GPIO_MODE_INPUT;
	gSwI2cGpioStruct.Pin = TP_INT_PIN;
	HAL_GPIO_Init(TP_INT_GPIO_PORT, (GPIO_InitTypeDef*)&gSwI2cGpioStruct);
	gSwI2cGpioStruct.Mode= GPIO_MODE_OUTPUT_OD;
	gSwI2cGpioStruct.Pin = TP_I2C_SCL_PIN;
	HAL_GPIO_Init(TP_I2C_SCL_GPIO_PORT, (GPIO_InitTypeDef*)&gSwI2cGpioStruct);
	gSwI2cGpioStruct.Pin = TP_I2C_SDA_PIN;
	HAL_GPIO_Init(TP_I2C_SDA_GPIO_PORT, (GPIO_InitTypeDef*)&gSwI2cGpioStruct);
}
void McuSwI2cDelayBit(void){
	volatile WORD wTemp= 13;
	while (wTemp--);
}
void McuSwI2cDelayByte(void){
	//volatile WORD wTemp= 400;
	//while (wTemp--);
}
HiLowType GetTpIntPin(void){
	if (HAL_GPIO_ReadPin(TP_INT_GPIO_PORT, TP_INT_PIN)){
		return ST_HIGH;
	}
	else{
		return ST_LOW;
	}
}
void McuSetScl(void){
	HAL_GPIO_WritePin(TP_I2C_SCL_GPIO_PORT, TP_I2C_SCL_PIN, GPIO_PIN_SET);
}
void McuClrScl(void){
	HAL_GPIO_WritePin(TP_I2C_SCL_GPIO_PORT, TP_I2C_SCL_PIN, GPIO_PIN_RESET);
}
void McuSetSda(void){
	HAL_GPIO_WritePin(TP_I2C_SDA_GPIO_PORT, TP_I2C_SDA_PIN, GPIO_PIN_SET);
}
void McuClrSda(void){
	HAL_GPIO_WritePin(TP_I2C_SDA_GPIO_PORT, TP_I2C_SDA_PIN, GPIO_PIN_RESET);
}
HiLowType McuGetSda(void){
	HiLowType Ret;
	gSwI2cGpioStruct.Pin = TP_I2C_SDA_PIN;
	gSwI2cGpioStruct.Mode = GPIO_MODE_INPUT;
	HAL_GPIO_Init(TP_I2C_SDA_GPIO_PORT, (GPIO_InitTypeDef*)&gSwI2cGpioStruct);
	if (HAL_GPIO_ReadPin(TP_I2C_SDA_GPIO_PORT, TP_I2C_SDA_PIN)){
		Ret= ST_HIGH;
	}
	else{
		Ret= ST_LOW;
	}
	gSwI2cGpioStruct.Mode = GPIO_MODE_OUTPUT_OD;
	HAL_GPIO_Init(TP_I2C_SDA_GPIO_PORT, &gSwI2cGpioStruct);
	return Ret;
}
HiLowType McuGetScl(void){
	HiLowType Ret;
	gSwI2cGpioStruct.Pin = TP_I2C_SCL_PIN;
	gSwI2cGpioStruct.Mode = GPIO_MODE_INPUT;
	HAL_GPIO_Init(TP_I2C_SCL_GPIO_PORT, &gSwI2cGpioStruct);
	if (HAL_GPIO_ReadPin(TP_I2C_SCL_GPIO_PORT, TP_I2C_SCL_PIN)){
		Ret= ST_HIGH;
	}
	else{
		Ret= ST_LOW;
	}
	gSwI2cGpioStruct.Mode = GPIO_MODE_OUTPUT_OD;
	HAL_GPIO_Init(TP_I2C_SCL_GPIO_PORT, &gSwI2cGpioStruct);
	return Ret;
}

const SwI2cType gSwI2c= {
	500, //WORD wCheckPinDummy;
	3, //BYTE ucAccessDummyTime;
	McuSwI2cDelayBit, McuSwI2cDelayByte, //fpSwI2cDelay SwI2cDelayBit, SwI2cDelayByte;
	McuSetScl, McuClrScl, //fpSwI2cOutput SetScl, ClrScl;
	McuSetSda, McuClrSda, //fpSwI2cOutput SetSda, ClrSda;
	McuGetScl, McuGetSda//fpSwI2cInput GetScl, GetSda;
};

void McuI2cWriteByte(
	BYTE ucDeviceAddr,
	BYTE ucByteAddr,
	BYTE ucValue)
{
	SwI2cWriteByte(
		&gSwI2c, //SwI2cType* pI2c,
		ucDeviceAddr, //BYTE ucSlaveAdr,
		ucByteAddr, //BYTE ucSubAdr,
		ucValue //BYTE ucVal
		);
}

BYTE McuI2cReadByte(
	BYTE ucDeviceAddr,
	BYTE ucByteAddr)
{
	return SwI2cReadByte(
		&gSwI2c, //SwI2cType* pI2c,
		ucDeviceAddr, //BYTE ucSlaveAdr,
		ucByteAddr //BYTE ucSubAdr
		);
}

void McuI2CReadBuffer(BYTE ucDeviceAddr, BYTE ucOffset, BYTE* pucBuff, BYTE ucLen){
	SwI2cBurstReadBytes(
		&gSwI2c, //SwI2cType* pI2c,
		ucDeviceAddr, //BYTE ucSlaveAdr,
		ucOffset, //BYTE ucSubAdr,
		pucBuff, //BYTE *pucBuff,
		ucLen //WORD ucBufLen
		);
}
void McuI2CWriteBuffer(BYTE ucDeviceAddr, BYTE ucOffset, BYTE* pucBuff, BYTE ucLen){
	SwI2cBurstWriteBytes(
		&gSwI2c, //SwI2cType* pI2c,
		ucDeviceAddr, //BYTE ucSlaveAdr,
		ucOffset, //BYTE ucSubAdr,
		pucBuff, //BYTE *pucBuff,
		ucLen //WORD ucBufLen
		);
}

static void TpGsl1680Delay(DWORD dwTime){
	HAL_Delay(dwTime);
}


Gsl1680ConstType gGsl1680Const= {
	1, //BYTE ucUsedFingers;
	TpGsl1680Delay, //void (*Delay)(DWORD dwTime);
	McuI2CReadBuffer, //void (*I2CReadBuffer)(BYTE ucDeviceAddr, BYTE ucReg, BYTE* pucBuff, BYTE ucLen);
	McuI2CWriteBuffer //void (*I2CWriteBuffer)(BYTE ucDeviceAddr, BYTE ucReg, BYTE* pucBuff, BYTE ucLen);
};

Gsl1680VarType gGsl1680Var;

Gsl1680Type gGsl1680={
	&gGsl1680Const, //PGsl1680Const pConst;
	&gGsl1680Var //PGsl1680Var pVar;
};

/*
void TpShowInfo(void){
	if (GetTpIntPin()== ST_HIGH){
		TpGsl1680Read(&gGsl1680);
		Printf("X0:%d,", TpGsl1680GetFingerInfo(&gGsl1680, 0).wX);
		Printf("Y0:%d,", TpGsl1680GetFingerInfo(&gGsl1680, 0).wY);
		Printf("S0:%d, ", (BYTE)TpGsl1680GetFingerInfo(&gGsl1680, 0).bActive);
		Printf("X1:%d,", TpGsl1680GetFingerInfo(&gGsl1680, 1).wX);
		Printf("Y1:%d,", TpGsl1680GetFingerInfo(&gGsl1680, 1).wY);
		Printf("S1:%d,", (BYTE)TpGsl1680GetFingerInfo(&gGsl1680, 1).bActive);
		PutStr("  \r");
	}
}
*/

TS_DrvTypeDef TpGsl1680_ts_drv =
{
  TpGsl1680_Init,
  TpGsl1680_ReadID,
  TpGsl1680_Reset,

  TpGsl1680_TS_Start,
  TpGsl1680_TS_DetectTouch,
  TpGsl1680_TS_GetXY,

  TpGsl1680_TS_EnableIT,
  TpGsl1680_TS_ClearIT,
  TpGsl1680_TS_ITStatus,
  TpGsl1680_TS_DisableIT,
};

void TpGsl1680_Init(WORD DeviceAddr)
{
	McuI2CInit();
	TpGsl1680Init(&gGsl1680);

}

void TpGsl1680_Reset(WORD DeviceAddr)
{

}

uint16_t TpGsl1680_ReadID(WORD DeviceAddr)
{
  return 0;
}

void TpGsl1680_TS_Start(WORD DeviceAddr)
{
}

uint8_t TpGsl1680_TS_DetectTouch(WORD DeviceAddr)
{
	TpGsl1680Read(&gGsl1680);
	return TpGsl1680GetFingerInfo(&gGsl1680, 0).bActive;
	/*
	if (GetTpIntPin()== ST_HIGH){
		return 1;
	}
	else{
		return 0;
	}
	*/
}

void TpGsl1680_TS_GetXY(WORD DeviceAddr, WORD *X, WORD *Y)
{
	//TpGsl1680Read(&gGsl1680);
	*X= TpGsl1680GetFingerInfo(&gGsl1680, 0).wX;
	*Y= TpGsl1680GetFingerInfo(&gGsl1680, 0).wY;
}


void TpGsl1680_TS_EnableIT(WORD DeviceAddr)
{
}


void TpGsl1680_TS_DisableIT(WORD DeviceAddr)
{
}


BYTE TpGsl1680_TS_ITStatus(WORD DeviceAddr)
{
  return 0;
}


void TpGsl1680_TS_ClearIT(WORD DeviceAddr)
{
}

