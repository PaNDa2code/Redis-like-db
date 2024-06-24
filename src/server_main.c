#include "network_utils.h"

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

  CHECK_ERROR(listen(server_fd, 5), != 0, "listen");

  pthread_mutex_t accept_mutex;
  pthread_mutex_init(&accept_mutex, NULL);

  pthread_t thread_pool[THREAD_COUNT];

  for (int i = 0; i < THREAD_COUNT; ++i){
    pthread_t thread;
    pthread_attr_t thread_attr;
    pthread_attr_init(&thread_attr);
    pthread_attr_setscope(&thread_attr, PTHREAD_SCOPE_SYSTEM);
    pthread_create(&thread, &thread_attr, accept_connection_thread, &server_fd);
    thread_pool[i] = thread;
    pthread_attr_destroy(&thread_attr);
  }

  for (int i = 0; i < THREAD_COUNT; ++i) {
    pthread_join(thread_pool[i], NULL);
  }

  close(server_fd);

  return 0;
}
