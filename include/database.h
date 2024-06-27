#pragma once
#include <sys/types.h>
#include <time.h>
#include "parser.h"

typedef struct {
  time_t insertion_time;
  time_t expiry_time;
  size_t data_size;
  u_char p_data[1];
} value_t;


int set_value_by_key(linkedList_node_t *key_node, linkedList_node_t *value_node, time_t expiry_time);
 
int get_value_by_key(linkedList_node_t *key_node, value_t** value_node);

int init_database();

int destroy_database();

#define INFINITE_TIME 0x7FFFFFFFFFFFFFFFll

#define ADD_KEYVALUE(key, value, hm_name, k, p_absent)                         \
  {                                                                            \
    k = kh_put(str, hm_name, key, &p_absent);                                  \
    kh_value(hm_name, k) = value;                                              \
  }


#pragma end
