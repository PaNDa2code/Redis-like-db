#include "headers.h"


int PESP_format(pesp_data_header *data, char **message, size_t *n) {
  switch (data->type_header) {
  case PESP_SIMPLE_STRING:
    break;

  case PESP_SIMPLE_ERROR:
    break;

  case PESP_BULK_STRING:
    *message = NULL;
    *n = 0;
    bulk_string_to_cstr((pesp_bulk_string *)data, message, n);
    break;
  }

  return 0;
}

int bulk_string_to_cstr(pesp_bulk_string *bulk_string, char **str, size_t *size) {

  if (0 == *size || NULL == *str){
    *str = malloc(1);
    *size = 1;
  }

  int size_digits_count = log10(bulk_string->Length) + 1;

  size_t n = bulk_string->Length + size_digits_count + 5; // $ + \r\n + \r\n

  size_t write_offset = *size - 1;

  *size += n;

  *str = realloc(*str, *size);

  snprintf(*str + write_offset, n+1,"$%lu\r\n%s\r\n", bulk_string->Length, bulk_string->Buffer);

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
