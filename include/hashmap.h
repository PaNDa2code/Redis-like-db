#pragma once
#include <stdint.h>
#include <stdio.h>

#define DEFAULT_HASHMAP_SIZE 1000000

enum _hash_bucket_status {
  BUCKET_EMPTY = 0x00,
  BUCKET_OCCUPIED = 0x01,
  BUCKET_ERASED = 0x02,
};

typedef struct _hash_bucket {
  unsigned char status;
  char *key;
  void *value;
  struct _hash_bucket *next;
} hashmap_bucket_t;

typedef void (*free_function_t)(void *);

typedef struct {
  size_t capacity;
  size_t occupied_buckets;
  size_t max_collitions;
  free_function_t free_value;
  free_function_t free_key;
  hashmap_bucket_t buckets[0];
} hashmap_t;

uint64_t str_hash(char *string);

hashmap_t *new_n_hashmap(size_t n);

hashmap_t *new_hashmap();

int hashmap_resize(hashmap_t **hashmap, size_t new_capacity);

int hashmap_add(hashmap_t *hashmap, char *key, void *value);

int hashmap_set(hashmap_t *hashmap, char *key, void *value);

int hashmap_get(hashmap_t *hashmap, char *key, void **value);

int hashmap_del(hashmap_t *hashmap, char *key);

void free_hashmap(hashmap_t *hashmap);

#define HASHMAP_FOR_EACH(hashmap, key_var, value_var, code)                    \
  char *key_var, *value_var;                                                   \
  for (size_t _i = 0; _i < hashmap->capacity; ++_i) {                          \
    hashmap_bucket_t *_bucket = &hashmap->buckets[_i];                         \
    if (BUCKET_EMPTY == _bucket->status) {                                     \
      continue;                                                                \
    }                                                                          \
    if (BUCKET_OCCUPIED == _bucket->status) {                                  \
      key_var = _bucket->key;                                                  \
      value_var = _bucket->value;                                              \
      code;                                                                    \
    }                                                                          \
    hashmap_bucket_t *_next_bucket = _bucket->next;                            \
    while (_next_bucket != NULL) {                                             \
      _bucket = _next_bucket;                                                  \
      _next_bucket = _bucket->next;                                            \
      if (BUCKET_OCCUPIED == _bucket->status) {                                \
        key_var = _bucket->key;                                                \
        value_var = _bucket->value;                                            \
        code;                                                                  \
      }                                                                        \
    }                                                                          \
  }

#define HASHMAP_FOR_EACH_BUCKET(map, bucket, code)                             \
  hashmap_bucket_t *bucket = NULL;                                             \
  for (size_t i = 0; i < map->capacity; ++i) {                                 \
    bucket = &map->buckets[i];                                                 \
    if (BUCKET_EMPTY == bucket->status)                                        \
      continue;                                                                \
    if (BUCKET_OCCUPIED == bucket->status) {                                   \
      do {                                                                     \
        code;                                                                  \
        bucket = bucket->next;                                                 \
      } while (bucket);                                                        \
    }                                                                          \
  }

#define FOR_EACH(map, key_var, value_var)                                      \
  for (size_t _i = 0; _i < map->capacity; ++_i) {                              \
    hashmap_bucket_t *_bucket = &map->buckets[_i];                             \
    if (BUCKET_EMPTY == _bucket->status) {                                     \
      continue;                                                                \
    }                                                                          \
    if (BUCKET_OCCUPIED == _bucket->status) {                                  \
      key_var = _bucket->key;                                                  \
      value_var = _bucket->value;                                              \
    }                                                                          \
    for (hashmap_bucket_t *_inner_bucket = _bucket; _inner_bucket != NULL;     \
         _inner_bucket = _inner_bucket->next) {                                \
      if (BUCKET_OCCUPIED == _inner_bucket->status) {                          \
        key_var = _inner_bucket->key;                                          \
        value_var = _inner_bucket->value;

#define END_FOR_EACH                                                           \
  }                                                                            \
  }                                                                            \
  }
