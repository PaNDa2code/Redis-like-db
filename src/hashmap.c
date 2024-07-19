#include "hashmap.h"
#include "commands_functions.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

hashmap_t *new_hashmap() { return new_n_hashmap(HASHMAP_SIZE); }

hashmap_t *new_n_hashmap(size_t n) {
  hashmap_t *map = malloc(sizeof(hashmap_t) + sizeof(hashmap_bucket_t) * n);
  memset(map, 0, sizeof(hashmap_t) + sizeof(hashmap_bucket_t) * n);
  if (NULL == map)
    return NULL;
  map->capacity = n;
  map->free_value = free;
  return map;
}

int hashmap_resize(hashmap_t **hashmap, size_t new_capacity) {
  char *key, *value;
  hashmap_t *new_hm = new_n_hashmap(new_capacity);
  FOR_EACH((*hashmap), key, value) {
    hashmap_set(new_hm, key, value); 
  } END_FOR_EACH;
  new_hm->free_value = (*hashmap)->free_value;
  free_hashmap(*hashmap);
  *hashmap = new_hm;
  return RE_SUCCESS;
}

int hashmap_set(hashmap_t *hashmap, char *key, void *value) {

  if (!hashmap || !key || !value)
    return RE_INVALID_ARGS;

  uint64_t index = str_hash(key);

  hashmap_bucket_t *bucket = &hashmap->buckets[index];

  if (bucket->status == BUCKET_OCCUPIED) {
    hashmap_bucket_t *prev = NULL;
    do {
      if (strcmp(key, bucket->key) == 0){
        if(hashmap->free_value) hashmap->free_value(bucket->value);
        bucket->value = value;
        return RE_SUCCESS;
      }
      prev = bucket;
      bucket = bucket->next;
    } while (bucket);

    prev->next = malloc(sizeof(hashmap_bucket_t));
    bucket = prev->next;

  } else if (bucket->status == BUCKET_EMPTY ||
             bucket->status == BUCKET_ERASED) {
  } else {
    return RE_FAILED;
  }

  bucket->key = strdup(key);
  bucket->value = value;
  bucket->status = BUCKET_OCCUPIED;

  return RE_SUCCESS;
}

int hashmap_add(hashmap_t *hashmap, char *key, void *value) {

  if (!hashmap || !key || !value)
    return RE_INVALID_ARGS;

  uint64_t index = str_hash(key);

  hashmap_bucket_t *bucket = &hashmap->buckets[index];

  if (bucket->status == BUCKET_OCCUPIED) {
    hashmap_bucket_t *prev = NULL;
    do {
      if (strcmp(key, bucket->key) == 0)
        return RE_KEY_EXISTS;
      prev = bucket;
      bucket = bucket->next;
    } while (bucket);

    prev->next = malloc(sizeof(hashmap_bucket_t));
    bucket = prev->next;

  } else if (bucket->status == BUCKET_EMPTY ||
             bucket->status == BUCKET_ERASED) {
  } else {
    return RE_FAILED;
  }

  bucket->key = strdup(key);
  bucket->value = value;
  bucket->status = BUCKET_OCCUPIED;

  return RE_SUCCESS;
}

int hashmap_get(hashmap_t *hashmap, char *key, void **value) {

  if (!hashmap || !key)
    return RE_INVALID_ARGS;

  uint64_t index = str_hash(key);

  hashmap_bucket_t *bucket = &hashmap->buckets[index];

  if (bucket->status == BUCKET_EMPTY) {
    return RE_KEY_NOT_FOUND;
  }

  while (bucket) {
    if (bucket->status == BUCKET_OCCUPIED && strcmp(key, bucket->key) == 0) {
      *value = bucket->value;
      return RE_SUCCESS;
    }
    bucket = bucket->next;
  }

  return RE_KEY_NOT_FOUND;
}

void free_hashmap(hashmap_t *hashmap) {

  if (NULL == hashmap)
    return;

  for (size_t i = 0; i < HASHMAP_SIZE; ++i) {
    hashmap_bucket_t *bucket = &hashmap->buckets[i];
    if (BUCKET_EMPTY == bucket->status) {
      continue;
    }
    if (BUCKET_OCCUPIED == bucket->status) {
      free(bucket->key);
      if (NULL != hashmap->free_value)
        hashmap->free_value(bucket->value);
    }

    bucket = bucket->next;

    while (NULL != bucket) {
      hashmap_bucket_t *next_bucket = bucket->next;
      if (BUCKET_OCCUPIED == bucket->status) {
        free(bucket->key);
        if (NULL != hashmap->free_value)
          hashmap->free_value(bucket->value);
      }
      free(bucket);
      bucket = next_bucket;
    }
  }
  free(hashmap);
}

int hashmap_del(hashmap_t *hashmap, char *key) {

  uint64_t index = str_hash(key);

  hashmap_bucket_t *bucket = &hashmap->buckets[index];

  if (BUCKET_EMPTY == bucket->status)
    return RE_KEY_NOT_FOUND;

  hashmap_bucket_t *prev_bucket = NULL;

  while (bucket) {
    if (BUCKET_OCCUPIED == bucket->status && strcmp(bucket->key, key) == 0) {
      free(bucket->key);
      bucket->status = BUCKET_ERASED;
      if (NULL != hashmap->free_value)
        hashmap->free_value(bucket->value);
      if (prev_bucket) {
        prev_bucket->next = bucket->next;
        free(bucket);
      }
    }
    prev_bucket = bucket;
    bucket = bucket->next;
  }

  return RE_SUCCESS;
}
