/*!Copyright(c) 2013-2014 Shenzhen TP-LINK Technologies Co.Ltd.
 *
 *\file		xt_urlfilter.c
 *\brief	kernel/netfilter part for url filter. 
 *
 *\author	Hu Luyao
 *\version	1.0.0
 *\date		23Dec13
 *
 *\history 	\arg 1.0.0, creat this based on "URLFILTER" mod from soho.  
 * 				    
 */

/***************************************************************************/
/*						CONFIGURATIONS				 	 */
/***************************************************************************/


/***************************************************************************/
/*						INCLUDE_FILES					 */
/***************************************************************************/
#include <linux/module.h>
#include <linux/skbuff.h>
#include <linux/if_arp.h>
#include <linux/if_ether.h>
#include <linux/etherdevice.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#include <linux/netfilter_ipv4.h>
#include <linux/netfilter_ipv6.h>
#include <linux/netfilter/x_tables.h>
#include <linux/version.h>

#if (LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,34))
#include "compat_xtnu.h"
#endif

#include "xt_urlfilter.h"

/***************************************************************************/
/*						DEFINES						 */
/***************************************************************************/
#define HOST_STR    "\r\nHost: "
#define DEBUG       0
 
/***************************************************************************/
/*						TYPES							 */
/***************************************************************************/
 enum FILTER_TYPE{
    HTTP,
    DNS,
    TYPE_SUM
 };

/***************************************************************************/
/*						EXTERN_PROTOTYPES					 */
/***************************************************************************/


/***************************************************************************/
/*						LOCAL_PROTOTYPES					 */
/***************************************************************************/
/*!
 *\fn           static void _transDomain2Buf(unsigned char *dns, unsigned char *buf)
 *\brief        Trans the dns domain in packet to buf with '.'.
 *\param[in]    dns      Domain name in packet.
 *\param[out]   buf      Buffer to store the domain.
 *\return       N/A
 */
static unsigned int _transDomain2Buf(unsigned char *dns, unsigned char *buf, signed int bufLen);

/*!
 *\fn           unsigned char *_url_strstr(const unsigned char* start, const unsigned char* end, 
                                        const unsigned char* strCharSet)
 *\brief        find the url in str zone
 *\param[in]    start           start ptr of str zone.
 *\param[in]    end             end ptr of str zone.
 *\param[in]    strCharSet      the url you want to find
 *\return       url postion
 */
static unsigned char *_url_strstr(const unsigned char* start, const unsigned char* end, const unsigned char* strCharSet);

/*!
 *\fn           static bool _acsm_check(char* url_start, char* url_end, const xt_acsm_map *info)
 *\brief        find the url in str zone
 *\param[in]    url_start           start ptr of the url you want to find.
 *\param[in]    url_end             end ptr of the url you want to find.
 *\param[in]    info                the acsm. walk it!
 *\return       found or not
 */
static bool _acsm_check(char* url_start, char* url_end, const xt_acsm_map *info);

/*!
 *\fn           static bool urlfilter_xt_match(const struct sk_buff *skb, struct xt_action_param *param)
 *\brief        find str zone (host in http or query in DNS ) and match it
 *\return       found or not
 */
static bool urlfilter_xt_match(const struct sk_buff *skb, struct xt_action_param *param);

/*!
 *\fn           static int urlfilter_xt_checkentry(const struct xt_mtchk_param *param)
 *\brief        Called when user tries to insert an entry of this type
 *\return       SUCCESS or not
 */
static int urlfilter_xt_checkentry(const struct xt_mtchk_param *param);

/*!
 *\fn           static int __init urlfilter_init(void)
 *\brief        mod init
 *\return       SUCCESS or not
 */
static int __init urlfilter_init(void);

/*!
 *\fn           static void __exit urlfilter_exit(void)
 *\brief        mod exit
 *\return       none
 */
static void __exit urlfilter_exit(void);

/***************************************************************************/
/*						VARIABLES						 */
/***************************************************************************/
static struct xt_match urlfilter_match = { 
    .name           = "urlfilter",
    .family         = NFPROTO_UNSPEC,
    .match          = urlfilter_xt_match,
    .checkentry     = urlfilter_xt_checkentry,
    .matchsize      = XT_ALIGN(sizeof(xt_acsm_map)),
    .me             = THIS_MODULE,
};
 
/***************************************************************************/
/*						LOCAL_FUNCTIONS					 */
/***************************************************************************/
/*!
 *\fn           static void _transDomain2Buf(unsigned char *dns, unsigned char *buf)
 *\brief        Trans the dns domain in packet to buf with '.'.
 *\param[in]    dns      Domain name in packet.
 *\param[out]   buf      Buffer to store the domain.
 *\return       N/A
 */
static unsigned int _transDomain2Buf(unsigned char *dns, unsigned char *buf, signed int bufLen)
{
    signed int index;
    signed int orig_bufLen = bufLen;

    while(('\0' != *dns) && (bufLen > 0))
    {
        for (index = *dns; (index > 0) && (bufLen > 0); index --, bufLen --)
        {
            *(buf++) = *(++dns);
        }
        *(buf ++) = '.';
        dns ++;
        bufLen --;
    }

    if (bufLen < orig_bufLen)
    {
        bufLen ++;
        buf --;
    }
    
    *buf = '\0';
    return (orig_bufLen - bufLen);
}

#if 0
static void _printk_acsm(xt_acsm_map *info)
{
    /* 将xt_acsm_map 以一种较为显式的方式打印出来 */

    int i =0;
    int tmp = 0;
    
    printk("state num: %d\n", info->state_num);
    printk("--abcdefghijklmnopqrstuvwxyz0123456789.:/_-\n");
    for (; i<URLFILTER_MAX_STATE_NUM; i++)
    {
        printk("%d", i);
        for (tmp=0; tmp<URLFILTER_MAX_CHARSET_LEN; tmp++)
        {
            if ( (info->acsm_goto_map[i].NextState[tmp]) == ACSM_FAIL_STATE ) 
            {
                printk("-");
            }
            else if ( (info->acsm_goto_map[i].NextState[tmp]) == ACSM_SUCCESS_STATE ) 
            {
                printk("+");
            }
            else
            {
                printk("%d", (info->acsm_goto_map[i].NextState[tmp]));
            }
        }
        printk("\n");  
    }
}
#endif

/*!
 *\fn           unsigned char *_url_strstr(const unsigned char* start, const unsigned char* end, 
                                        const unsigned char* strCharSet)
 *\brief        find the url in str zone
 *\param[in]    start           start ptr of str zone.
 *\param[in]    end             end ptr of str zone.
 *\param[in]    strCharSet      the url you want to find
 *\return       url postion
 */
static unsigned char *_url_strstr(const unsigned char* start, const unsigned char* end, const unsigned char* strCharSet)
{
    const unsigned char *s_temp = start;        /*the s_temp point to the s*/

    int l1, l2;

    l2 = strlen(strCharSet);
    if (!l2)
        return (unsigned char *)start;

    l1 = end - s_temp + 1;

    while (l1 >= l2)
    {
        l1--;
        if (!memcmp(s_temp, strCharSet, l2))
            return (unsigned char *)s_temp;
        s_temp++;
    }

    return NULL;
}

/*!
 *\fn           static bool _acsm_check(char* url_start, char* url_end, const xt_acsm_map *info)
 *\brief        find the url in str zone
 *\param[in]    url_start           start ptr of the url you want to find.
 *\param[in]    url_end             end ptr of the url you want to find.
 *\param[in]    info                the acsm. walk it!
 *\return       found or not
 */
static bool _acsm_check(char* url_start, char* url_end, const xt_acsm_map *info)
{
    signed int state   = 0;
    char *start;
    char *end;
    start = url_start;
    end   = url_end;

    for (; start <= end; start++)
    {
#if DEBUG
        printk("%c ", *start);
        printk("%d",info->xlatstate[(int) *start]);
#endif
        state = (signed int) info->acsm_goto_map[state].NextState[info->xlatstate[(int) *start]];
#if DEBUG
        printk("state: %d\n", state);
#endif
        if (ACSM_SUCCESS_STATE == state)
        {
#if DEBUG
            printk("success\n");
#endif
            return TRUE;
        }
    }
#if DEBUG
    printk("\n");
#endif
    return FALSE;
}

/*!
 *\fn           static bool urlfilter_xt_match(const struct sk_buff *skb, struct xt_action_param *param)
 *\brief        find str zone (host in http or query in DNS ) and match it
 *\return       found or not
 */
static bool urlfilter_xt_match(const struct sk_buff *skb, struct xt_action_param *param)
{
 
    const xt_acsm_map *info = param->matchinfo;
    const struct iphdr *iph = ip_hdr(skb);

    if (info->type == HTTP)
    {
        struct tcphdr *tcph = (void *)iph + iph->ihl*4; /* Might be TCP, UDP */

        unsigned char*   http_payload_start = NULL;
        unsigned char*   http_payload_end = NULL;

        
        unsigned char*   host_str_start = NULL;
        unsigned char*   url_start = NULL;
        unsigned char*   url_end = NULL;
        unsigned char*   crlf_pos = NULL;
        
        /* int found_crlf = 0; */
        
        http_payload_start = (unsigned char *)tcph + tcph->doff*4;
        http_payload_end = http_payload_start + (iph->tot_len - iph->ihl*4 - tcph->doff*4) - 1;

        /* check "\r\nHOST:" in payload, pass other pkts, by zhanjinguang, 11Aug10 */
        if (http_payload_start < http_payload_end)
        {
            host_str_start = _url_strstr(http_payload_start, http_payload_end, HOST_STR);
        }

        if (host_str_start != NULL)
        {
            url_start = host_str_start + 8;

            crlf_pos = _url_strstr(url_start, http_payload_end, "\r\n");

            if (crlf_pos == NULL)
            {
                return info->mode;  
            }
            url_end = crlf_pos - 1;

            /* now we have url_start, url_end */
            return  _acsm_check(url_start, url_end, info);
        }

        /* 
            if you can not find Host
            black list: do not match
            white list: match
            anyway, let it pass
        */
        else
        {
            return info->mode;
        }

    }
    
    /* handle dns request */
    else if (info->type == DNS)
    {
        
        // I can not use "index" because of a warning. I do not mean it!
        signed int i_count;
        signed int dns_len;

        void  *dns_pkt;
        unsigned char *pTmp;
        unsigned char domain[URLFILTER_MAX_DNS_SIZE];

        unsigned int pkt_len = 0;
        unsigned int domain_len = 0;

        urlfilter_dns_header *pDnsHdr;

        struct udphdr *udph = (void *)iph + iph->ihl*4;
         
        dns_len = (unsigned int) ntohs(udph->len) - sizeof(struct udphdr) - sizeof(urlfilter_dns_header);


        if(dns_len < 0)
        {
#if DEBUG
                printk("dns_len < 0!\n");
#endif
            return info->mode;
        }
        if (dns_len >= URLFILTER_MAX_DNS_SIZE)
        {
#if DEBUG
                printk("dns_len >= URLFILTER_MAX_DNS_SIZE!\n");
#endif
            return info->mode;
        }

        dns_pkt = (void *) udph + sizeof(struct udphdr);
        pDnsHdr = (urlfilter_dns_header *)dns_pkt;

        if (0 != (ntohs(pDnsHdr->flags) & 0x8000)) /* If not request */
        {
#if DEBUG
                printk("not request!\n");
#endif
            return info->mode;
        }

        pTmp = (unsigned char *)pDnsHdr + sizeof(urlfilter_dns_header);

        /* See the packet header, most time it only has one request url, but to do more, we check
         * all about it. */
#if DEBUG
            printk("pDnsHdr->nQDCount ==%d \n", ntohs(pDnsHdr->nQDCount));
#endif
        for (i_count = 0; i_count < ntohs(pDnsHdr->nQDCount) && pkt_len < dns_len; i_count ++)
        {
#if DEBUG
                printk("dns%d \n", i_count);
#endif
            domain_len = _transDomain2Buf(pTmp, domain, URLFILTER_MAX_DNS_SIZE - 1);

#if DEBUG
                printk("domain:%s \n", domain);
#endif

            if (1 == _acsm_check(domain, domain + domain_len, info))
            {
#if DEBUG
                printk("==== matched ==== \n");
#endif
                return TRUE;
            }            
            
            pkt_len += domain_len + 4 + 1;
            pTmp    += domain_len + 4 + 1;
        }

        return 0;

    }

    return 0;
	
}	
	
/*!
 *\fn           static int urlfilter_xt_checkentry(const struct xt_mtchk_param *param)
 *\brief        Called when user tries to insert an entry of this type
 *\return       SUCCESS or not
 */
static int urlfilter_xt_checkentry(const struct xt_mtchk_param *param)
{
    /* WARNING!!!: linux 2.6里面是 返回1合法，linux 3.4却是返回0合法 */
	if ( param->match->matchsize != XT_ALIGN(sizeof(xt_acsm_map)) )
	{
#if DEBUG
		printk ( "matchsize=%d, sizeof=%d \n", param->match->matchsize, sizeof(xt_acsm_map) );
#endif
		return 1;
	}

	return 0;
}

/*!
 *\fn           static int __init urlfilter_init(void)
 *\brief        mod init
 *\return       SUCCESS or not
 */
static int __init urlfilter_init(void)
{
    return xt_register_match(&urlfilter_match);
}

/*!
 *\fn           static void __exit urlfilter_exit(void)
 *\brief        mod exit
 *\return       none
 */
static void __exit urlfilter_exit(void)
{
    xt_unregister_match(&urlfilter_match);
}


/***************************************************************************/
/*						PUBLIC_FUNCTIONS					 */
/***************************************************************************/


/***************************************************************************/
/*						GLOBAL_FUNCTIONS					 */
/***************************************************************************/
module_init(urlfilter_init);
module_exit(urlfilter_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Hu Luyao <huluyao@tp-link.net>");
MODULE_DESCRIPTION("Xtables: url match");
MODULE_ALIAS("ipt_urlfilter");
MODULE_ALIAS("ip6t_urlfilter");
