#include "lock.h"
#include <stdlib.h>

struct mutex* mutex_init()
{
    struct mutex* self = (struct mutex*)malloc(sizeof(struct mutex));

    if (self)
    {
        pthread_mutex_init(&self->mutex, NULL);
    }

    return self;
}

void mutex_lock(struct mutex* self)
{
    if (self)
    {
        pthread_mutex_lock(&self->mutex);
    }
}

void mutex_unlock(struct mutex* self)
{
    if (self)
    {
        pthread_mutex_unlock(&self->mutex);
    }
}

void mutex_destroy(struct mutex* self)
{
    if (self)
    {
        pthread_mutex_destroy(&self->mutex);
        free(self);
    }
}

struct condition* condition_init()
{
    struct condition* self = (struct condition*)malloc(sizeof(struct condition));
    if (self)
    {
        pthread_cond_init(&self->condition, NULL);
    }

    return self;
}

void condition_wait(struct condition* self, struct mutex* lock)
{
    if (self && lock)
    {
        pthread_cond_wait(&self->condition, &lock->mutex);
    }
}

void condition_notify_one(struct condition* self)
{
    if (self)
    {
        pthread_cond_signal(&self->condition);
    }
}

void condition_notify_all(struct condition* self)
{
    if (self)
    {
        pthread_cond_broadcast(&self->condition);
    }
}

void condition_destroy(struct condition* self)
{
    if (self)
    {
        pthread_cond_destroy(&self->condition);
        free(self);
    }
}

struct rwlock* rwlock_init()
{
    struct rwlock* self = (struct rwlock*)malloc(sizeof(struct rwlock));

    if (self)
    {
        pthread_rwlock_init(&self->rwlock, NULL);
    }

    return self;
}

void rwlock_rdlock(struct rwlock* self)
{
    if (self)
    {
        pthread_rwlock_rdlock(&self->rwlock);
    }
}

void rwlock_wrlock(struct rwlock* self)
{
    if (self)
    {
        pthread_rwlock_wrlock(&self->rwlock);
    }
}

void rwlock_unlock(struct rwlock* self)
{
    if (self)
    {
        pthread_rwlock_unlock(&self->rwlock);
    }
}

void rwlock_destroy(struct rwlock* self)
{
    if (self)
    {
        pthread_rwlock_destroy(&self->rwlock);
        free(self);
    }
}