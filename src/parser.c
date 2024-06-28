#include "headers.h"
#include "PESP_formatter.h"

void parse(char *input, pesp_array **command_array) {
  if (input == NULL || *input != PESP_ARRAY || command_array == NULL)
    return;
  input++;

  if (!isdigit(*input))
    return;

  size_t array_length = strtoul(input, &input, 10);

  if (array_length <= 0)
    return;

  *command_array = malloc(sizeof(pesp_array_data_entry) * array_length);

  if (*command_array == NULL)
    return;

  (*command_array)->Length = array_length;

  for (int i = 0; i < array_length; ++i) {

    while (*input != 0 && *(input - 1) != PESP_BULK_STRING)
      input++;

    if (!isdigit(*input)) {
      free(*command_array);
      *command_array = NULL;
      return;
    }
    size_t bulk_string_length = strtoul(input, &input, 10);

    size_t bulk_string_size = sizeof(pesp_bulk_string) + bulk_string_length;

    pesp_bulk_string *bulk_string = malloc(bulk_string_size);

    bulk_string->header.type_header = PESP_BULK_STRING;
    bulk_string->header.size = bulk_string_size;

    if (bulk_string == NULL) {
      free(*command_array);
      *command_array = NULL;
      return;
    }

    while (*(input - 1) != '\n')
      input++;

    bulk_string->Length = bulk_string_length;
    char *buff = bulk_string->Buffer;
    memcpy(buff, input, bulk_string_length);
    buff[bulk_string_length] = 0;

    (*command_array)->List[i].data_type = PESP_BULK_STRING;
    (*command_array)->List[i].data_buffer = bulk_string;
  }
  return;
}

void free_bulk_array(pesp_array *bulk_array) {
  for (int i = 0; i < bulk_array->Length; ++i) {
    // if the bulk is set to the data base, don't free it.
    // so i don't have to allocte it twice.
    if(!((pesp_bulk_string*)bulk_array->List[i].data_buffer)->is_set)
      free(bulk_array->List[i].data_buffer);
  }
  free(bulk_array);
}

#ifdef TEST_PARSER
int main() {
  char input[] = "*2\r\n$4\r\nECHO\r\n$11\r\nHelloWorld!\r\n";
  pesp_array *command_array = NULL;
  parse(input, &command_array);
  pesp_bulk_string *bulk = command_array->List[0].data_buffer;
  bulk = command_array->List[1].data_buffer;
  free_bulk_array(command_array);
  return 0;
}
#endif
