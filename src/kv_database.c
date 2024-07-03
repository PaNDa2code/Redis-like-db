#include "commands_functions.h"
#include "data_structures.h"
#include "timespec_util.h"
#include "includes.h"
#include "kv_database.h"

map(char *, string_container_t *) kv_hashmap;

pthread_mutex_t read_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t write_mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t reading_lock_cond = PTHREAD_COND_INITIALIZER;

bool writing = false;

int init_kv_hashmap() {
  printf("initializing kv hashmap\n");
  init(&kv_hashmap);
  return 0;
}

int cleanup_kv_hashmap() {
  pthread_mutex_lock(&read_mutex);

  for_each(&kv_hashmap, key, value) {
    free( (*value)->string );
    free(*value);
    free(*key);
  }
  cleanup(&kv_hashmap);

  pthread_mutex_unlock(&read_mutex);
  return RE_SUCCESS;
}


int insert_kv(char *key, string_ptr_t value, uint64_t expiry_ms) {

  int return_value;
  /*printf("kv_hashmap before insertion: %p\n", kv_hashmap);*/

 if (key == NULL || value == NULL) {
    return_value = RE_INVALID_ARGS;
    goto EXIT;
  }

  if (check_kv(key) != RE_KEY_NOT_FOUND) {
    return_value = RE_KEY_EXISTS;
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

  value_container->string = value;
  value_container->key_ptr = strdup(key);


  if (!insert(&kv_hashmap, value_container->key_ptr, value_container)) {
    free(value_container);
    return_value = RE_OUT_OF_MEMORY;
    goto EXIT;
  }

  clock_gettime(CLOCK_REALTIME, &value_container->insertion_time);

  if(expiry_ms){
    value_container->expiry_time = value_container->insertion_time;
    add_mc_timestamp(&value_container->expiry_time, expiry_ms);
  } else {
    memset(&value_container->expiry_time, 0, sizeof(struct timespec));
  }

  value->refrance_count++;

  return_value = RE_SUCCESS;

EXIT:
  // what this lines do is like:
  // hey other threads i have done writing data, you can do whatever you want
  writing = false;
  pthread_cond_broadcast(&reading_lock_cond);
  pthread_mutex_unlock(&write_mutex);
  return return_value;
}

int lookup_kv(char *key, string_ptr_t *value) {


  // checking it there is a writing thread running and wait for it;
  pthread_mutex_lock(&read_mutex);
  while (writing)
    pthread_cond_wait(&reading_lock_cond, &read_mutex);
  pthread_mutex_unlock(&read_mutex);

  if (key == NULL || value == NULL)
    return RE_INVALID_ARGS;


  if(check_kv(key) != RE_KEY_EXISTS)
    return RE_KEY_NOT_FOUND;

  void* g = get(&kv_hashmap, key);

  string_container_t *contianer = *((string_container_t **)g);
   
  *value = contianer->string;

  return RE_SUCCESS;
}

int delete_kv(char* key){
  // this will wait if the mutex is already locked until it's unlocked
  // according to the man page of pthread_mutex_lock()
  pthread_mutex_lock(&write_mutex);
  void *g = get(&kv_hashmap, key); 
  string_container_t *contianer = *((string_container_t **)g);
  free(contianer->string);
  free(contianer->key_ptr);
  free(contianer);
  erase(&kv_hashmap, key);
  pthread_mutex_unlock(&write_mutex);

  return RE_SUCCESS;
}

int check_kv(char* key){
  if(key == NULL)
    return RE_INVALID_ARGS;

  void* g = get(&kv_hashmap, key);

  if(g == NULL)
    return RE_KEY_NOT_FOUND;

  string_container_t *contianer = *((string_container_t **)g);

  if(!contianer->expiry_time.tv_sec)
    return RE_KEY_EXISTS;

  struct timespec ts_now;

  clock_gettime(CLOCK_REALTIME, &ts_now);

  if (!cmpr_timestamp(&ts_now, &contianer->expiry_time, LESS_THAN)){
    delete_kv(key);
    return RE_KEY_NOT_FOUND;
  }

  return RE_KEY_EXISTS;
}

