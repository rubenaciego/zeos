#include "keyboard.h"
#include "list.h"
#include "sched.h"

char keyboard_buffer_array[KEYBOARD_BUF_CAP+1];
roundbuf_t keyboard_rbuf;

extern struct list_head input_blocked;


void init_keyboard()
{
    roundbuf_init(&keyboard_rbuf, keyboard_buffer_array, KEYBOARD_BUF_CAP+1);
}

void keyboard_update(char c)
{
    roundbuf_push(&keyboard_rbuf, c);

    if (!list_empty(&input_blocked))
    {
        struct list_head* first = list_first(&input_blocked);
        struct task_struct* t = list_head_to_task_struct(first);
        int l = roundbuf_get_occupation(&keyboard_rbuf);
        if (l >= t->blocking_length)
            update_process_state_rr(t, &readyqueue);
    }
}
