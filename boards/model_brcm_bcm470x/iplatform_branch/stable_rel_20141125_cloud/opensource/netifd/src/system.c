/*
 * netifd - network interface daemon
 * Copyright (C) 2012 Felix Fietkau <nbd@openwrt.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
#include "netifd.h"
#include "system.h"
#include <fcntl.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

static const struct blobmsg_policy tunnel_attrs[__TUNNEL_ATTR_MAX] = {
	[TUNNEL_ATTR_TYPE] = { "mode", BLOBMSG_TYPE_STRING },
	[TUNNEL_ATTR_LOCAL] = { "local", BLOBMSG_TYPE_STRING },
	[TUNNEL_ATTR_REMOTE] = { "remote", BLOBMSG_TYPE_STRING },
	[TUNNEL_ATTR_TTL] = { "ttl", BLOBMSG_TYPE_INT32 },
	[TUNNEL_ATTR_6RD_PREFIX] = { "6rd-prefix", BLOBMSG_TYPE_STRING },
	[TUNNEL_ATTR_6RD_RELAY_PREFIX] = { "6rd-relay-prefix", BLOBMSG_TYPE_STRING },
	[TUNNEL_ATTR_LINK] = { "link", BLOBMSG_TYPE_STRING },
};

const struct config_param_list tunnel_attr_list = {
	.n_params = __TUNNEL_ATTR_MAX,
	.params = tunnel_attrs,
};

void system_fd_set_cloexec(int fd)
{
#ifdef FD_CLOEXEC
	fcntl(fd, F_SETFD, fcntl(fd, F_GETFD) | FD_CLOEXEC);
#endif
}

int system_exec_cmd(char *cmd)
{
    pid_t pid;
    int status = 0;

    if ((pid = vfork()) < 0)
    {
        return -1;
    }
    else if (pid == 0)
    {
        /* child process */
        char *argv[4];
        int fd;
        
        fd = open("/dev/null", O_RDWR);
        dup2(fd, 0);
        dup2(fd, 1);
        dup2(fd, 2);
        close(fd);

        argv[0] = "sh";
        argv[1] = "-c";
        argv[2] = cmd;
        argv[3] = NULL;
        execvp("sh", argv);

        /* not usually reached */
        exit(1);
    }
    else
    {
        /* parent process */
        waitpid(pid, &status, 0);
    }

    if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
    {
        return 0;
    }

    return -1;
}

int system_exec_fmt(char *fmt, ...)
{
    char buf[1024];
    va_list valist;

    va_start(valist, fmt);
    vsnprintf(buf, sizeof(buf), fmt, valist);
    va_end(valist);

    return system_exec_cmd(buf);
}

