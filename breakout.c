#include <breakout.h>
#include <utils.h>
#include <userio.h>
#include <libc.h>

#define BOARD_ROWS 5
#define BOARD_COLS 15
#define BE_FIRST_LINE 1
#define BE_FIRST_COL 2
#define BBLOCK_SIZE 5
#define BE_LAST_COL (BE_FIRST_COL + BBLOCK_SIZE*BOARD_COLS)


char board[BOARD_ROWS][BOARD_COLS];
char bar[] = "     ";


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
        write(1,bar,BBLOCK_SIZE);
      else {
        set_bg_color(BLACK);
        write(1,bar,BBLOCK_SIZE);
        set_bg_color(color+i);
      }
    }
  }
}

void print_del_block(int i, int j) {
  move_cursor(BE_FIRST_COL+BBLOCK_SIZE*j, BE_FIRST_LINE+i);
  set_bg_color(BLACK);
  write(1,bar,BBLOCK_SIZE);
}


void print_ball(int x, int y) {
  move_cursor(x, y);
  set_bg_color(BLACK);
  set_fg_color(LIGHT_GRAY);
  write(1,"O",1);
}

void erase_ball(int x, int y) {
  move_cursor(x, y);
  set_bg_color(BLACK);
  set_fg_color(GRAY);
  write(1," ",1);
}

float ball_x;
float ball_y;
float ball_vx =0.2;
float ball_vy = 0;

void move_ball() {
  float px = ball_x;
  float py = ball_y;
  
  float nx = ball_x + ball_vx;
  float ny = ball_y + ball_vy;
  
  if(nx < BE_FIRST_COL || nx > BE_LAST_COL) {
    nx = px - ball_vx/2;
    ball_vx = -ball_vx;
  }
  
  erase_ball(px,py);
  print_ball(nx, ny);
  ball_x =  nx;
  ball_y = ny;
}

void init_breakout() {
  __asm__ __volatile__(
		"fninit");
  
  ball_x += 20.0f;
  ball_y += 20.0f;
  init_board();
  print_board();
  print_ball(ball_x, ball_y);
  
  while(1) {
    move_ball();
    volatile int time_loss = 0;
    for (int i = 0; i < 1200000; ++i)
      ++time_loss;
  }
}
