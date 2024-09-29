#pragma once
#include "includes.h"
#include "data_structures.h"

enum {
  RE_SUCCESS,
  RE_FAILED,
  RE_KEY_NOT_FOUND,
  RE_KEY_EXISTS,
  RE_OUT_OF_MEMORY,
  RE_INVALID_ARGS,
  RE_TYPE_MISS_MATCH
};

int kv_get(string_tokens_t *command_tokens, int client_fd);
int kv_set(string_tokens_t *command_tokens, int client_fd);
int ping(string_tokens_t *command_tokens, int client_fd);

