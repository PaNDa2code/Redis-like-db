#include "network_utils.h"
#include "parser.h"

void *accept_connection_thread(void *pServer_fd) {
  char buffer[BUFFER_SIZE] = {0};
  struct sockaddr_in client_addr;
  socklen_t client_addr_len = sizeof(client_addr);
  while (1) {
    int client_fd = accept(*(int *)pServer_fd, (struct sockaddr *)&client_addr, &client_addr_len);
    if (client_fd == -1) {
      perror("accept");
      continue;
    }
    char *ip_str = inet_ntoa(client_addr.sin_addr);
    printf("[*] Client connected\t ip: %s\n", ip_str);
    while (1) {
      ssize_t bytes_read = read(client_fd, buffer, BUFFER_SIZE);
      if (bytes_read == -1) {
        perror("read");
        break;
      }
      else if (bytes_read == 0) {
        printf("[*] client disconnected\n");
        break;
      }
      linkedList_t head;
      parse(buffer, &head);
      if(!run_command(&head, client_fd))
        send(client_fd, "$0\r\n\r\n", 6, 0);
      memset(buffer, 0, BUFFER_SIZE);
    }
    close(client_fd);
  }
  return NULL;
}
