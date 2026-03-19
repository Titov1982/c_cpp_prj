#include <stddef.h>
#include <stdio.h>
#include <sys/ucontext.h>
#include <ucontext.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <sys/time.h>
#include <stdatomic.h>

#define FIBER_NUMBER        5
#define FIBER_STACK_SIZE    8192

// Fiber context
struct fiber_context_t
{
    ucontext_t context;
    volatile int finished;        // 0 - работает, 1 - завершена
};

// Параметры нити
struct fiber_func_params_t
{
    char name[16];          // Название нити
    size_t count;           // Счетчик нити
};

// Глобальные данные
struct fiber_context_t main_cxt;
struct fiber_context_t f_cxt_list[FIBER_NUMBER];
struct fiber_func_params_t ffps[FIBER_NUMBER];

volatile int cur_context_num = 0;           // Индекс текущей выполняемой нити
_Atomic int num_finished_fibers = 0;        // Количество завершённых нитей
struct itimerval timer;                     // Таймер

// Планировщик нитей 
void
fiber_sheduler(void)
{
    // Если все нити завершены, ничего не делаем
    if (num_finished_fibers >= FIBER_NUMBER) {
        return;
    }

    // Ищем следующую незавершённую нить, начиная со следующей за текущей
    int next_context = (cur_context_num + 1) % FIBER_NUMBER;
    // Крутим цикл пока не найдем следующую незавершенную нить
    while (f_cxt_list[next_context].finished) {
        next_context = (next_context + 1) % FIBER_NUMBER;
        // Если все кроме текущей завершены, остаёмся в текущей (ничего не делаем)
        if (next_context == cur_context_num) {
            return;
        }
    }

    // Переключаемся на найденную нить
    int old = cur_context_num;
    cur_context_num = next_context;
    swapcontext(&f_cxt_list[old].context, &f_cxt_list[next_context].context);

}

// Обработчик сигнала таймера (переключение контекста)
void
timer_interrupt([[maybe_unused]]int sig, [[maybe_unused]]siginfo_t* si, [[maybe_unused]]void* uc)
{
    fiber_sheduler();
}

// Настройка таймера и обработчика сигнала
void
setup_timer()
{
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_sigaction = timer_interrupt;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART | SA_SIGINFO;
    if (sigaction(SIGALRM, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = 100;    // 100 или 1000 мкс
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = 10;        // первый срабатывание через 10 мкс
    if (setitimer(ITIMER_REAL, &timer, NULL) == -1) {
        perror("setitimer");
        exit(EXIT_FAILURE);
    }
}

// Остановка таймера
void
stop_timer()
{
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = 0;
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = 0;
    setitimer(ITIMER_REAL, &timer, NULL);
}

// Создание нити (fibre)
void
fiber_create(size_t stack_size, void (*fiber_func)(int), int idx,
             struct fiber_context_t* main_cxt, struct fiber_context_t* f_cxt)
{
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
    f_cxt->context.uc_link = &main_cxt->context;   // по завершении вернуться в главную нить

    f_cxt->finished = 0;   // нить активна

    // Создаём контекст, передавая индекс как целочисленный аргумент
    makecontext(&f_cxt->context, (void (*)())fiber_func, 1, idx);
}

// Функция нити (принимает индекс нити)
void
fiber_function(int idx)
{
    // Печатаем счетчик нити
    for (size_t i = 0; i < 10001; i++) {
        usleep(5);    // небольшая задержка, чтобы дать шанс переключиться
        ffps[idx].count = i;
        printf("%s \tcount: %zu\n", ffps[idx].name, i);
        fflush(stdout);
    }

    // Помечаем нить как завершённую
    f_cxt_list[idx].finished = 1;
    atomic_fetch_add(&num_finished_fibers, 1);
}

int
main()
{
    // Инициализируем главный контекст
    if (getcontext(&main_cxt.context) == -1) {
        perror("getcontext main");
        exit(EXIT_FAILURE);
    }
    main_cxt.context.uc_stack.ss_sp = calloc(FIBER_STACK_SIZE, 1);
    if (!main_cxt.context.uc_stack.ss_sp) {
        perror("calloc main stack");
        exit(EXIT_FAILURE);
    }
    main_cxt.context.uc_stack.ss_size = FIBER_STACK_SIZE;
    main_cxt.context.uc_link = NULL;   // главная нить никуда не возвращается

    // Создаём рабочие нити
    for (int i = 0; i < FIBER_NUMBER; i++) {
        snprintf(ffps[i].name, sizeof(ffps[i].name), "fiber-%d", i);
        ffps[i].count = 0;
        fiber_create(FIBER_STACK_SIZE, fiber_function, i, &main_cxt, &f_cxt_list[i]);
    }

    // Запускаем таймер
    setup_timer();

    // Переключаемся на первую нить, сохраняя контекст главной нити в main_cxt.context
    // После завершения всех нитей управление вернётся сюда (через uc_link)
    swapcontext(&main_cxt.context, &f_cxt_list[0].context);

    // 
    fiber_sheduler();

    // Этот код выполнится после того, как все нити завершатся
    stop_timer();

    // Выводим окончательный результат по счетчикам нитей
    printf("-----------------------------------\n");
    for (int i = 0; i < FIBER_NUMBER; i++) {
        printf("%s:\t[%ld]\n", ffps[i].name, ffps[i].count);
    }

    printf("All fibers finished. Main exiting.\n");

    // Освобождаем память
    for (int i = 0; i < FIBER_NUMBER; i++) {
        free(f_cxt_list[i].context.uc_stack.ss_sp);
    }
    free(main_cxt.context.uc_stack.ss_sp);

    return EXIT_SUCCESS;
}
