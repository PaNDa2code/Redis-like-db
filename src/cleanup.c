#include "network_utils.h"
#include <signal.h>
#include <stdio.h>

extern int cleanup_commands_hashmap();
extern pthread_t thread_pool[THREAD_COUNT];

extern int keep_threads_running;
extern int server_fd;

void signal_handler(int sig) {
  printf("[!] Cleanning up\n");
  cleanup_commands_hashmap();

  if (sig == SIGINT || sig == SIGTERM) {
    keep_threads_running = 0;
    close(server_fd);
  }
  /*for(int i = 0; i < THREAD_COUNT; ++i) {*/
  /*  pthread_detach(thread_pool[i]);*/
  /*}*/

  /*signal(sig, SIG_DFL);*/
  /*raise(sig);*/
}
