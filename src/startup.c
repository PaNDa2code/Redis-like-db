#include "command_handler.h"
#include "kv_database.h"
#include "includes.h"


int startup(){
  printf("Starting up\n");
  init_commands_map();
  init_kv_hashmap();
  return 0;
}
