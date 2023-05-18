#include <breakout.h>
#include <utils.h>
#include <userio.h>
#include <libc.h>

#define BOARD_ROWS 5
#define BOARD_COLS 15
#define BE_FIRST_LINE 1
#define BE_FIRST_COL 2
#define BBLOCK_SIZE 5
#define BBAR_SIZE 7
#define BE_LAST_COL (BE_FIRST_COL + BBLOCK_SIZE*BOARD_COLS)

#define BAR_LINE 22

char board[BOARD_ROWS][BOARD_COLS];
char bar[] = "            ";


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
  if (x < 0 || y < 0) return;
  move_cursor(x, y);
  set_bg_color(BLACK);
  set_fg_color(LIGHT_GRAY);
  write(1,"O",1);
}

volatile float ball_x;
volatile float ball_y;
float ball_vx =0.0005;
float ball_vy = -0.00005;


float bbar_x = 6;

int pos_brick(float x, float y, int* bi, int* bj) {
  float x_off = x - BE_FIRST_COL;
  float y_off = y - BE_FIRST_LINE;
  
  if (y_off > BOARD_ROWS) return 0;
  
  *bi = y_off;
  *bj = (x_off/BBLOCK_SIZE);
  return board[*bi][*bj];
}




int move_ball() {
  float px = ball_x;
  float py = ball_y;
  
  float nx = ball_x + ball_vx;
  float ny = ball_y + ball_vy;
  
  //wall reflection
  if (nx < BE_FIRST_COL) {
    nx = 2*BE_FIRST_COL - nx;
    px = BE_FIRST_COL;
    ball_vx = -ball_vx;
  } else if (nx > BE_LAST_COL) {
    nx = 2*BE_LAST_COL - nx;
    px = BE_LAST_COL;
    ball_vx = -ball_vx;
  }
  
  // top reflection
  if (ny < BE_FIRST_LINE) {
    ny = 2*BE_FIRST_COL - ny;
    py = BE_LAST_COL;
    ball_vy = -ball_vy;
  }
  
  int bi, bj;
  if (pos_brick(nx, ny, &bi, &bj)) {
    board[bi][bj] = 0;
    //TODO
    ball_vx = -ball_vx;
    ball_vy = -ball_vy;
  }
  
  
  if (ny > BAR_LINE) {
    if (bbar_x <= nx && nx <= bbar_x + BBAR_SIZE) {
      ny = 2*BAR_LINE - ny;
      ball_vy = -ball_vy;
    } else {
      return 0;
    }
  }
  
  ball_x = nx;
  ball_y = ny;
  
  return 1;
}

void print_bar() {
  move_cursor(bbar_x, BAR_LINE);
  set_bg_color(WHITE);
  write(1,bar,BBAR_SIZE);
  set_bg_color(BLACK);
}

void draw() {
  clear_screen();
  print_board();
  print_ball(ball_x, ball_y);
  print_bar();
}

int playing = 1;

float barv = 0;

void move_bar() {
  float nbarx = bbar_x + barv;
  if (nbarx < BE_FIRST_COL || BE_LAST_COL <= nbarx + BBAR_SIZE) {
    barv = -0.5*barv;
    return;
  }
  bbar_x = nbarx;
}

void draw_thread(void* arg) {
  while(playing) {
    draw();
    yield();
  }
}

void sleep(int l) {
  volatile int time_loss = 0;
  for (int i = 0; i < l; ++i)
    ++time_loss;
}

void physics_thread(void* arg) {
  while(playing) {
    
    if (!move_ball()) {
      ball_x = -1;
      
      sleep(12000000);
      ball_x = 40;
      ball_y = 10;      
    }
    move_bar();
    sleep(120);
  }
}

#define BAR_VEL 0.0005

void control_thread(void* arg) {

  char c;
  
  while(1) {
    read(&c,1);
    if (c == 'a') {
       barv -= BAR_VEL;
    } else if (c == 'd') {
      barv += BAR_VEL;
    } else {
      barv = 0;
    }
  }
}

void init_breakout() {
  __asm__ __volatile__(
		"fninit");
  
  ball_x += 20.0f;
  ball_y += 20.0f;
  init_board();
  print_board();
  mutex_init(&bbar_x);
  create_thread(draw_thread, 0);
  create_thread(physics_thread, 0);
  create_thread(control_thread, 0);
  while(1);
  //print_ball(ball_x, ball_y);page

    
}
