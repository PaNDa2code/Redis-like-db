#pragma once
#include "hashmap.h"
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#define dynamic_array(type)                                                    \
  struct {                                                                     \
    size_t size;                                                               \
    size_t allocated_length;                                                   \
    size_t length;                                                             \
    size_t item_size;                                                          \
    void (*value_free_function)(type);                                         \
    pthread_mutex_t mutex;                                                     \
    type *buffer;                                                              \
  }

#define dynamic_array_init(array_ptr)                                          \
  ({                                                                           \
    dynamic_array(typeof((*array_ptr)->buffer[0])) *__dynamic_array =          \
        malloc(sizeof(*__dynamic_array));                                      \
    __dynamic_array->item_size = sizeof(typeof((*array_ptr)->buffer[0]));      \
    pthread_mutex_init(&__dynamic_array->mutex, NULL);                         \
    __dynamic_array->allocated_length = 10;                                    \
    __dynamic_array->length = 0;                                               \
    __dynamic_array->value_free_function = NULL;                               \
    __dynamic_array->size =                                                    \
        __dynamic_array->item_size * __dynamic_array->allocated_length;        \
    __dynamic_array->buffer = malloc(__dynamic_array->size);                   \
    memset(__dynamic_array->buffer, 0, __dynamic_array->size);                 \
    *array_ptr = (void *)__dynamic_array;                                      \
  })

#define dynamic_array_init_with_size(array_ptr, array_size)                    \
  ({                                                                           \
    dynamic_array(typeof((*array_ptr)->buffer[0])) *__dynamic_array =          \
        malloc(sizeof(*__dynamic_array));                                      \
    __dynamic_array->item_size = sizeof(typeof((*array_ptr)->buffer[0]));      \
    pthread_mutex_init(&__dynamic_array->mutex, NULL);                         \
    __dynamic_array->allocated_length = array_size;                            \
    __dynamic_array->length = 0;                                               \
    __dynamic_array->value_free_function = NULL;                               \
    __dynamic_array->size =                                                    \
        __dynamic_array->item_size * __dynamic_array->allocated_length;        \
    __dynamic_array->buffer = malloc(__dynamic_array->size);                   \
    memset(__dynamic_array->buffer, 0, __dynamic_array->size);                 \
    *array_ptr = (void *)__dynamic_array;                                      \
  })

#define free_dynamic_array(array)                                              \
  ({                                                                           \
    if (array->value_free_function) {                                          \
      void (*value_free_function)(typeof(array->buffer[0])) =                  \
          array->value_free_function;                                          \
      dynamic_array_for_each(array, value) value_free_function(value);         \
    }                                                                          \
    pthread_mutex_destroy(&array->mutex);                                      \
    free(array->buffer);                                                       \
    free(array);                                                               \
  })

#define dynamic_array_resize_no_mutex(array, new_allocated_length)             \
  ({                                                                           \
    array->buffer =                                                            \
        realloc(array->buffer, new_allocated_length * array->item_size);       \
    array->allocated_length = new_allocated_length;                            \
  })

#define dynamic_array_resize(array, new_length)                                \
  ({                                                                           \
    pthread_mutex_lock(&array->mutex);                                         \
    array->buffer = realloc(array->buffer, new_length * array->item_size);     \
    array->allocated_length = new_length;                                      \
    array->length = new_length;                                                \
    pthread_mutex_unlock(&array->mutex);                                       \
  })

#define dynamic_array_push(array, value)                                       \
  ({                                                                           \
    pthread_mutex_lock(&array->mutex);                                         \
    if (array->length == array->allocated_length)                              \
      dynamic_array_resize_no_mutex(array, array->length * 2);                 \
    array->buffer[array->length] = value;                                      \
    array->length++;                                                           \
    pthread_mutex_unlock(&array->mutex);                                       \
  })

#define dynamic_array_push_in_order(array, value, cmp_fun)                     \
  ({                                                                           \
    pthread_mutex_lock(&array->mutex);                                         \
    if (array->length == array->allocated_length) {                            \
      dynamic_array_resize(array, array->length * 2);                          \
    }                                                                          \
    int64_t i;                                                                 \
    for (i = array->length - 1; (i >= 0 && cmp_fun(array->buffer[i], value));  \
         --i) {                                                                \
      array->buffer[i + 1] = array->buffer[i];                                 \
    }                                                                          \
    array->buffer[i + 1] = value;                                              \
    array->length++;                                                           \
    pthread_mutex_unlock(&(array)->mutex);                                     \
  })

#define dynamic_array_pop(array, index)                                        \
  ({                                                                           \
    size_t _index = ((index) == - 1) ? (array)->length - 1 : (index); \
    pthread_mutex_lock(&(array)->mutex);                                       \
    typeof((array)->buffer[0]) _result = (array)->buffer[_index];              \
    for (size_t i = _index; i < (array)->length - 1; ++i) {                    \
      (array)->buffer[i] = (array)->buffer[i + 1];                             \
    }                                                                          \
    (array)->length--;                                                         \
    pthread_mutex_unlock(&(array)->mutex);                                     \
    _result;                                                                   \
  })

#define dynamic_array_pop_no_mutex(array, index)                               \
  ({                                                                           \
    size_t _index = ((index) == - 1) ? (array)->length - 1 : (index); \
    typeof((array)->buffer[0]) _result = (array)->buffer[_index];              \
    if ((array)->length == 1 && index == 0) {                                  \
      (array)->buffer[0] = (typeof((array)->buffer[0]))0;                      \
    }                                                                          \
    for (size_t i = _index; i < (array)->length - 1; ++i) {                    \
      (array)->buffer[i] = (array)->buffer[i + 1];                             \
    }                                                                          \
    (array)->length--;                                                         \
    _result;                                                                   \
  })

#define dynamic_array_get(array, index)                                        \
  ({                                                                           \
    size_t _index = ((index) == - 1) ? (array)->length - 1 : (index); \
    pthread_mutex_lock(&array->mutex);                                         \
    typeof((array)->buffer[0]) _result = array->buffer[index];                 \
    pthread_mutex_unlock(&array->mutex);                                       \
    _result;                                                                   \
  })

#define dynamic_array_for_each(array, value)                                   \
  typeof(array->buffer[0]) value;                                              \
  for (size_t __i = 0, __ph = pthread_mutex_lock(&(array)->mutex);             \
       __i < (array)->length && (value = (array)->buffer[__i], 1);             \
       __i++, (__i == (array)->length ? pthread_mutex_unlock(&(array)->mutex)  \
                                      : (void)0))

#define dynamic_array_for_each_index(array, value, idx)                        \
  typeof(array->buffer[0]) value;                                              \
  for (size_t __i = 0, __ph = pthread_mutex_lock(&(array)->mutex), idx = __i;  \
       __i < (array)->length && (idx = __i, 1) &&                              \
       (value = (array)->buffer[__i], 1);                                      \
       __i++, (__i == (array)->length ? pthread_mutex_unlock(&(array)->mutex)  \
                                      : (void)0))

#define dynamic_array_for_each_index_no_mutex(array, value, idx)               \
  typeof(array->buffer[0]) value;                                              \
  for (size_t __i = 0, idx = __i; __i < (array)->length && (idx = __i, 1) &&   \
                                  (value = (array)->buffer[__i], 1);           \
       __i++)

#define dynamic_array_index(array, value)                                      \
  ({                                                                           \
    dynamic_array_for_each_index(array, v, i) {                                \
      if (v == value)                                                          \
        i;                                                                     \
    }                                                                          \
    -1;                                                                        \
  })

#define dynamic_array_index_no_mutex(array, value)                             \
  ({                                                                           \
    dynamic_array_for_each_index_no_mutex(array, v, index) {                   \
      if (v == value)                                                          \
        index;                                                                 \
    }                                                                          \
    -1;                                                                        \
  })

#define dynamic_array_find_and_remove(array, value)                            \
  ({                                                                           \
    pthread_mutex_lock(&array->mutex);                                         \
    ssize_t index = dynamic_array_index_no_mutex(array, value);                 \
    if (index != -1)                                                           \
      dynamic_array_pop_no_mutex(array, index);                                \
    pthread_mutex_unlock(&array->mutex);                                       \
  })
