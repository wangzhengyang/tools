#include <stdlib.h>

void* memory_alloc(int size)
{
    return malloc(size);
}

void memory_free(void* ptr)
{
    free(ptr);
}

void* memory_realloc(void* ptr, int size)
{
    return realloc(ptr, size);
}
