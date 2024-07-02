#include "includes.h"
#include "data_structures.h"
#include <math.h>


int string_to_bulkstring(string_ptr_t string, char** buffer, size_t *n) {
  int length_digits_count = log10(string->length) + 1;
  *n = length_digits_count + string->length + 5; // $, \r\n, \r\n
  *buffer = malloc(*n + 1);
  snprintf(*buffer, *n + 1, "$%lu\r\n%s\r\n", string->length, string->buffer);
  return 0;
}
