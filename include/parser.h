#pragma once
#include "PESP_formatter.h"

typedef enum {
  COMMAND_PING,
} COMMANDS;

typedef enum {
  RESP_INT    = ':',
  RESP_STRING = '+',
  RESP_BULK   = '$',
  RESP_ARRAY  = '*',
  RESP_ERROR  = '-'
} RESP;

typedef struct _linked_list {
  struct _linked_list* next_node;
  struct _linked_list* prev_node;
  unsigned long length;
  char buffer[1];
} linkedList_node_t;

typedef linkedList_node_t* linkedList_entry_t;



void parse(char *input, pesp_array **command_array);

void free_bulk_array(pesp_array *bulk_array);

int run_command(pesp_array* command_head, int responce_fd);

#pragma end
