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

typedef enum
{
  BLACK = 0,
  BLUE,
  GREEN,
  CYAN,
  RED,
  MAGENTA,
  BROWN,
  GRAY,
  NUM_COLORS
} color_t;

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

int __attribute__ ((__section__(".text.main")))
  main(void)
{
  char buff[256];

  move_cursor(0, 10);
  set_fg_color(BLUE);
  set_bg_color(RED);

  buff[0] = 'A';
  write(1, buff, 1);
  remove_last();

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
  }

  return 0;
}
