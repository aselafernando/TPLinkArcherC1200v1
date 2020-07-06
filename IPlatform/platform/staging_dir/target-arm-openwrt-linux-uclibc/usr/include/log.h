/*
 * System log module.
 * Copyright(c) 2014 Shenzhen TP-LINK Technologies Co.Ltd.
 *
 * Author  : Ye Qianchuan <yeqianchuan@tp-link.net>
 * Detail  : System log interface for C.
 * Version : 1.0
 * Date    : 3 Apr, 2014
 */

#ifndef __LOG_LOG_H__
#define __LOG_LOG_H__

#ifndef __KERNEL__
extern void logvp(unsigned int proj_id, unsigned int msg_id,
                    unsigned int pid, const char *argv[]);
#endif

extern void logv(unsigned int proj_id, unsigned int msg_id,
                   const char *argv[]);
extern void _log(unsigned int proj_id, unsigned int msg_id, ...);

#define LP_STRING    (1)
#define LP_INT32     (2)
#define LP_UINT32    (3)
#define LP_UINT32HEX (4)
#define LP_IPV4      (5)

#define LP_END       (0)

#define log(proj_id, msg_id, ...)                   \
    _log(proj_id, msg_id, ##__VA_ARGS__, LP_END)
#define LOG(msg_id, ...)                        \
    log(LOG_PROJ_ID, msg_id, ##__VA_ARGS__)

#endif  /* __LOG_LOG_H__ */
