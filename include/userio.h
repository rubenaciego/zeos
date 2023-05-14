#ifndef USERIO_H
#define USERIO_H

typedef enum
{
  BLACK = 0,
  BLUE,
  GREEN,
  CYAN,
  RED,
  MAGENTA,
  BROWN,
  LIGHT_GRAY,
  NUM_DIM_COLORS,
  DARK_GRAY = 16,
  LIGHT_BLUE,
  LIGHT_GREEN,
  LIGHT_CYAN,
  LIGHT_RED,
  LIGHT_MAGENTA,
  YELLOW,
  WHITE,
  MAX_B_COLORS
} color_t;

#define SCREEN_COLUMNS 80
#define SCREEN_ROWS    25

void remove_last();

void move_cursor(unsigned char x, unsigned char y);

void set_fg_color(color_t color);

void set_bg_color(color_t color);

void clear_screen();

#endif
