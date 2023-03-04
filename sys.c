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

#define LECTURA 0
#define ESCRIPTURA 1

#define MAX_WRITE_SIZE 16384

char write_buff[MAX_WRITE_SIZE];

int check_fd(int fd, int permissions)
{
  if (fd!=1) return -EBADF;
  if (permissions!=ESCRIPTURA) return -EACCES;
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
  if (buff == NULL) return -EFAULT;
  if (len < 0 || len >= MAX_WRITE_SIZE) return -EFBIG;
  res = copy_from_user(buff, write_buff, len);
  if (res == -1) return -ENOMEM;
  return sys_write_console(write_buff, len);
}
