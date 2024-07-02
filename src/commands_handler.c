#include "includes.h"
#include "parse_command.h"
#include "commands_functions.h"

typedef int (*command_function_t)(string_ptr_t argv[], size_t argc, int client_fd);

map(char *, command_function_t) commands_map;

int init_commands_map() {

  printf("initializing commands map\n");

  init(&commands_map);

  insert(&commands_map, "PING", ping);
  insert(&commands_map, "GET", kv_get);
  insert(&commands_map, "SET", kv_set);
  return 0;
}

void clean_commands_map() {
  cleanup(&commands_map);
}

void command_run(string_ptr_t* command_string_array, int n, int client_fd){
  char *key = command_string_array[0]->buffer;
  void *val = get(&commands_map, key);
  string_ptr_t *argv = NULL;

  if(!val){
    send(client_fd, "-command not found\r\n", 20, 0);
    return;
  }

  if(n > 1){
    argv = &command_string_array[1];
  }

  (*(command_function_t*)val)(argv, n - 1, client_fd);

  return;
}

