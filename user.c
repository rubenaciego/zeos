#include <libc.h>
#include <userio.h>
#include <breakout.h>

int __attribute__ ((__section__(".text.main")))
  main(void)
{
  clear_screen();
  init_breakout();

  return 0;
}

void  __attribute__ ((__section__(".thread_wrapper_sec"))) 
  thread_wrapper(void (*function)(void* arg), void* parameter ) {
  function(parameter);
  exit_thread();
}
