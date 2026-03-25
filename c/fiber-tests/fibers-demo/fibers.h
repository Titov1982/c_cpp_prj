#ifndef FIBERS_H_
#define FIBERS_H_

#include <stdio.h>
#include <signal.h>
#include <stdatomic.h>
// #include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <ucontext.h>
#include <unistd.h>

#define FIBER_NUMBER 5

#define FIBER_TIMER_SWITCH 100 // 100 или 1000 мкс
#define FIBER_STACK_SIZE 8192


// Идентификатиор волокон
typedef size_t fid_t;


// Fiber context
struct fiber_context_t {
  fid_t fid;
  ucontext_t context;
  volatile int finished; // 0 - работает, 1 - завершена
  
};


struct fibers_t {
  struct fiber_context_t main_cxt;
  struct fiber_context_t sched_cxt;
  volatile int cur_context_num;    // Индекс текущего выполняемого волокна
  _Atomic int num_finished_fibers; // Количество завершённых волокон
  struct fiber_context_t f_cxt_list[FIBER_NUMBER];
};

extern struct itimerval timer__;              // Таймер
extern struct fibers_t fibers__;              // Общая управляющая структура для волокон






// Инициализация управляющей структуры для волокон
void fibers_init();

// Настройка и запуск планировщика
void fiber_sched_run();

// Завершение работы планировщика
void fiber_sched_stop();

// Создание волокна (fibre)
void fiber_create(size_t stack_size, void (*fiber_func)(void*),
        void* param, struct fiber_context_t *f_cxt);

// Получить id волокна
fid_t getfid();

// Освобождение ресурсов волокон
void fibers_destroy();



#endif
