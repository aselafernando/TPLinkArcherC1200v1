/*!Copyright(c) 2013-2014 Shenzhen TP-LINK Technologies Co.Ltd.
 *
 *\file     libxt_httphost.c
 *\brief    userspace/iptables part for http host filter. 
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
#include <stdbool.h>
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <netinet/in.h>
#include <xtables.h>
#include <linux/netfilter.h>
#include "xt_httphost.h"


/***************************************************************************/
/*                      DEFINES                      */
/***************************************************************************/

 
/***************************************************************************/
/*                      TYPES                            */
/***************************************************************************/
static const struct option httphost_opts[] = {
    {.name = "host", .has_arg = true, .val = '1'},
    {.name = "mode", .has_arg = true, .val = '2'},
    {.name = "type", .has_arg = true, .val = '3'},
    XT_GETOPT_TABLEEND,
};


/***************************************************************************/
/*                      EXTERN_PROTOTYPES                    */
/***************************************************************************/


/***************************************************************************/
/*                      LOCAL_PROTOTYPES                     */
/***************************************************************************/
/*!
 *\fn           static void httphost_help(void)
 *\brief        help information
 *\return       N/A
 */
static void httphost_help(void);

/*!
 *\fn           static int _parse_write(const char *host, size_t len, struct _xt_httphost_info *info)
 *\brief        write urls into info->host_names[]
 *\return       N/A
 */
static int _parse_write(const char *host, size_t len, struct _xt_httphost_info *info);

/*!
 *\fn           static void _parse_spilt(const char *arg, struct _xt_httphost_info *info)
 *\brief        将以a,b,c长字符串形式的url集合分解
 *\return       none
 */
static void _parse_spilt(const char *arg, struct _xt_httphost_info *info);

/*!
 *\fn          static int httphost_parse(int c, char **argv, int invert, unsigned int *flags,
                         const void *entry, struct xt_entry_match **match)
 *\brief        xt_entry_match **match 是提供给内核模块的match数据块，我们的目标就是往其中填写match数据
 *\return       success or not
 */
static int httphost_parse(int c, char **argv, int invert, unsigned int *flags,
                         const void *entry, struct xt_entry_match **match);

/*!
 *\fn           static void httphost_check(unsigned int flags)
 *\brief        check the flags. 0 means error.
 *\return       none
 */
static void httphost_check(unsigned int flags);

/*!
 *\fn           static void httphost_print(const void *ip, const struct xt_entry_match *match, int numeric)
 *\brief        iptables print
 *\return       none
 */
static void httphost_print(const void *ip, const struct xt_entry_match *match, int numeric);

/*!
 *\fn           static void httphost_save(const void *ip, const struct xt_entry_match *match)
 *\brief        iptables save
 *\return       none
 */
static void httphost_save(const void *ip, const struct xt_entry_match *match);

/*!
 *\fn           static void httphost_init(struct xt_entry_match *match)
 *\brief        iptables init
 *\return       none
 */
static void httphost_init(struct xt_entry_match *match);

/***************************************************************************/
/*                      VARIABLES                        */
/***************************************************************************/
static struct xtables_match httphost_match = { 
    .family         = NFPROTO_UNSPEC,
    .name           = "httphost",
    .version        = XTABLES_VERSION,
    .size           = XT_ALIGN(sizeof(struct _xt_httphost_info)),
    .userspacesize  = XT_ALIGN(sizeof(struct _xt_httphost_info)),
    .help           = httphost_help,
    .parse          = httphost_parse,
    .init           = httphost_init,
    .final_check    = httphost_check,
    .print          = httphost_print,
    .save           = httphost_save,
    .extra_opts     = httphost_opts,
};

 
/***************************************************************************/
/*                      LOCAL_FUNCTIONS                  */
/***************************************************************************/
/*!
 *\fn           static void httphost_help(void)
 *\brief        help information
 *\return       N/A
 */
static void httphost_help(void)
{
    printf("httphost match options:\n"
    "--httphost host1[,host2][,host3]…  --mode [mode] --type [type]   Match http host section value\n");
}

/*!
 *\fn           static int _parse_write(const char *host, size_t len, struct _xt_httphost_info *info)
 *\brief        write urls into info->host_names[]
 *\return       N/A
 */
static int _parse_write(const char *host, size_t len, struct _xt_httphost_info *info)
{
    /* make sure len < URLFILTER_MAX_URL_LEN*/
    if (strncpy(info->host_names[info->count], host, len) == NULL)
    {
        return FALSE;
    }
    info->count ++;
    return TRUE;
}

/*!
 *\fn           static void _parse_spilt(const char *arg, struct _xt_httphost_info *info)
 *\brief        将以a,b,c长字符串形式的url集合分解
 *\return       none
 */
static void _parse_spilt(const char *arg, struct _xt_httphost_info *info)
{
    char *pIndex;

    /* write arg to info*/
    while ((pIndex = strchr(arg, ',')) != NULL)
    {   
        if (pIndex == arg || !_parse_write(arg, pIndex - arg, info))
        {
            xtables_error(PARAMETER_PROBLEM, "Bad host name \"%s\"", arg);
        }
        arg = pIndex + 1;
    }

    if (!*arg)
    {
        xtables_error(PARAMETER_PROBLEM, "\"--httphost\" requires a list of "
                          "host name with no spaces, e.g. "
                          "www.baidu.com,www.taobao.com");
    }

    if (strlen(arg) == 0 || !_parse_write(arg, strlen(arg), info))
    {
        xtables_error(PARAMETER_PROBLEM, "Bad host name \"%s\"", arg);
    }
}

/*!
 *\fn          static int httphost_parse(int c, char **argv, int invert, unsigned int *flags,
                         const void *entry, struct xt_entry_match **match)
 *\brief        xt_entry_match **match 是提供给内核模块的match数据块，我们的目标就是往其中填写match数据
 *\return       success or not
 */
static int httphost_parse(int c, char **argv, int invert, unsigned int *flags,
                         const void *entry, struct xt_entry_match **match)
{
    int i;
    struct _xt_httphost_info *info = (struct _xt_httphost_info *)(*match)->data;
    /* c means the "--" option. look "value" in urlfilter_opts[] */
    switch (c) {
    /* urls */
    case '1':
        _parse_spilt(optarg, info);
        *flags = 1;
        for (i = 0; i < URLFILTER_MAX_URL_NUM; ++i)
        {
            if ('\0' == *(info->host_names[i]))
            {
                break;
            }
            printf("url #%d = %s\n",i ,info->host_names[i]);
        }
        break;

    /* mode */
    case '2':
        if (!strcmp(optarg, "white"))
        {
            info->mode = 1;
        }
        else if (!strcmp(optarg, "black"))
        {
            info->mode = 0;
        }
        else
        {
            info->mode = -1;
        }
        break;

    /* type */
    case '3':
        if (!strcmp(optarg, "http"))
        {
            info->type = 0;
        }
        else if (!strcmp(optarg, "dns"))
        {
            info->type = 1;
        }
        else
        {
            info->type = -1;
        }
        break;

    default:
        return FALSE;
    }
    return TRUE;
}

/*!
 *\fn           static void httphost_check(unsigned int flags)
 *\brief        check the flags. 0 means error.
 *\return       none
 */
static void httphost_check(unsigned int flags)
{
    if (flags == 0)
    {
        xtables_error(PARAMETER_PROBLEM, "httphost match: You must specify `--httphost'\n ");
    }
}

/*!
 *\fn           static void httphost_print(const void *ip, const struct xt_entry_match *match, int numeric)
 *\brief        iptables print
 *\return       none
 */
static void httphost_print(const void *ip, const struct xt_entry_match *match, int numeric)
{
    int i;
    char arg[URLFILTER_MAX_URL_NUM*(URLFILTER_MAX_URL_LEN + 1)];
    char mode[16];
    char type[16];

    struct _xt_httphost_info *info = (struct _xt_httphost_info *)match->data;

     /* init */
    arg[0] = '\0';

    if ('\0' != *(info->host_names[0]))
    {
        strcat(arg, info->host_names[0]);

        for (i = 1; i < URLFILTER_MAX_URL_NUM; ++i)
        {
            if ('\0' == *(info->host_names[i]))
            {
                break;
            }
            strcat(arg, ",");
            strcat(arg, info->host_names[i]);      
        } 
    }

    switch (info->mode) 
    {
    case 0:
        strcpy(mode, "black");
        break;
    case 1:
        strcpy(mode, "white");
        break;
    default:
        strcpy(mode, "none");
    }

    switch (info->type) 
    {
    case 0:
        strcpy(type, "http");
        break;
    case 1:
        strcpy(type, "dns");
        break;
    default:
        strcpy(type, "none");
    }

    printf("--host %s --mode %s --type %s ", arg, mode, type);
}

/*!
 *\fn           static void httphost_save(const void *ip, const struct xt_entry_match *match)
 *\brief        iptables save
 *\return       none
 */
static void httphost_save(const void *ip, const struct xt_entry_match *match)
{
    int i;
    char arg[URLFILTER_MAX_URL_NUM*(URLFILTER_MAX_URL_LEN + 1)];
    char mode[16];
    char type[16];

    struct _xt_httphost_info *info = (struct _xt_httphost_info *)match->data;

     /* init */
    arg[0] = '\0';

    if ('\0' != *(info->host_names[0]))
    {
        strcat(arg, info->host_names[0]);

        for (i = 1; i < URLFILTER_MAX_URL_NUM; ++i)
        {
            if ('\0' == *(info->host_names[i]))
            {
                break;
            }
            strcat(arg, ",");
            strcat(arg, info->host_names[i]);      
        } 
    }

    switch (info->mode) 
    {
    case 0:
        strcpy(mode, "black");
        break;
    case 1:
        strcpy(mode, "white");
        break;
    default:
        strcpy(mode, "none");
    }

    switch (info->type) 
    {
    case 0:
        strcpy(type, "http");
        break;
    case 1:
        strcpy(type, "dns");
        break;
    default:
        strcpy(type, "none");
    }
    printf("--host %s --mode %s --type %s ", arg, mode, type);
}

/*!
 *\fn           static void httphost_init(struct xt_entry_match *match)
 *\brief        iptables init
 *\return       none
 */
static void httphost_init(struct xt_entry_match *match)
{
    struct _xt_httphost_info *info = (struct _xt_httphost_info *)match->data;
    info->count = 0;
}

/***************************************************************************/
/*                      PUBLIC_FUNCTIONS                     */
/***************************************************************************/
/*!
 *\fn           static void _init(void)
 *\brief        iptables register
 *\return       none
 */
static __attribute__((constructor)) void _httphost_mt_ldr(void)
{
    xtables_register_match(&httphost_match);
}

/***************************************************************************/
/*                      GLOBAL_FUNCTIONS                     */
/***************************************************************************/
