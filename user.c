#include <libc.h>
#include <stats.h>

char buff[24];

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
  
  //char* p = (char *) 0x23;
  //*p = 'x';

  char* msg = "\nHello from user\n";
  write(1, msg, strlen(msg));

  int pid = getpid();
  itoa(pid, buff);
  char* msg2 = "getpid() = ";
  write(1, msg2, strlen(msg2));
  write(1, buff, strlen(buff));
  write(1, "\n", 1);

  int pidfork = fork();

  if (pidfork == -1)
  {
    char* error = "Fork error (-1)\n";
    write(1, error, strlen(error));
  }
  else
  {
    itoa(pidfork, buff);
    char* forkmsg = "fork returned ";
    write(1, forkmsg, strlen(forkmsg));
    write(1, buff, strlen(buff));
    write(1, "\n", 1);

    if (pidfork == 0)
    {
      
      int t0 = gettime();

      while (gettime() - t0 < 10000);
      exit();
    }
  }
  
  while(1) { 
    volatile int j = 0;
    for (int i = 0; i < 10000000; ++i) {
      j+=1;
    }
    struct stats stat_data;
    int res = get_stats(getpid(), &stat_data);
    if (res != 0) {
      char* msg = "Error getting stats \n";
      write(1, msg, strlen(msg));
    }
    struct stats stat_data2;
    res = get_stats(pidfork, &stat_data2);
    if (res != 0) {
      char* msg = "Error getting stats \n";
      write(1, msg, strlen(msg));
    }
  }
}
