#include "headers.h"

KHASH_MAP_INIT_STR(str, pesp_data_header *);
khash_t(str) * data_hashmap;

int init_database() {
  data_hashmap = kh_init(str);
  return 0;
}

int destroy_database() {
  khint_t k;
  for (k = kh_begin(data_hashmap); k != kh_end(data_hashmap); ++k) {
    if (kh_exist(data_hashmap, k)) {
      pesp_bulk_string *value = (pesp_bulk_string *)kh_value(data_hashmap, k);
      free((char *)kh_key(data_hashmap, k));
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
                     pesp_bulk_string *value_bulk_string,
                     uint64_t validtion_time_msec) {

  if (key_exist(key_bulk_string->Buffer) ||
      key_bulk_string->header.type_header != PESP_BULK_STRING ||
      value_bulk_string->header.type_header != PESP_BULK_STRING) {
    return -1;
  }

  khint_t k;
  int ret = 0;

  struct timespec insertion_ts;
  clock_gettime(CLOCK_REALTIME, &insertion_ts);

  struct timespec expiry_ts = {0, 0};

  if (validtion_time_msec != INFINITE_TIME) {

    expiry_ts.tv_sec = validtion_time_msec / 1000 + insertion_ts.tv_sec;
    expiry_ts.tv_nsec =
        (validtion_time_msec % 1000) * 1000000 + insertion_ts.tv_nsec;

    if (expiry_ts.tv_nsec >= 1000000000) {
      expiry_ts.tv_sec += expiry_ts.tv_nsec / 1000000000;
      expiry_ts.tv_nsec %= 1000000000;
    }
  }

  value_bulk_string->header.insertion_time = insertion_ts;
  value_bulk_string->header.expiry_time = expiry_ts;

  ADD_KEYVALUE(strdup(key_bulk_string->Buffer),
               (pesp_data_header *)value_bulk_string, data_hashmap, k, ret);

  if (ret == 0) {
    return -1;
  }

  value_bulk_string->is_set = true;

  return 0;
};

static inline int is_expired(struct timespec *current_ts,
                             struct timespec *expiry_ts) {
  return current_ts->tv_sec > expiry_ts->tv_sec ||
         (current_ts->tv_sec == expiry_ts->tv_sec &&
          current_ts->tv_nsec > expiry_ts->tv_nsec);
}

int get_value_by_key(pesp_bulk_string *key_bulk_string,
                     pesp_bulk_string **value_bulk_string) {

  *value_bulk_string = NULL;

  int k = kh_get(str, data_hashmap, key_bulk_string->Buffer);
  int is_missing = (k == kh_end(data_hashmap));

  if (is_missing) {
    return GET_KEY_NOT_EXIST;
  }

  pesp_bulk_string *value_bulk = (pesp_bulk_string *)kh_value(data_hashmap, k);

  struct timespec *expiry_ts = &value_bulk->header.expiry_time;

  if (expiry_ts->tv_sec == 0) {
    *value_bulk_string = value_bulk;
    return GET_SUCCESS;
  }

  struct timespec current_ts;

  clock_gettime(CLOCK_REALTIME, &current_ts);

  if (is_expired(&current_ts, expiry_ts)) {
    free(value_bulk);
    kh_del(str, data_hashmap, k);
    return GET_KEY_NOT_EXIST;
  }

  *value_bulk_string = value_bulk;

  return GET_SUCCESS;
}

