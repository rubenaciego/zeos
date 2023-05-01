#include <libc.h>

void thread_func(void* param) {
  char* m = "\nThread\n";
  write(1, m, strlen(m));
  volatile time_loss = 0;
  for (int i = 0; i < 60000000; ++i) {
      ++time_loss;
    }
  char* s = "\nThread2\n";
  write(1, s, strlen(s));
}


int __attribute__ ((__section__(".text.main")))
  main(void)
{
  char buff[256];
  
  create_thread(thread_func, 0);
  
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

void  __attribute__ ((__section__(".thread_wrapper_sec"))) 
  thread_wrapper(void (*function)(void* arg), void* parameter ) {
  function(parameter);
  exit_thread();
}
