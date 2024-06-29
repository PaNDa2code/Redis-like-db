#pragma once
#include "headers.h"

enum {
  PESP_SIMPLE_STRING = '+',
  PESP_SIMPLE_ERROR = '-',
  PESP_INT = ':',
  PESP_BULK_STRING = '$',
  PESP_ARRAY = '*',
  PESP_NULL = '_',
  PESP_BOOL = '#',
  PESP_DOUBLE = ',',
  PESP_BIG_NUMBER = '(',
  PESP_BULK_ERROR = '!',
  PESP_VERBATIM_STRING = '=',
  PESP_MAP = '%',
  PESP_SET = '~',
  PESP_PUSH = '>',
};

typedef struct {
  char type_header;
  size_t size;
  struct timespec insertion_time;
  struct timespec expiry_time;
} pesp_data_header;

typedef struct {
  pesp_data_header header;
  bool is_set;
  size_t Length;
  char Buffer[1]; // null byte
} pesp_bulk_string;

typedef void* pesp_pointer;

typedef struct {
  int data_type;
  pesp_pointer data_buffer;
} pesp_array_data_entry;

typedef struct {
  size_t    Length;
  pesp_array_data_entry List[0];
} pesp_array;


int PESP_format(pesp_data_header *data, char **message, size_t *n);

#pragma end
