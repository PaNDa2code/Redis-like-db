#include "parser.h"
#include "headers.h"
#include "network_utils.h"

void parse(char *input, linkedList_entry_t* linkedList) {
  *linkedList = NULL;
  linkedList_node_t *prev_node = NULL;
  input++;
  int array_length = atoi(input);
  while (*(input - 1) != '$') input++;
  for (int i = 0; i < array_length; ++i) {
    
    int bulk_size = atoi(input);

    while (*(input - 1) != '\n') input++;

    linkedList_node_t *current_node = malloc(sizeof(linkedList_node_t) + bulk_size + 1);

    memset(current_node, 0, sizeof(linkedList_node_t) + bulk_size + 1);

    if (*linkedList == NULL) { *linkedList = current_node; }

    current_node->length = bulk_size;
    current_node->prev_node = (prev_node != NULL)? prev_node : NULL;
    memcpy(current_node->buffer, input, bulk_size);
    current_node->buffer[bulk_size] = 0;
    if (prev_node != NULL) {
      prev_node->next_node = current_node;
    }
    prev_node = current_node;

    // don't judge the syntex.
    // i am using gdb ^_^
    while (*input != 0 && *(input - 1) != '$') input++;
  }
}

void free_linked_list(linkedList_entry_t head) {
  while (head->next_node != NULL) {
    head = head->next_node;
    free(head->prev_node);
  }
  free(head);
}

#ifdef TEST_PARSER
int main() {
  /*char input[] = "*2\r\n$4\r\nECHO\r\n$11\r\nHelloWorld!\r\n";*/
  char input[] = "*1\r\n$4\r\nPING\r\n";
  linkedList_entry_t Entry;
  parse(input, &Entry);
  printf("%s ", Entry->buffer);
  /*printf("%s", Entry->next_node->buffer);*/
  free_linked_list(Entry);
  return 0;
}
#endif
