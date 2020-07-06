/*!Copyright(c) 2013-2014 Shenzhen TP-LINK Technologies Co.Ltd.
 *
 *\file     libxt_app.c
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
#include "xt_app.h"


/***************************************************************************/
/*                      DEFINES                      */
/***************************************************************************/

 
/***************************************************************************/
/*                      TYPES                            */
/***************************************************************************/
static const struct option app_opts[] = {
    {.name = "id", .has_arg = true, .val = '1'},
    XT_GETOPT_TABLEEND,
};


/***************************************************************************/
/*                      EXTERN_PROTOTYPES                    */
/***************************************************************************/


/***************************************************************************/
/*                      LOCAL_PROTOTYPES                     */
/***************************************************************************/
/*!
 *\fn           static void app_help(void)
 *\brief        help information
 *\return       N/A
 */
static void app_help(void);

/*!
 *\fn          static int app_parse(int c, char **argv, int invert, unsigned int *flags,
                         const void *entry, struct xt_entry_match **match)
 *\brief        xt_entry_match **match 是提供给内核模块的match数据块，我们的目标就是往其中填写match数据
 *\return       success or not
 */
static int app_parse(int c, char **argv, int invert, unsigned int *flags,
                         const void *entry, struct xt_entry_match **match);

/*!
 *\fn           static void app_check(unsigned int flags)
 *\brief        check the flags. 0 means error.
 *\return       none
 */
static void app_check(unsigned int flags);

/*!
 *\fn           static void app_print(const void *ip, const struct xt_entry_match *match, int numeric)
 *\brief        iptables print
 *\return       none
 */
static void app_print(const void *ip, const struct xt_entry_match *match, int numeric);

/*!
 *\fn           static void app_save(const void *ip, const struct xt_entry_match *match)
 *\brief        iptables save
 *\return       none
 */
static void app_save(const void *ip, const struct xt_entry_match *match);

/*!
 *\fn           static void _parse_spilt(const char *arg, struct _xt_app_info *info)
 *\brief        将以a,b,c长字符串形式的url集合分解
 *\return       none
 */
static void _parse_spilt(const char *arg, struct _xt_app_info *info);

/*!
 *\fn           static void app_init(struct xt_entry_match *match)
 *\brief        iptables init
 *\return       none
 */
static void app_init(struct xt_entry_match *match);

/*!
 *\fn           static int _parse_write(const char *host, size_t len, struct _xt_app_info *info)
 *\brief        write urls into info->host_names[]
 *\return       N/A
 */
static int _parse_write(const char *host, struct _xt_app_info *info);

/***************************************************************************/
/*                      VARIABLES                        */
/***************************************************************************/
static struct xtables_match app_match = { 
    .family         = NFPROTO_UNSPEC,
    .name           = "app",
    .version        = XTABLES_VERSION,
    .size           = XT_ALIGN(sizeof(struct _xt_app_info)),
    .userspacesize  = XT_ALIGN(sizeof(struct _xt_app_info)),
    .help           = app_help,
    .parse          = app_parse,
    .init           = app_init,
    .final_check    = app_check,
    .print          = app_print,
    .save           = app_save,
    .extra_opts     = app_opts,
};

 
/***************************************************************************/
/*                      LOCAL_FUNCTIONS                  */
/***************************************************************************/
/*!
 *\fn           static void app_help(void)
 *\brief        help information
 *\return       N/A
 */
static void app_help(void)
{
    printf("app match options:\n"
    "-m app --id \n");
}

/*!
 *\fn           static int _parse_write(const char *host, size_t len, struct _xt_app_info *info)
 *\brief        write urls into info->host_names[]
 *\return       N/A
 */
static int _parse_write(const char *host, struct _xt_app_info *info)
{
    sscanf(host, "%hd", &(info->id[info->count]));
    printf("id_%d: %hd\n", info->count, info->id[info->count]);
    info->count ++;
    return TRUE;
}

/*!
 *\fn           static void _parse_spilt(const char *arg, struct _xt_app_info *info)
 *\brief        将以a,b,c长字符串形式的url集合分解
 *\return       none
 */
static void _parse_spilt(const char *arg, struct _xt_app_info *info)
{
    char *pIndex;

    /* write arg to info*/
    while ((pIndex = strchr(arg, ',')) != NULL)
    {   
        if (pIndex == arg || !_parse_write(arg, info))
        {
            xtables_error(PARAMETER_PROBLEM, "Bad host name \"%s\"", arg);
        }
        arg = pIndex + 1;
    }

    if (!*arg)
    {
        xtables_error(PARAMETER_PROBLEM, "\"--app\" requires a list of "
                          "host name with no spaces, e.g. "
                          "www.baidu.com,www.taobao.com");
    }

    if (strlen(arg) == 0 || !_parse_write(arg, info))
    {
        xtables_error(PARAMETER_PROBLEM, "Bad host name \"%s\"", arg);
    }
}

/*!
 *\fn          static int app_parse(int c, char **argv, int invert, unsigned int *flags,
                         const void *entry, struct xt_entry_match **match)
 *\brief        xt_entry_match **match 是提供给内核模块的match数据块，我们的目标就是往其中填写match数据
 *\return       success or not
 */
static int app_parse(int c, char **argv, int invert, unsigned int *flags,
                         const void *entry, struct xt_entry_match **match)
{
    struct _xt_app_info *info = (struct _xt_app_info *)(*match)->data;
    /* c means the "--" option. look "value" in urlfilter_opts[] */
    switch (c) {
    /* id */
    case '1':
        _parse_spilt(optarg, info);
        *flags = 1;
        break;

    default:
        return FALSE;
    }
    return TRUE;
}

/*!
 *\fn           static void app_check(unsigned int flags)
 *\brief        check the flags. 0 means error.
 *\return       none
 */
static void app_check(unsigned int flags)
{
    if (flags == 0)
    {
        xtables_error(PARAMETER_PROBLEM, "app match: You must specify `--id'\n ");
    }
}

/*!
 *\fn           static void app_print(const void *ip, const struct xt_entry_match *match, int numeric)
 *\brief        iptables print
 *\return       none
 */
static void app_print(const void *ip, const struct xt_entry_match *match, int numeric)
{
    struct _xt_app_info *info = (struct _xt_app_info *)match->data;
    int i;
    char arg[MAX_ID * MAX_ID_LEN];
    char tmp[MAX_ID_LEN];

    memset(arg, 0, MAX_ID * MAX_ID_LEN);
    memset(tmp, 0, MAX_ID_LEN);

     /* init */
    arg[0] = '\0';

    if ((info->id[0]) >= 0)
    {
        sprintf(tmp, "%hd", info->id[0]);
        strcat(arg, tmp);
        memset(tmp, 0, MAX_ID_LEN);

        for (i = 1; i < MAX_ID; ++i)
        {
            if ((info->id[i]) <= 0)
            {
                break;
            }
            strcat(arg, ",");
            sprintf(tmp, "%hd", info->id[i]);
            strcat(arg, tmp);
            memset(tmp, 0, MAX_ID_LEN);      
        } 
    }
    printf("--id %s ", arg);
}

/*!
 *\fn           static void app_save(const void *ip, const struct xt_entry_match *match)
 *\brief        iptables save
 *\return       none
 */
static void app_save(const void *ip, const struct xt_entry_match *match)
{
    struct _xt_app_info *info = (struct _xt_app_info *)match->data;
    int i;
    char arg[MAX_ID * MAX_ID_LEN];
    char tmp[MAX_ID_LEN];

    memset(arg, 0, MAX_ID * MAX_ID_LEN);
    memset(tmp, 0, MAX_ID_LEN);

     /* init */
    arg[0] = '\0';

    if ((info->id[0]) >= 0)
    {
        sprintf(tmp, "%hd", info->id[0]);
        strcat(arg, tmp);
        memset(tmp, 0, MAX_ID_LEN);

        for (i = 1; i < MAX_ID; ++i)
        {
            if ((info->id[i]) <= 0)
            {
                break;
            }
            strcat(arg, ",");
            sprintf(tmp, "%hd", info->id[i]);
            strcat(arg, tmp);
            memset(tmp, 0, MAX_ID_LEN);      
        } 
    }
    printf("--id %s ", arg);
}

/*!
 *\fn           static void app_init(struct xt_entry_match *match)
 *\brief        iptables init
 *\return       none
 */
static void app_init(struct xt_entry_match *match)
{
    struct _xt_app_info *info = (struct _xt_app_info *)match->data;
    memset(info, 0, sizeof(struct _xt_app_info));
}

/***************************************************************************/
/*                      PUBLIC_FUNCTIONS                     */
/***************************************************************************/
/*!
 *\fn           static void _init(void)
 *\brief        iptables register
 *\return       none
 */
static __attribute__((constructor)) void _app_mt_ldr(void)
{
    xtables_register_match(&app_match);
}

/***************************************************************************/
/*                      GLOBAL_FUNCTIONS                     */
/***************************************************************************/
