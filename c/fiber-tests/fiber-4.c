#include <stdio.h>
#include <ucontext.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define FIBER_NUMBER        2
#define FIBER_STACK_SIZE    8192

// Fiber context
struct fiber_context_t
{
  ucontext_t context;
  void (*fiber_func)(void);
  void* fiber_func_args;
};

// Fiber args
struct fiber_func_params_t
{
  char* name;
  size_t count;
};



// Fiber create
void
fiber_create(size_t fiber_stack_size, void *fiber_func, struct fiber_context_t* main_cxt, struct fiber_context_t* f_cxt)
{
  getcontext(&(f_cxt->context));
  f_cxt->context.uc_stack.ss_sp = calloc(fiber_stack_size, sizeof(char));  // Выделяем память под стек
  f_cxt->context.uc_stack.ss_size = fiber_stack_size;                      // Устанавливаем размер стека
  f_cxt->context.uc_link = &(main_cxt->context);                           // Переходим в основной контекст после завершения нити

  f_cxt->fiber_func = fiber_func;
}

// Fiber start
void
fiber_start(struct fiber_context_t* old_f_cxt, struct fiber_context_t* new_f_cxt)
{
  makecontext(&(new_f_cxt->context), (void (*))new_f_cxt->fiber_func, 2, old_f_cxt, new_f_cxt);
}

// Fiber switch
void
fiber_switch(struct fiber_context_t* old_f_cxt, struct fiber_context_t* new_f_cxt)
{
  int res = swapcontext(&(old_f_cxt->context), &(new_f_cxt->context));
  if (res == -1) {
    fprintf(stderr, "swapcontext: %s\n", strerror(errno));
  }
}

// Fiber function
void 
fiber_function_1(struct fiber_context_t* fiber_context_out, struct fiber_context_t* fiber_context_in)
{
  const char* fiber_name = ((struct fiber_func_params_t*)(fiber_context_in->fiber_func_args))->name;
  size_t* fiber_count = &((struct fiber_func_params_t*)(fiber_context_in->fiber_func_args))->count;
  
  while (*fiber_count < 5) {
    printf("%s: context is active\n", fiber_name);
    printf("%s: do work!\n", fiber_name);
    usleep(300000);
    (*fiber_count)++;
    printf("%s count: %zu\n", fiber_name, *fiber_count);
    fiber_switch(fiber_context_in, fiber_context_out);
    printf("%s: context is active again\n", fiber_name);
  } 
}

int
main(int argc, char *argv[])
{
    // Определяем основной контекст
  struct fiber_context_t main_cxt;
    // Определяем массив рабочих контексты
  struct fiber_context_t f_cxt_list[FIBER_NUMBER];
    // Определяем массив структур с параметрами рабочих контекстов
  struct fiber_func_params_t ffps[FIBER_NUMBER];

    // Создаем рабочие контексты
  for (int i = 0; i < FIBER_NUMBER; i++) {
    fiber_create(FIBER_STACK_SIZE , (void (*))fiber_function_1, &main_cxt, &(f_cxt_list[i]));

    ffps[i].name = calloc(15, sizeof(char));
    sprintf(ffps[i].name, "fiber-%d", i);
    ffps[i].count = 0;

    f_cxt_list[i].fiber_func_args = (void*)&(ffps[i]);
  }

    // Настраиваем переключение контекствов на следующий (round-robin)
  for (int i = 0; i < FIBER_NUMBER; i++) {
    int next = (i + 1) % FIBER_NUMBER;
    fiber_start(&(f_cxt_list[next]), &(f_cxt_list[i]));
  }


  printf("main: switch to %s\n", ((struct fiber_func_params_t*)(f_cxt_list[0].fiber_func_args))->name);
  fiber_switch(&main_cxt, &(f_cxt_list[0]));
  printf("main: return from %s\n", ((struct fiber_func_params_t*)(f_cxt_list[0].fiber_func_args))->name);

  printf("main: exit\n");


    // Очищаем память с параметрами рабочих потоков и их стеками
  for (int i = 0; i < FIBER_NUMBER; i++) {
      free((void*)(ffps[i].name));
      free(f_cxt_list[i].context.uc_stack.ss_sp);
  }

}
