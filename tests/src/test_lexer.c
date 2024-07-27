#include "parse_command.h"
#include <stdio.h>

int main() {
  char input[] = "*2\r\n$5\r\nHello\r\n$5\r\nWrold\r\n";
  string_tokens_t *str_tokens;
  command_tokenize(input, &str_tokens);

  for (int i = 0; i < str_tokens->tokens_count; i++) {
    printf("%s, ", str_tokens->tokens[i]);
  }
  free(str_tokens);
}
