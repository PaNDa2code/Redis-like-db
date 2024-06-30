#include "headers.h"

pthread_t thread_pool[THREAD_COUNT];

int keep_threads_running = 1;
int server_fd = 0;

int port_number = DEFUALT_PORT;

extern void signal_handler(int sig);

static inline void handle_args(int argc, char *argv[]) {
  int opt;
  int option_index = 0;

  static struct option long_options[] = {
    {"port", required_argument, 0, 'p'},
    {NULL, 0, 0, 0}
  };

  while ((opt = getopt_long(argc, argv, "p:", long_options, &option_index)) != -1) {
    switch (opt) {
      case 'p':
        port_number = atoi(optarg);
        break;
      case '?':
        break;
      default:
        fprintf(stderr, "Unkown option\n");
        break;
    }
  }
}

int main(int argc, char* argv[]) {
  handle_args(argc, argv);
  signal(SIGINT, signal_handler);
  signal(SIGTERM, signal_handler);

  struct timeval time_out;
  time_out.tv_sec = 1;
  time_out.tv_usec = 0;

  server_fd = socket(AF_INET, SOCK_STREAM, 0);

  CHECK_ERROR(server_fd, == -1, "socket");

  int reuse = 1;
  CHECK_ERROR( setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)), < 0, "socket");

  struct sockaddr_in serv_addr = {
      .sin_family = AF_INET,
      .sin_port = htons(port_number),
      .sin_addr.s_addr = htonl(INADDR_ANY),
  };

  printf("[!] Initialinzing data hashmap\n");

  init_database();

  CHECK_ERROR(bind(server_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)), != 0, "bind");

  printf("[!] listening for a connection on port: %u\n",
         ntohs(serv_addr.sin_port));

  CHECK_ERROR(listen(server_fd, THREAD_COUNT), != 0, "listen");

  for (int i = 0; i < THREAD_COUNT; ++i) {
    pthread_t thread;
    pthread_create(&thread, NULL, (void *)accept_connection_thread, &server_fd);
    thread_pool[i] = thread;
  }

  for (int i = 0; i < THREAD_COUNT; ++i) {
    pthread_join(thread_pool[i], NULL);
  }

  close(server_fd);
  return 0;
}
