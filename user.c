#include <libc.h>

int __attribute__ ((__section__(".text.main")))
  main(void)
{
  char buff[256];
  while (1)
  {
    volatile int time_loss = 0;
    for (int i = 0; i < 60000000; ++i) {
      ++time_loss;
    }

    int l = read(buff, 5);
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
