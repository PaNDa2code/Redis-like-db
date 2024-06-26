#pragma once

#include <arpa/inet.h>
#include <asm-generic/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <signal.h>

#define CHECK_ERROR(call, cmb, msg)                                            \
  if ((call)cmb) {                                                             \
    perror("[-] " msg);                                                        \
    return -1;                                                                 \
  }

void accept_connection_thread(int *server_fd);

#define BUFFER_SIZE 1024
#define THREAD_COUNT 2

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/select.h>


#pragma end
