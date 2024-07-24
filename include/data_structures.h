#pragma once
#include "includes.h"

typedef struct {
  size_t length;
  uint32_t refrance_count;
  char buffer[1];
} string_t, *string_ptr_t;

typedef struct string_slice {
  char *base;
  size_t start;
  size_t end;
} string_slice_t;

typedef struct {
  struct timespec insertion_time, expiry_time;
  string_ptr_t string;
} string_container_t;
