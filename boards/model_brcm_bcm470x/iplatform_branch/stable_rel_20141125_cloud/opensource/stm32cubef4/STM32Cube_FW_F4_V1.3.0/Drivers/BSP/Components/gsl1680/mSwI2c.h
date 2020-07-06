
#ifndef M_SW_I2C_H
#define M_SW_I2C_H

#include "Types.h"

#define I2C_ACKNOWLEDGE        	ST_LOW
#define I2C_NON_ACKNOWLEDGE    ST_HIGH

typedef void (*fpSwI2cOutput)(void);
typedef HiLowType (*fpSwI2cInput)(void);
typedef void (*fpSwI2cDelay)(void);

typedef enum _I2cIoTransType{
    I2C_TRANS_READ,
    I2C_TRANS_WRITE
}I2cIoTransType;

typedef const struct _SwI2cType{
	WORD wCheckPinDummy; 
	BYTE ucAccessDummyTime;  
	fpSwI2cDelay SwI2cDelayBit, SwI2cDelayByte;
	fpSwI2cOutput SetScl, ClrScl;
	fpSwI2cOutput SetSda, ClrSda;
	fpSwI2cInput GetScl, GetSda;
}SwI2cType;

extern void SwI2cStop(SwI2cType* pI2c);
extern BYTE SwI2cReceiveByte(SwI2cType* pI2c, bool bAck);
extern HighLow SwI2cSendByte(SwI2cType* pI2c, BYTE ucVal);
extern bool SwI2cAccessStart(SwI2cType* pI2c, BYTE ucSlaveAdr, I2cIoTransType Trans);
extern void SwI2cInit(SwI2cType* pI2c);
extern void SwI2cBurstReadBytes(
	SwI2cType* pI2c,
	BYTE ucSlaveAdr, 
	BYTE ucSubAdr, 
	BYTE *pucBuff, 
	WORD ucBufLen);
extern void SwI2cBurstWriteBytes(
	SwI2cType* pI2c,
	BYTE ucSlaveAdr, 
	BYTE ucSubAdr, 
	BYTE *pucBuff, 
	BYTE ucBufLen);
extern BYTE SwI2cReadByte(
	SwI2cType* pI2c,
	BYTE ucSlaveAdr, 
	BYTE ucSubAdr);
extern void SwI2cWriteByte(
	SwI2cType* pI2c,
	BYTE ucSlaveAdr, 
	BYTE ucSubAdr, 
	BYTE ucVal);

#endif

