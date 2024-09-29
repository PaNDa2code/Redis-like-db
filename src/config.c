#include "config.h"
#include "commands_functions.h"
#include "hashmap.h"
#include <stddef.h>

config_option_t config[] = {
    {"port", OPT_INT, .value_int = 6379},
    {"maxclients", OPT_INT, .value_int = 50},
    {"maxmemory", OPT_INT, .value_int = (int64_t)500 MB},
    {"role", OPT_STRING, .value_string = "master"},
};

int change_config_opt(char *opt_name, void *new_value, char new_value_type) {
  if (opt_name == NULL || new_value == NULL)
    return RE_INVALID_ARGS;

  for (int i = 0; i < sizeof(config) / sizeof(config_option_t); i++)
    if (strcmp(opt_name, config[i].option_name) == 0) {
      if (config[i].value_type != new_value_type)
        return RE_TYPE_MISS_MATCH;
      config[i].value = new_value;
      break;
    }
  return RE_FAILED;
}
