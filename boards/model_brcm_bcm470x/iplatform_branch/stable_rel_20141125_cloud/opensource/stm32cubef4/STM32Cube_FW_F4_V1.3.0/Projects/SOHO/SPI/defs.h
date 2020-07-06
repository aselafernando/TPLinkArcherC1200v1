
#ifndef __DEFS_H_GENERIC
#define __DEFS_H_GENERIC

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>


int spi_raw_receive(uint8_t* buf,uint16_t size,uint32_t timeout);

char * num_to_str( uint32_t a);


int spi_raw_send(uint8_t* buf,uint16_t size,uint32_t timeout);


unsigned int crc( unsigned char *buf, int len );

int spi_send_packet(const char *in,unsigned int len,unsigned int timeout);


int spi_recv_packet(char *out,unsigned int *len,unsigned int timeout);





















#endif /*  */