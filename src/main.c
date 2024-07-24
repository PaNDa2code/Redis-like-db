#include "client_thread.h"
#include "logger.h"
#include "dynamic_array.h"
#include "includes.h"
#include "network.h"
#include <bits/getopt_core.h>
#include <getopt.h>
#include <pthread.h>
#include <unistd.h>
#include "assci_art.h"

static inline void handler_args(int argc, char *argv[]);

dynamic_array(pthread_t) *thread_pool;
dynamic_array(int) *clients_fds;

uint32_t connected_clients = 0;
uint32_t maxclients = DEFUALT_MAX_CLIENTS;

bool keep_running = true;
int server_fd;
int port_number = DEFUALT_PORT;

void signal_handler(int sigint);

extern int startup();

int main(int argc, char *argv[]) {

  handler_args(argc, argv);

  startup();

  print_assci_art("Redis-Like-db", "PaNDa2code");

  signal(SIGINT, signal_handler);

  pthread_t ptid;

  handler_args(argc, argv);

  int new_socket;
  struct sockaddr_in address;
  socklen_t addr_len = sizeof(address);
  int opt = 1;

  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
    perror("socket");
    return -1;
  }

  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
                 sizeof(opt))) {
    perror("setsockopt");
    return -1;
  }

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(port_number);

  if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    perror("bind");
    return -1;
  }

  if (listen(server_fd, DEFUALT_MAX_CLIENTS) < 0) {
    perror("listen");
    return -1;
  }

  /*printf("Listening on port %d\n", port_number);*/
  LOG("Listening on port %d", port_number);

  while (keep_running) {
    if ((new_socket =
             accept(server_fd, (struct sockaddr *)&address, &addr_len)) < 0) {
      if (!keep_running) {
        break;
      }
      perror("accept");
      continue;
    }

    if (connected_clients == maxclients) {
      char mas[] = "-Server reached max number of clients.\r\n";
      printf("%s", mas + 1);
      send(new_socket, mas, sizeof(mas) - 1, 0);
      close(new_socket);
      continue;
    }

    dynamic_array_push(clients_fds, new_socket);

    void* arg = &new_socket;
    if (pthread_create(&ptid, NULL, handle_client, arg) !=
        0) {
      perror("pthread_create");
      close(new_socket);
    } else {
      LOG("Accepted connection - ip:%s:%d", inet_ntoa(address.sin_addr), address.sin_port);
      dynamic_array_push(thread_pool, ptid);

      // detaching the threads so "The resources of TH will therefore be freed
      // immediately when it terminates" based on man page of pthread_detach
      pthread_detach(ptid);
      connected_clients++;
    }
  }

  // wait for all threads to exit normaly.
  // bad trick, but it works.
  // can't use joining here becase we detached the threads

  while (connected_clients > 0) {
    usleep(250);
  }

  close(server_fd);
}

static inline void handler_args(int argc, char *argv[]) {
  int opt;
  int option_index = 0;

  static struct option long_options[] = {{"port", required_argument, 0, 'p'},
                                         {NULL, 0, 0, 0}};

  while ((opt = getopt_long(argc, argv, "p:", long_options, &option_index)) !=
         -1) {
    switch (opt) {
    case 'p':
      port_number = atoi(optarg);
      break;
    case '?':
      exit(1);
      break;
    default:
      fprintf(stderr, "Unkown option %c\n", opt);
      break;
    }
  }
}
