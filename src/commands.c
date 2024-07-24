#include "commands_functions.h"
#include "data_structures.h"
#include "dynamic_array.h"
#include "format_responce.h"
#include "includes.h"
#include "kv_database.h"

int ping(void *command_dinamic_array, int client_fd) {
  SEND_C_STRING("+PONG\r\n");
  return 0;
};

int kv_get(void *command_dinamic_array, int client_fd) {
  dynamic_array(string_ptr_t) *command = command_dinamic_array;
  if (command->length < 2) {
    SEND_C_STRING("-command \'GET\' requires arg \'KEY\'\r\n");
    return RE_INVALID_ARGS;
  }
  char *key = dynamic_array_get(command, 1)->buffer;
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

int kv_set(void *command_dinamic_array, int client_fd) {

  dynamic_array(string_ptr_t) *command = command_dinamic_array;

  uint64_t expiry_ms = 0;

  if (command->length < 3) {
    SEND_C_STRING(
        "-command \'SET\' requires argumnts \'KEY\' and \'VALUE\'\r\n");
    return 0;
  } else if (command->length > 4) {
    if (strcmp(dynamic_array_get(command, 3)->buffer, "PX") == 0) {
      if (!(command->length > 4)) {
        SEND_C_STRING("-missing \'PX\' value\r\n");
        return 0;
      }
      expiry_ms = strtoull(dynamic_array_get(command, 4)->buffer, NULL, 10);
    } else {
      SEND_C_STRING("-Unkown keyword ");
      SEND_STRING_T(dynamic_array_get(command, 3));
      SEND_C_STRING("\r\n");
    }
  }

  char *key = dynamic_array_get(command, 1)->buffer;
  string_ptr_t value = dynamic_array_get(command, 2);

  int insert_re = insert_kv(key, value, expiry_ms);

  switch (insert_re) {
  case RE_SUCCESS:
    SEND_C_STRING("+OK\r\n");
    break;
  /*case RE_KEY_EXISTS:*/
    /*SEND_C_STRING("-key is already exists\r\n");*/
    /*break;*/
  case RE_FAILED:
    SEND_C_STRING("-insertion failed\r\n");
    break;
  case RE_OUT_OF_MEMORY:
    SEND_C_STRING("-system out of memory\r\n");
    break;
  }

  return RE_SUCCESS;
}
