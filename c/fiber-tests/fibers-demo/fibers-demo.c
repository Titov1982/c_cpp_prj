


/*
 * Добавлена основная структура fibers_t для хранения параметров об управлении волокнами.
 * Эта структура передается во все функции, которые нуждаются в управляющих параметрах.
 * Для удобства добавлена функция инициализации структуры fibres_t.
 * Структкра fibers_t сделана глобальной, чтобы управляющие параметры был доступны в обработчике сигнала.
 * Произведено разделение кода волокон на fibers.h и fibers.c.
 * Добавлен параметр и код для определения id текущего волокна и функция для его получения.
 * Параметры в волокно теперь пердаются через void*, в конкретном случае, через структуру параметров.
 */

// #define _GNU_SOURCE
// #define _POSIX_C_SOURCE 199309 

#include "fibers.h"


// Параметры нити
struct fiber_func_params_t {
  char name[16]; // Название волокна
  size_t count;  // Счетчик волокна
};


// Глобальные данные
struct fiber_func_params_t ffps[FIBER_NUMBER];




// void fiber_work_func(int idx) {
//
//   // Печатаем счетчик нити
//   for (size_t i = 0; i < 5; i++) {
//     usleep(5); // небольшая задержка, чтобы дать шанс переключиться
//     ffps[idx].count = i;
//     printf("%s\t (%zu) \tcount: %zu\n", ffps[idx].name, getfid(), i);
//     fflush(stdout);
//   }
// }


// Функция нити (принимает индекс нити)
void fiber_function(void* param) {
  struct fiber_func_params_t* p = (struct fiber_func_params_t*)param;
  // Печатаем счетчик нити
  for (size_t i = 0; i < 5; i++) {
    usleep(5); // небольшая задержка, чтобы дать шанс переключиться
    p->count = i;
    printf("%s\t (%zu) \tcount: %zu\n", p->name, getfid(), i);
    fflush(stdout);
  }
  
  // fiber_work_func()


  // Помечаем нить как завершённую
  fibers__.f_cxt_list[fibers__.cur_context_num].finished = 2;
  
  // Атомарно увеличиваем счетчик завершенных нитей
  atomic_fetch_add(&fibers__.num_finished_fibers, 1);
}


int main() {

  fibers_init();

  // Создаём рабочие волокна
  for (int i = 0; i < FIBER_NUMBER; i++) {
    snprintf(ffps[i].name, sizeof(ffps[i].name), "fiber-%d", i);
    ffps[i].count = 0;
    // fiber_create(FIBER_STACK_SIZE, fiber_function, i,
    //              &fibers__.f_cxt_list[i]);
    fiber_create(FIBER_STACK_SIZE, fiber_function, &ffps[i], &fibers__.f_cxt_list[i]);
  }

  // Запускаем планировщик волокон
  fiber_sched_run();

  /**
   * Этот код выполнится после того, как все рабочие волокна
   * и планировщик завершится.
   */

  // Останавливаем работу планировщика
  fiber_sched_stop();

  printf("All fibers finished. Main exiting.\n");

  // Выводим окончательный результат по счетчикам волокон
  printf("-----------------------------------\n");
  for (int i = 0; i < FIBER_NUMBER; i++) {
    printf("%s:\t[%ld]\n", ffps[i].name, ffps[i].count);
  }


  fibers_destroy();
  // // Освобождаем память
  // for (int i = 0; i < FIBER_NUMBER; i++) {
  //   free(fibers__.f_cxt_list[i].context.uc_stack.ss_sp);
  // }
  // free(fibers__.sched_cxt.context.uc_stack.ss_sp);
  // free(fibers__.main_cxt.context.uc_stack.ss_sp);

  return EXIT_SUCCESS;
}
