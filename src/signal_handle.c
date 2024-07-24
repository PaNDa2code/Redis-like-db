#include "includes.h"
#include "network.h"
#include "dynamic_array.h"
#include <stdint.h>
#include <sys/socket.h>

extern bool keep_running;
extern int server_fd;
/*extern uint32_t maxclients;*/
/*extern dynamic_array(pthread_t) *thread_pool;*/
extern dynamic_array(int) *clients_fds;
extern int cleanup_kv_hashmap();
extern void clean_commands_map();

void signal_handler(int sigint) {

  char ask[] = "\n[!] Are you sure you want close the server(y/n)? ";

  write(STDOUT_FILENO, ask, sizeof(ask));
  char answer[2];
  read(STDIN_FILENO, answer, 2);

  if (*answer != 'y') {
    return;
  }

  keep_running = false;

  dynamic_array_for_each(clients_fds, clients_fd) {
    shutdown(clients_fd, SHUT_RD);
    close(clients_fd);
  }

  free_dynamic_array(clients_fds);
  /*free_dynamic_array(thread_pool);*/

  cleanup_kv_hashmap();
  clean_commands_map();
  shutdown(server_fd, SHUT_RD);
}
