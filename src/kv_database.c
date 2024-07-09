#include "kv_database.h"
#include "commands_functions.h"
#include "data_structures.h"
#include "hashmap.h"
#include "timespec_util.h"
#include <bits/time.h>
#include <time.h>

/*map(char *, string_container_t *) kv_hashmap;*/
hashmap_t *kv_hashmap;

pthread_mutex_t read_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t write_mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t reading_lock_cond = PTHREAD_COND_INITIALIZER;

bool writing = false;

int init_kv_hashmap() {
  printf("initializing kv hashmap\n");
  kv_hashmap = new_hashmap();
  return 0;
}

void free_container(void *contianer) {
  free(((string_container_t *)contianer)->string);
  free(contianer);
}

int cleanup_kv_hashmap() {
  pthread_mutex_lock(&read_mutex);

  char *key;
  char *value;

  free_hashmap(kv_hashmap, free_container);

  pthread_mutex_unlock(&read_mutex);
  return RE_SUCCESS;
}

int insert_kv(char *key, string_ptr_t value, uint64_t expiry_ms) {

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

  int re = insert_to_hashmap(kv_hashmap, key, (char *)value_container);

  if (re == RE_KEY_EXISTS) {
    string_container_t *tmp;
    get_from_hashmap(kv_hashmap, key, (char **)&tmp);
    if (tmp->expiry_time.tv_sec) {
      struct timespec ts;
      clock_gettime(CLOCK_REALTIME, &ts);
      if (cmpr_timestamp(&ts, &tmp->expiry_time) != LESS_THAN) {
        free(value_container);
        value_container = tmp;
        re = RE_SUCCESS;
      }
    }
  }

  if (re != RE_SUCCESS) {
    free(value_container);
    return_value = re;
    goto EXIT;
  }

  value_container->string = value;

  clock_gettime(CLOCK_REALTIME, &value_container->insertion_time);

  if (expiry_ms) {
    value_container->expiry_time = value_container->insertion_time;
    add_mc_timestamp(&value_container->expiry_time, expiry_ms);
  } else {
    memset(&value_container->expiry_time, 0, sizeof(struct timespec));
  }

  value->refrance_count++;

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

  if (get_from_hashmap(kv_hashmap, key, (char **)&contianer) != RE_SUCCESS) {
    return RE_KEY_NOT_FOUND;
  };

  if (contianer->expiry_time.tv_sec) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    if (cmpr_timestamp(&ts, &contianer->expiry_time) != LESS_THAN) {
      delete_kv(key);
      return RE_KEY_NOT_FOUND;
    }
  }

  *value = contianer->string;

  return RE_SUCCESS;
}

int delete_kv(char *key) {
  // this will wait if the mutex is already locked until it's unlocked
  // according to the man page of pthread_mutex_lock()
  pthread_mutex_lock(&write_mutex);

  erase_hashmap_bucket(kv_hashmap, key, free_container);

  pthread_mutex_unlock(&write_mutex);
  return RE_SUCCESS;
}

/**/
/*int check_kv(char* key){*/
/*  if(key == NULL)*/
/*    return RE_INVALID_ARGS;*/
/**/
/*  void* g = get(&kv_hashmap, key);*/
/**/
/*  if(g == NULL)*/
/*    return RE_KEY_NOT_FOUND;*/
/**/
/*  string_container_t *contianer = *((string_container_t **)g);*/
/**/
/*  if(!contianer->expiry_time.tv_sec)*/
/*    return RE_KEY_EXISTS;*/
/**/
/*  struct timespec ts_now;*/
/**/
/*  clock_gettime(CLOCK_REALTIME, &ts_now);*/
/**/
/*  if (!cmpr_timestamp(&ts_now, &contianer->expiry_time, LESS_THAN)){*/
/*    delete_kv(key);*/
/*    return RE_KEY_NOT_FOUND;*/
/*  } */
/*  r */
