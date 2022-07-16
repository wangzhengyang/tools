#ifndef MEMORY_H
#define MEMORY_H

void* memory_alloc(int size);

void memory_free(void* ptr);

void* memory_realloc(void* ptr, int size);

#endif