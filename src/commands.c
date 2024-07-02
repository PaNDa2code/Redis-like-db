#include "commands_functions.h"
#include "data_structures.h"
#include "format_responce.h"
#include "includes.h"
#include "kv_database.h"

#define SEND_STRING(s)                                                         \
  {                                                                            \
    char m[] = s;                                                              \
    send(client_fd, m, sizeof(m) - 1, 0);                                      \
  }

int ping(string_ptr_t argv[], size_t argc, int client_fd) {
  SEND_STRING("+PONG\r\n");
  return 0;
};

int kv_get(string_ptr_t argv[], size_t argc, int client_fd) {
  if (argc < 1) {
    SEND_STRING("-command \'GET\' requires arg \'KEY\'\r\n");
    return RE_INVALID_ARGS;
  }
  char *key = argv[0]->buffer;
  string_ptr_t value;
  int lookup_return = lookup_kv(key, &value);
  if (lookup_return == RE_KEY_NOT_FOUND) {
    SEND_STRING("$-1\r\n");
    return 0;
  }
  size_t n;
  char *buffer;
  string_to_bulkstring(value, &buffer, &n);
  send(client_fd, buffer, n, 0);
  free(buffer);
  return RE_SUCCESS;
}

int kv_set(string_ptr_t argv[], size_t argc, int client_fd) {
  if (argc < 2) {
    SEND_STRING("-command \'SET\' requires argumnts \'KEY\' and \'VALUE\'\r\n");
    return 0;
  }

  char* key = argv[0]->buffer;
  string_ptr_t value = argv[1];

  int insert_re = insert_kv(key, value, 0);

  switch(insert_re) {
    case RE_SUCCESS:
      SEND_STRING("+OK\r\n");
      break;
    case RE_KEY_EXISTS:
      SEND_STRING("-key is already exists\r\n");
      break;
    case RE_FAILED:
      SEND_STRING("-insertion failed\r\n");
      break;
    case RE_OUT_OF_MEMORY:
      SEND_STRING("-system out of memory\r\n");
      break;
  }

  return RE_SUCCESS;
}