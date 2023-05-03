/*
 * io.h - Definició de l'entrada/sortida per pantalla en mode sistema
 */

#ifndef __IO_H__
#define __IO_H__

#include <types.h>

#define NUM_COLUMNS 80
#define NUM_ROWS    25

typedef struct {
    Byte x;
    Byte y;
    Byte palette;
} screen_t;

extern screen_t screen;

Byte inb(unsigned short port);

/** Screen functions **/
/**********************/

void init_screen();
void printc(char c);
void printc_xy(Byte x, Byte y, char c);
void printk(char *string);
void move_cursor(Byte x, Byte y);
void get_cursor(Byte* x, Byte* y);

#endif  /* __IO_H__ */
