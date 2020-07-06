/************************************************************
Copyright (C) 2010-2011 TP-LINK TECHNOLOGIES CO., LTD.
File name	: tp_ipc.h
Version		: 1.0
Description	: Head file of tp_ipc.c
Author		: ZQQ
Create date	: 16Mar2011
History:
---------------------------------------------------------------
1. 16Mar2011 ZQQ Create the file 
****************************************************************/
#ifndef _TP_IPC_H_
#define _TP_IPC_H_
#define MAX_IPV6_ADDRESS_LENGTH	44

#include "signalpipe.h"
typedef struct
{
	int	type;
	int status;
	unsigned char v6ip[MAX_IPV6_ADDRESS_LENGTH];
	unsigned char gw[MAX_IPV6_ADDRESS_LENGTH];
	unsigned char v6prefix[MAX_IPV6_ADDRESS_LENGTH];
	int prefixLen;
	unsigned char v6dns[2][MAX_IPV6_ADDRESS_LENGTH];
}dhcp6cInfo;

#define IPC_DEBUG
//#undef 	IPC_DEBUG
#ifdef  IPC_DEBUG
#define ipc_msg(...) printf("%s %s %d : ",__FILE__,__FUNCTION__,__LINE__); \
		printf(__VA_ARGS__); \
		printf("\r\n")
#else
#define ipc_msg(...)
#endif


int tp_ipc_init();
int dhcp6c_ipc_send(int dstMid, unsigned char* ptr, int nbytes);
int tp_udhcp_sp_fd_set(fd_set *pRfds);
int dhcp6c_ipc_rcv(int module_id);
void tp_dhcp_ipc_fork(int module_id);
int recv_httpd_cmd();
int update_info();
void setDns(unsigned char* pDns1, unsigned char* pDns2);
void setPrefix(unsigned char* pv6Prefix, int prefixLength);
void setIp(unsigned char* pv6Ip);
void setType(int type);
void setStatus(int status);
int unRegisterMid();
#endif
