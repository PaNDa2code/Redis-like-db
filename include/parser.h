#pragma once

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
} linkedList_t;



void parse(char* input, void* linkedList);

int run_command(linkedList_t* command_head, int responce_fd);

#pragma end
