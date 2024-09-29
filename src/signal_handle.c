#include "dynamic_array.h"
#include "expiry_cleaner.h"
#include "includes.h"
#include "network.h"
#include <stdint.h>
#include <sys/socket.h>

extern bool keep_running;
extern int server_fd;
extern dynamic_array(pthread_t) * thread_pool;
extern dynamic_array(int) * clients_fds;
extern int cleanup_kv_hashmap();
extern void clean_commands_map();
extern int dump_data();
void signal_handler(int sigint) {

  char ask1[] = "\n[!] Are you sure you want close the server(y/n)? ";

  write(STDOUT_FILENO, ask1, sizeof(ask1));
  char answer[2];
  read(STDIN_FILENO, answer, 2);

  if (*answer != 'y') {
    return;
  }

  char ask2[] = "\n[!] Do you like to dump memory data(y/n)? ";
  write(STDOUT_FILENO, ask2, sizeof(ask2));
  read(STDIN_FILENO, answer, 2);

  keep_running = false;

  dynamic_array_for_each(clients_fds, clients_fd) {
    shutdown(clients_fd, SHUT_RD);
    close(clients_fd);
  }

  expiry_cleanup_exit();

  if (*answer == 'y') {
    dump_data();
  }

  cleanup_kv_hashmap();
  clean_commands_map();
  shutdown(server_fd, SHUT_RD);
}
