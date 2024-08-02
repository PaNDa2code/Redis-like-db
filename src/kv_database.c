#include "kv_database.h"
#include "commands_functions.h"
#include "expiry_cleaner.h"
#include "hashmap.h"
#include "timespec_util.h"
#include "rwlock.h"

hashmap_t *kv_hashmap;

rwlock_t rwlock = RWLOCK_INITIALIZER;

void free_container(void *contianer);

int init_kv_hashmap() {
  printf("initializing kv hashmap\n");
  kv_hashmap = new_hashmap();
  kv_hashmap->free_value = free_container;
  kv_hashmap->free_key = free;
  return 0;
}

void free_container(void *contianer) {
  if (((string_container_t *)contianer)->expiry_time.tv_sec &&
      !((string_container_t *)contianer)->expired) {
    cancel_container_timer(contianer);
  }
  free(((string_container_t *)contianer)->string);
  free(contianer);
}

int cleanup_kv_hashmap() {
  w_lock(&rwlock);
  free_hashmap(kv_hashmap);
  w_unlock(&rwlock);
  return RE_SUCCESS;
}

int insert_kv(char *key, char *value, uint64_t expiry_ms) {

  int return_value;

  if (key == NULL || value == NULL) {
    return RE_INVALID_ARGS;
  }

  string_container_t *value_container = malloc(sizeof(string_container_t));

  if (!value_container) {
    // it can be memory or fragmantion issue
    return RE_OUT_OF_MEMORY;
  }

  char *keyd = strdup(key);
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

  w_lock(&rwlock);

  return_value = hashmap_set(kv_hashmap, keyd, value_container);

  if (return_value != RE_SUCCESS) {
    free(keyd);
    free(value_container);
  } else {
    if (kv_hashmap->max_collitions > 10 ||
        (float)kv_hashmap->occupied_buckets / kv_hashmap->capacity >= 0.75)
      hashmap_resize(&kv_hashmap, kv_hashmap->capacity * 2);
  }

  w_unlock(&rwlock);

  return return_value;
}

int lookup_kv(char *key, string_ptr_t *value) {

  if (key == NULL || value == NULL)
    return RE_INVALID_ARGS;

  string_container_t *contianer = NULL;

  r_lock(&rwlock);

  if (hashmap_get(kv_hashmap, key, (void **)&contianer) != RE_SUCCESS) {
    r_unlock(&rwlock);
    return RE_KEY_NOT_FOUND;
  };

  *value = contianer->string;

  r_unlock(&rwlock);
  return RE_SUCCESS;
}

int delete_kv(char *key) {
  if (key == NULL) {
    return RE_INVALID_ARGS;
  }
  int re = 0;
  w_lock(&rwlock);
  re = hashmap_del(kv_hashmap, key);
  w_unlock(&rwlock);
  return re;
}
