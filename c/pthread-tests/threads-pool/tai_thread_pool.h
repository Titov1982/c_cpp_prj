#ifndef _TAI_THREAD_POOL_
#define _TAI_THREAD_POOL_

#define _GNU_SOURCE

// #include <stddef.h>
// #include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <pthread.h>
#include <threads.h>
// #include <time.h>
#include <unistd.h>



typedef enum {
  RUNNING,
  SUSPEND,
  CANCELING,
} Worker_status;

typedef void* (*worker_func)(void*);
typedef void* (*runner_func)(void*);

typedef struct thread {
  void* params;
  pthread_t worker_id;
  //int worker_tid;
  pthread_attr_t worker_attr;
  pthread_mutex_t worker_mutex;
  pthread_cond_t worker_cond;
  void* (*worker_func)(void*);
  Worker_status worker_status;
  void* (*runner_func)(void*);
} Thread;

typedef struct worker_pool {
  Thread** pool;
  size_t worker_count;
} Worker_pool;


static void* thread_func(void* arg);

Thread* worker_create(Thread* ptr_worker);

void worker_set_runner(Thread* ptr_worker,
                       void* (*runner_func)(void*), void* params);

void worker_start(Thread* ptr_worker);

void worker_cancel(Thread* ptr_worker);

static void worker_destroy(Thread* ptr_worker);

Worker_status worker_get_status(Thread* ptr_worker);

Worker_pool* worker_pool_create(size_t worker_count);

void worker_pool_delete(Worker_pool* ptr_worker_pool);


#endif 
