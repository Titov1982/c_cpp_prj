#include <stdio.h>
#include <ucontext.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>


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
  const char* name;
  size_t count;
};



// Fiber create
void
fiber_create(size_t fiber_stack_size, void *fiber_func, struct fiber_context_t* main_cxt, struct fiber_context_t* f_cxt)
{
  getcontext(&(f_cxt->context));
  f_cxt->context.uc_stack.ss_sp = calloc(fiber_stack_size, sizeof(char));  // Выделяем память под стек;
  f_cxt->context.uc_stack.ss_size = fiber_stack_size;                      // Устанавливаем размер стека
  f_cxt->context.uc_link = &(main_cxt->context);                           // Основной контекст (временно)

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
  char* fiber_name = ((struct fiber_func_params_t*)(fiber_context_in->fiber_func_args))->name;
  size_t* fiber_count = &((struct fiber_func_params_t*)(fiber_context_in->fiber_func_args))->count;
  
  while (*fiber_count < 1010) {
    printf("%s: context is active\n", fiber_name);
    printf("%s: do work!\n", fiber_name);
    usleep(500000);
    (*fiber_count)++;
    printf("%s count: %zu\n", fiber_name, *fiber_count);
    fiber_switch(fiber_context_in, fiber_context_out);
    printf("%s: context is active again\n", fiber_name);
  } 
}

int
main(int argc, char *argv[])
{
  struct fiber_context_t main_cxt;


  struct fiber_context_t f_cxt_1;
  struct fiber_func_params_t ffp1 = {
    .name = "fiber-1",
    .count = 0,
  };
  f_cxt_1.fiber_func_args = (void*)&ffp1; 


  struct fiber_context_t f_cxt_2;
  struct fiber_func_params_t ffp2 = {
    .name = "fiber-2",
    .count = 1000,
  };
  f_cxt_2.fiber_func_args = (void*)&ffp2;

  
  fiber_create(8192, (void (*))fiber_function_1, &main_cxt, &f_cxt_1);
  fiber_start(&f_cxt_2, &f_cxt_1);


  fiber_create(8192, (void (*))fiber_function_1, &main_cxt, &f_cxt_2);
  fiber_start(&f_cxt_1, &f_cxt_2);
  

  printf("main: switch to %s\n", ((struct fiber_func_params_t*)(f_cxt_1.fiber_func_args))->name);
  fiber_switch(&main_cxt, &f_cxt_1);
  printf("main: return from %s\n", ((struct fiber_func_params_t*)(f_cxt_1.fiber_func_args))->name);

  printf("main: exit\n");

}
