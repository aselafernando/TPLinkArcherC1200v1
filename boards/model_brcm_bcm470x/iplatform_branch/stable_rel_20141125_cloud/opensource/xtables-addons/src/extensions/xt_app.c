/*!Copyright(c) 2013-2014 Shenzhen TP-LINK Technologies Co.Ltd.
 *
 *\file     xt_app.c
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
#include <linux/netfilter_ipv4.h>
#include <linux/netfilter/x_tables.h>
#include <net/netfilter/nf_conntrack_app.h>
//#include <log.h>
#include "xt_app.h"

/***************************************************************************/
/*                      DEFINES                      */
/***************************************************************************/
#define DEBUG       0
#define MOD_QOS		259
 
/***************************************************************************/
/*                      TYPES                            */
/***************************************************************************/


/***************************************************************************/
/*                      EXTERN_PROTOTYPES                    */
/***************************************************************************/


/***************************************************************************/
/*                      LOCAL_PROTOTYPES                     */
/***************************************************************************/

/*!
 *\fn           static bool match(const struct sk_buff *skb, struct xt_action_param *param)
 *\brief        find the url in skb (host in http or querys in dns)
 *\return       found or not
 */
static bool match(const struct sk_buff *skb, struct xt_action_param *param);

/*!
 *\fn           static int __init app_init(void)
 *\brief        mod init
 *\return       SUCCESS or not
 */
static int __init app_init(void);

/*!
 *\fn           static void __exit app_exit(void)
 *\brief        mod exit
 *\return       none
 */
static void __exit app_exit(void);


/*kmod-log*/

extern void _log(unsigned int proj_id, unsigned int msg_id, ...);

#define LP_STRING    (1)
#define LP_INT32     (2)
#define LP_UINT32    (3)
#define LP_UINT32HEX (4)
#define LP_IPV4      (5)

#define LP_END       (0)

#define log(proj_id, msg_id, ...)                   \
    _log(proj_id, msg_id, ##__VA_ARGS__, LP_END)

/***************************************************************************/
/*                      VARIABLES                        */
/***************************************************************************/
static struct xt_match app_match = { 
    .name           = "app",
    .family         = NFPROTO_UNSPEC,
    .match          = match,
    .matchsize      = XT_ALIGN(sizeof(struct _xt_app_info)),
    .me             = THIS_MODULE,
};
 
/***************************************************************************/
/*                      LOCAL_FUNCTIONS                  */
/***************************************************************************/
/*!
 *\fn           static bool match(const struct sk_buff *skb, struct xt_action_param *param)
 *\brief        find the url in skb (host in http or querys in dns)
 *\return       found or not
 */
static bool match(const struct sk_buff *skb, struct xt_action_param *param)
{   
   
    const struct _xt_app_info *info = param->matchinfo;
    enum ip_conntrack_info ctInfo;
    struct nf_conntrack_app *pAppInfo;
    struct nf_conn *pCtEntry = NULL;
    int i = 0;
    int count = info->count;

    pCtEntry = nf_ct_get(skb, &ctInfo);
    if (NULL == pCtEntry)
    {
        return FALSE;
    }

    pAppInfo = nf_ct_get_app(pCtEntry);
    if (NULL == pAppInfo)
    {
        return FALSE;
    }

    for (i = 0; i < count; ++i)
    {
        if ( ((pAppInfo->appidntfy_flag[IP_CT_DIR_ORIGINAL].app_id_index & 0x0fff) == (info->id[i] & 0x0fff)) && 
            ((pAppInfo->appidntfy_flag[IP_CT_DIR_REPLY].app_id_index & 0xfff) == (info->id[i] & 0xfff)) )
        {
            log(MOD_QOS, 1, LP_INT32, info->id[i]);
            return TRUE;
        }
    }

    return FALSE;
}

/*!
 *\fn           static int __init app_init(void)
 *\brief        mod init
 *\return       SUCCESS or not
 */
static int __init app_init(void)
{
    return xt_register_match(&app_match);
}

/*!
 *\fn           static void __exit app_exit(void)
 *\brief        mod exit
 *\return       none
 */
static void __exit app_exit(void)
{
    xt_unregister_match(&app_match);
}

/***************************************************************************/
/*                      PUBLIC_FUNCTIONS                     */
/***************************************************************************/


/***************************************************************************/
/*                      GLOBAL_FUNCTIONS                     */
/***************************************************************************/
module_init(app_init);
module_exit(app_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Hu Luyao <huluyao@tp-link.net>");
MODULE_DESCRIPTION("Xtables: app mark match");
MODULE_ALIAS("ipt_app");
