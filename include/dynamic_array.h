#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma once

typedef struct {
  size_t length;
  size_t capacity;
  size_t last_element_index;
  char buffer[0];
} dynamic_array_header_t;

#define dynamic_array(type, size)                                              \
  ({                                                                           \
    dynamic_array_header_t *header =                                           \
        malloc(sizeof(dynamic_array_header_t) + sizeof(type) * size);          \
    if (NULL == header)                                                        \
      (type *)NULL;                                                            \
    else {                                                                     \
      header->length = size;                                                   \
      header->capacity = sizeof(type);                                         \
      header->last_element_index = -1;                                         \
    }                                                                          \
    (type *)&header->buffer;                                                   \
  })

#define __header(array)                                                        \
  ((dynamic_array_header_t *)((char *)(array) - sizeof(dynamic_array_header_t)))

#define array_length(array) __header(array)->last_element_index + 1

#define resize_array(array, new_size)                                          \
  ({                                                                           \
    dynamic_array_header_t *header_r =                                         \
        realloc(__header(array), sizeof(dynamic_array_header_t) +              \
                                     __header(array)->capacity * new_size);    \
    if (NULL == header_r)                                                      \
      -1;                                                                      \
    else {                                                                     \
      header_r->length = new_size;                                             \
      array = (void *)&header_r->buffer;                                       \
    }                                                                          \
    0;                                                                         \
  })

#define array_append(array, value)                                             \
  ({                                                                           \
    dynamic_array_header_t *header_a = __header(array);                        \
    if (header_a->last_element_index + 1 == header_a->length) {                \
      if (0 != resize_array(array, header_a->length * 2))                      \
        -1;                                                                    \
    }                                                                          \
    array[++header_a->last_element_index] = value;                             \
    0;                                                                         \
  })
#define array_pop(array, index)                                                \
  ({                                                                           \
    dynamic_array_header_t *header_p = __header(array);                        \
    if (0 > index || header_p->last_element_index < index)                     \
      -1;                                                                      \
    else {                                                                     \
      for (size_t __i = index; __i < header_p->last_element_index; ++__i)      \
        array[__i] = array[__i + 1];                                           \
      header_p->last_element_index--;                                          \
      0;                                                                       \
    }                                                                          \
  })

#define array_remove_by_value(array, value)                                    \
  ({                                                                           \
    dynamic_array_header_t *header_p = __header(array);                        \
    int __found = 0;                                                           \
    for (size_t __i = 0; __i < header_p->last_element_index; ++__i) {          \
      if (!__found && value == array[__i]) {                                   \
        __found = 1;                                                           \
      }                                                                        \
      if (__found) {                                                           \
        array[__i] = array[__i + 1];                                           \
      }                                                                        \
    }                                                                          \
    header_p->last_element_index--;                                            \
    0;                                                                         \
})

#define for_each(array, value)                                                 \
  for (size_t __i = 0; __header(array)->last_element_index != -1 &&            \
                       __i <= __header(array)->last_element_index &&           \
                       ((*value = array[__i]) || 1);                           \
       ++__i)

#define free_dynamic_array(array) free(__header(array))

void merge(void *array, size_t left, size_t mid, size_t right,
           size_t element_size, int (*comparator)(const void *, const void *));

void merge_sort_rec(void *array, size_t left, size_t right, size_t element_size,
                    int (*comparator)(const void *, const void *));

/*#define merge_sort(array, comparator)                                          \*/
/*  merge_sort_rec(array, 0, array_length(array) - 1, __header(array)->capacity, \*/
/*                 comparator)*/
/**/
/*void merge(void *array, size_t left, size_t mid, size_t right,*/
/*           size_t element_size, int (*comparator)(const void *, const void *)) {*/
/*  size_t n1 = mid - left + 1;*/
/*  size_t n2 = right - mid;*/
/**/
/*  void *L = malloc(n1 * element_size);*/
/*  void *R = malloc(n2 * element_size);*/
/**/
/*  memcpy(L, (char *)array + left * element_size, n1 * element_size);*/
/*  memcpy(R, (char *)array + (mid + 1) * element_size, n2 * element_size);*/
/**/
/*  size_t i = 0, j = 0, k = left;*/
/*  while (i < n1 && j < n2) {*/
/*    if (comparator((char *)L + i * element_size,*/
/*                   (char *)R + j * element_size) <= 0) {*/
/*      memcpy((char *)array + k * element_size, (char *)L + i * element_size,*/
/*             element_size);*/
/*      i++;*/
/*    } else {*/
/*      memcpy((char *)array + k * element_size, (char *)R + j * element_size,*/
/*             element_size);*/
/*      j++;*/
/*    }*/
/*    k++;*/
/*  }*/
/**/
/*  while (i < n1) {*/
/*    memcpy((char *)array + k * element_size, (char *)L + i * element_size,*/
/*           element_size);*/
/*    i++;*/
/*    k++;*/
/*  }*/
/**/
/*  while (j < n2) {*/
/*    memcpy((char *)array + k * element_size, (char *)R + j * element_size,*/
/*           element_size);*/
/*    j++;*/
/*    k++;*/
/*  }*/
/**/
/*  free(L);*/
/*  free(R);*/
/*}*/
/**/
/*void merge_sort_rec(void *array, size_t left, size_t right, size_t element_size,*/
/*                    int (*comparator)(const void *, const void *)) {*/
/*  if (left < right) {*/
/*    size_t mid = left + (right - left) / 2;*/
/*    merge_sort_rec(array, left, mid, element_size, comparator);*/
/*    merge_sort_rec(array, mid + 1, right, element_size, comparator);*/
/*    merge(array, left, mid, right, element_size, comparator);*/
/*  }*/
/*}*/
