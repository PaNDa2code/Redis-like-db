#include "command_handler.h"
#include "kv_database.h"
#include "includes.h"
#include "network.h"
#include "dynamic_array.h"
#include "expiry_cleaner.h"

extern dynamic_array(pthread_t)* thread_pool;
extern dynamic_array(int)* clients_fds;
extern pthread_t expiry_cleanup_ptid;
extern dynamic_array(string_container_t *) * timed_data_queue;

int startup() {
  dynamic_array_init(&thread_pool);
  thread_pool->value_free_function = NULL;
  dynamic_array_init(&clients_fds);
  clients_fds->value_free_function = NULL;
  dynamic_array_init(&timed_data_queue);
  timed_data_queue->value_free_function = NULL;

  pthread_create(&expiry_cleanup_ptid, NULL, (void *(*)(void *))expiry_cleanup, NULL);
  init_commands_map();
  init_kv_hashmap();
  return 0;
}
