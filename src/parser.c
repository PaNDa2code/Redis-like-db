#include "parser.h"
#include "network_utils.h"

void parse(char* input, void* head) {
  linkedList_t* perv_node = head;
  perv_node->next_node = NULL;
  perv_node->prev_node = NULL;

  input++; // Skip '*'
  int bulk_count = atoi(input);
  while(*input != '\n') input++;
  input++; // Skip '\n'
  for(int i = 0; i < bulk_count; ++i){
    input++; // Skip '$'
    int bulk_size = atoi(input);
    while(*input != '\n') input++;
    input++; // Skip '\n'

    linkedList_t* current_node = malloc(sizeof(linkedList_t) + bulk_size + 1);
    current_node->length = bulk_size;

    perv_node->next_node = current_node;
    current_node->prev_node = perv_node;
    memcpy(current_node->buffer, input, bulk_size);
    current_node->buffer[bulk_size] = 0;
    while(*input != '$' && *input != 0) input++;
    perv_node = current_node;
  }
}

void free_linked_list(linkedList_t* head){
  linkedList_t* current = head->next_node;
  linkedList_t* next;
  while(current->next_node != NULL) {
    next = current->next_node;
    free(current);
    current = next;
  }
  free(current);
}

#ifdef TEST_PARSER
int main() {
  char input[] = "*2\r\n$4\r\nECHO\r\n$11\r\nHelloWorld!\r\n";
  linkedList_t Entry = {0};
  parse(input, &Entry);
  printf("%s ", Entry.next_node->buffer);
  printf("%s", Entry.next_node->next_node->buffer);
  free_linked_list(&Entry);
  return 0;
}
#endif
