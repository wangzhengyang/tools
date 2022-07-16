#ifndef THREAD_H
#define THREAD_H

#include <stdbool.h>

typedef struct thread_s* thread_t;
typedef void* (*thread_func_t)(void*);

thread_t thread_create(thread_func_t function, void* param, bool autodestroy);

void thread_start(thread_t thread);

void thread_destroy(thread_t thread);

void thread_sleep(int milliesecond);

void set_thread_name(const char* name);

char* thread_name();

int set_thread_affinity(int cpu_id);

#endif