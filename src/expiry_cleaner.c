#include "data_structures.h"
#include "dynamic_array.h"
#include "kv_database.h"
#include "timespec_util.h"
#include <pthread.h>
#include <stdio.h>

dynamic_array(string_container_t *) * timed_data_queue;

extern bool keep_running;

pthread_mutex_t sleep_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t sleep_condtion = PTHREAD_COND_INITIALIZER;
pthread_t expiry_cleanup_ptid;

bool sorting_str_container_helper(string_container_t *val1, string_container_t *val2) {
  return check_cmpr_timestamp(&val1->expiry_time, &val2->expiry_time, LESS_THAN);
}

void expiry_cleanup() {
  string_container_t *value;
  struct timespec now;
  int rc;

  dynamic_array_init(&timed_data_queue);
  timed_data_queue->value_free_function = NULL;

  while (keep_running) {

    pthread_mutex_lock(&sleep_mutex);
    if (timed_data_queue->length) {
      // this will return the last item in the array
      value = dynamic_array_get(timed_data_queue, -1);

      rc = pthread_cond_timedwait(&sleep_condtion, &sleep_mutex, &value->expiry_time);

      switch (rc) {
      case ETIMEDOUT:
        // I'll relay on `pthread_cond_timedwait` and will not do time check.
        value = dynamic_array_pop(timed_data_queue, -1);
        value->expired = true;
        delete_kv(value->key);
        break;
      case EINTR:
        break;
      }
    } else {
      pthread_cond_wait(&sleep_condtion, &sleep_mutex);
    }
    pthread_mutex_unlock(&sleep_mutex);
  }

  free_dynamic_array(timed_data_queue);
}

void expiry_data_push(string_container_t *value) {
  pthread_mutex_lock(&sleep_mutex);
  pthread_cond_signal(&sleep_condtion);

  dynamic_array_push_in_order(timed_data_queue, value, sorting_str_container_helper);

  pthread_mutex_unlock(&sleep_mutex);
}

void cancel_container_timer(string_container_t *container) {
  if (container == NULL) {
    return;
  }
  pthread_mutex_lock(&sleep_mutex);
  dynamic_array_for_each_index(timed_data_queue, cont, index){
    if(cont == container){
      dynamic_array_pop_no_mutex(timed_data_queue, index);
      dynamic_array_for_each_break
    }
  }
  pthread_cond_signal(&sleep_condtion);
  pthread_mutex_unlock(&sleep_mutex);
}

void expiry_cleanup_exit() {
  pthread_cond_broadcast(&sleep_condtion);
  pthread_cond_destroy(&sleep_condtion);
  pthread_mutex_destroy(&sleep_mutex);
  pthread_join(expiry_cleanup_ptid, NULL);
}

