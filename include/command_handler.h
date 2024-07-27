#pragma once
#include "data_structures.h"
#include "dynamic_array.h"
#include "includes.h"
#include "parse_command.h"

#define RES_ERROR(mes, fd)                                                     \
  {                                                                            \
    char res[] = mes;                                                          \
    send(fd, res, strlen(res), 0);                                             \
  }

int init_commands_map();
void command_run(string_tokens_t* command_tokens, int client_fd);
