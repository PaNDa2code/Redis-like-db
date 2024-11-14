#include "rdb_parse.h"
#include "dynamic_array.h"
#include "hashmap.h"
#include "data_structures.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

dynamic_array(string_container_t*) * parse_output_ptr = NULL;

int parse_RDB_file(char *file_name) {
  unsigned char buffer[1024] = {0};
  size_t readed_bytes = 0;
  size_t i = 0;
  FILE *file = fopen(file_name, "rb");
  printf("file_name: %s", file_name);

  if (file == NULL) {
    perror("fopen");
    return -1;
  }

  // Read and check the RDB header
  readed_bytes = fread(buffer, sizeof(RDB_header), 1, file);
  if (readed_bytes != 1) {
    perror("fread");
    fclose(file);
    return -1;
  }

  if (memcmp(buffer, RDB_HEADER_MAGIC, sizeof(RDB_HEADER_MAGIC) - 1)) {
    fprintf(stderr, "file \"%s\" is corrupted or not RDB format\n", file_name);
    fclose(file);
    return -1;
  }

  dynamic_array_init(&parse_output_ptr);
  /*parse_output_ptr->value_free_function = (void(*))free;*/

  // Look for the start of database section (FE 00)
  while ((readed_bytes = fread(buffer, 1, sizeof(buffer), file)) > 0) {
    for (i = 0; i < readed_bytes; i++) {
      if (buffer[i] == 0xFE && buffer[i + 1] == 0x00) {
        printf("Found database section at offset %zu\n",
               ftell(file) - readed_bytes + i);
        i += 4; // Skip the 0x00 byte
        goto L0;
      }
    }
  }

  // Start parsing the database section
  while ((readed_bytes = fread(buffer, 1, sizeof(buffer), file)) > 0) {
    for (i = 0; i < readed_bytes;) {
      if (buffer[i] == 0xFF)
        return 0;
      // Check for expiry information
      if (buffer[i] == 0xFD || buffer[i] == 0xFC) {
        /*printf("Found expire info, type: %s\n",*/
        /*buffer[i] == 0xFD ? "seconds" : "milliseconds");*/

        if (buffer[i] == 0xFD) {
          // Expiry in seconds (4 bytes)
          i++;
          unsigned int expire_time = *((unsigned int *)(buffer + i));
          /*printf("Expire timestamp (seconds): %u\n", expire_time);*/
          i += 4;
        } else if (buffer[i] == 0xFC) {
          // Expiry in milliseconds (8 bytes)
          i++;
          unsigned long expire_time = *((unsigned long *)(buffer + i));
          /*printf("Expire timestamp (milliseconds): %lu\n", expire_time);*/
          i += 8;
        }
        continue;
      }
    L0:
      // Value type and key parsing
      i++;
      unsigned char value_type = buffer[i];
      /*printf("Value type: %02X\n", value_type);*/

      if (value_type == 0x00) {
        // String key-value pair
        unsigned char key_len = buffer[i++];

        char *key = malloc(key_len + 1);
        memcpy(key, buffer + i, key_len);
        key[key_len] = '\0';
        i += key_len;

        unsigned char value_len = buffer[i++];

        /*char *value = malloc(value_len + 1);*/
        /*memcpy(value, buffer + i, value_len);*/
        /*value[value_len] = '\0';*/
        /*i += value_len;*/

        string_container_t* str = malloc(sizeof(string_container_t));
        memset(str, 0, sizeof(string_container_t));

        str->string = malloc(sizeof(string_t) + value_len);
        memcpy(str->string->buffer, buffer + i, value_len);
        str->key = key;

        dynamic_array_push(parse_output_ptr, str);
      }
    }
  }

  fclose(file);
  return 0;
}
