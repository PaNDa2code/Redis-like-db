#include "PESP_formatter.h"
#include "database.h"
#include "headers.h"
#include "khash.h"
#include "parser.h"
#include <stdio.h>

typedef int (*command_func_t)(pesp_array *, int);

int echo(pesp_array *input, int responce_fd);
int ping(pesp_array *input, int responce_fd);
int command(pesp_array *input, int responce_fd);
int set(pesp_array *input, int responce_fd);
int get(pesp_array *input, int responce_fd);

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t write_cond = PTHREAD_COND_INITIALIZER;
int is_writing = 0;

KHASH_MAP_INIT_STR(str, command_func_t)

khash_t(str) * hashmap;
int hash_is_set = 0;

void upper(char *str) {
  while (*str != 0) {
    *str = toupper(*str);
    str++;
  }
}

int init_commands_hashmap() {
  printf("[i] Allocating commands hashmap........\n");
  hashmap = kh_init(str);
  khint_t k;
  int absent;

  ADD_KEYVALUE("PING", ping, hashmap, k, absent);
  ADD_KEYVALUE("ECHO", echo, hashmap, k, absent);
  ADD_KEYVALUE("COMMAND", command, hashmap, k, absent);
  ADD_KEYVALUE("SET", set, hashmap, k, absent);
  ADD_KEYVALUE("GET", get, hashmap, k, absent);
  hash_is_set = 1;
  printf("[*] Done\n");
  return 0;
};

int run_command(pesp_array *input, int responce_fd) {
  if (!hash_is_set)
    init_commands_hashmap();

  pesp_bulk_string *first_string = input->List[0].data_buffer;
  upper(first_string->Buffer);

  int k = kh_get(str, hashmap, first_string->Buffer);

  if (kh_exist(hashmap, k)) {
    kh_value(hashmap, k)(input, responce_fd);
  } else {
    char buffer[first_string->Length + 30];
    sprintf(buffer, "-ERR unknown command \"%s\"\r\n", first_string->Buffer);
    send(responce_fd, buffer, strlen(buffer), 0);
  }

  return 0;
}

int echo(pesp_array *input, int responce_fd) {
  if (input->Length <= 1) {
    send(responce_fd, "$0\r\n\r\n", 6, 0);
    return 0;
  }
  pesp_bulk_string *bulk_string = input->List[1].data_buffer;
  char *message;
  size_t size;
  PESP_format(&bulk_string->header, &message, &size);
  send(responce_fd, message, size - 1, 0);
  free(message);
  return 0;
}

int ping(pesp_array *input, int responce_fd) {
  send(responce_fd, "+PONG\r\n", 7, 0);
  return 0;
}

int command(pesp_array *input, int responce_fd) {
  send(responce_fd, "$0\r\n\r\n", 6, 0);
  return 0;
}

int set(pesp_array *input, int responce_fd) {
  pthread_mutex_lock(&mutex);
  is_writing = 1;
  pthread_cond_broadcast(&write_cond);
  is_writing = 0;

  /*therads are locked hear*/

  if (set_value_by_key(input->List[1].data_buffer, input->List[2].data_buffer,
                       0) == 0) {
    send(responce_fd, "+OK\r\n", 5, 0);
  } else {
    send(responce_fd, "-Error\r\n", 8, 0);
  };

  pthread_mutex_unlock(&mutex);
  return 0;
}

int get(pesp_array *input, int responce_fd) {
  pthread_mutex_lock(&mutex);
  while (is_writing) {
    pthread_cond_wait(&write_cond, &mutex);
  }
  pthread_mutex_unlock(&mutex);
  pesp_bulk_string *value;
  if (get_value_by_key(input->List[1].data_buffer, &value) == 0) {
    char *message;
    size_t size;
    PESP_format(&value->header, &message, &size);
    send(responce_fd, message, size - 1, 0);
  } else {
    send(responce_fd, "$-1\r\n", 5, 0);
  }
  return 0;
}

int cleanup_commands_hashmap() {
  kh_destroy(str, hashmap);
  return 0;
}
