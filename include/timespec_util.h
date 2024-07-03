#include "includes.h"

enum RELATIONS {
  GREATER_THAN = 1,
  LESS_THAN = -1,
  EQUAL = 0,
};

bool cmpr_timestamp(struct timespec *t1, struct timespec *t2, enum RELATIONS check_case);

void add_mc_timestamp(struct timespec *ts, uint64_t msec);
