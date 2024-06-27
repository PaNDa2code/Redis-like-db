#include "network_utils.h"
#include "headers.h"

extern int cleanup_commands_hashmap();
extern pthread_t thread_pool[THREAD_COUNT];

extern int keep_threads_running;
extern int server_fd;
extern int client_fd;
volatile sig_atomic_t signal_received = 0;

void signal_handler(int sig) {
  if (signal_received == 1) {
    return;
  }
  signal_received = 1;
  char response;
  write(STDOUT_FILENO, "\nAre you sure you want to close the server (y/n)? ", 50);
  read(STDIN_FILENO, &response, sizeof(response));
  if (response != 'y') {
    signal_received = 0;
    return;
  }
  write(STDOUT_FILENO, "[!] Cleaning up\n", 16);
  cleanup_commands_hashmap();

  if (sig == SIGINT) {
    keep_threads_running = 0;
    if (client_fd != 0)
      shutdown(client_fd, SHUT_RD);
    if (server_fd != 0)
      shutdown(server_fd, SHUT_RD);
  }

  exit(EXIT_SUCCESS); // Or raise(sig) if you want to raise the signal again
                      // after cleanup.
}
