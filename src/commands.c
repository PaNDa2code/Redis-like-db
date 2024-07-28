#include "commands_functions.h"
#include "data_structures.h"
#include "dynamic_array.h"
#include "format_responce.h"
#include "includes.h"
#include "kv_database.h"

int ping(string_tokens_t *command_tokens, int client_fd) {
  SEND_C_STRING("+PONG\r\n");
  return 0;
};

int kv_get(string_tokens_t *command_tokens, int client_fd) {
  if (command_tokens->tokens_count < 2) {
    SEND_C_STRING("-command \'GET\' requires arg \'KEY\'\r\n");
    return RE_INVALID_ARGS;
  }
  char *key = command_tokens->tokens[1];
  string_ptr_t value;
  int lookup_return = lookup_kv(key, &value);
  if (lookup_return == RE_KEY_NOT_FOUND) {
    SEND_C_STRING("$-1\r\n");
    return 0;
  }
  size_t n;
  char *buffer;
  string_to_bulkstring(value, &buffer, &n);
  send(client_fd, buffer, n, 0);
  free(buffer);
  return RE_SUCCESS;
}

int kv_set(string_tokens_t *command_tokens, int client_fd) {

  uint64_t expiry_ms = 0;

  if (command_tokens->tokens_count < 3) {
    SEND_C_STRING(
        "-command \'SET\' requires argumnts \'KEY\' and \'VALUE\'\r\n");
    return 0;
  } else if (command_tokens->tokens_count > 3) {
    if (strcmp(command_tokens->tokens[3], "PX") == 0) {
      if (command_tokens->tokens_count < 5) {
        SEND_C_STRING("-missing \'PX\' value\r\n");
        return 0;
      }
      expiry_ms = strtoull(command_tokens->tokens[4], NULL, 10);
    } else {
      SEND_C_STRING("-Unkown keyword ");
      /*SEND_C_STRING(command_tokens->tokens[3]);*/
      SEND_C_STRING("\r\n");
    }
  }

  char *key = command_tokens->tokens[1];
  char *value = command_tokens->tokens[2];

  int insert_re = insert_kv(key, value, expiry_ms);

  switch (insert_re) {
  case RE_SUCCESS:
    SEND_C_STRING("+OK\r\n");
    break;
  case RE_FAILED:
    free(value);
    SEND_C_STRING("-insertion failed\r\n");
    break;
  case RE_OUT_OF_MEMORY:
    free(value);
    SEND_C_STRING("-system out of memory\r\n");
    break;
  }
  return RE_SUCCESS;
}
