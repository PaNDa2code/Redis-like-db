
#include <time.h>
#define LOG(fmt, ...)                                                          \
  ({                                                                           \
    char buffer[20];                                                           \
    time_t now = time(NULL);                                                   \
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localtime(&now));    \
    printf("[*] %s - " fmt "\n", buffer, ##__VA_ARGS__);                            \
  })
