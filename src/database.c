#include "database.h"
#include "PESP_formatter.h"
#include "headers.h"
#include "khash.h"
#include "parser.h"

KHASH_MAP_INIT_STR(str, void *);

khash_t(str) * data_hashmap;

int init_database() {
  data_hashmap = kh_init(str);
  return 0;
}

int destroy_database() {
  khint_t k;
  for (k = kh_begin(data_hashmap); k != kh_end(data_hashmap); ++k) {
    if (kh_exist(data_hashmap, k)) {
      pesp_bulk_string *value = kh_value(data_hashmap, k);
      printf("Freeing %s\n", value->Buffer);
      free(value);
      value = NULL;
    }
  }
  kh_destroy(str, data_hashmap);
  return 0;
}

int key_exist(char *key) {
  int ret = 0, is_missing = 0;
  khint_t k;
  k = kh_get(str, data_hashmap, key);
  is_missing = (k == kh_end(data_hashmap));
  return (is_missing) ? 0 : 1;
};

int set_value_by_key(pesp_bulk_string *key_bulk_string,
                     pesp_bulk_string *value_bulk_string, time_t expiry_time) {

  if (key_exist(key_bulk_string->Buffer) ||
      key_bulk_string->header.type_header != PESP_BULK_STRING ||
      value_bulk_string->header.type_header != PESP_BULK_STRING) {
    return -1;
  }

  khint_t k;
  int ret = 0;

  ADD_KEYVALUE(key_bulk_string->Buffer, value_bulk_string, data_hashmap, k, ret);

  if (ret == 0) {
    return -1;
  }

  value_bulk_string->is_set = true;
  return 0;
};

int get_value_by_key(pesp_bulk_string *key_bulk_string, pesp_bulk_string **value_bulk_string) {
  int k = kh_get(str, data_hashmap, key_bulk_string->Buffer);
  int is_missing = (k == kh_end(data_hashmap));

  if (is_missing) {
    return -1;
  }

  /**value_node = kh_value(data_hashmap, k);*/
  *value_bulk_string = kh_value(data_hashmap, k);

  return 0;
}
