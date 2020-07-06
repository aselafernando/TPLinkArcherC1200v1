#include <bits/uClibc_mutex.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>
#define __UCLIBC_MUTEX_INIT_VAR(M)                                      ((void)0)
/* Directory stream type.  */
struct __dirstream {
  /* file descriptor */
  int dd_fd;

  /* offset of the next dir entry in buffer */
  size_t dd_nextloc;

  /* bytes of valid entries in buffer */
  size_t dd_size;

  /* -> directory buffer */
  void *dd_buf;

  /* offset of the next dir entry in directory. */
  off_t dd_nextoff;

  /* total size of buffer */
  size_t dd_max;

  /* lock */
  __UCLIBC_MUTEX(dd_lock);
};

int faccessat(int fd, const char * file, int type, int flag)
{
	return syscall(4, SYS_faccessat, fd, file, type, flag);
}

static DIR *fd_to_DIR(int fd, __blksize_t size)
{
	DIR *ptr;

	ptr = (DIR *)malloc(sizeof(*ptr));
	if (!ptr)
		return NULL;

	ptr->dd_fd = fd;
	ptr->dd_nextloc = ptr->dd_size = ptr->dd_nextoff = 0;
	ptr->dd_max = size;
	if (ptr->dd_max < 512)
		ptr->dd_max = 512;

	ptr->dd_buf = calloc(1, ptr->dd_max);
	if (!ptr->dd_buf) {
		free(ptr);
		return NULL;
	}
	__UCLIBC_MUTEX_INIT_VAR(ptr->dd_lock);

	return ptr;
}

DIR *fdopendir(int fd)
{
	int flags;
	struct stat st;

	if (fstat(fd, &st))
		return NULL;
	if (!S_ISDIR(st.st_mode)) {
		errno =ENOTDIR;
		return NULL;
	}

	flags = fcntl(fd, F_GETFL);
	if (flags == -1)
		return NULL;
	if ((flags & O_ACCMODE) == O_WRONLY) {
		errno = EINVAL;
		return NULL;
	}

	return fd_to_DIR(fd, st.st_blksize);
}

