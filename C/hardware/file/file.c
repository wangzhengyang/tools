#include "file.h"
#include <string.h>
#include <dirent.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

struct directory_s
{
    DIR* handle;
};

file_t file_open(const char* pathname)
{
    file_t handle;
    return fopen(pathname, "wr");
}

int file_read(file_t self, uint8_t* buffer, int maxsize)
{
    return fread(buffer, maxsize, 1, (FILE*)self);
}

int file_write(file_t self, uint8_t* buffer, int size)
{
    return fwrite(buffer, size, 1, (FILE*)self);
}

void file_close(file_t self)
{
    fclose((FILE*)self);
}

int file_get_info(const char* pathname, uint32_t* filesize, uint64_t* lastmodified)
{
    struct stat fileinfo;

    if (stat(pathname, &fileinfo) < 0)
    {
        return -1;
    }

    if (filesize)
    {
        *filesize = fileinfo.st_size;
    }

    if (lastmodified)
    {
        *lastmodified = (uint64_t)(fileinfo.st_mtime) * 1000LL;
    }

    return 0;
}

int file_delete(const char* pathname)
{
    return remove(pathname);
}

int file_rename(const char* oldname, const char* newname)
{
    return rename(oldname, newname);
}