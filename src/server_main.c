#include "network_utils.h"
#include <bits/types/struct_timeval.h>
#include <fcntl.h>
#include <pthread.h>

pthread_t thread_pool[THREAD_COUNT];

int keep_threads_running = 1;
int server_fd = 0;
extern void signal_handler(int sig);

int main() {
  signal(SIGINT, signal_handler);
  signal(SIGTERM, signal_handler);

  struct timeval time_out;
  time_out.tv_sec = 1;
  time_out.tv_usec = 0;

  server_fd = socket(AF_INET, SOCK_STREAM, 0);

  /*int flags = fcntl(server_fd, F_GETFL, 0);*/
  /*flags = flags | O_NONBLOCK;*/
  /*CHECK_ERROR(fcntl(server_fd, F_SETFL, flags), != 0, "fcntl");*/


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

  for (int i = 0; i < THREAD_COUNT; ++i) {
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
