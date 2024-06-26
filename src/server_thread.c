#include "network_utils.h"
#include "parser.h"
#include <signal.h>
#include <sys/select.h>

extern int keep_threads_running;

void accept_connection_thread(int *pServer_fd) {
  char buffer[BUFFER_SIZE] = {0};
  struct sockaddr_in client_addr;
  socklen_t client_addr_len = sizeof(client_addr);

  fd_set readfds;
  struct timeval timeout;
  while (keep_threads_running) {
    FD_ZERO(&readfds);
    FD_SET(*pServer_fd, &readfds);
    timeout.tv_sec = 3;
    timeout.tv_usec = 0;

    int select_ret = select((*pServer_fd) + 1, &readfds, NULL, NULL, &timeout);
    int client_fd;
    if (select_ret == -1) {
      perror("select");
      return;
    } else if (select_ret == 0) {
      printf("[!] timeout accurred!\n");
      continue;
    }

    if (FD_ISSET(*pServer_fd, &readfds)) {
      client_fd = accept(*pServer_fd, (struct sockaddr *)&client_addr,
                         &client_addr_len);
      if (client_fd == -1) {
        perror("accept");
        return;
      }
    }

    char *ip_str = inet_ntoa(client_addr.sin_addr);
    printf("[*] Client connected\t ip: %s\n", ip_str);

    while (keep_threads_running) {
      FD_ZERO(&readfds);
      FD_SET(client_fd, &readfds); // Watch client_fd for incoming data
      timeout.tv_sec = 1;
      timeout.tv_usec = 0;

      int select_ret = select(client_fd + 1, &readfds, NULL, NULL, &timeout);

      if (select_ret == -1) {
        perror("select");
        break; // Exit loop on select error
      } else if (select_ret == 0) {
        printf("[!] Timeout occurred!\n");
        continue; // Retry select on timeout
      }

      if (FD_ISSET(client_fd, &readfds)) {
        ssize_t bytes_read = read(client_fd, buffer, BUFFER_SIZE);

        if (bytes_read == -1) {
          perror("read");
          break; // Exit loop on read error
        } else if (bytes_read == 0) {
          printf("[*] Client disconnected\n");
          break; // Exit loop when client disconnects
        }

        linkedList_t head;
        parse(buffer, &head);
        run_command(&head, client_fd);
        memset(buffer, 0, BUFFER_SIZE); // Clear buffer for next read
      }
    }
    close(client_fd);
  }
  return;
}
