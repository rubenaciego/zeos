/*
 * io.h - Definició de l'entrada/sortida per pantalla en mode sistema
 */

#ifndef __IO_H__
#define __IO_H__

#include <types.h>

/** Screen functions **/
/**********************/

Byte inb (unsigned short port);
void printc(char c);
void print_color(char c, Byte color);
void printc_xy(Byte x, Byte y, char c);
void printk(char *string);
void printk_color(char* string, Byte color);

#endif  /* __IO_H__ */
