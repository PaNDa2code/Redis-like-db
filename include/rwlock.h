#pragma once

#include <limits.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdint.h>

#define IS_WRITING ((size_t)1)
#define WRITER ((size_t)1 << 1)
#define Count ((sizeof(size_t) * CHAR_BIT - 1) / 2)
#define READER ((size_t)(1ULL << (1 + Count)))
#define WRITER_MASK ((size_t)((~(size_t)0) << 2))
#define READER_MASK ((size_t)((~(size_t)0) << (1 + Count)))

typedef struct _rwlock {
  _Atomic size_t state;
  pthread_mutex_t mutex;
  sem_t semaphore;
} rwlock_t;

#define RWLOCK_INITIALIZER {0, PTHREAD_MUTEX_INITIALIZER, {0}}

void r_lock(rwlock_t *rwlock);
void r_unlock(rwlock_t *rwlock);
void w_lock(rwlock_t *rwlock);
void w_unlock(rwlock_t *rwlock);
