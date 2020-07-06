#include <sys/syscall.h>

extern int openat(int fd, const char *file, int oflag);

int openat(int fd, const char *file, int oflag)
{
	return syscall(3, SYS_openat, fd, file, oflag);
}

