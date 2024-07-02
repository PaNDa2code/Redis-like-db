#include "includes.h"
#include "data_structures.h"

int init_kv_hashmap();

int insert_kv(char* key, string_ptr_t value, uint64_t expiry_ms);

int lookup_kv(char* key, string_ptr_t* value);
