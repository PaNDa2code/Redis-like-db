#include "includes.h"
#include "data_structures.h"

int parse_command(char *input, string_ptr_t **str_array, size_t *n) {

  input++;
  size_t array_len = strtoul(input, NULL, 10);

  if (array_len <= 0)
    return -1;

  *n = array_len;

  string_ptr_t *array = malloc(sizeof(string_t *) * array_len);

  memset(array, 0, array_len * sizeof(string_t *));

  for (int i = 0; i < array_len; ++i) {
    while (*input != 0 && *(input - 1) != '$')
      input++;

    size_t string_len = strtoul(input, NULL, 10);

    string_ptr_t string_ptr = malloc(sizeof(string_t) + string_len);

    if (NULL == string_ptr) {
      free(array);
    }

    string_ptr->length = string_len;
    string_ptr->refrance_count = 0;

    while (*input != 0 && *(input - 1) != '\n')
      input++; // go to the frist chracter next to \n

    mempcpy(string_ptr->buffer, input, string_len);

    string_ptr->buffer[string_len] = 0;

    array[i] = string_ptr;
  }
  *str_array = array;
  return 0;
};

void print_str_array(string_ptr_t *array, size_t n) {
  for (int i = 0; i < n; ++i) {
    printf("%s\n", array[i]->buffer);
  }
}

void free_string_array(string_ptr_t *array, size_t n) {
  for (int i = 0; i < n; ++i) {
    if (array[i]->refrance_count <= 0)
      free(array[i]);
  }
  free(array);
}

