#include "headers.h"
#include "network_utils.h"
#include "parser.h"

extern int keep_threads_running;
int client_fd;

void accept_connection_thread(int *pServer_fd) {
  char *buffer = malloc(BUFFER_SIZE);
  struct sockaddr_in client_addr;
  socklen_t client_addr_len = sizeof(client_addr);

  while (keep_threads_running) {
    client_fd = accept(*pServer_fd, (struct sockaddr *)&client_addr, &client_addr_len);
    if (client_fd == -1) {
      if (keep_threads_running)
        perror("accept");
      break;
    }

    char *ip_str = inet_ntoa(client_addr.sin_addr);
    printf("[*] Client connected\t ip: %s\n", ip_str);

    while (keep_threads_running) {
      ssize_t bytes_read = read(client_fd, buffer, BUFFER_SIZE);

      if (bytes_read == -1) {
        perror("read");
        break; // Exit loop on read error
      } else if (bytes_read == 0) {
        printf("[*] Client disconnected\n");
        break; // Exit loop when client disconnects
      }

      /*linkedList_entry_t head;*/
      /*parse(buffer, &head);*/ 
      pesp_array* command_array = NULL;
      parse(buffer, &command_array);

      run_command(command_array, client_fd);
      free_bulk_array(command_array);
      memset(buffer, 0, BUFFER_SIZE); // Clear buffer for next read
    }
  }
  close(client_fd);
  free(buffer);
  return;
}
