

#ifndef _TP_GSLX680_H_
#define _TP_GSLX680_H_

#include "Types.h"

#define GSL1680_I2C_ADDR			0x80
#define FINGER_MAX					5

typedef const struct _Gsl1680FwType{
    BYTE ucOffset;
    DWORD dwValue;
}Gsl1680FwType;

typedef struct _FingerInfoType{
	volatile WORD wX, wY;
	volatile bool bActive;
}FingerInfoType;

typedef const struct _Gsl1680ConstType{
	BYTE ucUsedFingers;
	void (*Delay)(DWORD dwTime);
	void (*I2CReadBuffer)(BYTE ucDeviceAddr, BYTE ucReg, BYTE* pucBuff, BYTE ucLen);
	void (*I2CWriteBuffer)(BYTE ucDeviceAddr, BYTE ucReg, BYTE* pucBuff, BYTE ucLen);
}Gsl1680ConstType, *PGsl1680Const;

typedef struct _Gsl1680VarType{
	FingerInfoType FingerInfo[FINGER_MAX];
}Gsl1680VarType, *PGsl1680Var;

typedef const struct _Gsl1680Type{
	PGsl1680Const pConst;
	PGsl1680Var pVar;
}Gsl1680Type, *PGsl1680;

extern void TpGsl1680Init(PGsl1680 pTp);
extern void TpGsl1680Read(PGsl1680 pTp);
extern FingerInfoType TpGsl1680GetFingerInfo(PGsl1680 pTp, BYTE ucFinger);

#endif 

