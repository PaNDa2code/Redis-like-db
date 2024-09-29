#include "kv_database.h"
#include "commands_functions.h"
#include "data_structures.h"
#include "expiry_cleaner.h"
#include "hashmap.h"
#include "rwlock.h"
#include "timespec_util.h"
#include <time.h>

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

void format_timespec(FILE *file, struct timespec *t) {
  struct tm *Time = localtime(&t->tv_sec);
  fprintf(file, "%04d-%02d-%02d %02d:%02d:%02d",
          Time->tm_year + 1900, // Year since 1900
          Time->tm_mon + 1,     // Month (0-11, so add 1)
          Time->tm_mday,        // Day of the month
          Time->tm_hour,        // Hour (0-23)
          Time->tm_min,         // Minute (0-59)
          Time->tm_sec);
};

int dump_data() {
  char *file_name = "backup.json";
  FILE *file = fopen(file_name, "w");

  if (file == NULL) {
    perror("fopen");
    return -1;
  }

  printf("[#] dumping data to \"%s\"\n", file_name);
  fwrite("{\n", 1, 2, file);

  int first = 1;
  char *key;
  string_container_t *value;

  FOR_EACH(kv_hashmap, key, value) {
    if (!first) {
      fwrite(",\n", 1, 2, file);
    }
    first = 0;
    fprintf(file, "  \"%s\": { \"value\": \"%s\"", key, value->string->buffer);

    fprintf(file, ", \"insertionTime\": \"");
    format_timespec(file, &value->insertion_time);
    fprintf(file, "\"");

    if (value->expiry_time.tv_sec) {
      fprintf(file, ", \"expiryTime\": \"");
      format_timespec(file, &value->expiry_time);
      fprintf(file, "\"");
    }

    fprintf(file, " }");
  } END_FOR_EACH;

  fwrite("\n}\n", 1, 3, file);
  fclose(file);

  return 0;
}

int load_data(char *file_name) { return 0; };
