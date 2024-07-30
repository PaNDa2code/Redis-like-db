#include "kv_database.h"
#include "commands_functions.h"
#include "data_structures.h"
#include "dynamic_array.h"
#include "expiry_cleaner.h"
#include "hashmap.h"
#include "timespec_util.h"
#include <bits/time.h>
#include <stdbool.h>
#include <time.h>

hashmap_t *kv_hashmap;

pthread_mutex_t read_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t write_mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t reading_lock_cond = PTHREAD_COND_INITIALIZER;

bool writing = false;

extern dynamic_array(string_container_t *) * timed_data_queue;

void free_container(void *contianer);

int init_kv_hashmap() {
  printf("initializing kv hashmap\n");
  kv_hashmap = new_hashmap();
  kv_hashmap->free_value = free_container;
  kv_hashmap->free_key = free;
  return 0;
}

void free_container(void *contianer) {
  if (((string_container_t *)contianer)->expiry_time.tv_sec) {
    cancel_container_timer(contianer);
  }
  free(((string_container_t *)contianer)->string);
  free(contianer);
}

int cleanup_kv_hashmap() {
  pthread_mutex_lock(&read_mutex);
  free_hashmap(kv_hashmap);
  pthread_mutex_unlock(&read_mutex);
  return RE_SUCCESS;
}

int insert_kv(char *key, char *value, uint64_t expiry_ms) {

  int return_value;

  if (key == NULL || value == NULL) {
    return_value = RE_INVALID_ARGS;
    goto EXIT;
  }

  pthread_mutex_lock(&write_mutex);

  writing = true;

  string_container_t *value_container = malloc(sizeof(string_container_t));

  if (!value_container) {
    // it can be memory or fragmantion issue
    return_value = RE_OUT_OF_MEMORY;
    goto EXIT;
  }

  char *keyd = strdup(key);

  int re = hashmap_set(kv_hashmap, keyd, value_container);

  if (re != RE_SUCCESS) {
    free(keyd);
    free(value_container);
    return_value = re;
    goto EXIT;
  }

  size_t value_len = strlen(value);
  value_container->string = malloc(sizeof(string_t) + value_len);
  value_container->key = keyd;
  value_container->string->length = value_len;
  value_container->expired = false;
  memcpy(value_container->string->buffer, value, value_len);
  value_container->string->buffer[value_len] = 0;

  clock_gettime(CLOCK_REALTIME, &value_container->insertion_time);

  if (expiry_ms) {
    value_container->expiry_time = value_container->insertion_time;
    add_mc_timestamp(&value_container->expiry_time, expiry_ms);
    expiry_data_push(value_container);
  } else {
    memset(&value_container->expiry_time, 0, sizeof(struct timespec));
  }

  if ((float)kv_hashmap->occupied_buckets / kv_hashmap->capacity > 0.75 ||
      kv_hashmap->max_collitions >= 10) {
    hashmap_resize(&kv_hashmap, kv_hashmap->capacity * 2);
  }

  return_value = RE_SUCCESS;

EXIT:
  // what this lines do is like:
  // hey other threads, I have done writing data, you can do whatever you want
  writing = false;
  pthread_cond_broadcast(&reading_lock_cond);
  pthread_mutex_unlock(&write_mutex);
  return return_value;
}

int lookup_kv(char *key, string_ptr_t *value) {

  // checking it there is a writing thread running and wait for it;
  /*pthread_mutex_lock(&read_mutex);*/
  if (writing)
    pthread_cond_wait(&reading_lock_cond, &read_mutex);
  /*pthread_mutex_unlock(&read_mutex);*/

  if (key == NULL || value == NULL)
    return RE_INVALID_ARGS;

  string_container_t *contianer = NULL;

  if (hashmap_get(kv_hashmap, key, (void **)&contianer) != RE_SUCCESS) {
    return RE_KEY_NOT_FOUND;
  };

  *value = contianer->string;

  return RE_SUCCESS;
}

int lookup_kv_container(char *key, string_container_t **container) {

  // checking it there is a writing thread running and wait for it;
  pthread_mutex_lock(&read_mutex);
  /*if (writing)*/
  /*pthread_cond_wait(&reading_lock_cond, &read_mutex);*/
  /*pthread_mutex_unlock(&read_mutex);*/

  if (key == NULL || container == NULL)
    return RE_INVALID_ARGS;

  string_container_t *contianer = NULL;

  if (hashmap_get(kv_hashmap, key, (void **)&contianer) != RE_SUCCESS) {
    return RE_KEY_NOT_FOUND;
  };

  *container = contianer;

  return RE_SUCCESS;
}
int delete_kv(char *key) {
  // this will wait if the mutex is already locked until it's unlocked
  // according to the man page of pthread_mutex_lock()
  pthread_mutex_lock(&write_mutex);

  string_container_t *value;
  hashmap_get(kv_hashmap, key, (void **)&value);
  dynamic_array_find_and_remove(timed_data_queue, value);

  hashmap_del(kv_hashmap, key);

  pthread_mutex_unlock(&write_mutex);
  return RE_SUCCESS;
}
