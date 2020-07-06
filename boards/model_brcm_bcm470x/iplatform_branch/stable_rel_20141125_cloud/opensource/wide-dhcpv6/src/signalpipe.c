/* signalpipe.c
 *
 * Signal pipe infrastructure. A reliable way of delivering signals.
 *
 * Russ Dill <Russ.Dill@asu.edu> December 2003
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include "signalpipe.h"

#include <sys/time.h>
 #include <unistd.h>


static int signal_pipe[2];

static void signal_handler(int sig)
{
	if (send(signal_pipe[1], &sig, sizeof(sig), MSG_DONTWAIT) < 0)
		printf("%s %d",__FUNCTION__,__LINE__);
}


/* Call this before doing anything else. Sets up the socket pair
 * and installs the signal handler */
int udhcp_sp_setup(void)
{
	#if 0
	socketpair(AF_LOCAL, SOCK_STREAM, 0, signal_pipe);
	printf("%s %d signal_pipe[0] = %d, signal_pipe[1]=%d\r\n",__FUNCTION__,__LINE__,signal_pipe[0],signal_pipe[1]);
	//signal(SIGUSR1, signal_handler);
	//signal(SIGUSR2, signal_handler);
	/* 这里要去掉，因为client6_init函数中也注册了这个信号的处理函数，这里再注册的话导致进程不能被杀死 added by ZQQ, 22Mar2011 */
	//signal(SIGTERM, signal_handler);
	#endif
	int res = 0;
	res = pipe(signal_pipe);
	if (res < 0)
	{
		perror("pipe error:");
		return -1;
	}
	return 1;
}


/* Quick little function to setup the rfds. Will return the
 * max_fd for use with select. Limited in that you can only pass
 * one extra fd */
int udhcp_sp_fd_set(fd_set *rfds, int extra_fd)
{
	FD_ZERO(rfds);
	//printf("%s %d signal_pipe[0] = %d\r\n",__FUNCTION__,__LINE__,signal_pipe[0]);
	FD_SET(signal_pipe[0], rfds);
	if (extra_fd >= 0)
		FD_SET(extra_fd, rfds);
	return signal_pipe[0] > extra_fd ? signal_pipe[0] : extra_fd;
}


/* Read a signal from the signal pipe. Returns 0 if there is
 * no signal, -1 on error (and sets errno appropriately), and
 * your signal on success */
int udhcp_sp_read(fd_set *rfds)
{
	int sig = 0;
	if (!FD_ISSET(signal_pipe[0], rfds))
		return 0;
	if (read(signal_pipe[0], &sig, sizeof(sig)) < 0)
		return -1;
	return sig;
}

/* if msg queue has some data, we inform dhcp using pipe */
void udhcpv6_sp_ipc_inform(int cmd)
{
	#if 0
	int nSnd = 0;
	nSnd = send(signal_pipe[1], &cmd, sizeof(cmd), MSG_DONTWAIT);
	if (nSnd < 0)
	{
		perror("udhcp_sp_ipc_inform send");
	}
	else
	{
		printf("%s %d send %d bytes to socket %d.\r\n", __FUNCTION__, __LINE__, nSnd, signal_pipe[1]);
	}
	#endif
	int nWrt = 0;
	nWrt = write(signal_pipe[1], &cmd, sizeof(cmd));
	if (nWrt < 0)
	{
		perror("udhcp_sp_ipc_inform send");
	}
	else
	{
		printf("%s %d send %d bytes to fd %d.\r\n", __FUNCTION__, __LINE__, nWrt, signal_pipe[1]);
	}
}

void getSignalPipe(int* rfd, int* wfd)
{
	*rfd = signal_pipe[0];
	*wfd = signal_pipe[1];
	return;
}

int getReadPipe()
{
	return signal_pipe[0];
}


