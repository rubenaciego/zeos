#include <userio.h>
#include <utils.h>
#include <libc.h>

void remove_last()
{
  unsigned short msg = '\b';
  write(1, (char*)&msg, 2);
}

void move_cursor(unsigned char x, unsigned char y)
{
  static char buff[10] = {0x1b, '[', 'x', 'x', 'x', ';', 'y', 'y', 'y', 'H'};

  for (int i = 0; i < 3; ++i)
  {
    buff[4 - i] = (x % 10) + '0';
    buff[8 - i] = (y % 10) + '0';
    x /= 10;
    y /= 10;
  }

  write(1, buff, 10);
}
void set_fg_color(color_t color)
{
  static char buff[5] = {0x1b, '[', '3', 'x', 'm'};
  static char buff2[5] = {0x1b, '[', '9', 'x', 'm'};

  if (color < NUM_DIM_COLORS)
  {
    buff[3] = color + '0';
    write(1, buff, 5);
  } else if (NUM_DIM_COLORS < color && color < MAX_B_COLORS) {
    buff2[3] = (color&0xf) + '0';
    write(1, buff2, 5);
  }
}

void set_bg_color(color_t color)
{
  static char buff[5] = {0x1b, '[', '4', 'x', 'm'};
  static char buff2[5] = {0x1b, '[', '1', 'x', 'm'};

  if (color < NUM_DIM_COLORS)
  {
    buff[3] = color + '0';
    write(1, buff, 5);
  } else if (NUM_DIM_COLORS < color && color < MAX_B_COLORS) {
    buff2[3] = (color&0xf) + '0';
    write(1, buff2, 5);
  }
}

void clear_screen()
{
  unsigned long long zero = 0;
  
  for (int i = 0; i < SCREEN_ROWS * SCREEN_COLUMNS / sizeof(zero); ++i)
    write(1, (char*)&zero, sizeof(zero));

  move_cursor(0, 0);
}
