#include <libc.h>

int __attribute__ ((__section__(".text.main")))
  main(void)
{
  char buff[256];
  int pid = fork();

  if (pid == 0)
  {
    while (1)
    {
      char* str = "Second process here!\n";
      write(1, str, strlen(str));

     volatile int time_loss = 0;
     for (int i = 0; i < 60000000; ++i)
        ++time_loss;
    }
  }

  while (1)
  {
    int l = read(buff, 1);
    char nl = '\n';

    for (int i = 0; i < l; ++i)
    {
      char* m = "Pressed ";
      write(1, m, strlen(m));
      write(1, &buff[i], 1);
      write(1, &nl, 1); 
    }

    write(1, &nl, 1);
  }

  return 0;
}
