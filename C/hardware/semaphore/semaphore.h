#ifndef SEMAPHONE_H
#define SEMAPHONE_H

typedef void* semaphore_t;

semaphore_t semaphore_create(int initialValue);

void semaphore_wait(semaphore_t self);

void semaphore_post(semaphore_t self);

void semaphore_destroysSemaphore_t self);


#endif