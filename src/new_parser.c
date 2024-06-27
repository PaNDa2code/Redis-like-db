#include "PESP_formatter.h"
#include "headers.h"

void parse(char *input, pesp_array **command_array) {
  if (*input != PESP_ARRAY)
    return;

  input++;
  size_t array_length = atoi(input);

  if (array_length <= 0)
    return;

  *command_array = malloc(sizeof(pesp_data_entry) * array_length);
  (*command_array)->Length = array_length;

  for (int i = 0; i < array_length; ++i) {

    while (*input != 0 && *(input - 1) != '$')
      input++;

    size_t bulk_string_length = atoi(input);

    size_t bulk_string_size = sizeof(pesp_bulk_string) + bulk_string_length;

    pesp_bulk_string *bulk_string = malloc(bulk_string_size);
    memset(bulk_string, 0, bulk_string_size);

    while (*(input - 1) != '\n')
      input++;

    bulk_string->Length = bulk_string_length;
    memcpy(bulk_string->Buffer, input, bulk_string_length);

    (*command_array)->List[i].data_type = PESP_BULK_STRING;
    (*command_array)->List[i].data_buffer = bulk_string;
  }
}

void free_bulk_array(pesp_array *bulk_array) {
  for(int i = 0; i < bulk_array->Length; ++i) {
    free(bulk_array->List[i].data_buffer);
  }
  free(bulk_array);
}


#ifdef TEST_PARSER
int main() {
  char input[] = "*2\r\n$4\r\nECHO\r\n$11\r\nHelloWorld!\r\n";
  pesp_array *command_array = NULL;
  parse(input, &command_array);
  pesp_bulk_string* bulk = command_array->List[0].data_buffer;
  printf("%s\n", bulk->Buffer);
  bulk = command_array->List[1].data_buffer;
  printf("%s\n", bulk->Buffer);
  free_bulk_array(command_array);
  return 0;
}
#endif
