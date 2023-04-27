#ifndef ROUNDBUFFER_H
#define ROUNDBUFFER_H

// capacity = length - 1
typedef struct {
    char* buffer;
    int length;
    int first;
    int next;
} roundbuf_t;

void roundbuf_init(roundbuf_t* rbuf, char* buffer_dir, int len);
int roundbuf_is_empty(roundbuf_t* rbuf);
int roundbuf_is_full(roundbuf_t* rbuf);
int roundbuf_get_occupation(roundbuf_t* rbuf);
void roundbuf_push(roundbuf_t* rbuf, char c);
char roundbuf_pop(roundbuf_t* rbuf);
int roundbuf_copy_to(roundbuf_t* rbuf, char* dst, int len);

#endif // ROUNDBUFFER_H
