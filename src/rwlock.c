#include "rwlock.h"
#include <pthread.h>

void r_lock(rwlock_t *rwlock) {
  pthread_mutex_lock(&rwlock->w_mutex); // Wait for writers to finish
  pthread_mutex_lock(&rwlock->r_mutex);
  rwlock->readers++;
  if (rwlock->readers == 1)
    pthread_mutex_lock(&rwlock->rw_mutex); // First reader locks resource

  pthread_mutex_unlock(&rwlock->r_mutex);
  pthread_mutex_unlock(&rwlock->w_mutex);
}

void r_unlock(rwlock_t *rwlock) {
  pthread_mutex_lock(&rwlock->r_mutex);
  rwlock->readers--;
  if (rwlock->readers == 0) {
    pthread_mutex_unlock(&rwlock->rw_mutex); // Last reader unlocks resource
  }
  pthread_mutex_unlock(&rwlock->r_mutex);
}

void w_lock(rwlock_t *rwlock) {
  pthread_mutex_lock(&rwlock->w_mutex);
  pthread_mutex_lock(&rwlock->rw_mutex);
}

void w_unlock(rwlock_t *rwlock) {
  pthread_mutex_unlock(&rwlock->rw_mutex);
  pthread_mutex_unlock(&rwlock->w_mutex);
}
