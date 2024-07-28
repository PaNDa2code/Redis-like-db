#include "data_structures.h"
#include "dynamic_array.h"
#include "kv_database.h"
#include "timespec_util.h"

dynamic_array(string_container_t *) * timed_data_queue;

extern bool keep_running;

pthread_mutex_t sleep_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t sleep_condtion = PTHREAD_COND_INITIALIZER;
pthread_t expiry_cleanup_ptid;
bool stop = false;

bool sorting_str_container_helper(string_container_t *val1,
                                  string_container_t *val2) {
  return check_cmpr_timestamp(&val1->expiry_time, &val2->expiry_time,
                              LESS_THAN);
}

void expiry_cleanup() {
  string_container_t *value;
  struct timespec now;
  int rc;

  while (keep_running) {

    pthread_mutex_lock(&sleep_mutex);
    if (timed_data_queue->length) {

      printf("We got data to wait for.\n");

      // this will return the last item in the array
      value = dynamic_array_get(timed_data_queue, timed_data_queue->length - 1);

      rc = pthread_cond_timedwait(&sleep_condtion, &sleep_mutex,
                                  &value->expiry_time);

      switch (rc) {
      case ETIMEDOUT:
        clock_gettime(CLOCK_REALTIME, &now);
        if (check_cmpr_timestamp(&now, &value->expiry_time, LESS_THAN || EQUAL)) {
          dynamic_array_pop(timed_data_queue, -1);
          delete_kv(value->key);
        }
        break;
      case EINTR:
        break;
      }
    } else {
      printf("There is no timed data, waiting for it.\n");
      pthread_cond_wait(&sleep_condtion, &sleep_mutex);
    }
    pthread_mutex_unlock(&sleep_mutex);
  }
}

void expiry_data_push(string_container_t *value) {

  pthread_mutex_lock(&sleep_mutex);
  pthread_cond_signal(&sleep_condtion);

  dynamic_array_push_in_order(timed_data_queue, value,
                              sorting_str_container_helper);
  dynamic_array_for_each_index(timed_data_queue, tvalue, index) {
    printf("timed_data_queue[%zu]=%s\n", index, tvalue->key);
  };

  pthread_mutex_unlock(&sleep_mutex);
}

void expiry_cleanup_exit() {}
