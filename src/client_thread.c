
#include "command_handler.h"
#include "dynamic_array.h"
#include "kv_database.h"
#include "parse_command.h"
#include <stdint.h>

extern bool keep_running;
extern uint32_t connected_clients;

#define MAX_BUFFER_SIZE 1024

extern dynamic_array(pthread_t)* thread_pool;
extern dynamic_array(int)* clients_fds;

void *handle_client(void *arg) {
  int client_socket = *((int *)arg);
  char buffer[MAX_BUFFER_SIZE] = {0};
  ssize_t readed_bytes;
  while (keep_running) {
    readed_bytes = read(client_socket, buffer, MAX_BUFFER_SIZE);
    if (readed_bytes < 0) {
      perror("read");
    } else if (readed_bytes == 0) {
      break;
    } else {
      string_ptr_t *arr;
      size_t n;
      parse_command(buffer, &arr, &n);
      command_run(arr, n, client_socket);
      free_string_array(arr, n);
      memset(buffer, 0, MAX_BUFFER_SIZE);
    }
  }

  close(client_socket);
  dynamic_array_find_and_remove(clients_fds, client_socket);
  connected_clients--;
  return NULL;
}
