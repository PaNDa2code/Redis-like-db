#include "rwlock.h"
#include <pthread.h>
#include <stdatomic.h>

#include "rwlock.h"
#include <stdio.h>
#include <stdlib.h>

void r_lock(rwlock_t *rwlock) {
    size_t state = atomic_load_explicit(&rwlock->state, memory_order_acquire);

    while (1) {
        if (state & (IS_WRITING | WRITER_MASK)) {
            pthread_mutex_lock(&rwlock->mutex);
            pthread_mutex_unlock(&rwlock->mutex);
            state = atomic_load_explicit(&rwlock->state, memory_order_acquire);
            continue;
        }

        if (atomic_compare_exchange_weak_explicit(
                &rwlock->state, &state, state + READER,
                memory_order_acquire, memory_order_relaxed)) {
            break;
        }
    }
}

void r_unlock(rwlock_t *rwlock) {
    size_t state = atomic_fetch_sub_explicit(&rwlock->state, READER, memory_order_release);

    if ((state & READER_MASK) == READER && (state & IS_WRITING)) {
        sem_post(&rwlock->semaphore);
    }
}

void w_lock(rwlock_t *rwlock) {
    atomic_fetch_add_explicit(&rwlock->state, WRITER, memory_order_acquire);

    pthread_mutex_lock(&rwlock->mutex);

    size_t state = atomic_fetch_add_explicit(&rwlock->state, IS_WRITING - WRITER, memory_order_acquire);

    if (state & READER_MASK) {
        sem_wait(&rwlock->semaphore);
    }
}

void w_unlock(rwlock_t *rwlock) {
    atomic_fetch_and_explicit(&rwlock->state, ~IS_WRITING, memory_order_release);

    pthread_mutex_unlock(&rwlock->mutex);
}
