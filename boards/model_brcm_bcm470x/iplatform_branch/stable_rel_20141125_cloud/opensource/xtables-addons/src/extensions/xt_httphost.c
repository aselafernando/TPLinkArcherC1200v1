/*!Copyright(c) 2013-2014 Shenzhen TP-LINK Technologies Co.Ltd.
 *
 *\file     xt_httphost.c
 *\brief    kernel/netfilter part for http host filter. 
 *
 *\author   Hu Luyao
 *\version  1.0.0
 *\date     23Dec13
 *
 *\history  \arg 1.0.0, creat this based on "multiurl" mod from soho.  
 *                  
 */

/***************************************************************************/
/*                      CONFIGURATIONS                   */
/***************************************************************************/


/***************************************************************************/
/*                      INCLUDE_FILES                    */
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

#include "xt_httphost.h"
#if (LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,34))
#include "compat_xtnu.h"
#endif

/***************************************************************************/
/*                      DEFINES                      */
/***************************************************************************/
#define HOST_STR    "\r\nHost: "
#define GET_STR     "GET "
#define HTTP_STR    " HTTP"
#define DEBUG       0
 
/***************************************************************************/
/*                      TYPES                            */
/***************************************************************************/
 enum FILTER_TYPE{
    HTTP,
    DNS,
    TYPE_SUM
 };


/***************************************************************************/
/*                      EXTERN_PROTOTYPES                    */
/***************************************************************************/


/***************************************************************************/
/*                      LOCAL_PROTOTYPES                     */
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
 *\fn           static bool match(const struct sk_buff *skb, struct xt_action_param *param)
 *\brief        find the url in skb (host in http or querys in dns)
 *\return       found or not
 */
static bool match(const struct sk_buff *skb, struct xt_action_param *param);

/*!
 *\fn           static int __init httphost_init(void)
 *\brief        mod init
 *\return       SUCCESS or not
 */
static int __init httphost_init(void);

/*!
 *\fn           static void __exit httphost_exit(void)
 *\brief        mod exit
 *\return       none
 */
static void __exit httphost_exit(void);


/***************************************************************************/
/*                      VARIABLES                        */
/***************************************************************************/
static struct xt_match httphost_match = { 
    .name           = "httphost",
    .family         = NFPROTO_UNSPEC,
    .match          = match,
    .matchsize      = XT_ALIGN(sizeof(struct _xt_httphost_info)),
    .me             = THIS_MODULE,
};
 
/***************************************************************************/
/*                      LOCAL_FUNCTIONS                  */
/***************************************************************************/
/*!
 *\fn           static void _transDomain2Buf(unsigned char *dns, unsigned char *buf)
 *\brief        Trans the dns domain in packet to buf with '.'.
 *
 *\param[in]    dns      Domain name in packet.
 *\param[out]   buf      Buffer to store the domain.
 *
 *\return       N/A
 */
static unsigned int _transDomain2Buf(unsigned char *dns, 
                                     unsigned char *buf, signed int bufLen)
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

/*!
 *\fn           unsigned char *_url_strstr(const unsigned char* start, const unsigned char* end, 
                                        const unsigned char* strCharSet)
 *\brief        find the url in str zone
 *\param[in]    start           start ptr of str zone.
 *\param[in]    end             end ptr of str zone.
 *\param[in]    strCharSet      the url you want to find
 *\return       url postion
 */
static unsigned char *_url_strstr(const unsigned char* start, 
                                  const unsigned char* end, const unsigned char* strCharSet)
{
    const unsigned char *s_temp = start;        /*the s_temp point to the s*/

    int l1, l2;

    l2 = strlen(strCharSet);
    
    if (!l2)
    {
        return (unsigned char *)start;
    }

    l1 = end - s_temp + 1;

    while (l1 >= l2)
    {
        l1--;

        if (!memcmp(s_temp, strCharSet, l2))
        {
            return (unsigned char *)s_temp;
        }

        s_temp++;
    }

    return NULL;
}

/*!
 *\fn           static bool match(const struct sk_buff *skb, struct xt_action_param *param)
 *\brief        find the url in skb (host in http or querys in dns)
 *\return       found or not
 */
static bool match(const struct sk_buff *skb, struct xt_action_param *param)
{   
   
    const struct _xt_httphost_info *info = param->matchinfo;
    const struct iphdr *iph = ip_hdr(skb);
    
    /* handle http request */
    if (info->type == HTTP)
    {
        
        unsigned char*   http_payload_start = NULL;
        unsigned char*   http_payload_end = NULL;
  
        unsigned char*   host_str_start = NULL;
        unsigned char*   HOST_start = NULL;
        unsigned char*   HOST_end = NULL;

        unsigned char*   URI_start = NULL;
        unsigned char*   URI_end = NULL;


        
        int i;

        struct tcphdr *tcph = (void *)iph + iph->ihl*4;

        http_payload_start = (unsigned char *)tcph + tcph->doff*4;
        http_payload_end = http_payload_start + (iph->tot_len - iph->ihl*4 - tcph->doff*4) - 1;

        if (http_payload_start < http_payload_end)
        {
            host_str_start = _url_strstr(http_payload_start, http_payload_end, HOST_STR);
            URI_start = _url_strstr(http_payload_start, http_payload_end, GET_STR);
        }



        if (host_str_start != NULL && URI_start != NULL)
        {
            HOST_start = host_str_start + 8;
            URI_start  = URI_start + 4;

            HOST_end = _url_strstr(HOST_start, http_payload_end, "\r\n");
            URI_end = _url_strstr(URI_start, http_payload_end, HTTP_STR);

            if (HOST_end == NULL || URI_end == NULL)
            {
#if DEBUG
                printk("can not find end of host or GET\n");
#endif
                return info->mode;   
            }

#if DEBUG
            printk("HOST: ");
            for (; HOST_start != HOST_end; ++HOST_start)
            {
            	printk("%2x ", *HOST_start);
            }
            printk("\n");

            printk("URI: ");
            for (; URI_start != URI_end; ++URI_start)
            {
            	printk("%2x ", *URI_start);
            }
            printk("\n");        
#endif
           
            for (i = 0; i < info->count; ++i)
            {    
#if DEBUG
                printk("host%d: %s\n", i, info->host_names[i]);
#endif

                if ( _url_strstr(HOST_start, HOST_end, info->host_names[i]) || 
                		_url_strstr(URI_start, URI_end, info->host_names[i]) )
                {
#if DEBUG
                    printk("==== matched %s ====\n", info->host_names[i]);
#endif     
                    return TRUE;
                }
            }
#if DEBUG
            printk("Host OK, not matched\n");
#endif 
            return FALSE;
        }

        /* 
            if you can not find Host
            black list: do not match
            white list: match
            anyway, let it pass
        */
        else
        {
#if DEBUG
            printk("no Host\n");
#endif 
            return info->mode;
        }
    }

    /* handle dns request */
    else if (info->type == DNS)
    {
        
        // I can not use "index" because of a warning. I do not mean it!
        signed int i;
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
            for (i = 0; i < info->count; ++i)
            {    
#if DEBUG
                printk("host%d: %s\n", i, info->host_names[i]);
#endif

                if (_url_strstr(domain, domain + domain_len, info->host_names[i]))
                {
#if DEBUG
                    printk("==== matched %s ====\n", info->host_names[i]);
#endif     
                    return TRUE;
                }
            }
            
            pkt_len += domain_len + 4 + 1;
            pTmp    += domain_len + 4 + 1;
        }

        return FALSE;
    }
    return FALSE;
}

/*!
 *\fn           static int __init httphost_init(void)
 *\brief        mod init
 *\return       SUCCESS or not
 */
static int __init httphost_init(void)
{
    return xt_register_match(&httphost_match);
}

/*!
 *\fn           static void __exit httphost_exit(void)
 *\brief        mod exit
 *\return       none
 */
static void __exit httphost_exit(void)
{
    xt_unregister_match(&httphost_match);
}

/***************************************************************************/
/*                      PUBLIC_FUNCTIONS                     */
/***************************************************************************/


/***************************************************************************/
/*                      GLOBAL_FUNCTIONS                     */
/***************************************************************************/
module_init(httphost_init);
module_exit(httphost_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Hu Luyao <huluyao@tp-link.net>");
MODULE_DESCRIPTION("Xtables: http host name match");
MODULE_ALIAS("ipt_httphost");
MODULE_ALIAS("ip6t_httphost");
