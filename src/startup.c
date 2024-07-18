#include "command_handler.h"
#include "kv_database.h"
#include "includes.h"
#include "network.h"
#include "dynamic_array.h"

extern pthread_t* thread_pool;
extern int* clients_fds;

int startup() {
  thread_pool = dynamic_array(pthread_t, DEFUALT_MAX_CLIENTS);
  clients_fds = dynamic_array(int, DEFUALT_MAX_CLIENTS);

  init_commands_map();
  init_kv_hashmap();
  return 0;
}
