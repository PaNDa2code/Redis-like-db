#include "includes.h"
#include "data_structures.h"

int init_kv_hashmap();

int insert_kv(char* key, char *value, uint64_t expiry_ms);

int lookup_kv(char* key, string_ptr_t* value);

int lookup_kv_container(char *key, string_container_t **container);

int delete_kv(char* key);

int check_kv(char* key);
