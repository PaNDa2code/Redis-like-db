#pragma once
#include <sys/types.h>

#define INFINITE_TIME 0xFFFFFFFFFFFFFFFFull

typedef u_int64_t TimeMillis;

#define ADD_KEYVALUE(key, value, hm_name, k, absent)                           \
  {                                                                            \
    k = kh_put(str, hm_name, key, &absent);                                    \
    kh_value(hm_name, k) = value;                                              \
  }

#define UNIX_TIME_MILLISECONDS()                                               \
  ({                                                                           \
    struct timespec ts;                                                        \
    clock_gettime(CLOCK_REALTIME, &ts);                                        \
    (u_int64_t)(ts.tv_sec) * 1000 + ts.tv_nsec / 1000000;                      \
  })

#pragma end
