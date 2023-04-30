#include "roundbuffer.h"

void roundbuf_init(roundbuf_t* rbuf, char* buffer_dir, int len)
{
    rbuf->buffer = buffer_dir;
    rbuf->length = len;
    rbuf->first = 0;
    rbuf->next = 0;
}

int roundbuf_is_empty(roundbuf_t* rbuf)
{
    return rbuf->first == rbuf->next;
}

int roundbuf_is_full(roundbuf_t* rbuf)
{
    return rbuf->first == (rbuf->next + 1) % rbuf->length;
}

int roundbuf_get_occupation(roundbuf_t* rbuf)
{
    int occupation = (rbuf->next - rbuf->first + rbuf->length) % rbuf->length;
    return occupation;
}

void roundbuf_push(roundbuf_t* rbuf, char c)
{
    rbuf->buffer[rbuf->next] = c;
    rbuf->next = (rbuf->next + 1) % rbuf->length;

    if (rbuf->next == rbuf->first)
        rbuf->first = (rbuf->first + 1) % rbuf->length;
}

char roundbuf_pop(roundbuf_t* rbuf)
{
    if (roundbuf_is_empty(rbuf)) return '\0';

    char c = rbuf->buffer[rbuf->first];
    rbuf->first = (rbuf->first + 1) % rbuf->length;
    return c;
}

int roundbuf_copy_to(roundbuf_t* rbuf, char* dst, int len)
{
    int occ = roundbuf_get_occupation(rbuf);
    if (occ < len) len = occ;

    for (int i = 0; i < len; ++i) {
        dst[i] = rbuf->buffer[(rbuf->first + i)%rbuf->length];
    }

    rbuf->first = (rbuf->first + len) % rbuf->length;
    return len;
}
