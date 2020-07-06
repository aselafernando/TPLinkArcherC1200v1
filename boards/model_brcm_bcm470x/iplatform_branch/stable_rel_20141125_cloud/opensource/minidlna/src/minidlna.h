/*
 * =====================================================================================
 *
 *       Filename:  minidlna.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/06/2014 02:03:28 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Zhu Xianfeng<zhuxianfeng@tp-link.net>
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef __MINIDLNA_H__
#define __MINIDLNA_H__

#include <sys/epoll.h>

typedef void (*EVENT_HANDLER)(void *data);

struct event_handler
{
    void *data;
    EVENT_HANDLER handler;
};

struct event_struct
{
    int sock;
    struct event_handler read;
    struct event_handler write;
};

struct event_struct *event_malloc(int sock);
void event_free(struct event_struct *evt);
int event_ctl(struct event_struct *evt, int opt, int evts);
int event_del(struct event_struct *evt);

#endif
