#include "tai_thread_pool.h"

// Управляющая функция рабочего потока
static void* thread_func(void* arg) {
  Thread* ptr_worker = (Thread*)arg;
  void* result = NULL;
  while (true) {
    ptr_worker->worker_status = SUSPEND;
    // ##########    Блок остановки потока после создания    ##########   
    pthread_mutex_lock(&(ptr_worker->worker_mutex));
    pthread_cond_wait(&(ptr_worker->worker_cond), &(ptr_worker->worker_mutex));
    // Если после запуска потока он имеет статус завершения, то завершаем поток
    if (ptr_worker->worker_status == CANCELING) {
      pthread_exit(result);
    }
    pthread_mutex_unlock(&(ptr_worker->worker_mutex));
    // Меняем статус потока
    ptr_worker->worker_status = RUNNING;
    // ################################################################

    // ########    Вызов функции полезной нагрузки потока    ##########
    //  Если функция полезной нагрузки для потока определена, то запускаем ее,
    //  иначе засыпаем
    if ((ptr_worker->runner_func != NULL) && (ptr_worker->params != NULL)) {
      // Вызываем полезную нагрузку
      result = ptr_worker->runner_func(ptr_worker->params);
    }
    // ################################################################
  }
  pthread_exit(NULL);
}

// Создать рабочий поток
Thread* worker_create(Thread* ptr_worker) {
  ptr_worker = (Thread*)malloc(sizeof(Thread)); 
  
  ptr_worker->params = NULL; 
  ptr_worker->worker_func = thread_func;
  ptr_worker->runner_func = NULL;
  
  pthread_attr_init(&(ptr_worker->worker_attr));
  pthread_setattr_default_np(&(ptr_worker->worker_attr));

  pthread_mutex_init(&(ptr_worker->worker_mutex), NULL);
  pthread_cond_init(&(ptr_worker->worker_cond), NULL);

  pthread_create(&(ptr_worker->worker_id), &(ptr_worker->worker_attr),
                 ptr_worker->worker_func, ptr_worker);

  return ptr_worker;
}

// Установить функцию полезной нагрузки рабочего потока и ее параметры
void worker_set_runner(Thread* ptr_worker,
                       void* (*runner_func)(void*), void* params) {
  ptr_worker->params = params; 
  ptr_worker->runner_func = runner_func;
}

// Запустить рабочий поток на исполнение
void worker_start(Thread* ptr_worker) {
  pthread_mutex_lock(&(ptr_worker->worker_mutex));
  pthread_cond_signal(&(ptr_worker->worker_cond));
  pthread_mutex_unlock(&(ptr_worker->worker_mutex));
}

// Завершить управляющую функцию рабочего потока
void worker_cancel(Thread* ptr_worker) {
  ptr_worker->worker_status = CANCELING;
  worker_start(ptr_worker);
}

// Удалить рабочий поток
static void worker_destroy(Thread* ptr_worker) {
  pthread_mutex_destroy(&(ptr_worker->worker_mutex));
  pthread_cond_destroy(&(ptr_worker->worker_cond));
  free(ptr_worker);
}


Worker_status worker_get_status(Thread* ptr_worker) {
  return ptr_worker->worker_status;
}


// Создать пул потоковё
Worker_pool* worker_pool_create(size_t worker_count) {
  Worker_pool* ptr_worker_pool = (Worker_pool*) malloc(sizeof(Worker_pool));
  ptr_worker_pool->pool = (Thread**) malloc(sizeof(Thread*) * worker_count);
  ptr_worker_pool->worker_count = worker_count;
  for (size_t i = 0; i < worker_count; i++) {
    ptr_worker_pool->pool[i] = worker_create(ptr_worker_pool->pool[i]);
  }
  return ptr_worker_pool;
}

// Удалить пул потоков
void worker_pool_delete(Worker_pool* ptr_worker_pool) {
  for (int i = 0; i < ptr_worker_pool->worker_count; i++) {
    worker_destroy(ptr_worker_pool->pool[i]);
  }
  free(ptr_worker_pool->pool);
  free(ptr_worker_pool);
}
