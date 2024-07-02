
#include "parse_command.h"
#include "command_handler.h"
#include "kv_database.h"

extern bool keep_running;
extern int connected_clients;

#define TCP_MMS 1460
#define MAX_BUFFER_SIZE TCP_MMS

void *handle_client(void *new_socket) {
  int client_socket = *(int *)new_socket;
  char buffer[MAX_BUFFER_SIZE] = {0};
  ssize_t readed_bytes;
  while (keep_running) {
    readed_bytes = read(client_socket, buffer, MAX_BUFFER_SIZE);
    if (readed_bytes < 0) {
      perror("read");
    } else if (readed_bytes == 0) {
      printf("Client disconnected\n");
      break;
    } else {
      /*printf("Message from client: \n%s\n", buffer);*/
      string_ptr_t *arr;
      size_t n;
      parse_command(buffer, &arr, &n);
      command_run(arr, n, client_socket);
      free_string_array(arr, n);
      memset(buffer, 0, MAX_BUFFER_SIZE);
    }
  }

  close(client_socket);
  *(int*)new_socket = 0;
  connected_clients--;
  return NULL;
}
