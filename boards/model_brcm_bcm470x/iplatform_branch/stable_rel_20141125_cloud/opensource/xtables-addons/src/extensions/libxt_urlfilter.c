/*!Copyright(c) 2013-2014 Shenzhen TP-LINK Technologies Co.Ltd.
 *
 *\file     libxt_urlfilter.c
 *\brief    userspace/iptables part for url filter. 
 *
 *\author   Hu Luyao
 *\version  1.0.0
 *\date     23Dec13
 *
 *\history  \arg 1.0.0, based on ac 
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
#include <xtables.h>
#include <netinet/in.h>
#include <linux/netfilter.h>
#include "xt_urlfilter.h"


/***************************************************************************/
/*                      DEFINES                      */
/***************************************************************************/
#define DEBUG 0

 
/***************************************************************************/
/*                      TYPES                            */
/***************************************************************************/
/*!
 *\typedef  QNODE
 *\brief    Type definition of struct _QNODE.
 */
typedef struct _QNODE
{
    signed int state;
    struct _QNODE *next;
}QNODE;

/*!
 *\typedef  QUEUE
 *\brief    Simple QUEUE Structure.
 */
typedef struct _QUEUE
{
    QNODE *head;
    QNODE *tail;
    signed int count;
}QUEUE;


/***************************************************************************/
/*                      EXTERN_PROTOTYPES                    */
/***************************************************************************/


/***************************************************************************/
/*                      LOCAL_PROTOTYPES                     */
/***************************************************************************/
/*!
 *\fn           static void _init_xlatState(xt_acsm_map *info)
 *\brief        translate the state-char table
 *\param[in]    info   xt_acsm_map
 *\return       N/A
 */
static void _init_xlatState(xt_acsm_map *info);

/*!
 *\fn           static void _queue_init (QUEUE * s) 
 *\brief        Init queue.
 *\param[in]    s       Queue header.
 *\return       N/A
 */
static void _queue_init(QUEUE * s);

/*!
 *\fn           static void _queue_add (QUEUE * s, signed int state) 
 *\brief        Add Tail Item to queue.
 *\param[in]    s       Queue header.
 *\param[in]    state   New state.
 *\return       N/A
 */
static void _queue_add(QUEUE * s, signed int state);

/*!
 *\fn           static void _queue_remove (QUEUE * s) 
 *\brief        Remove Head Item from queue.
 *
 *\param[in]    s       Queue header.
 *
 *\return       state
 */
static signed int _queue_remove(QUEUE * s);

/*!
 *\fn           static void _queue_count (QUEUE * s) 
 *\brief        Item count in queue.
 *
 *\param[in]    s       Queue header.
 *
 *\return       N/A
 */
static signed int _queue_count(QUEUE * s);

/*!
 *\fn           static void _queue_count (QUEUE * s) 
 *\brief        Free all the queue.
 *
 *\param[in]    s       Queue header.
 *
 *\return       N/A
 */
static void _queue_free(QUEUE * s);

/*!
 *\fn           static void _acsm_buildNFA (xt_acsm_map *info) 
 *\brief        生成各状态的失败跳转 前面的queue是在这里使用的工具
 *\return       N/A
 */
static void _acsm_buildNFA (xt_acsm_map *info);

/*!
 *\fn           _acsm_addPatternStates (const char *url, size_t len, xt_acsm_map *info) 
 *\brief        add a url into ac state machine
 *\return       success or not
 */
static int _acsm_addPatternStates (const char *url, size_t len, xt_acsm_map *info);

/*!
 *\fn           _acsm_addPatternStates (const char *url, size_t len, xt_acsm_map *info) 
 *\brief        add a url into ac state machine
 *\return       success or not
 */
static int _acsm_addPatternStates (const char *url, size_t len, xt_acsm_map *info);

/*!
 *\fn           static int _goto_write(const char *url, size_t len, xt_acsm_map *info)
 *\brief        interface，将一个独立的url添加进状态转移表中
 *\return       success or not
 */
static int _goto_write(const char *url, size_t len, xt_acsm_map *info);

/*!
 *\fn           static int _fail_write(xt_acsm_map *info)
 *\brief        interface, build the fail jump map
 *\return       success or not
 */
static int _fail_write(xt_acsm_map *info);

/*!
 *\fn           static void _parse_spilt(const char *arg, xt_acsm_map *info)
 *\brief        将以a,b,c长字符串形式的url集合分解
 *\return       none
 */
static void _parse_spilt(const char *arg, xt_acsm_map *info);

/*!
 *\fn           static void urlfilter_help(void)
 *\brief        help information
 *\return       N/A
 */
static void urlfilter_help(void);

/*!
 *\fn           static int urlfilter_parse(int c, char **argv, int invert, unsigned int *flags,
                         const void *entry, struct xt_entry_match **match)
 *\brief        xt_entry_match **match 是提供给内核模块的match数据块，我们的目标就是往其中填写match数据
 *\return       success or not
 */
static int urlfilter_parse(int c, char **argv, int invert, unsigned int *flags,
                         const void *entry, struct xt_entry_match **match);

/*!
 *\fn           static void urlfilter_check(unsigned int flags)
 *\brief        check the flags. 0 means error.
 *\return       none
 */
static void urlfilter_check(unsigned int flags);

/*!
 *\fn           static void urlfilter_print(const void *ip, const struct xt_entry_match *match, int numeric)
 *\brief        iptables print
 *\return       none
 */
static void urlfilter_print(const void *ip, const struct xt_entry_match *match, int numeric);

/*!
 *\fn           static void urlfilter_save(const void *ip, const struct xt_entry_match *match)
 *\brief        iptables print
 *\return       none
 */
static void urlfilter_save(const void *ip, const struct xt_entry_match *match);

/*!
 *\fn           static void urlfilter_init(struct xt_entry_match *match)
 *\brief        iptables init
 *\return       none
 */
static void urlfilter_init(struct xt_entry_match *match);

/***************************************************************************/
/*                      VARIABLES                        */
/***************************************************************************/
static const struct option urlfilter_opts[] = {
    {.name = "url", .has_arg = true, .val = '1'},
    {.name = "mode", .has_arg = true, .val = '2'},
    {.name = "type", .has_arg = true, .val = '3'},
    XT_GETOPT_TABLEEND,
};

static struct xtables_match ipt_urlfilter_match = { 
    .family         = NFPROTO_UNSPEC,
    .name           = "urlfilter",
    .version        = XTABLES_VERSION,
    .size           = XT_ALIGN(sizeof(xt_acsm_map)),
    .userspacesize  = XT_ALIGN(sizeof(xt_acsm_map)),
    .help           = urlfilter_help,
    .parse          = urlfilter_parse,  /* parse the url list, make the ACSM (ac state machine with goto, failure, no output) */
    .init           = urlfilter_init,   
    .final_check    = urlfilter_check,  
    .print          = urlfilter_print,  /* show url list, not ACSM */
    .save           = urlfilter_save,
    .extra_opts     = urlfilter_opts,
};
 
/***************************************************************************/
/*                      LOCAL_FUNCTIONS                  */
/***************************************************************************/
#if 0
/*!
 *\fn           static void _print_acsm(xt_acsm_map *info)
 *\brief        将xt_acsm_map 以一种较为显式的方式打印出来
 *\param[in]    info   xt_acsm_map
 *\return       N/A
 */
static void _print_acsm(xt_acsm_map *info)
{
    int i =0;
    int tmp = 0;
    
    printf("state num: %d\n", info->state_num);
    printf("--abcdefghijklmnopqrstuvwxyz0123456789.:/_-\n");
    for (; i<URLFILTER_MAX_STATE_NUM; i++)
    {
        printf("%d", i);
        for (tmp=0; tmp<URLFILTER_MAX_CHARSET_LEN; tmp++)
        {
            if ( (info->acsm_goto_map[i].NextState[tmp]) == ACSM_FAIL_STATE ) 
            {
                printf("-");
            }
            else if ( (info->acsm_goto_map[i].NextState[tmp]) == ACSM_SUCCESS_STATE ) 
            {
                printf("+");
            }
            else
            {
                printf("%d", (info->acsm_goto_map[i].NextState[tmp]));
            }
        }
        printf("\n");  
    }
}
#endif

/*!
 *\fn           static void _init_xlatState(xt_acsm_map *info)
 *\brief        translate the state-char table
 *\param[in]    info   xt_acsm_map
 *\return       N/A
 */
static void _init_xlatState(xt_acsm_map *info)
{
    signed int cnt;
    signed int i = 0;

    memset(info->xlatstate, 0, sizeof(info->xlatstate));

    for (cnt = 'a'; cnt <= 'z'; cnt ++)
    {
        
        info->xlatstate[cnt] = ++i;

        /* This is the upper Ip_s8acter 'A'~'Z'. */
        info->xlatstate[cnt - 32] = i;
    }

    for (cnt = '0'; cnt <= '9'; cnt ++)
    {
        info->xlatstate[cnt] = ++i;
    }

    info->xlatstate['.'] = ++ i;
    info->xlatstate[':'] = ++ i;
    info->xlatstate['/'] = ++ i;
    info->xlatstate['_'] = ++ i;
    info->xlatstate['-'] = ++ i;
    return;
}

/*!
 *\fn           static void _queue_init (QUEUE * s) 
 *\brief        Init queue.
 *
 *\param[in]    s       Queue header.
 *
 *\return       N/A
 */
static void _queue_init(QUEUE * s) 
{
    s->head  = NULL;
    s->tail  = NULL;
    s->count = 0;
    return;
}

/*!
 *\fn           static void _queue_add (QUEUE * s, signed int state) 
 *\brief        Add Tail Item to queue.
 *
 *\param[in]    s       Queue header.
 *\param[in]    state   New state.
 *
 *\return       N/A
 */
static void _queue_add(QUEUE * s, signed int state) 
{
    
    QNODE * q;
    if (!s->head)
    {
        q = (QNODE *) malloc(sizeof(QNODE));
      
        if (q == NULL)
        {
            printf("queue mem error");
        }
        s->tail  = q;
        s->head  = q;
        q->state = state;
        q->next  = NULL;
    }
    else
    {
        q = (QNODE *) malloc(sizeof(QNODE));
        if (q == NULL)
        {
            printf("queue mem error");
        }
        q->state      = state;
        q->next       = NULL;
        s->tail->next = q;
        s->tail       = q;
    }
    s->count++;
}

/*!
 *\fn           static void _queue_remove (QUEUE * s) 
 *\brief        Remove Head Item from queue.
 *
 *\param[in]    s       Queue header.
 *
 *\return       state
 */
static signed int _queue_remove(QUEUE * s) 
{
    signed int state = 0;
    QNODE * q;
    if (s->head)
    {
        q       = s->head;
        state   = q->state;
        s->head = s->head->next;
        s->count--;
        if (!s->head)
        {
            s->tail  = 0;
            s->count = 0;
        }
        free(q);
    }
    return state;
}

/*!
 *\fn           static void _queue_count (QUEUE * s) 
 *\brief        Item count in queue.
 *
 *\param[in]    s       Queue header.
 *
 *\return       N/A
 */
static signed int _queue_count(QUEUE * s) 
{
    return s->count;
}


/*!
 *\fn           static void _queue_count (QUEUE * s) 
 *\brief        Free all the queue.
 *
 *\param[in]    s       Queue header.
 *
 *\return       N/A
 */
static void _queue_free(QUEUE * s) 
{
    while (_queue_count (s))
    {
        _queue_remove (s);
    }
    return;
}

/*!
 *\fn           static void urlfilter_help(void)
 *\brief        help information
 *\return       N/A
 */
static void urlfilter_help(void)
{
    printf("urlfilter match options:\n"
        "-m urlfilter --url [url1][,url2][,url3]… --mode [mode] --type [type]  Match url info\n");
}

/*!
 *\fn           static void _acsm_buildNFA (xt_acsm_map *info) 
 *\brief        生成各状态的失败跳转 前面的queue是在这里使用的工具
 *\return       N/A
 */
static void _acsm_buildNFA (xt_acsm_map *info) 
{
#if DEBUG
    printf("buildNFA\n");
#endif
    signed int r, s;
    signed int i;
    signed int fs, next;
    QUEUE q, *queue = &q;
                
    /* Init a Queue */ 
    _queue_init (queue);
  
    /* Add the state 0 transitions 1st */ 
    for (i = 0; i < URLFILTER_MAX_CHARSET_LEN; i++)
    {
        s = info->acsm_goto_map[0].NextState[i];
        if (s)
        {
            _queue_add (queue, s);
            info->acsm_goto_map[s].FailState = 0;
        }
    }

    /* Build the fail state transitions for each valid state */ 
    while (_queue_count (queue) > 0)
    {
        r = _queue_remove (queue);
 
        /* Find Final States for any Failure */ 
        for (i = 0; i < URLFILTER_MAX_CHARSET_LEN; i++)
        {
            /* init */
            fs = 0;
            next = 0;
            
            /* 要对成功状态做特殊处理 */
            if ((s = info->acsm_goto_map[r].NextState[i]) != ACSM_FAIL_STATE)
            {
                if (s != ACSM_SUCCESS_STATE)
                {
                    _queue_add (queue, s);
                }
                
                fs = (signed int) info->acsm_goto_map[r].FailState;
               
                while ((next=info->acsm_goto_map[fs].NextState[i]) < 0)
                {
                    fs = (signed int) info->acsm_goto_map[fs].FailState;
                }
               
                info->acsm_goto_map[s].FailState = next;
            }

        }
    }

    _queue_free (queue);
    return;
}

/*!
 *\fn           static void _acsm_convertNFA2DFA (xt_acsm_map *info) 
 *\brief        把不确定跳转(NFA)转换为确定跳转(DFA)
 *\return       N/A
 */
static void _acsm_convertNFA2DFA (xt_acsm_map *info) 
{
#if DEBUG
    printf("buildDFA\n");
#endif
    signed int r, s;
    signed int i;
    QUEUE q, *queue = &q;
  
    /* Init a Queue */ 
    _queue_init (queue);
  
    /* Add the state 0 transitions 1st */ 
    for (i = 0; i < URLFILTER_MAX_CHARSET_LEN; i++)
    {
        s = info->acsm_goto_map[0].NextState[i];
        if (s)
        {
        _queue_add (queue, s);
        }
    }
    /* Start building the next layer of transitions */ 
    while (_queue_count (queue) > 0)
    {
        r = _queue_remove (queue);

        /* State is a branch state */ 
        for (i = 0; i < URLFILTER_MAX_CHARSET_LEN; i++)
        {
            if ((s = info->acsm_goto_map[r].NextState[i]) > 0)
            {
                _queue_add (queue, s);
            }

            else if (s == ACSM_SUCCESS_STATE)
            {
                /* do nothing */
            }
 
            else
            {
                info->acsm_goto_map[r].NextState[i] =
                info->acsm_goto_map[info->acsm_goto_map[r].FailState].NextState[i];       
            }
        }
        
    }
    /* Clean up the queue */ 
    _queue_free (queue);
    return;
}

/*!
 *\fn           _acsm_addPatternStates (const char *url, size_t len, xt_acsm_map *info) 
 *\brief        add a url into ac state machine
 *\return       success or not
 */
static int _acsm_addPatternStates (const char *url, size_t len, xt_acsm_map *info) 
{
#if DEBUG
    printf("add a url\n");
#endif
    int state=0, next;
     /* 
     *  Match up pattern with existing states
     */ 
    for (; len > 1; url++, len--)
    {
        next = info->acsm_goto_map[state].NextState[info->xlatstate[(int) *url]];
        if (next < 0)
        {
            break;
        }
        state = next;
    }
  
    /*
     *   Add new states for the rest of the url bytes, 1 state per byte
     */ 
    if (next != ACSM_SUCCESS_STATE)
    {
        for (; len > 1; url++, len--)
        {
            info->state_num++;
            info->acsm_goto_map[state].NextState[info->xlatstate[(int) *url]] = info->state_num;
            state = info->state_num;
        }
        
        /* 这里可能引起问题 ，重点关注一下*/
        info->acsm_goto_map[state].NextState[info->xlatstate[(int) *url]] = ACSM_SUCCESS_STATE;
    }

    return TRUE;
       
}

/*!
 *\fn           static int _goto_write(const char *url, size_t len, xt_acsm_map *info)
 *\brief        interface，将一个独立的url添加进状态转移表中
 *\return       success or not
 */
static int _goto_write(const char *url, size_t len, xt_acsm_map *info)
{
    /* make sure len < URLFILTER_STRLEN*/
#if DEBUG
    printf("url = %s, len = %d \n", url, len);
#endif
    /* 构筑跳转表 */
    return _acsm_addPatternStates(url, len, info);
}

/*!
 *\fn           static int _fail_write(xt_acsm_map *info)
 *\brief        interface, build the fail jump map
 *\return       success or not
 */
static int _fail_write(xt_acsm_map *info)
{
#if DEBUG
    printf("fail_write\n");
#endif
    int i;
    for (i = 0; i < URLFILTER_MAX_CHARSET_LEN; i++)
    {
        if (info->acsm_goto_map[0].NextState[i] == ACSM_FAIL_STATE)
        {
            info->acsm_goto_map[0].NextState[i] = 0;
        }
    }

    _acsm_buildNFA (info);
    _acsm_convertNFA2DFA (info);

    return TRUE; 
}

/*!
 *\fn           static void _parse_spilt(const char *arg, xt_acsm_map *info)
 *\brief        将以a,b,c长字符串形式的url集合分解
 *\return       none
 */
static void _parse_spilt(const char *arg, xt_acsm_map *info)
{
    char *pIndex;

    /* write arg to info*/
    while ((pIndex = strchr(arg, ',')) != NULL)
    {
        if (pIndex == arg || !_goto_write(arg, pIndex - arg, info))
        {
            xtables_error(PARAMETER_PROBLEM, "Bad url name \"%s\"", arg);
        }
        arg = pIndex + 1;
    }

    if (!*arg)
    {
        xtables_error(PARAMETER_PROBLEM, "\"--urlfilter\" requires a list of "
                          "host name with no spaces, e.g. "
                          "www.baidu.com,www.taobao.com");
    }

    if (strlen(arg) == 0 || !_goto_write(arg, strlen(arg), info))
    {
        xtables_error(PARAMETER_PROBLEM, "Bad host name \"%s\"", arg);
    }

    _fail_write(info);

}


/*!
 *\fn           static int urlfilter_parse(int c, char **argv, int invert, unsigned int *flags,
                         const void *entry, struct xt_entry_match **match)
 *\brief        xt_entry_match **match 是提供给内核模块的match数据块，我们的目标就是往其中填写match数据
 *\return       success or not
 */
static int urlfilter_parse(int c, char **argv, int invert, unsigned int *flags,
                         const void *entry, struct xt_entry_match **match)
{

#if DEBUG 
    printf("urlfilter_parse\n");
#endif

    /* 对应内存 */
    xt_acsm_map *info = (xt_acsm_map *)(*match)->data;

    /* c means the "--" option. look "value" in urlfilter_opts[] */
    switch (c) {
    case '1':
#if DEBUG 
        printf("optarg = %s\n",optarg);
#endif   
        strncpy(info->para, optarg, URLFILTER_MAX_URL_NUM*(URLFILTER_MAX_URL_LEN + 1));
        info->para[URLFILTER_MAX_URL_NUM*(URLFILTER_MAX_URL_LEN + 1)-1] = '\0'; 
        _parse_spilt(optarg, info);
#if DEBUG
            printf("states:%d\n", info->state_num);
            printf("max states:%d\n", MAX_STATE);
#endif
        if ((info->state_num) >= MAX_STATE)
        {
#if DEBUG
            printf("too many states, out of memory\n");
#endif
            *flags = 0;
        }

        else
        {
            *flags = 1;
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
 *\fn           static void urlfilter_check(unsigned int flags)
 *\brief        check the flags. 0 means error.
 *\return       none
 */
static void urlfilter_check(unsigned int flags)
{

#if DEBUG 
    printf("urlfilter_check: flags = %u\n", flags);
#endif

    if (flags == 0)
    {
        xtables_error(PARAMETER_PROBLEM, "input error: too many urls or no urls'\n ");
    }
}

/*!
 *\fn           static void urlfilter_print(const void *ip, const struct xt_entry_match *match, int numeric)
 *\brief        iptables print
 *\return       none
 */
static void urlfilter_print(const void *ip, const struct xt_entry_match *match, int numeric)
{

    char mode[16];
    char type[16];

    xt_acsm_map *info = (xt_acsm_map *)match->data;

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

    printf("--url %s --mode %s --type %s ", info->para, mode, type);
}

/*!
 *\fn           static void urlfilter_save(const void *ip, const struct xt_entry_match *match)
 *\brief        iptables print
 *\return       none
 */
static void urlfilter_save(const void *ip, const struct xt_entry_match *match)
{

    char mode[16];
    char type[16];

    xt_acsm_map *info = (xt_acsm_map *)match->data;

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

    printf("--url %s --mode %s --type %s ", info->para, mode, type);

}


/*!
 *\fn           static void urlfilter_init(struct xt_entry_match *match)
 *\brief        iptables init
 *\return       none
 */
static void urlfilter_init(struct xt_entry_match *match)
{

#if DEBUG 
    printf("urlfilter_init\n");
#endif

    xt_acsm_map *info = (xt_acsm_map *)match->data;

    /* init xt_acsm_map*/
    /* 由于代码中是calloc，因此全部被默认初始化为0 */
    /* 这里将其初始化为fail状态 */
    memset(info, ACSM_FAIL_STATE, sizeof(xt_acsm_map));
    info->state_num = 0;

    /*_init_xlatCase();*/
    _init_xlatState(info); 
    
}


/***************************************************************************/
/*                      IPTABLES_FUNCTIONS                     */
/***************************************************************************/


static __attribute__((constructor)) void _urlfilter_mt_ldr(void)
{
    xtables_register_match(&ipt_urlfilter_match);
}


/***************************************************************************/
/*                      GLOBAL_FUNCTIONS                     */
/***************************************************************************/
