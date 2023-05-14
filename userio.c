#include<userio.h>
#include<utils.h>

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

  if (color < NUM_COLORS)
  {
    buff[3] = color + '0';
    write(1, buff, 5);
  }
}

void set_bg_color(color_t color)
{
  static char buff[5] = {0x1b, '[', '4', 'x', 'm'};

  if (color < NUM_COLORS)
  {
    buff[3] = color + '0';
    write(1, buff, 5);
  }
}
