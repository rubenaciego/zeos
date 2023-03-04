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

  int res = write(1, "Hello from user!\n", 18);
  if (res == -1) perror();

  itoa(-1, buff);
  write(1, buff, 10);

  while(1) { }
}
