#include "fifo.h"
#include <stdlib.h>
#include <string.h>

static int is_power_of_2(unsigned int n)
{
    return (n != 0 && ((n & (n - 1)) == 0));
}

static unsigned int roundup_pow_of_two(unsigned int n)
{
    if (is_power_of_2(n))
    {
        return n;
    }

    unsigned int max = 0x80000000;
    while ((max & n) == 0)
    {
        max = max >> 1;
    }

    return max << 1;
}

struct fifo* fifo_init(unsigned int size)
{
    struct fifo* q = (struct fifo*)malloc(sizeof(struct fifo));

    if (q)
    {
        q->size = roundup_pow_of_two(size);
        q->buffer = (unsigned char*)malloc(sizeof(q->size));
        q->in = q->out = 0;
    }

    return q;
}

void fifo_free(struct fifo* q)
{
    if (q)
    {
        if (q->buffer)
        {
            free(q->buffer);
        }
        free(q);
    }
}

static inline unsigned int fifo_off(struct fifo* q, unsigned int off)
{
    return off & (q->size - 1);
}

static inline void fifo_in_data(struct fifo* q, const void* from, unsigned int len, unsigned int offset)
{
    unsigned int l;
    offset = fifo_off(q, q->in + offset);

    l = len < (q->size - offset) ? len : q->size - offset;
    memcpy(q->buffer + offset, from, l);
    memcpy(q->buffer, from + l, len - l);
}

void fifo_add_in(struct fifo* q, unsigned int off)
{
    q->in += off;
}

int fifo_in(struct fifo* q, const void* from, unsigned int len)
{
    if (len > fifo_avail(q))
    {
        return -1;
    }

    fifo_in_data(q, from, len, 0);
    fifo_add_in(q, len);
    return 0;
}

void fifo_add_out(struct fifo* q, unsigned int off)
{
    q->out += off;
}

static inline void fifo_out_data(struct fifo* q, void* to, unsigned int len, unsigned int offset)
{
    unsigned int l;
    offset = fifo_off(q, q->out + offset);

    l = len < (q->size - offset) ? len : q->size - offset;
    memcpy(to, q->buffer + offset, l);
    memcpy(to + l, q->buffer, len - l);
}

int fifo_out(struct fifo* q, void* to, unsigned int len)
{
    if (len > fifo_len(q))
    {
        return -1;
    }

    fifo_out_data(q, to, len, 0);
    fifo_add_out(q, len);
    return 0;
}

int fifo_out_peek(struct fifo* q, void* to, unsigned int len, unsigned int offset)
{
    if (len > fifo_len(q))
    {
        return -1;
    }

    fifo_out_data(q, to, len, offset);
}

int fifo_initialized(const struct fifo* q)
{
    return q->buffer != NULL;
}

void fifo_reset(struct fifo* q)
{
    q->in = q->out = 0;
}

void fifo_reset_out(struct fifo* q)
{
    q->out = q->in;
}

unsigned int fifo_size(const struct fifo* q)
{
    return q->size;
}

unsigned int fifo_len(const struct fifo* q)
{
    return q->in - q->out;
}

int fifo_is_empty(const struct fifo* q)
{
    return q->in == q->out;
}

unsigned int fifo_avail(const struct fifo* q)
{
    return fifo_size(q) - fifo_len(q);
}

