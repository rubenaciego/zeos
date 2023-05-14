#include <breakout.h>
#include <utils.h>
#include <userio.h>
#include <libc.h>

#define BOARD_ROWS 5
#define BOARD_COLS 15
#define BE_FIRST_LINE 1
#define BE_FIRST_COL 2

char board[BOARD_ROWS][BOARD_COLS];
char bar[] = "    ";


void init_board() {
  for (int i = 0; i < BOARD_ROWS; ++i)
    for (int j = 0; j < BOARD_COLS; ++j)
      board[i][j] = 1;
}


void print_board() {
  color_t color = BLUE;
  for (int i = 0; i < BOARD_ROWS; ++i) {
    move_cursor(BE_FIRST_COL, BE_FIRST_LINE+i);
    set_bg_color(color+i);
    for (int j = 0; j < BOARD_COLS; ++j) {
      if (board[i][j])
        write(1,bar,sizeof(bar));
      else {
        set_bg_color(BLACK);
        write(1,bar,sizeof(bar));
        set_bg_color(color+i);
      }
    }
  }
}

void print_del_block(int i, int j) {
  move_cursor(BE_FIRST_COL+sizeof(bar)*j, BE_FIRST_LINE+i);
  set_bg_color(BLACK);
  write(1,bar,sizeof(bar));
}


void print_ball(int i, int j) {
  move_cursor(BE_FIRST_COL+j, BE_FIRST_LINE+i);
  set_bg_color(BLACK);
  set_fg_color(LIGHT_GRAY);
  write(1,"O",1);
}

int ball_x = 20;
int ball_y = 20;


void init_breakout() {
    init_board();
    print_board();
    print_ball(ball_x, ball_y);
    
    while(1);
}
