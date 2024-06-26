#include "network_utils.h"
#include <signal.h>
#include <stdio.h>
#include <sys/socket.h>

extern int cleanup_commands_hashmap();
extern pthread_t thread_pool[THREAD_COUNT];

extern int keep_threads_running;
extern int server_fd;
extern int client_fd;

void signal_handler(int sig) {
  
  char m = 'd';
  printf("\nAre you sure you want to close the server (y/n) ? ");
  scanf("%c", &m);

  if (m != 'y') {
    return;
  }

  printf("[!] Cleanning up\n");
  cleanup_commands_hashmap();

  if (sig == SIGINT || sig == SIGTERM) {
    keep_threads_running = 0;
    /*if(client_fd != 0)*/
      shutdown(client_fd, SHUT_RD);
    /*if(server_fd != 0)*/
      shutdown(server_fd, SHUT_RD);
  }

  /*signal(sig, SIG_DFL);*/
  /*raise(sig);*/
}
