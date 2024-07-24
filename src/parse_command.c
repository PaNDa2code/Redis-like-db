#include "parse_command.h"
#include "commands_functions.h"
#include "data_structures.h"
#include "dynamic_array.h"
#include <string.h>

int parse_command(char *input, void **str_ptr_array) {
  dynamic_array(string_ptr_t) * array;
  input++;
  size_t array_len = strtoul(input, NULL, 10);

  if (array_len <= 0)
    return RE_FAILED;

  dynamic_array_init_with_size(&array, array_len);
  array->value_free_function = free_string;

  for (size_t i = 0; i < array_len; ++i) {
    while (*input != 0 && *(input - 1) != '$')
      input++;

    size_t string_len = strtoul(input, NULL, 10);

    string_ptr_t string_ptr = malloc(sizeof(string_t) + string_len);

    if (NULL == string_ptr) {
      free_dynamic_array(array);
      return RE_FAILED;
    }

    string_ptr->length = string_len;
    string_ptr->refrance_count = 0;

    while (*input != 0 && *(input - 1) != '\n')
      input++; // go to the frist chracter next to \n

    mempcpy(string_ptr->buffer, input, string_len);

    string_ptr->buffer[string_len] = 0;

    dynamic_array_push(array, string_ptr);
  }

  *str_ptr_array = array;
  return RE_SUCCESS;
};

void free_string(string_ptr_t str) {
  if (((string_ptr_t)str)->refrance_count <= 0) {
    free(str);
  }
}
