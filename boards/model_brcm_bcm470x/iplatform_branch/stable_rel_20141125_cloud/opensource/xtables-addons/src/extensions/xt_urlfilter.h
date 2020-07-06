/*!Copyright(c) 2013-2014 Shenzhen TP-LINK Technologies Co.Ltd.
 *
 *\file     xt_urlfilter.h
 *\brief    kernel/netfilter part for ac based url filter. 
 *
 *\author   Hu Luyao
 *\version  1.0.0
 *\date     23Dec13
 *
 *\history  \arg 1.0.0, creat this mod.  
 *                  
 */
/***************************************************************************/
/*                      CONFIGURATIONS                   */
/***************************************************************************/
#ifndef _XT_URLFILTER_H_
#define _XT_URLFILTER_H_

/***************************************************************************/
/*                      INCLUDE_FILES                    */
/***************************************************************************/


/***************************************************************************/
/*                      DEFINES                      */
/***************************************************************************/
#define     URLFILTER_MAX_URL_NUM               (32)
#define     URLFILTER_MAX_URL_LEN               (31)
#define     URLFILTER_MAX_CHARSET_LEN           (48)
#define     URLFILTER_MTU_VALUE                 (1500)
#define     URLFILTER_MAX_DNS_SIZE              (512) 
#define     URLFILTER_ALPHABET_SIZE             (256)

#define     ACSM_FAIL_STATE                     (-1)
#define     ACSM_SUCCESS_STATE                  (-2)

/* warning: max state is limited by iptables rule mem size, which is short type */
/* MAX_STATE is REAL max state limited by memory */
#define     MAX_STATE                           (512)
#define     URLFILTER_MAX_STATE_NUM             (URLFILTER_MAX_URL_NUM * URLFILTER_MAX_URL_LEN)

#define     TRUE                                (1)
#define     FALSE                               (0)


/***************************************************************************/
/*                      TYPES                            */
/***************************************************************************/
/*!
 *\typedef  acsm_statetable
 *\brief    state jumper.
 */
typedef struct _acsm_statetable
{    
    /* 这里url的字符集只有48个，因此每一个状态的下一跳状态表是48项*/
    signed short      NextState[ URLFILTER_MAX_CHARSET_LEN ];  

    /* Failure state - used while building NFA & DFA  */
    signed short      FailState;  
}acsm_statetable; 

/*!
 *\typedef  xt_acsm_map
 *\brief    匹配用的核心结构体，用户态和内核态的桥梁
 */
typedef struct _xt_acsm_map
{
    /* 当前状态计数器 */
    unsigned int state_num;

    /* 
        black mode: 0
        white mode: 1
    */
    int mode;

    /* 
        http: 0
        dns : 1
    */
    int type;

    /* keep opt arg for iptables print and save*/
    char para[URLFILTER_MAX_URL_NUM*(URLFILTER_MAX_URL_LEN + 1)];
    
    /* 字符转换状态表 */
    unsigned char xlatstate[URLFILTER_ALPHABET_SIZE];

    /* 状态跳转表，采用二维数组实现 */
    acsm_statetable acsm_goto_map[MAX_STATE];

}xt_acsm_map;

/*!
 *\typedef  urlfilter_dns_header
 *\brief    dns_header.
 */
typedef struct _urlfilter_dns_header {
    unsigned short         transID;     /* packet ID */
    unsigned short         flags;       /* flag */
    unsigned short         nQDCount;    /* question section */
    unsigned short         nANCount;    /* answer section */
    unsigned short         nNSCount;    /* authority records section */
    unsigned short         nARCount;    /* additional records section */
} urlfilter_dns_header;

/***************************************************************************/
/*                      VARIABLES                        */
/***************************************************************************/


/***************************************************************************/
/*                      FUNCTIONS                        */
/***************************************************************************/

#endif