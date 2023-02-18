/*
 * io.c - 
 */

#include <io.h>
#include <utils.h>
#include <types.h>

/**************/
/** Screen  ***/
/**************/

#define NUM_COLUMNS 80
#define NUM_ROWS    25

Byte x, y=19;

/* Read a byte from 'port' */
Byte inb (unsigned short port)
{
  Byte v;

  __asm__ __volatile__ ("inb %w1,%0":"=a" (v):"Nd" (port));
  return v;
}

void newline()
{
  x = 0;
  if (y == NUM_ROWS - 1)
  {
    Word *screen = (Word *)0xb8000;
    copy_data(screen + NUM_COLUMNS, screen, 2 * NUM_COLUMNS * (NUM_ROWS - 1));
    
    for (int i = 0; i < NUM_COLUMNS; ++i)
      screen[(y * NUM_COLUMNS + i)] = 0;
  }
  else
  {
    ++y;
  }
}

void printc(char c)
{
  print_color(c, 0x02);
}

void print_color(char c, Byte color)
{
     __asm__ __volatile__ ( "movb %0, %%al; outb $0xe9" ::"a"(c)); /* Magic BOCHS debug: writes 'c' to port 0xe9 */
  if (c=='\n')
  {
    newline();
  }
  else
  {
    Word ch = (Word) (c & 0x00FF) | ((Word)color << 8);
    Word *screen = (Word *)0xb8000;
    screen[(y * NUM_COLUMNS + x)] = ch;
    if (++x >= NUM_COLUMNS)
    {
      newline();
    }
  }
}

void printc_xy(Byte mx, Byte my, char c)
{
  Byte cx, cy;
  cx=x;
  cy=y;
  x=mx;
  y=my;
  printc(c);
  x=cx;
  y=cy;
}

void printk(char *string)
{
  int i;
  for (i = 0; string[i]; i++)
    printc(string[i]);
}

void printk_color(char *string, Byte color)
{
  int i;
  for (i = 0; string[i]; i++)
    print_color(string[i], color);
}
