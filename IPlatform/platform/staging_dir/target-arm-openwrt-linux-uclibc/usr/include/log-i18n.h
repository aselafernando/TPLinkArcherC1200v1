/*
 * System log i18n database.
 * Copyright(c) 2014 Shenzhen TP-LINK Technologies Co.Ltd.
 *
 * Author  : Ye Qianchuan <yeqianchuan@tp-link.net>
 * Detail  : I18n database description
 * Version : 1.0
 * Date    : 3 Apr, 2014
 */

#ifndef __LOG_I18N_LOG_I18N_H__
#define __LOG_I18N_LOG_I18N_H__

#include <stdint.h>

#define LOG_I18N_DEFAULT_LANG      "en_US"

#define LOG_I18N_LIB               "/usr/lib/log-i18n/"
#define LOG_I18N_LIB_META          LOG_I18N_LIB "meta.so"
#define LOG_I18N_PROJ_NAME_MAX     (64)
#define LOG_I18N_LANG_NAME_MAX     (16)
#define LOG_I18N_LIB_LANG_PATH_MAX (64)

#define LOG_I18N_PROJ_ID_COMMON    (0)
#define LOG_I18N_PROJ_ID_MIN       (1)
#define LOG_I18N_PROJ_ID_MAX       (999)
#define LOG_I18N_MSG_ID_MIN        (0)
#define LOG_I18N_MSG_ID_MAX        (499)
#define LOG_I18N_MSG_ID_COMMON_MIN (500)
#define LOG_I18N_MSG_ID_COMMON_MAX (999)

#define LOG_I18N_MSG_TBL_SYMBOL    "log_msg_tbl"
#define LOG_I18N_META_TBL_SYMBOL   "log_meta_tbl"

#define LOG_MKID(_proj_id, _msg_id)                           \
    (uint32_t)(((uint16_t)_proj_id << 16) + (uint16_t)_msg_id)

struct log_msg_record
{
    uint32_t id;
    const char *msg;
};

struct log_prio_record
{
    uint32_t id;
    const char prio;
};

struct log_name_record
{
    uint16_t proj_id;
    const char *name;
};

struct log_msg_tbl
{
    const struct log_msg_record const *tbl;
    uint32_t len;
};

struct log_prio_tbl
{
    const struct log_prio_record const *tbl;
    uint32_t len;
};

struct log_name_tbl
{
    const struct log_name_record const *tbl;
    uint32_t len;
};

struct log_meta_tbl
{
    struct log_name_tbl name_tbl;
    struct log_prio_tbl prio_tbl;
};

#endif  /*__LOG_I18N_LOG_I18N_H__ */
