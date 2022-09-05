#ifndef BYTE_BUFFER_H
#define BYTE_BUFFER_H

#include <stdint.h>

typedef struct byte_buffer* byte_buffer_t;

byte_buffer_t byte_buffer_create(uint32_t max_size);

byte_buffer_t byte_buffer_init(uint8_t* buffer, uint32_t size, uint32_t max_size);

void byte_buffer_destroy(byte_buffer_t self);

int byte_buffer_append(byte_buffer_t self, uint8_t* data, uint32_t data_size);

int byte_buffer_appendbyte(byte_buffer_t self, uint8_t data);

void byte_buffer_reset(byte_buffer_t self);

uint8_t* byte_buffer(byte_buffer_t self);

uint32_t byte_buffer_maxsize(byte_buffer_t self);

uint32_t byte_buffer_size(byte_buffer_t self);


#endif