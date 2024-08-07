#include "commands_functions.h"
#include "data_structures.h"
#include "dynamic_array.h"
#include "hashmap.h"

typedef int (*command_function_t)(string_tokens_t *, int client_fd);

hashmap_t *commands_map = NULL;

int init_commands_map() {

  printf("initializing commands map\n");

  commands_map = new_hashmap();
  // since the values are functions, we don't want to free them.
  commands_map->free_value = NULL;
  commands_map->free_key = NULL;

  hashmap_set(commands_map, "PING", ping);
  hashmap_set(commands_map, "GET", kv_get);
  hashmap_set(commands_map, "SET", kv_set);

  return 0;
}

void clean_commands_map() {
  // Value of this hashmap is functions.
  free_hashmap(commands_map);
}

void command_run(string_tokens_t *command_tokens, int client_fd) {
  char *key = command_tokens->tokens[0];
  command_function_t function = NULL;
  if (hashmap_get(commands_map, key, (void **)&function) == RE_SUCCESS) {
    function(command_tokens, client_fd);
  } else {
    send(client_fd, "-Command not found\r\n", 20, 0);
    return;
  }
  return;
}
