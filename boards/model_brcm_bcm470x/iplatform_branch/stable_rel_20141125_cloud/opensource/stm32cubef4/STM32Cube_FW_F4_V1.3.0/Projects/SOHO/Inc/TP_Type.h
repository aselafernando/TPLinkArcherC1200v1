#ifndef __TP_TYPE_H__
#define __TP_TYPE_H__


/*********************************************************************
*
*       Macros
*
**********************************************************************
*/

#ifndef NULL
#define NULL		(0)
#endif

#ifndef   U8
  #define U8  unsigned char
#endif

#ifndef   I8
  #define I8  char
#endif

#ifndef CHAR
	#define CHAR char
#endif

#ifndef   I16
  #define I16 short
#endif

#ifndef   U16
  #define U16 unsigned short
#endif

#ifndef   I32
  #define I32 int
#endif

#ifndef   U32
  #define U32 unsigned int
#endif

#ifndef LONG
	#define LONG long
#endif

#ifndef ULONG
	#define ULONG unsigned long
#endif

#ifndef BOOL
	#define BOOL int
#endif

#ifndef TRUE
	#define TRUE		(1)
#endif

#ifndef FALSE
	#define FALSE		(0)
#endif

#ifndef STATUS
	#define STATUS			int
#endif

#ifndef RET_SUCCESS
	#define RET_SUCCESS			(0)
#endif

#ifndef RET_FAIL
	#define RET_FAIL			(-1)
#endif

#endif
