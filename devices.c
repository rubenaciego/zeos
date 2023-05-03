#include <io.h>
#include <utils.h>
#include <list.h>

// Queue for blocked processes in I/O 
struct list_head blocked;

int sys_write_console(char *buffer, int size)
{
  char esc = 0x1b;
  
  for (int i = 0; i < size; ++i)
  {
    // check terminal codes
    if (buffer[i] == esc)
    {

    }
    else
      printc(buffer[i]);
  }
  
  return size;
}
