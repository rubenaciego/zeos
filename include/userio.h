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
  GRAY,
  NUM_DIM_COLORS,
  B_BLACK = 16,
  B_RED,
  B_GREEN,
  B_CYAN,
  B_BLUE,
  B_MAGENTA,
  YELLOW,
  WHITE,
  MAX_B_COLORS
  
} color_t;



void remove_last();

void move_cursor(unsigned char x, unsigned char y);

void set_fg_color(color_t color);

void set_bg_color(color_t color);

#endif
