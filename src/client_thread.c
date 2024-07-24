#include "command_handler.h"
#include "commands_functions.h"
#include "dynamic_array.h"
#include "kv_database.h"
#include "parse_command.h"

extern bool keep_running;
extern uint32_t connected_clients;

#define MAX_BUFFER_SIZE 1024

extern dynamic_array(pthread_t) * thread_pool;
extern dynamic_array(int) * clients_fds;

void *handle_client(void *arg) {
  int client_socket = ((long long)arg);
  char buffer[MAX_BUFFER_SIZE] = {0};
  ssize_t readed_bytes;
  while (keep_running) {
    readed_bytes = read(client_socket, buffer, MAX_BUFFER_SIZE);
    if (readed_bytes < 0) {
      perror("read");
    } else if (readed_bytes == 0) {
      break;
    } else {
      dynamic_array(string_ptr_t) * array;
      size_t n;
      if (parse_command(buffer, (void *)&array) != RE_SUCCESS) {
        char res[] = "-Parsing command failed\r\n";
        send(client_socket, res, sizeof(res) - 1, 0);
      } else {
        command_run((void *)array, client_socket);
        free_dynamic_array(array);
      }
      memset(buffer, 0, MAX_BUFFER_SIZE);
    }
  }

  close(client_socket);
  dynamic_array_find_and_remove(clients_fds, client_socket);
  dynamic_array_find_and_remove(thread_pool, pthread_self());
  connected_clients--;
  return NULL;
}
