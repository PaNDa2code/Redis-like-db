#include "database.h"
#include "headers.h"
#include "khash.h"
#include "parser.h"

KHASH_MAP_INIT_STR(str, value_t *);

khash_t(str) * data_hashmap;

int init_database() {
  data_hashmap = kh_init(str);
  return 0;
}

int destroy_database() {
  kh_destroy(str, data_hashmap);
  return 0;
}

int key_exist(char *key){
  int ret = 0, is_missing = 0;
  khint_t k; 
  k = kh_get(str, data_hashmap, key);
  is_missing = (k == kh_end(data_hashmap));
  return (is_missing)?0:1;
};

int set_value_by_key(linkedList_node_t *key_node, linkedList_node_t *value_node,
                     time_t expiry_time) {
  if(key_exist(key_node->buffer)) {
    return -1;
  }

  khint_t k;
  int ret = 0;
  value_t *data = malloc(sizeof(value_t) + value_node->length);

  memcpy(data->p_data, value_node->buffer, value_node->length);
  data->data_size = value_node->length; 

  ADD_KEYVALUE(key_node->buffer, data, data_hashmap, k, ret);
  if(ret == 0) {
    free(data);
    return -1;
  }
  return 0;
};


int get_value_by_key(linkedList_node_t *key_node, value_t** value_node) {
  int k = kh_get(str, data_hashmap, key_node->buffer);
  int is_missing = (k == kh_end(data_hashmap));

  if(is_missing){
    printf("Value is not there");
    return -1;
  }

  *value_node = kh_value(data_hashmap, k);

  return 0;
}
