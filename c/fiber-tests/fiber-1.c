#include <stdio.h>
#include <ucontext.h>
#include <stdlib.h>



ucontext_t main_ctx;
ucontext_t func_ctx;
char func_stack[8192];

void fiber_function(const char* name)
{
  printf("%s: context is active\n", name);
  printf("%s: do work!\n", name);
  swapcontext(&func_ctx, &main_ctx);  // Возврат в main()
  printf("%s: context is active again\n", name);
}


int 
main(int argc, char *argv[])
{
  getcontext(&func_ctx);
  func_ctx.uc_stack.ss_sp = func_stack;
  func_ctx.uc_stack.ss_size = sizeof(func_stack);
  func_ctx.uc_link = &main_ctx;
  makecontext(&func_ctx, (void (*))fiber_function, 1, "fiber-1");

  printf("main: switch to fiber-1\n");
  swapcontext(&main_ctx, &func_ctx);
  printf("main: return from fiber-1\n");
  swapcontext(&main_ctx, &func_ctx);
  printf("main: exit\n");

}
