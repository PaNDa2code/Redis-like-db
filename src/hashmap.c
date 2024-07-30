#include "hashmap.h"
#include "commands_functions.h"

hashmap_t *new_hashmap() { return new_n_hashmap(DEFAULT_HASHMAP_SIZE); }

hashmap_t *new_n_hashmap(size_t n) {
  hashmap_t *map = malloc(sizeof(hashmap_t) + sizeof(hashmap_bucket_t) * n);
  if (NULL == map)
    return NULL;
  memset(map, 0, sizeof(hashmap_t) + sizeof(hashmap_bucket_t) * n);
  map->capacity = n;
  map->free_value = free;
  map->free_key = free;
  return map;
}

int hashmap_resize(hashmap_t **hashmap, size_t new_capacity) {
  char *key, *value;
  hashmap_t *new_hm = new_n_hashmap(new_capacity);
  FOR_EACH((*hashmap), key, value) { hashmap_set(new_hm, key, value); }
  END_FOR_EACH;

  new_hm->free_value = (*hashmap)->free_value, new_hm->free_key = (*hashmap)->free_key;
  (*hashmap)->free_key = NULL, (*hashmap)->free_value = NULL;

  free_hashmap(*hashmap);
  *hashmap = new_hm;
  return RE_SUCCESS;
}

int hashmap_set(hashmap_t *hashmap, char *key, void *value) {

  if (!hashmap || !key || !value)
    return RE_INVALID_ARGS;

  uint64_t index = str_hash(key) % hashmap->capacity;

  hashmap_bucket_t *bucket = hashmap->buckets + index;

  size_t collition = 0;
  if (bucket->status == BUCKET_OCCUPIED) {
    hashmap_bucket_t *prev = NULL;
    do {
      if (strcmp(key, bucket->key) == 0) {
        if (hashmap->free_value)
          hashmap->free_value(bucket->value);
        free(bucket->key);
        bucket->key = key;
        bucket->value = value;
        hashmap->occupied_buckets++;
        hashmap->max_collitions = (hashmap->max_collitions < collition)
                                      ? collition
                                      : hashmap->max_collitions;
        return RE_SUCCESS;
      }
      prev = bucket;
      bucket = bucket->next;
      collition++;
    } while (bucket);

    prev->next = malloc(sizeof(hashmap_bucket_t));
    bucket = prev->next;

  } else if (bucket->status == BUCKET_EMPTY ||
             bucket->status == BUCKET_ERASED) {
  } else {
    return RE_FAILED;
  }

  bucket->key = key;
  bucket->value = value;
  bucket->status = BUCKET_OCCUPIED;
  bucket->next = NULL;
  hashmap->occupied_buckets++;

  return RE_SUCCESS;
}

int hashmap_add(hashmap_t *hashmap, char *key, void *value) {

  if (!hashmap || !key || !value)
    return RE_INVALID_ARGS;

  uint64_t index = str_hash(key) % hashmap->capacity;

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

  bucket->key = key;
  bucket->value = value;
  bucket->status = BUCKET_OCCUPIED;

  return RE_SUCCESS;
}

int hashmap_get(hashmap_t *hashmap, char *key, void **value) {

  if (!hashmap || !key)
    return RE_INVALID_ARGS;

  uint64_t index = str_hash(key) % hashmap->capacity;

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

  for (size_t i = 0; i < hashmap->capacity; ++i) {
    hashmap_bucket_t *bucket = &hashmap->buckets[i];
    if (BUCKET_EMPTY == bucket->status) {
      continue;
    }
    if (BUCKET_OCCUPIED == bucket->status) {
      if (NULL != hashmap->free_key)
        hashmap->free_key(bucket->key);
      if (NULL != hashmap->free_value)
        hashmap->free_value(bucket->value);
    }

    bucket = bucket->next;

    while (NULL != bucket) {
      hashmap_bucket_t *next_bucket = bucket->next;
      if (BUCKET_OCCUPIED == bucket->status) {
        if (NULL != hashmap->free_key)
          hashmap->free_key(bucket->key);
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

  uint64_t index = str_hash(key) % hashmap->capacity;

  hashmap_bucket_t *bucket = &hashmap->buckets[index];

  if (BUCKET_EMPTY == bucket->status)
    return RE_KEY_NOT_FOUND;

  hashmap_bucket_t *prev_bucket = NULL;

  while (bucket) {
    if (BUCKET_OCCUPIED == bucket->status && strcmp(bucket->key, key) == 0) {
      bucket->status = BUCKET_ERASED;
      if (NULL != hashmap->free_key)
        hashmap->free_key(bucket->key);
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
