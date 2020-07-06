#include <sys/syscall.h>

int unshare(unsigned long unshare_flags)
{
	return syscall(1, SYS_unshare, unshare_flags);
}

