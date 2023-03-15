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

  char buff[250];
  
  int atime = gettime();
  itoa(atime, buff);
  int res = write(1, buff, strlen(buff));
  if (res == -1) perror();
  
  res = write(1, "\n", 1);
  if (res == -1) perror();
  
  volatile int time_loss = 0;
  for (int i = 0; i < 20000000; ++i) {
  	++time_loss;
  	
  }
  
  itoa(gettime(), buff);
  res = write(1, buff, strlen(buff));
  if (res == -1) perror();
  
  char* p = (char *) 0x23;
  //*p = 'x';

  char* msg = "\nHello from user\n";
  write(1, msg, strlen(msg));
  
  while(1) { }
}
