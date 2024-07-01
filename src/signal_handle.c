#include "includes.h"
#include "network.h"
#include <sys/socket.h>

extern bool keep_running;
extern int server_fd;
extern pthread_t *thread_pool;
extern int clients_fds[MAX_CLIENTS];

void signal_handler(int sigint) {

  char ask[] = "\nDo you close the server(y/n)? ";

  write(STDOUT_FILENO, ask, sizeof(ask));
  char answer[2];
  read(STDIN_FILENO, answer, 2);

  if (*answer != 'y') {
    return;
  }

  
  keep_running = false;

  for(int i = 0; i < MAX_CLIENTS; ++i){
    if (clients_fds[i])
      shutdown(clients_fds[i], SHUT_RD);
  }

  shutdown(server_fd, SHUT_RD);
}
