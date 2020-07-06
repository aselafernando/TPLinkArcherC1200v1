/*!Copyright(c) 2013-2014 Shenzhen TP-LINK Technologies Co.Ltd.
 *
 *\file		xt_httphost.h
 *\brief	kernel/netfilter part for http host filter. 
 *
 *\author	Hu Luyao
 *\version	1.0.0
 *\date		23Dec13
 *
 *\history 	\arg 1.0.0, creat this based on "multiurl" mod from soho.  
 * 				    
 */
/***************************************************************************/
/*						CONFIGURATIONS					 */
/***************************************************************************/
#ifndef	_XT_HTTPHOST_H_
#define	_XT_HTTPHOST_H_


/***************************************************************************/
/*						INCLUDE_FILES					 */
/***************************************************************************/


/***************************************************************************/
/*						DEFINES						 */
/***************************************************************************/
/* URLFILTER_MAX_URL_NUM should be detemined by profile */
#define 	URLFILTER_MAX_URL_NUM				(128)
#define		URLFILTER_MAX_URL_LEN				(255)
#define     URLFILTER_MTU_VALUE                 (1500)
#define     URLFILTER_MAX_DNS_SIZE              (512)

#define     TRUE                                (1)
#define     FALSE                               (0)


/***************************************************************************/
/*						TYPES							 */
/***************************************************************************/
typedef struct _xt_httphost_info
{
	/* how many host names*/
	int		count;

	/* 
		black list mode: 0
		white list mode: 1
	*/
	int     mode;

	/*
	    http : 0
	    dns  : 1
	*/
	int     type;
	
	/* urls */
	char	host_names[URLFILTER_MAX_URL_NUM][URLFILTER_MAX_URL_LEN + 1];
} xt_httphost_info;

typedef struct _urlfilter_dns_header {
    unsigned short        transID;        /* packet ID */
    unsigned short         flags;        /* flag */
    unsigned short         nQDCount;    /* question section */
    unsigned short         nANCount;    /* answer section */
    unsigned short         nNSCount;    /* authority records section */
    unsigned short         nARCount;    /* additional records section */
} urlfilter_dns_header;

/***************************************************************************/
/*						VARIABLES						 */
/***************************************************************************/


/***************************************************************************/
/*						FUNCTIONS						 */
/***************************************************************************/
#endif