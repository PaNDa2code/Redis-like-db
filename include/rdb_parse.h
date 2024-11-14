
#define RDB_HEADER_MAGIC "REDIS"

typedef struct {
  char magic[5];
  char version[4];
} RDB_header;


int parse_RDB_file(char *file_name);
