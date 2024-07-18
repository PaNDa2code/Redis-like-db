#pragma once
#include <stdint.h>

#define HASHMAP_SIZE 1000000

enum _hash_bucket_status {
  BUCKET_EMPTY = 0X00,
  BUCKET_OCCUPIED = 0X01,
  BUCKET_ERASED = 0x02,
};

/*enum _hashmap_functions_return_values {*/
/*  RV_SUCCESS,*/
/*  RV_KEY_NOT_FOUND,*/
/*  RV_KEY_EXIST,*/
/*  RV_FAILD,*/
/*  RV_BAD_ARGS*/
/*};*/

typedef struct _hash_bucket {
  unsigned char status;
  char *key;
  void *value;
  struct _hash_bucket *next;
} hashmap_bucket_t;

typedef struct {
  hashmap_bucket_t buckets[HASHMAP_SIZE];
} hashmap_t;

uint64_t str_hash(char *string);

hashmap_t *new_hashmap();

int init_hashmap(hashmap_t **hashmap);

int hashmap_set(hashmap_t *hashmap, char *key, void *value);

int hashmap_get(hashmap_t *hashmap, char *key, void **value);

int hashmap_del(hashmap_t *hashmap, char *key,
                         void (*free_value)(void *value));

void free_hashmap(hashmap_t *hashmap, void (*free_value)(void *value));

#define HASHMAP_FOR_EACH(hashmap, key_var, value_var, code)                    \
  char *key_var, *value_var;                                                   \
  for (size_t _i = 0; _i < HASHMAP_SIZE; ++_i) {                               \
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

#define FOR_EACH(map, key_var, value_var)                              \
  for (size_t _i = 0; _i < HASHMAP_SIZE; ++_i) {                               \
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
#pragma end
