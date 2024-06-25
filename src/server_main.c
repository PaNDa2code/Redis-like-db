#include "network_utils.h"
#include <pthread.h>

int main() {
  int server_fd = socket(AF_INET, SOCK_STREAM, 0);

  CHECK_ERROR(server_fd, == -1, "socket");

  int reuse = 1;
  CHECK_ERROR(
      setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)),
      < 0, "socket");

  struct sockaddr_in serv_addr = {
      .sin_family = AF_INET,
      .sin_port = htons(6379),
      .sin_addr.s_addr = htonl(INADDR_ANY),
  };

  CHECK_ERROR(bind(server_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)),
              != 0, "bind");

  printf("[!] Waiting for client to connect\n");

  CHECK_ERROR(listen(server_fd, THREAD_COUNT), != 0, "listen");

  pthread_t thread_pool[THREAD_COUNT];

  for (int i = 0; i < THREAD_COUNT; ++i){
    pthread_t thread;
    pthread_create(&thread, NULL, accept_connection_thread, &server_fd);
    thread_pool[i] = thread;
  }

  for (int i = 0; i < THREAD_COUNT; ++i) {
    pthread_join(thread_pool[i], NULL);
  }

  close(server_fd);

  return 0;
}
