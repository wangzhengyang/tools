#ifndef FIFO_H
#define FIFO_H

struct fifo
{
    unsigned char* buffer;
    unsigned int size;
    unsigned int in;
    unsigned int out;
};

struct fifo* fifo_init(unsigned int size);

void fifo_free(struct fifo* q);

int fifo_in(struct fifo* q, const void* from, unsigned int len);

void fifo_add_in(struct fifo* q, unsigned int off);

int fifo_out(struct fifo* q, void* to, unsigned int len);

void fifo_add_out(struct fifo* q, unsigned int off);

int fifo_out_peek(struct fifo* q, void* to, unsigned int len, unsigned int offset);

int fifo_initialized(const struct fifo* q);

void fifo_reset(struct fifo* q);

void fifo_reset_out(struct fifo* q);

unsigned int fifo_size(const struct fifo* q);

unsigned int fifo_len(const struct fifo* q);

int fifo_is_empty(const struct fifo* q);

unsigned int fifo_avail(const struct fifo* q);

#endif