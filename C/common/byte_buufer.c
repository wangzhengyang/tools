#include "byte_buffer.h"
#include <string.h>
#include <stdlib.h>

struct byte_buffer
{
    uint8_t* buffer;
    uint32_t size;
    uint32_t max_size;
};


byte_buffer_t byte_buffer_create(uint32_t max_size)
{
    byte_buffer_t self = (byte_buffer_t)malloc(sizeof(byte_buffer));

    if (self)
    {
        self->max_size = self->size = 0;
        self->buffer = (uint8_t*)malloc(sizeof(max_size));
        if (self->buffer)
        {
            self->max_size = max_size;
        }
    }

    return self;
}

byte_buffer_t byte_buffer_init(uint8_t* buffer, int size, int max_size)
{
    byte_buffer_t self = (byte_buffer_t)malloc(sizeof(byte_buffer));

    if (self)
    {
        self->buffer = buffer;
        self->size = size;
        self->max_size = max_size;
    }

    return self;
}

void byte_buffer_destroy(byte_buffer_t self)
{
    free(self->buffer);
    free(self);
}

int byte_buffer_append(byte_buffer_t self, uint8_t* data, uint32_t data_size)
{
    if (self->size + data_size <= self->max_size)
    {
        memcpy(self->buffer + self->size, data, data_size);
        self->size += data_size;
        return data_size;
    }

    return -1;
}

int byte_buffer_appendbyte(byte_buffer_t self, uint8_t data)
{
    if (self->size < self->max_size)
    {
        self->buffer[self->size] = data;
        self->size++;
        return 1;
    }

    return -1;
}

void byte_buffer_reset(byte_buffer_t self)
{
    self->size = 0;
}

uint8_t* byte_buffer(byte_buffer_t self)
{
    return self->buffer;
}

