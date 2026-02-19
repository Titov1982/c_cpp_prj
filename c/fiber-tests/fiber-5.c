#include <stdio.h>
#include <ucontext.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <sys/time.h>

#define FIBER_NUMBER        3
#define FIBER_STACK_SIZE    8192

// Fiber context
struct fiber_context_t
{
  ucontext_t context;
  void (*fiber_func)(void);
  void* fiber_func_args;
  int status;
};

// Fiber args
struct fiber_func_params_t
{
  char* name;
  size_t count;
};


struct fiber_context_t main_cxt;
struct fiber_context_t f_cxt_list[FIBER_NUMBER];
struct fiber_func_params_t ffps[FIBER_NUMBER]; 

int cur_context_num = 0;
// sigset_t set;

void
timer_int(int sig, siginfo_t* si, void* uc)
{
  int next_context = (cur_context_num + 1) % FIBER_NUMBER;

  while (f_cxt_list[next_context].status == 1) {
    next_context = ((next_context + 1) % FIBER_NUMBER) + 1;
  }

  int cur_tmp = cur_context_num; 
  cur_context_num = next_context;
  swapcontext(&(f_cxt_list[cur_tmp].context), &(f_cxt_list[next_context].context));
}

void
setup_timer()
{
    struct sigaction sa;
    sa.sa_sigaction = timer_int;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART | SA_SIGINFO;

    // sigemptyset(&set);              //+
    // sigaddset(&set, SIGALRM);       //+

    sigaction(SIGALRM, &sa, NULL);

    struct itimerval timer;
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = 10000;
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = 10;
    setitimer(ITIMER_REAL, &timer, NULL);

    // getcontext(&(main_cxt.context));        // Получаем основной контекст для возврата, после завершения всех нитей 
    setcontext(&(f_cxt_list[0].context));   // Переключаемся на первый рабочий контекст
}



// Fiber create
void
fiber_create(size_t fiber_stack_size,
    void *fiber_func, struct fiber_context_t* main_cxt,
    struct fiber_context_t* f_cxt)
{
  getcontext(&(f_cxt->context));
  f_cxt->context.uc_stack.ss_sp = calloc(fiber_stack_size, sizeof(char));  // Выделяем память под стек
  f_cxt->context.uc_stack.ss_size = fiber_stack_size;                      // Устанавливаем размер стека
  f_cxt->context.uc_link = &(main_cxt->context);                           // Переходим в основной контекст после завершения нити

  f_cxt->fiber_func = fiber_func;
  f_cxt->status = 0;

  makecontext(&(f_cxt->context), (void (*))f_cxt->fiber_func, 1, f_cxt);
}


// Fiber function
void 
fiber_function_1(struct fiber_context_t* fiber_context_in)
{
  char* fiber_name = ((struct fiber_func_params_t*)(fiber_context_in->fiber_func_args))->name;
  size_t* fiber_count = &((struct fiber_func_params_t*)(fiber_context_in->fiber_func_args))->count;
  
  while (*fiber_count < 4) {
    // usleep(100000);
    (*fiber_count)++;
    printf("%s count: %zu\n", fiber_name, *fiber_count);
  }
  fiber_context_in->status = 1;
}

int
main()
{
  for (int i = 0; i < FIBER_NUMBER; i++) {
    fiber_create(FIBER_STACK_SIZE, (void (*))fiber_function_1, &main_cxt, &(f_cxt_list[i]));
    
    ffps[i].name = calloc(15, sizeof(char));
    sprintf(ffps[i].name, "fiber-%d", i);
    ffps[i].count = 0;
    f_cxt_list[i].fiber_func_args = (void*)&ffps[i];
  }

  getcontext(&(main_cxt.context));        // Получаем основной контекст для возврата, после завершения всех нитей
  main_cxt.context.uc_stack.ss_sp = calloc(FIBER_STACK_SIZE, sizeof(char));  // Выделяем память под стек
  main_cxt.context.uc_stack.ss_size = FIBER_STACK_SIZE;                      // Устанавливаем размер стека
  
  setup_timer();

  for (int i = 0; i < FIBER_NUMBER; i++) {
    printf("%s: %d\n", 
        ((struct fiber_func_params_t*)(f_cxt_list[i].fiber_func_args))->name,
        f_cxt_list[i].status);
  }

  printf("main: exit\n");

  sleep(10);
}
