#ifndef FILE_H
#define FILE_H

#include <stdbool.h>
#include <stdint.h>

typedef void* file_t;
typedef struct directory_s* directory_t;

file_t file_open(const char* pathname);

int file_read(file_t self, uint8_t* buffer, int maxsize);

int file_write(file_t self, uint8_t* buffer, int size);

void file_close(file_t self);

int file_get_info(const char* pathname, uint32_t* filesize, uint64_t* lastmodified);

int file_delete(const char* pathname);

int file_rename(const char* oldname, const char* newname);

#endif