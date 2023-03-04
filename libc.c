/*
 * libc.c 
 */

#include <libc.h>
#include <errno.h>
#include <types.h>

int errno;

void itoa(int a, char *b)
{
  int i, i1;
  char c;
  
  if (a==0) { b[0]='0'; b[1]=0; return ;}
  if (a < 0) { b[0]='-'; a=-a; ++b; }
  
  i=0;
  while (a>0)
  {
    b[i]=(a%10)+'0';
    a=a/10;
    i++;
  }
  
  for (i1=0; i1<i/2; i1++)
  {
    c=b[i1];
    b[i1]=b[i-i1-1];
    b[i-i1-1]=c;
  }
  b[i]=0;
}

int strlen(char *a)
{
  int i;
  
  i=0;
  
  while (a[i]!=0) i++;
  
  return i;
}

void perror()
{
  char* msg;
  switch (errno)
  {
    case EBADF: msg = "Bad file descriptor\n"; break;
    case ENOMEM: msg = "Cannot allocate memory\n"; break;
    case EACCES: msg = "Permission denied\n"; break;
    case EFAULT: msg = "Bad address\n"; break;
    case EFBIG: msg = "File too large\n"; break;
    case ENOSYS: msg = "Function not implemented\n"; break;
    case EOVERFLOW: msg = "Value too large for defined data type\n"; break;
    default: msg = "Unknown error\n";
  }

  write(1, msg, strlen(msg));
}
