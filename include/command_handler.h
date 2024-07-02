#pragma once
#include "includes.h"
#include "parse_command.h"

#define RES_ERROR(mes, fd)                                                     \
  {                                                                            \
    char res[] = mes;                                                          \
    send(fd, res, strlen(res), 0);                                             \
  }

int init_commands_map();
void command_run(string_ptr_t* command_string_array, int n, int client_fd);
