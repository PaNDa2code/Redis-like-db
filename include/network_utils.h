#pragma once

#define CHECK_ERROR(call, cmb, msg)                                            \
  if ((call)cmb) {                                                             \
    perror("[-] " msg);                                                        \
    return -1;                                                                 \
  }

void accept_connection_thread(int *server_fd);

#define BUFFER_SIZE 1024*1024
#define THREAD_COUNT 7
#define DEFUALT_PORT 6379
#pragma end
