#include "includes.h"

extern bool keep_running;
extern int connected_clients;

void *handle_client(void *new_socket) {
  int client_socket = *(int *)new_socket;
  char buffer[1024] = {0};
  int valread;
  while (keep_running) {
    valread = read(client_socket, buffer, 1024);
    if (valread < 0) {
      perror("read");
    } else if (valread == 0) {
      printf("Client disconnected\n");
      break;
    } else {
      printf("Message from client: %s\n", buffer);
      send(client_socket, buffer, strlen(buffer), 0);
    }
  }

  printf("Closing clinet socket\n");
  close(client_socket);
  *(int*)new_socket = 0;
  connected_clients--;
  return NULL;
}
