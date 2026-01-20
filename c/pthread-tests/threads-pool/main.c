#define _GNU_SOURCE

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <pthread.h>
#include <threads.h>
#include <time.h>
#include <unistd.h>

#define TO_RED      "\x1B[31m"
#define TO_GREEN    "\x1B[32m"
#define TO_YELLOW   "\x1B[33m"
#define TO_BLUE     "\x1B[34m"
#define TO_MAGENTA  "\x1B[35m"
#define TO_NORMAL   "\033[0m"

#include "tai_thread_pool.h"

typedef struct params {
  int value;
  char* color;
} Params;


void* run_func(/*Params* params*/void* args) {
  Params* params = (Params*) args;
  for (size_t i = 0; i < 5; i++) {
    printf("%s" "[Worker %lu]: %d\n" TO_NORMAL,
           params->color, (unsigned long)pthread_self(),
           params->value);
  }
  return NULL;
}

void* run_func_2(/*Params* params*/ void* args) {
  Params* params = (Params*) args;
  for (size_t i = 0; i < 5; i++) {
    printf("%s" "[Worker %lu]: %d\n" TO_NORMAL,
           params->color, (unsigned long)pthread_self(),
           params->value + 100);
  }
  return NULL;
}


#define WORKER_COUNT 5

int main(int argc, char *argv[]) {

  Thread* workers_pool[WORKER_COUNT];

  Params p0 = { .value = 0, .color = TO_RED };
  Params p1 = { .value = 1, .color = TO_GREEN };
  Params p2 = { .value = 2, .color = TO_YELLOW };
  Params p3 = { .value = 3, .color = TO_BLUE };
  Params p4 = { .value = 4, .color = TO_MAGENTA };
  
  Params* params[WORKER_COUNT] = {&p0, &p1, &p2, &p3, &p4};


  Worker_pool* worker_pool = worker_pool_create(WORKER_COUNT);
  
  // for (size_t i = 0; i < WORKER_COUNT; i++) {
  //   worker_set_runner(worker_pool->pool[i], run_func, params[i]);
  // }
    worker_set_runner(worker_pool->pool[2], run_func, params[2]);
    worker_set_runner(worker_pool->pool[4], run_func, params[4]);

  sleep(1);

  for (size_t i = 0; i < WORKER_COUNT; i++) {
    worker_start(worker_pool->pool[i]);
  }

  sleep(1);

  for (size_t i = 0; i < WORKER_COUNT; i++) {
    worker_set_runner(worker_pool->pool[i], run_func_2, params[i]);
    printf("%s" "[Worker %lu]: %d\n" TO_NORMAL,
           params[i]->color, (unsigned long)worker_pool->pool[i]->worker_id,
           worker_pool->pool[i]->worker_status);
  }
  
  sleep(1);

  for (size_t i = 0; i < WORKER_COUNT; i++) {
    worker_start(worker_pool->pool[i]);
  }

  sleep(2);

  for (size_t i = 0; i < WORKER_COUNT; i++) {
    worker_cancel(worker_pool->pool[i]);
    printf("%s" "[Worker %lu]: %d\n" TO_NORMAL,
           params[i]->color, (unsigned long)worker_pool->pool[i]->worker_id,
           worker_get_status(worker_pool->pool[i]));
  }

  for (size_t i = 0; i < WORKER_COUNT; i++) {
    pthread_join(worker_pool->pool[i]->worker_id, NULL);
  }
  
  worker_pool_delete(worker_pool);

  return EXIT_SUCCESS;
}





