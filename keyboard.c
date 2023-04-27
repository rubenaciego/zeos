#include "keyboard.h"

char keyboard_buffer_array[KEYBOARD_BUF_CAP+1];
roundbuf_t keyboard_rbuf;


void init_keyboard()
{
    roundbuf_init(&keyboard_rbuf, keyboard_buffer_array, KEYBOARD_BUF_CAP+1);
}
