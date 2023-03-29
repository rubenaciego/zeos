/*
 * sys.c - Syscalls implementation
 */
#include <devices.h>

#include <utils.h>

#include <io.h>

#include <mm.h>

#include <mm_address.h>

#include <sched.h>

#include <errno.h>

#include <hardware.h>

#include <limits.h>


#define LECTURA 0
#define ESCRIPTURA 1

#define WRITE_BUFFER_SIZE 4096

char write_buff[WRITE_BUFFER_SIZE];

int check_fd(int fd, int permissions)
{
  if (fd != 1) return -EBADF;
  if (permissions != ESCRIPTURA) return -EACCES;
  return 0;
}

int sys_ni_syscall()
{
	return -ENOSYS; /*ENOSYS*/
}

int sys_getpid()
{
	return current()->PID;
}

int sys_fork()
{
  int PID=-1;

  // creates the child process
  
  return PID;
}

void sys_exit()
{  
}

int sys_write(int fd, char* buff, int len)
{
  int res = check_fd(fd, ESCRIPTURA);
  if (res < 0) return res;
  if (!access_ok(VERIFY_WRITE, buff, len)) return -EFAULT;
  if (len < 0) return -EFBIG;

  int wb = 0;
  while (len > 0)
  {
    int cpy_len = min(len, WRITE_BUFFER_SIZE);
    len -= cpy_len;
    res = copy_from_user(buff, write_buff, cpy_len);
    if (res == -1) return -ENOMEM;
    wb += sys_write_console(write_buff, cpy_len);
  }

  return wb;
}

int sys_gettime()
{
  if (zeos_ticks > INT_MAX) return -EOVERFLOW;
  return (int)zeos_ticks;
}
