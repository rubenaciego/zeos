/*
 * io.c - 
 */

#include <io.h>
#include <utils.h>
#include <types.h>

/**************/
/** Screen  ***/
/**************/

screen_t screen;

void init_screen()
{
  screen.x = 0;
  screen.y = 19;
  screen.palette = 0x02;
}

/* Read a byte from 'port' */
Byte inb (unsigned short port)
{
  Byte v;

  __asm__ __volatile__ ("inb %w1,%0":"=a" (v):"Nd" (port));
  return v;
}

void printc(char c)
{
     __asm__ __volatile__ ( "movb %0, %%al; outb $0xe9" ::"a"(c)); /* Magic BOCHS debug: writes 'c' to port 0xe9 */
  
  // \b -> move cursor one position back
  if (c == '\b')
  {
    if (screen.x != 0) --screen.x;
    else if (screen.y != 0)
    {
      --screen.y;
      screen.x = NUM_COLUMNS - 1;
    }
  }
  // \n -> new line
  else if (c == '\n')
  {
    screen.x = 0;
    screen.y=(screen.y+1)%NUM_ROWS;
  }
  else
  {
    Word ch = (Word) (c & 0x00FF) | (screen.palette << 8);
    Word *sc = (Word *)0xb8000;
    sc[(screen.y * NUM_COLUMNS + screen.x)] = ch;
    if (++screen.x >= NUM_COLUMNS)
    {
      screen.x = 0;
      screen.y=(screen.y+1)%NUM_ROWS;
    }
  }
}

void printc_xy(Byte mx, Byte my, char c)
{
  Byte cx, cy;
  cx=screen.x;
  cy=screen.y;
  screen.x=mx;
  screen.y=my;
  printc(c);
  screen.x=cx;
  screen.y=cy;
}

void printk(char *string)
{
  int i;
  for (i = 0; string[i]; i++)
    printc(string[i]);
}
