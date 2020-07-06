
#ifndef TYPES_H
#define TYPES_H

#include "Config.h"
//#include "Integer.h"

typedef unsigned char		BYTE;
typedef signed char		SBYTE;
typedef unsigned char		uchar;
typedef unsigned char		HighLow;
typedef unsigned short		WORD;
typedef signed short		SWORD;
typedef unsigned long		DWORD;
typedef signed long		SDWORD;
typedef signed char		Schar;
typedef signed int			Sint;
typedef signed long		Slong;
typedef unsigned char		StringType;
#ifdef ENABLE_64BIT_TYPE
typedef unsigned long long u64;
typedef signed long long	s64;
#endif

typedef enum{
  false=0,
  true=1
}bool;

//#define bool				BOOL
//#define FALSE				false
//#define TRUE					true

/*typedef void (*fpSetBoolValue)(BOOL);
typedef BOOL (*fpGetBoolValue)(void);

typedef void (*fpSetU8Value)(BYTE);
typedef BYTE (*fpGetU8Value)(void);

typedef void (*fpSetS8Value)(SBYTE);
typedef SBYTE (*fpGetS8Value)(void);

typedef void (*fpSetU16Value)(WORD);
typedef WORD (*fpGetU16Value)(void);

typedef void (*fpSetS16Value)(SWORD);
typedef SWORD (*fpGetS16Value)(void);

typedef void (*fpSetU32Value)(DWORD);
typedef DWORD (*fpGetU32Value)(void);

typedef void (*fpSetS32Value)(SDWORD);
typedef SDWORD (*fpGetS32Value)(void);*/

#define BIT0		(0x00000001)
#define BIT1		(0x00000002)
#define BIT2		(0x00000004)
#define BIT3		(0x00000008)
#define BIT4		(0x00000010)
#define BIT5		(0x00000020)
#define BIT6		(0x00000040)
#define BIT7		(0x00000080)
#define BIT8		(0x00000100)
#define BIT9		(0x00000200)
#define BIT10	(0x00000400)
#define BIT11	(0x00000800)
#define BIT12	(0x00001000)
#define BIT13	(0x00002000)
#define BIT14	(0x00004000)
#define BIT15	(0x00008000)
#define BIT16	(0x00010000)
#define BIT17	(0x00020000)
#define BIT18	(0x00040000)
#define BIT19	(0x00080000)
#define BIT20	(0x00100000)
#define BIT21	(0x00200000)
#define BIT22	(0x00400000)
#define BIT23	(0x00800000)
#define BIT24	(0x01000000)
#define BIT25	(0x02000000)
#define BIT26	(0x04000000)
#define BIT27	(0x08000000)
#define BIT28	(0x10000000)
#define BIT29	(0x20000000)
#define BIT30	(0x40000000)
#define BIT31	(0x80000000)

#define HIBYTE_REF(addr)				(*((BYTE *) & (addr)))
#define LOBYTE_REF(addr)			(*((BYTE *) & (addr + 1)))
#define MAKEWORD(value1, value2)	(((WORD)(value1)) * 0x100) + (value2)
#define MAKEUINT(value1, value2)		(((WORD)(value1)) * 0x100) + (value2)
#define H2BYTE(value)				((BYTE)((value) / 0x10000))
#define HIBYTE(value)				((BYTE)((value) / 0x100))
#define LOBYTE(value)				((BYTE)(value))

#define POWER2(bits)					((DWORD)BIT0<<bits)
#define BitMask(bits)					((DWORD)BIT0<<bits)
#define MAKE_MASK(bits)				(POWER2(bits)-1)

#define ANY_VALUE					0

#define _LOW						0
#define _HIGH						1

//#define RET_ERROR					FALSE
//#define RET_SUCCESS					TRUE

#define ON							true
#define OFF							false

#define DIFFERENT					true
#define SAME							false

#define ST_LOCK						true
#define ST_UNLOCK					false

typedef enum{
	ET_LITTLE_END= 0,
	ET_BIG_END= 1
}EndianType;

typedef enum{
	ST_LOW= 0,
	ST_HIGH= 1
}HiLowType;

typedef enum{
	EDGE_FALL= 0,
	EDGE_RISE= 1
}EdgeType;

typedef enum{
	PIN_LOW= 0,
	PIN_HIGH= 1,
	PIN_FLOAT= 2
}PinStateType;

typedef enum{
	ST_CLOSE= 0,
	ST_OPEN= 1
}OpenCloseType;

typedef enum{
	ST_OFF= 0,
	ST_ON= 1
}OnOffType;

#ifndef NULL
#define NULL		(void*)(0)
#endif



#endif

