#pragma once
#include "includes.h"
#include "data_structures.h"

enum {
  RE_SUCCESS,
  RE_FAILED,
  RE_KEY_NOT_FOUND,
  RE_KEY_EXISTS,
  RE_OUT_OF_MEMORY,
  RE_INVALID_ARGS
};

int kv_get(void* command_dinamic_array, int client_fd);
/*int kv_get(string_ptr_t argv[], size_t argc, int client_fd);*/
int kv_set(void* command_dinamic_array, int client_fd);
/*int kv_set(string_ptr_t argv[], size_t argc, int client_fd);*/
int ping(void* command_dinamic_array, int client_fd);
/*int ping(string_ptr_t argv[], size_t argc, int client_fd);*/

