#include "PESP_formatter.h"
#include "database.h"
#include "headers.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

int PESP_BULK_STRING_format(pesp_bulk_string *bulk_string, char **meassge, size_t *message_size);

int PESP_format(pesp_data_header *data, char **message, size_t *n) {
  switch (data->type_header) {
  case PESP_SIMPLE_STRING:
    break;

  case PESP_SIMPLE_ERROR:
    break;

  case PESP_BULK_STRING:
    *message = malloc(1);
    *n = 1;
    PESP_BULK_STRING_format((pesp_bulk_string *)data, message, n);
    break;
  }

  return 0;
}

int PESP_BULK_STRING_format(pesp_bulk_string *bulk_string, char **meassge, size_t *message_size) {

  int size_digits_count = log10(bulk_string->Length) + 1;

  size_t n = bulk_string->Length + size_digits_count + 5; // $ + \r\n + \r\n

  size_t write_offset = *message_size - 1;

  *message_size += n;

  *meassge = realloc(*meassge, *message_size);

  snprintf(*meassge + write_offset, n+1,"$%lu\r\n%s\r\n", bulk_string->Length, bulk_string->Buffer);

  return 0;
};

#ifdef test
int main() {
  char string[] = "HelloWorld!";
  int string_len = sizeof(string) - 1;
  pesp_bulk_string *bulk_string = malloc(sizeof(pesp_bulk_string) + string_len);
  memcpy(bulk_string->Buffer, string, string_len);
  bulk_string->Length = sizeof(string) - 1;
  char *message = malloc(1);
  size_t size = 1;
  PESP_BULK_STRING_format(bulk_string, &message, &size);
  PESP_BULK_STRING_format(bulk_string, &message, &size);
  PESP_BULK_STRING_format(bulk_string, &message, &size);
  printf("%s", message);
}
#endif
