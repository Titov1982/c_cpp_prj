#include "fibers.h"

struct itimerval timer__;   // Таймер
struct fibers_t fibers__;   // Общая управляющая структура для волокон

fid_t cur_fiber_fid;        // id активного в данный момент волокна 


fid_t getfid() {
    return cur_fiber_fid;
}


// Планировщик волокон
static void fiber_scheduler() {

  /* Так как мы переключили контекст из обработчика сигнала на планировщик,
   * то необходимо забокировать SIGALRM в планировщике, чтобы избежать
   * рекурсивных прерываний */
  sigset_t newmask, oldmask;
  sigemptyset(&newmask);
  sigaddset(&newmask, SIGALRM);
  sigprocmask(SIG_BLOCK, &newmask, &oldmask);

  /* Запуск первого волокна */
  fibers__.cur_context_num = 0;
 swapcontext(&fibers__.sched_cxt.context, &fibers__.f_cxt_list[0].context);

  while (fibers__.num_finished_fibers < FIBER_NUMBER) {
    // Ищем следующую незавершённую нить, начиная со следующей за текущей
    int next_context = (fibers__.cur_context_num + 1) % FIBER_NUMBER;
    // Крутим цикл пока не найдем следующую незавершенную нить
    while (fibers__.f_cxt_list[next_context].finished) {
      next_context = (next_context + 1) % FIBER_NUMBER;
      // Получаем id активного волокна
      cur_fiber_fid = fibers__.f_cxt_list[next_context].fid;
      // Если все кроме текущей завершены, остаёмся в текущей (ничего не делаем)
      if (next_context == fibers__.cur_context_num) {
        break;
      }
    }
    if (!fibers__.f_cxt_list[next_context].finished) {
      fibers__.cur_context_num = next_context;
      // Получаем id активного волокна
      cur_fiber_fid = fibers__.f_cxt_list[next_context].fid;
      /* Переключаемся на выбранную нить */
      swapcontext(&fibers__.sched_cxt.context,
                  &fibers__.f_cxt_list[next_context].context);
    } else {
      /* Все нити завершены – выходим */
      break;
    }
  }

  // Восстанавливаем маску сигналов и возвращаемся в main через uc_link
  sigprocmask(SIG_SETMASK, &oldmask, NULL);
}




// Обработчик сигнала таймера (переключение контекста на планировщик)
static void timer_interrupt([[maybe_unused]] int sig,
                            [[maybe_unused]] siginfo_t *si, void *uc) {

  // Если все волокна завершены, то выходим
  if (fibers__.num_finished_fibers >= FIBER_NUMBER)
    return;
  // Получаем контекст из которого переключились на обработчик сигнала
  fibers__.f_cxt_list[fibers__.cur_context_num].context = *(ucontext_t *)uc;
  // Переключаем контекст на планировщик
  swapcontext(&fibers__.f_cxt_list[fibers__.cur_context_num].context,
              &fibers__.sched_cxt.context);
}




// Настройка таймера и обработчика сигнала
static void setup_timer() {
  struct sigaction sa;
  memset(&sa, 0, sizeof(sa));
  sa.sa_sigaction = timer_interrupt;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART | SA_SIGINFO;
  if (sigaction(SIGALRM, &sa, NULL) == -1) {
    perror("sigaction");
    exit(EXIT_FAILURE);
  }

  timer__.it_interval.tv_sec = 0;
  timer__.it_interval.tv_usec =
      FIBER_TIMER_SWITCH; // Интервал прерывания таймера
  timer__.it_value.tv_sec = 0;
  timer__.it_value.tv_usec = 10; // Первый срабатывание через 10 мкс
  if (setitimer(ITIMER_REAL, &timer__, NULL) == -1) {
    perror("setitimer");
    exit(EXIT_FAILURE);
  }
}




// Остановка таймера
static void stop_timer() {
  timer__.it_interval.tv_sec = 0;
  timer__.it_interval.tv_usec = 0;
  timer__.it_value.tv_sec = 0;
  timer__.it_value.tv_usec = 0;
  setitimer(ITIMER_REAL, &timer__, NULL);
}




// Настройка и запуск планировщика
void fiber_sched_run() {
  // Настраиваем сигнал и устанавливаем таймер
  setup_timer();

  /* Переключаемся на контекст волокна планировщика, сохраняя контекст главного
   * волокна в main_cxt.context. После завершения всех рабочих волокон
   * управление вернётся в волокно планировщика и далее в основной контекст,
   * т.е. сюда (через uc_link) */
  swapcontext(&fibers__.main_cxt.context, &fibers__.sched_cxt.context);
}




// Завершение работы планировщика
void fiber_sched_stop() {
  // Останавливаем таймер
  stop_timer();
}


// fid_t create_fiber_id() {
//     fibers__.
// }



// Создание волокна (fibre)
void fiber_create(size_t stack_size, void (*fiber_func)(void*), void* param, struct fiber_context_t *f_cxt) {

  // Получаем текущий контекст (инициализация)
  if (getcontext(&f_cxt->context) == -1) {
    perror("getcontext");
    exit(EXIT_FAILURE);
  }

  // Выделяем стек
  f_cxt->context.uc_stack.ss_sp = calloc(stack_size, 1);
  if (!f_cxt->context.uc_stack.ss_sp) {
    perror("calloc stack");
    exit(EXIT_FAILURE);
  }
  f_cxt->context.uc_stack.ss_size = stack_size;
  f_cxt->context.uc_link =
      &fibers__.sched_cxt
           .context; // по завершении вернуться в волокно планировщика

  // Устанавливаем id волокна
  f_cxt->fid = (fid_t)f_cxt->context.uc_stack.ss_sp;

  f_cxt->finished = 0; // волокно активно

  // Создаём контекст волокна, передавая индекс как целочисленный аргумент
  makecontext(&f_cxt->context, (void (*)())fiber_func, 1, param);
}




// Инициализируем основной контекст
void main_cxt_init() {
  if (getcontext(&fibers__.main_cxt.context) == -1) {
    perror("getcontext main");
    exit(EXIT_FAILURE);
  }
  fibers__.main_cxt.context.uc_stack.ss_sp = calloc(FIBER_STACK_SIZE, 1);
  if (!fibers__.main_cxt.context.uc_stack.ss_sp) {
    perror("calloc main stack");
    exit(EXIT_FAILURE);
  }
  fibers__.main_cxt.context.uc_stack.ss_size = FIBER_STACK_SIZE;
  fibers__.main_cxt.context.uc_link =
      NULL; // главная нить никуда не возвращается
}




// Инициализируем контекст планировщика
void sched_context_init(void (*fiber_sched)()) {
  if (getcontext(&fibers__.sched_cxt.context) == -1) {
    perror("getcontext scheduler");
    exit(EXIT_FAILURE);
  }
  fibers__.sched_cxt.context.uc_stack.ss_sp = calloc(FIBER_STACK_SIZE, 1);
  if (!fibers__.sched_cxt.context.uc_stack.ss_sp) {
    perror("calloc scheduler stack");
    exit(EXIT_FAILURE);
  }
  fibers__.sched_cxt.context.uc_stack.ss_size = FIBER_STACK_SIZE;
  fibers__.sched_cxt.context.uc_link =
      &fibers__.main_cxt.context; // Переходим в главное волокно после
                                  // завершения планировщика
  makecontext(&fibers__.sched_cxt.context, fiber_sched, 0);
}




// Инициализируем управление волокнами
void fibers_init() {
  // Инициализируем главный контекст
  main_cxt_init();
  // Инициализация контекста планировщика
  sched_context_init(fiber_scheduler);
}




//
void fibers_destroy() {

  // Освобождаем память
  for (int i = 0; i < FIBER_NUMBER; i++) {
    free(fibers__.f_cxt_list[i].context.uc_stack.ss_sp);
  }
  free(fibers__.sched_cxt.context.uc_stack.ss_sp);
  free(fibers__.main_cxt.context.uc_stack.ss_sp);
}
