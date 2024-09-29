#pragma once
#include "includes.h"

enum option_value_type { OPT_STRING, OPT_INT, OPT_FLOAT };

#define KB *1024
#define MB KB KB
#define GB KB MB


typedef struct {
  char *option_name;
  char value_type;
  union {
    void *value;
    char *value_string;
    int64_t value_int;
    double value_float;
  };
} config_option_t;



typedef struct {
  config_option_t options[0];
} config_options_array_t;
