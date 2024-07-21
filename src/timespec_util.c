#include "timespec_util.h"
#include <stdbool.h>
#include <time.h>

enum RELATIONS cmpr_timestamp(struct timespec *t1, struct timespec *t2) {

  if (t1->tv_sec > t2->tv_sec ||
      (t1->tv_sec == t2->tv_sec && t1->tv_nsec > t2->tv_nsec))
    return GREATER_THAN;

  else if (t1->tv_sec < t2->tv_sec ||
           (t1->tv_sec == t2->tv_sec && t1->tv_nsec < t2->tv_nsec))
    return LESS_THAN;

  else if (t1->tv_sec == t2->tv_sec && t1->tv_nsec == t2->tv_nsec)
    return EQUAL;

  return -1;
};

bool check_cmpr_timestamp(struct timespec *t1, struct timespec *t2,
                          int checkif) {
  int is = cmpr_timestamp(t1, t2);
  int e = EQUAL & checkif;
  int l = LESS_THAN & checkif;
  int g = GREATER_THAN & checkif;

  if (e && is == EQUAL)
    return true;
  if (l && is == LESS_THAN)
    return true;
  if (g && is == GREATER_THAN)
    return true;

  return false;
}

#define E6 1000000
#define E9 1000000000

void add_mc_timestamp(struct timespec *ts, uint64_t msec) {
  ts->tv_sec += msec / 1000;
  ts->tv_nsec += (msec % 1000) * E6;
  if (ts->tv_nsec >= E9) {
    ts->tv_sec += ts->tv_nsec / E9;
    ts->tv_nsec %= E9;
  }
}
