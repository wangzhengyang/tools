#include "thread.h"

#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

struct thread_s
{
    thread_func_t function;
    void* param;
    pthread_t pthread;
    int state;
    bool autodestroy;
};

thread_t thread_create(thread_func_t function, void* param, bool autodestroy)
{
    thread_t thread = malloc(sizeof(struct thread_s));

    thread->function = function;
    thread->param = param;
    thread->state = 0;
    thread->autodestroy = autodestroy;

    return thread;
}

static void* thread_automatic_destroy(void* param)
{
    thread_t thread = (thread_t*)param;

    thread->function(thread->param);

    free(thread);

    pthread_exit(NULL);
}

void thread_start(thread_t thread)
{
    if (thread->autodestroy)
    {
        pthread_create(&thread->pthread, NULL, thread_automatic_destroy, thread);
        pthread_detach(thread->pthread);
    }
    else
    {
        pthread_create(&thread->pthread, NULL, thread->function, thread->param);
    }

    thread->state = 1;
}

void thread_destroy(thread_t thread)
{
    if (thread->state == 1)
    {
        pthread_join(thread->pthread, NULL);
    }

    free(thread);
}

void thread_sleep(int milliesecond)
{
    usleep(milliesecond * 1000);
}

void set_thread_name(const char* name)
{
    assert(strlen(name) < 16);
    pthread_setname_np(pthread_self(), name);
}

char* thread_name()
{
    char name[16] = { 0 };
    pthread_t id = pthread_self();
    int ret = pthread_getname_np(id, name, 16);
    char* str = (char*)malloc(16);
    if (ret != 0)
    {
        snprintf(str, 16, "%d", (unsigned long long)id);
    }
    else
    {
        snprintf(str, 16, "%s", name);
    }

    return str;
}

int set_thread_affinity(int cpu_id)
{
    cpu_set_t mask;
    CPU_ZERO(&mask);

    if (cpu_id >= 0)
    {
        CPU_SET(cpu_id, &mask);
    }

    int ret = pthread_setaffinity_np(pthread_self(), sizeof(mask), &mask);

    if (ret != 0)
    {
        return -1;
    }

    return 0;
}
