#include "parse_command.h"
#include "commands_functions.h"
#include "data_structures.h"

int command_tokenize(char *input, string_tokens_t **str_ptr_array) {
  char *token;

  size_t tokens_count = strtoul(input + 1, &input, 10);

  *str_ptr_array =
      malloc(sizeof(string_tokens_t) + tokens_count * sizeof(size_t));
  (*str_ptr_array)->tokens_count = tokens_count;

  char **tokens = (*str_ptr_array)->tokens;

  for (size_t i = 0; i < tokens_count; i++) {
    input = strchr(input, '$') + 1;
    size_t strlen = strtoul(input, NULL, 10);
    input = strchr(input, '\n') + 1;
    tokens[i] = input;
    input += strlen + 2;
    tokens[i][strlen] = 0;
  }
  return RE_SUCCESS;
};

