#define _GNU_SOURCE

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>
#include <sys/syscall.h>
#include <threads.h>
#include <time.h>

#include <unistd.h>


#define TO_RED      "\x1B[31m"
#define TO_GREEN    "\x1B[32m"
#define TO_YELLOW   "\x1B[33m"
#define TO_NORMAL   "\033[0m"



// Структура для обмена данными с потоком
typedef struct params {
    int value;                      // Передаваемая переданный
    char* color;                    // Символьная последовательность для хранения цвета вывода символов в консоль
    int ret;                        // Возвращаемое значение
} Params;


typedef struct thread {
    Params params;                  // Структура параметров потока
    pthread_t worker_id;            // id потока
    pthread_attr_t worker_attr;     // Аттребуты потока
    void* (*worker)(void*);         // Указатель на функцию рабочего потока
} Thread;


// Функция исполняемая в потоке
void* worker(void* value) {
    
    // Получаем id текущего потока
    int tid = syscall(SYS_gettid);

    // Кладем id в структуру для чтения из главного потока
    ((Thread*)value)->params.ret = tid;

    // Читаем параметр переданный в поток
    int val = ((Thread*)value)->params.value;
    for (int i = 0; i < val; i++) {
        printf(((Thread*)value)->params.color);
        printf("[worker %d]: %d\n", tid, i);
        printf(TO_NORMAL);
        // Засыпаем
        thrd_sleep(&(struct timespec){.tv_nsec = 1000 * 10 * 1}, NULL);
    }
    printf(((Thread*)value)->params.color);
    printf("[worker %d]: finish\n", tid);
    printf(TO_NORMAL);
    
    return EXIT_SUCCESS;
}


// Настраиваем рабочий поток 
void worker_create(Thread* th) {
    pthread_attr_init(&(th->worker_attr));
    pthread_setattr_default_np(&(th->worker_attr));
}

// Стартуем рабочий поток
void worker_start(Thread* th) {
    pthread_create(&(th->worker_id), &(th->worker_attr), th->worker, th);
}




int main(int argc, char* argv[]) {

    Thread th_1 = {
        .params.color = TO_RED,
        .params.value = 10,
        .params.ret = 0,
        .worker = worker,
    };

    Thread th_2 = {
        .params.color = TO_YELLOW,
        .params.value = 10,
        .params.ret = 0,
        .worker = worker,
    };

    worker_create(&th_1);
    worker_create(&th_2);
    worker_start(&th_1);
    worker_start(&th_2);
  
    thrd_sleep(&(struct timespec){.tv_nsec = 1000 * 10 * 10}, NULL);
    printf("WORKER 1: tid = %d\n", th_1.params.ret);
    printf("WORKER 2: tid = %d\n", th_2.params.ret);

    for (int i = 0; i < 10; i++) {
        printf(TO_GREEN);
        printf("[main]: %d\n", i);
        printf(TO_NORMAL);
        thrd_sleep(&(struct timespec){.tv_nsec = 1000 * 10 * 5}, NULL);
    }
   
    printf(TO_GREEN);
    printf("[main]: finish\n");
    printf(TO_NORMAL);

    void* ret;
    void* ret_2;
    pthread_join(th_1.worker_id, &ret);
    pthread_join(th_2.worker_id, &ret_2);

    // printf("\n");
    // printf("\x1B[31mTexting\033[0m\t\t");
    // printf("\x1B[32mTexting\033[0m\t\t");
    // printf("\x1B[33mTexting\033[0m\t\t");
    // printf("\x1B[34mTexting\033[0m\t\t");
    // printf("\x1B[35mTexting\033[0m\n");
    //
    // printf("\x1B[36mTexting\033[0m\t\t");
    // printf("\x1B[36mTexting\033[0m\t\t");
    // printf("\x1B[36mTexting\033[0m\t\t");
    // printf("\x1B[37mTexting\033[0m\t\t");
    // printf("\x1B[93mTexting\033[0m\n");
    //
    // printf("\033[3;42;30mTexting\033[0m\t\t");
    // printf("\033[3;43;30mTexting\033[0m\t\t");
    // printf("\033[3;44;30mTexting\033[0m\t\t");
    // printf("\033[3;104;30mTexting\033[0m\t\t");
    // printf("\033[3;100;30mTexting\033[0m\n");
    //
    // printf("\033[3;47;35mTexting\033[0m\t\t");
    // printf("\033[2;47;35mTexting\033[0m\t\t");
    // printf("\033[1;47;35mTexting\033[0m\t\t");
    // printf("\t\t");
    // printf("\n");

    return EXIT_SUCCESS;
}

