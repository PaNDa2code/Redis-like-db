#pragma once

#include <pthread.h>
#include <stdint.h>

typedef struct _rwlock {
  pthread_mutex_t w_mutex;
  pthread_mutex_t r_mutex;
  pthread_mutex_t rw_mutex;
  uint32_t readers;
} rwlock_t;

#define RWLOCK_INITIALIZER {PTHREAD_MUTEX_INITIALIZER, PTHREAD_MUTEX_INITIALIZER, PTHREAD_MUTEX_INITIALIZER, 0}

void r_lock(rwlock_t *rwlock);
void r_unlock(rwlock_t *rwlock);
void w_lock(rwlock_t *rwlock);
void w_unlock(rwlock_t *rwlock);
