#include "khash.h"
#include "network_utils.h"
#include "parser.h"
#include <pthread.h>

typedef int (*command_func_t)(linkedList_t *, int);

int echo(linkedList_t *input, int responce_fd);
int ping(linkedList_t *input, int responce_fd);
int command(linkedList_t *input, int responce_fd);
int set(linkedList_t *input, int responce_fd);

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

#define ADD_KEYVALUE(key, value)                                               \
  {                                                                            \
    k = kh_put(str, hashmap, key, &absent);                                    \
    kh_value(hashmap, k) = value;                                              \
  }

KHASH_MAP_INIT_STR(str, command_func_t)

khash_t(str) * hashmap;
int hash_is_set = 0;

void upper(char *str) {
  while (*str != 0) {
    *str = toupper(*str);
    str++;
  }
}

int init_commands_hashmap() {
  printf("[i] Allocating commands hashmap........\n");
  hashmap = kh_init(str);
  khint_t k;
  int absent;

  ADD_KEYVALUE("PING", ping);
  ADD_KEYVALUE("ECHO", echo);
  ADD_KEYVALUE("COMMAND", command);
  ADD_KEYVALUE("SET", set);
  hash_is_set = 1;
  printf("[*] Done\n");
  return 0;
};

int run_command(linkedList_t *command_head, int responce_fd) {
  if (!hash_is_set)
    init_commands_hashmap();
  upper(command_head->next_node->buffer);
  int k = kh_get(str, hashmap, command_head->next_node->buffer);

  if (kh_exist(hashmap, k)) {
    linkedList_t* command_arg = command_head->next_node->next_node ? (command_head->next_node->next_node) : NULL;
    kh_value(hashmap, k)(command_arg, responce_fd);
  } else {
    send(responce_fd, "-ERR unknown command\r\n", 22, 0);
  }

  free_linked_list(command_head);
  return 0;
}

int echo(linkedList_t *input, int responce_fd) {
  if (input != NULL){
    send(responce_fd, "$0\r\n\r\n", 6, 0);
  }
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

int command(linkedList_t *input, int responce_fd) {
  send(responce_fd, "$0\r\n\r\n", 6, 0);
  return 0;
}

int set(linkedList_t *input, int responce_fd) {
  pthread_mutex_lock(&mutex);
  send(responce_fd, "+PONG\r\n", 7, 0); 
  pthread_mutex_unlock(&mutex);
  return 0;
}

int cleanup_commands_hashmap() {
  kh_destroy(str, hashmap);
  return 0;
}
