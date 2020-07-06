/******************************************************************************
*
* Copyright (c) 2010 TP-LINK Technologies CO.,LTD.
* All rights reserved.
*
* FILE NAME  :   md4-new.c
* VERSION    :   1.0
* DESCRIPTION:   另外一套md4散列算法，支持被散列的参数长度最大为256B.
*
* AUTHOR     :   huangwenzhong <huangwenzhong@tp-link.net>
* CREATE DATE:   11/28/2012
*
* HISTORY    :
* 01   11/28/2012  huangwenzhong     Create.
*
******************************************************************************/
#ifndef __MD4_NEW_H_
#define __MD4_NEW_H_

#include <string.h>

#ifndef u64
#define u64 unsigned long long
#endif

#ifndef u32
#define u32 unsigned int
#endif

#ifndef u8
#define u8 unsigned char
#endif

#define	MD4_NEW_BLOCK_LENGTH		(64)
#define	MD4_NEW_DIGEST_LENGTH		(16)
#define MD4_NEW_MAX_HASH_TEXT_LEN	(256)
	
typedef struct MD4_NEW_Context {
	u32 state[4];			/* state */
	u64 count;			/* number of bits, mod 2^64 */
	u8 buffer[MD4_NEW_BLOCK_LENGTH];	/* input buffer */
} MD4_NEW_CTX;

void md4_vector(size_t num_elem, const u8 *addr[], const size_t *len, u8 *mac);

#endif
