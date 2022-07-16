#ifndef LOCK_H
#define LOCK_H

#ifndef _GNU_SOURCE
#   define _GNU_SOURCE
#endif

#include <pthread.h>

struct mutex
{
    pthread_mutex_t mutex;
};

struct mutex* mutex_init();

void mutex_lock(struct mutex* self);

void mutex_unlock(struct mutex* self);

void mutex_destroy(struct mutex* self);

struct condition
{
    pthread_cond_t condition;
};

struct condition* condition_init();

void condition_wait(struct condition* self, struct mutex* lock);

void condition_notify_one(struct condition* self);

void condition_notify_all(struct condition* self);

void condition_destroy(struct condition* self);

struct rwlock
{
    pthread_rwlock_t rwlock;
};

struct rwlock* rwlock_init();

void rwlock_rdlock(struct rwlock* self);

void rwlock_wrlock(struct rwlock* self);

void rwlock_unlock(struct rwlock* self);

void rwlock_destroy(struct rwlock* self);

#endif