#include "hashmap.h"
#include "commands_functions.h"

typedef int (*command_function_t)(string_ptr_t argv[], size_t argc, int client_fd);

hashmap_t* commands_map = NULL;

int init_commands_map() {

  printf("initializing commands map\n");

  commands_map = new_hashmap();
  // since the values are functions, we don't want to free them.
  commands_map->free_value = NULL;

  hashmap_set(commands_map, "PING", ping);
  hashmap_set(commands_map, "GET", kv_get);
  hashmap_set(commands_map, "SET", kv_set);

  return 0;
}

void clean_commands_map() {
  // Value of this hashmap is functions.
  free_hashmap(commands_map);
}

void command_run(string_ptr_t* command_string_array, int n, int client_fd){
  char *key = command_string_array[0]->buffer;
  command_function_t function = NULL;
  hashmap_get(commands_map, key, (void**)&function);
  string_ptr_t *argv = NULL;

  if(!function){
    send(client_fd, "-command not found\r\n", 20, 0);
    return;
  }

  if(n > 1){
    argv = &command_string_array[1];
  }

  function(argv, n - 1, client_fd);

  return;
}

