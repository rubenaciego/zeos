#include <libc.h>

char buff[24];

int pid;

int add(int a, int b)
{
  return a + b;
}

int __attribute__ ((__section__(".text.main")))
  main(void)
{
    /* Next line, tries to move value 0 to CR3 register. This register is a privileged one, and so it will raise an exception */
    /* __asm__ __volatile__ ("mov %0, %%cr3"::"r" (0) ); */

  int atime = gettime();
  char buf[50];
  itoa(atime, buf);
  int res = write(1, buf, strlen(buf));
  if (res == -1) perror();
  write(1, buf, strlen(buf));
  
  char* p = 0x23;
  *p = 'x';
  
  
  while(1) { }
}
