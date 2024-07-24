#pragma once
#include "includes.h"
#include "data_structures.h"

#define SEND_C_STRING(s)                                                       \
  {                                                                            \
    char m[] = s;                                                              \
    send(client_fd, m, sizeof(m) - 1, 0);                                      \
  }

#define SEND_STRING_T(s)                                                       \
  { send(client_fd, s->buffer, s->length, 0); }


int string_to_bulkstring(string_ptr_t string, char** buffer, size_t *n);
