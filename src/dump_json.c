#include "data_structures.h"
#include "hashmap.h"
#include "includes.h"

extern hashmap_t *kv_hashmap;

void format_timespec(FILE *file, struct timespec *t) {
  struct tm *Time = localtime(&t->tv_sec);
  fprintf(file, "%04d-%02d-%02d %02d:%02d:%02d", Time->tm_year + 1900,
          Time->tm_mon + 1, Time->tm_mday, Time->tm_hour, Time->tm_min,
          Time->tm_sec);
};

int dump_data() {
  char *file_name = "backup.json";
  FILE *file = fopen(file_name, "w");

  if (file == NULL) {
    perror("fopen");
    return -1;
  }

  printf("[#] dumping data to \"%s\"\n", file_name);
  fwrite("{\n", 1, 2, file);

  int first = 1;
  char *key;
  string_container_t *value;

  FOR_EACH(kv_hashmap, key, value) {
    if (!first) {
      fwrite(",\n", 1, 2, file);
    }
    first = 0;
    fprintf(file, "  \"%s\": { \"value\": \"%s\"", key, value->string->buffer);

    fprintf(file, ", \"insertionTime\": \"");
    format_timespec(file, &value->insertion_time);
    fprintf(file, "\"");

    if (value->expiry_time.tv_sec) {
      fprintf(file, ", \"expiryTime\": \"");
      format_timespec(file, &value->expiry_time);
      fprintf(file, "\"");
    }

    fprintf(file, " }");
  }
  END_FOR_EACH;

  fwrite("\n}\n", 1, 3, file);
  fclose(file);

  return 0;
}

int load_data(char *file_name) { return 0; };
