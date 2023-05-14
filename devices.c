#include <io.h>
#include <utils.h>
#include <list.h>

// Queue for blocked processes in I/O 
struct list_head input_blocked;

int sys_write_console(char *buffer, int size)
{
  char esc = 0x1b;
  
  for (int i = 0; i < size; ++i)
  {
    Byte terminal_print = 0;
    
    // check terminal codes
    if (buffer[i] == esc && size - i > 1 && buffer[i+1] == '[')
    {
      // foreground color        [ 3 color m
      // foreground bright color [ 9 color m
      // background color        [ 4 color m
      // bright background color [ 1 color m
      if (size - i >= 5 && buffer[i+4] == 'm')
      {
        char fg_bg = buffer[i+2];
        char color = buffer[i+3];

        if (color >= '0' && color <= '7')
        {
          Byte col = color - '0';
          if (fg_bg == '3')
          {
            screen.palette = (screen.palette & 0xf0) | col;
            i += 4;
            terminal_print = 1;
          }
          else if (fg_bg == '9')
          {
            screen.palette = (screen.palette & 0xf0) | (col | 8);
            i += 4;
            terminal_print = 1;
          }
          else if (fg_bg == '4')
          {
            screen.palette = (screen.palette & 0x0f) | (col << 4);
            i += 4;
            terminal_print = 1;
          }
          else if (fg_bg == '1')
          {
            screen.palette = (screen.palette & 0x0f) | ((col | 8) << 4);
            i += 4;
            terminal_print = 1;
          }
        }
      }
      // move_cursor [ xxx ; yyy H
      else if (size - i >= 10 && buffer[i+5] == ';' && buffer[i+9] == 'H')
      {
        int x = 0;
        int y = 0;

        for (int j = 0; j < 3; ++j)
        {
          char cx = buffer[i+j+2];
          char cy = buffer[i+j+6];
          if (cx < '0' || cx > '9' || cy < '0' || cy  > '9')
          {
            x = y = -1;
            break;
          }

          x = 10 * x + cx - '0';
          y = 10 * y + cy - '0';
        }

        if (x >= 0 && x < NUM_COLUMNS && y >= 0 && y < NUM_ROWS)
        {
          screen.x = (Byte)x;
          screen.y = (Byte)y;
          i += 9;
          terminal_print = 1;
        }
      }
    }
    
    if (!terminal_print)
      printc(buffer[i]);
  }
  
  return size;
}
