#pragma once
#include "parser.h"
#include <sys/types.h>
#include <time.h>

typedef struct {
  time_t insertion_time;
  time_t expiry_time;
  size_t data_size;
  pesp_array_data_entry data;
} value_t;

int set_value_by_key(pesp_bulk_string *key_bulk_string,
                     pesp_bulk_string *value_bulk_string, time_t expiry_time);

int get_value_by_key(pesp_bulk_string *key_bulk_string,
                     pesp_bulk_string **value_bulk_string);

int init_database();

int destroy_database();

#define INFINITE_TIME 0x7FFFFFFFFFFFFFFFll

#define ADD_KEYVALUE(key, value, hm_name, k, absent)                           \
  {                                                                            \
    k = kh_put(str, hm_name, key, &absent);                                    \
    kh_value(hm_name, k) = value;                                              \
  }

#pragma end
