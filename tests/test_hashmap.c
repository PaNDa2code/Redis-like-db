#include "hashmap.h" 
#include <stdlib.h>
#include <string.h>

int main() {
  hashmap_t *hashmap;
  size_t i;
  char buffer[10];
  char *key = buffer, *value = "Value";

  hashmap = new_n_hashmap(1);
  hashmap->free_value = NULL;

  for(i = 0; i < 100; ++i){
    snprintf(key, 10, "key%zu", i);
    hashmap_set(hashmap, strdup(key), value);
  }

  hashmap_resize(&hashmap, 100);

  FOR_EACH(hashmap, key, value) {
    printf("%s:%s\n", key, value);
  } END_FOR_EACH;


  FOR_EACH(hashmap, key, value) {
    hashmap_del(hashmap, key);
  } END_FOR_EACH;

  for(i = 0; i < 100; ++i){
    snprintf(key, 10, "key%zu", i);
    hashmap_set(hashmap, strdup(key), value);
  }

  /*free(hashmap);*/
  free_hashmap(hashmap);
  return 0;
}
