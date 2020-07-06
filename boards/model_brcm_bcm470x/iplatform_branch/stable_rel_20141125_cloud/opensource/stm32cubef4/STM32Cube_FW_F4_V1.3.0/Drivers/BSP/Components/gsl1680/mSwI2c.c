
#include "Config.h"

#include "mSwI2c.h"

static void SwI2cSetSclCheck(SwI2cType* pI2c, HiLowType bSet){
	WORD ucDummy;
	if (bSet== ST_HIGH){
		pI2c->SetScl();
	}
	else{
		pI2c->ClrScl();
	}
	if (bSet == ST_HIGH){
		ucDummy = pI2c->wCheckPinDummy;
		while ((pI2c->GetScl() == ST_LOW) && (ucDummy--)) ;
	}
}
static void SwI2cSetSdaCheck(SwI2cType* pI2c, HiLowType bSet){
	WORD ucDummy;
	if (bSet== ST_HIGH){
		pI2c->SetSda();
	}
	else{
		pI2c->ClrSda();
	}
	if (bSet == ST_HIGH){
		ucDummy = pI2c->wCheckPinDummy;
		while ((pI2c->GetSda() == ST_LOW) && (ucDummy--)) ;
	}
}
static bool SwI2cStart(SwI2cType* pI2c){
	bool bStatus = true;
	SwI2cSetSdaCheck(pI2c, ST_HIGH);
	pI2c->SwI2cDelayBit();
	SwI2cSetSclCheck(pI2c, ST_HIGH);
	pI2c->SwI2cDelayBit();
	if ((pI2c->GetScl() == ST_LOW) || (pI2c->GetSda() == ST_LOW)){
		bStatus = false;
	}
	else{
		pI2c->ClrSda();
		pI2c->SwI2cDelayBit();
		pI2c->ClrScl();
	}
	return bStatus;
}
void SwI2cStop(SwI2cType* pI2c){
	pI2c->ClrScl();
	pI2c->SwI2cDelayBit();
	pI2c->ClrSda();
	pI2c->SwI2cDelayBit();
	SwI2cSetSclCheck(pI2c, ST_HIGH);
	pI2c->SwI2cDelayBit();
	SwI2cSetSdaCheck(pI2c, ST_HIGH);
	pI2c->SwI2cDelayBit();
}
BYTE SwI2cReceiveByte(SwI2cType* pI2c, bool bAck){
	BYTE ucReceive = 0;
	BYTE ucMask = BIT7;
	while(ucMask){
		pI2c->SetSda();
		SwI2cSetSclCheck(pI2c, ST_HIGH);
		pI2c->SwI2cDelayBit();
		if(pI2c->GetSda() == ST_HIGH)
			ucReceive |= ucMask;
		pI2c->ClrScl();
		pI2c->SwI2cDelayBit();
		ucMask >>= 1;
	}
	if (bAck== true){
		SwI2cSetSdaCheck(pI2c, I2C_ACKNOWLEDGE);
	}
	else{
		SwI2cSetSdaCheck(pI2c, I2C_NON_ACKNOWLEDGE);
	}
	pI2c->SwI2cDelayBit();
	SwI2cSetSclCheck(pI2c, ST_HIGH);
	pI2c->SwI2cDelayBit();
	pI2c->ClrScl();
	pI2c->SwI2cDelayBit();
	return ucReceive;
}
HighLow SwI2cSendByte(SwI2cType* pI2c, BYTE ucVal){
	BYTE ucMask = BIT7;
	HiLowType bAck;
	while(ucMask){
		if (ucVal & ucMask)
			SwI2cSetSdaCheck(pI2c, ST_HIGH);
		else
			SwI2cSetSdaCheck(pI2c, ST_LOW);
		pI2c->SwI2cDelayBit();
		SwI2cSetSclCheck(pI2c, ST_HIGH);
		pI2c->SwI2cDelayBit();
		pI2c->ClrScl();
		pI2c->SwI2cDelayBit();
		ucMask >>= 1;
	}
	// recieve acknowledge
	pI2c->SetSda();
	pI2c->SwI2cDelayBit();
	SwI2cSetSclCheck(pI2c, ST_HIGH);
	pI2c->SwI2cDelayBit();
	bAck = pI2c->GetSda();
	pI2c->ClrScl();
	pI2c->SwI2cDelayBit();
	return (bAck);
}
bool SwI2cAccessStart(SwI2cType* pI2c, BYTE ucSlaveAdr, I2cIoTransType Trans){
	BYTE ucDummy;
	if (Trans == I2C_TRANS_READ){
		ucSlaveAdr |= BIT0;
	}
	else{
		ucSlaveAdr &= ~BIT0;
	}
	ucDummy = pI2c->ucAccessDummyTime;
	while (ucDummy--){
		if (SwI2cStart(pI2c) == false){
			continue;
		}
		if (SwI2cSendByte(pI2c, ucSlaveAdr) == I2C_ACKNOWLEDGE){ // check acknowledge
			return true;
		}
		SwI2cStop(pI2c);
		pI2c->SwI2cDelayByte();
	}
	return false;
}
void SwI2cInit(SwI2cType* pI2c){

}

void SwI2cBurstReadBytes(
	SwI2cType* pI2c,
	BYTE ucSlaveAdr,
	BYTE ucSubAdr,
	BYTE *pucBuff,
	WORD ucBufLen)
{
	BYTE ucDummy;
	ucDummy = pI2c->ucAccessDummyTime;
	while(ucDummy--){
		if (SwI2cAccessStart(pI2c, ucSlaveAdr, I2C_TRANS_WRITE) == false){
			continue;
		}
		if (SwI2cSendByte(pI2c, ucSubAdr) == I2C_NON_ACKNOWLEDGE){
			continue;
		}
		if (SwI2cAccessStart(pI2c, ucSlaveAdr, I2C_TRANS_READ) == false){
			continue;
		}
		while(ucBufLen--){
			*pucBuff = SwI2cReceiveByte(pI2c, (ucBufLen>0)?true:false);
			pucBuff++;
		}
		break;
	}
	SwI2cStop(pI2c);
}

void SwI2cBurstWriteBytes(
	SwI2cType* pI2c,
	BYTE ucSlaveAdr,
	BYTE ucSubAdr,
	BYTE *pucBuff,
	BYTE ucBufLen)
{
	BYTE ucDummy;
	ucDummy = pI2c->ucAccessDummyTime;
	while(ucDummy--){
		if (SwI2cAccessStart(pI2c, ucSlaveAdr, I2C_TRANS_WRITE) == false){
			continue;
		}
		if (SwI2cSendByte(pI2c, ucSubAdr) == I2C_NON_ACKNOWLEDGE){
			continue;
		}
		while(ucBufLen--){
			SwI2cSendByte(pI2c, *pucBuff);
			pucBuff++;
		}
		break;
	}
	SwI2cStop(pI2c);
}

BYTE SwI2cReadByte(
	SwI2cType* pI2c,
	BYTE ucSlaveAdr,
	BYTE ucSubAdr)
{
	BYTE ucBuf;
	SwI2cBurstReadBytes(pI2c, ucSlaveAdr, ucSubAdr, &ucBuf, 1);
	return ucBuf;
}

void SwI2cWriteByte(
	SwI2cType* pI2c,
	BYTE ucSlaveAdr,
	BYTE ucSubAdr,
	BYTE ucVal)
{
	SwI2cBurstWriteBytes(pI2c, ucSlaveAdr, ucSubAdr, &ucVal, 1);
}








