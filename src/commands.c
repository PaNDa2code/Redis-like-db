#include "khash.h"
#include "network_utils.h"
#include "parser.h"

typedef int (*command_func_t)(linkedList_t *, int);

int echo(linkedList_t *input, int responce_fd);
int ping(linkedList_t *input, int responce_fd);

#define ADD_KEYVALUE(key, value) { \
  k = kh_put(str, hashmap, key, &absent); \
  kh_value(hashmap, k) = value; \
}

KHASH_MAP_INIT_STR(str, command_func_t)

int run_command(linkedList_t *command_head, int responce_fd) {
  khash_t(str) *hashmap = kh_init(str);
  khint_t k;
  int absent;

  ADD_KEYVALUE("PING", ping);
  ADD_KEYVALUE("ECHO", echo);

  k = kh_get(str, hashmap, command_head->next_node->buffer);

  kh_value(hashmap, k)(command_head->next_node->next_node, responce_fd);
  return 0;
}

int echo(linkedList_t *input, int responce_fd) {
  printf("ECHO\n");
  char buffer[input->length + 50];
  memset(buffer, 0, sizeof(buffer));
  sprintf(buffer, "$%lu\r\n%s\r\n", input->length, input->buffer);
  send(responce_fd, buffer, strlen(buffer), 0);
  return 0;
}

int ping(linkedList_t *input, int responce_fd) {
  send(responce_fd, "+PONG\r\n", 7, 0);
  return 0;
}
