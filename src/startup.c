#include "command_handler.h"
#include "kv_database.h"
#include "includes.h"
#include "network.h"
#include "dynamic_array.h"

extern dynamic_array(pthread_t)* thread_pool;
extern dynamic_array(int)* clients_fds;

int startup() {
  dynamic_array_init(&thread_pool);
  dynamic_array_init(&clients_fds);

  init_commands_map();
  init_kv_hashmap();
  return 0;
}
