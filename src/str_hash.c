#include <stdint.h>
#include "hashmap.h"

uint64_t str_hash(char* string) {
  uint64_t hash = 0, p = 31, m = UINT64_MAX, p_pow = 1;

  while(0 != *string){
    hash = (hash + (*string - 'a' + 1) * p_pow) % m;
    p_pow = (p_pow * p) % m;
    string++;
  }

  return hash;
}
