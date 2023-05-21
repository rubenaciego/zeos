#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

#include "roundbuffer.h"

#define KEYBOARD_BUF_CAP 255

extern char keyboard_buffer_array[KEYBOARD_BUF_CAP+1];
extern roundbuf_t keyboard_rbuf;

void init_keyboard();
void keyboard_update(char c);

#endif /* __KEYBOARD_H__ */
