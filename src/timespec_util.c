#include "timespec_util.h"
#include <time.h>

enum RELATIONS cmpr_timestamp(struct timespec *t1, struct timespec *t2) {

  if (t1->tv_sec > t2->tv_sec || (t1->tv_sec == t2->tv_sec && t1->tv_nsec > t2->tv_nsec))
    return GREATER_THAN;

  else if (t1->tv_sec < t2->tv_sec || (t1->tv_sec == t2->tv_sec && t1->tv_nsec < t2->tv_nsec))
      return LESS_THAN;

  else if (t1->tv_sec == t2->tv_sec && t1->tv_nsec == t2->tv_nsec)
      return EQUAL;

  return 9;
};

#define E6 1000000
#define E9 1000000000

  void add_mc_timestamp(struct timespec * ts, uint64_t msec) {
    ts->tv_sec += msec / 1000;
    ts->tv_nsec += (msec % 1000) * E6;
    if (ts->tv_nsec >= E9) {
      ts->tv_sec += ts->tv_nsec / E9;
      ts->tv_nsec %= E9;
    }
  }
