#include "semaphore.h"

#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>

semaphore_t semaphore_create(int initialValue)
{
    semaphore_t self = (semaphore_t)malloc(sizeof(sem_t));

    sem_init((sem_t*)self, 0, initialValue);

    return self;
}

void semaphore_wait(semaphore_t self)
{
    sem_wait((sem_t*)self);
}

void semaphore_post(semaphore_t self)
{
    sem_post((sem_t*)self);
}

void semaphore_destroy(semaphore_t self)
{
    sem_destroy((sem_t*)self);
    free(self);
}
