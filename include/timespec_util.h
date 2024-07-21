#include "includes.h"

enum RELATIONS {
  GREATER_THAN = 0b0001,
  LESS_THAN = 0b0010,
  EQUAL = 0b0100,
};

enum RELATIONS cmpr_timestamp(struct timespec *t1, struct timespec *t2);

bool check_cmpr_timestamp(struct timespec *t1, struct timespec *t2, int checkif);

void add_mc_timestamp(struct timespec *ts, uint64_t msec);
