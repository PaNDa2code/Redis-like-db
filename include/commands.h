#pragma once
#include "headers.h"

int echo(pesp_array *input, int responce_fd);
int ping(pesp_array *input, int responce_fd);
int command(pesp_array *input, int responce_fd);
int set(pesp_array *input, int responce_fd);
int get(pesp_array *input, int responce_fd);

typedef int (*command_func_t)(pesp_array *, int);
#pragma end
