#pragma once
#include "headers.h"

int set_value_by_key(pesp_bulk_string *key_bulk_string,
                     pesp_bulk_string *value_bulk_string, uint64_t validtion_time_msec);

int get_value_by_key(pesp_bulk_string *key_bulk_string,
                     pesp_bulk_string **value_bulk_string);

enum GET_RETURN {
  GET_SUCCESS,
  GET_KEY_NOT_EXIST,
  GET_ERROR
};


int init_database();

int destroy_database();
#pragma end
