#define _GNU_SOURCE

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>
#include <sys/syscall.h>
#include <threads.h>
#include <time.h>

#include <unistd.h>


// Структура для обмена данными с потоком
typedef struct params {
    int value;
    int ret;
} Params;


// Функция исполняемая в потоке
void* worker(void* value) {
    
    // Получаем id текущего потока
    int tid = syscall(SYS_gettid);

    // Кладем id в структуру для чтения из главного потока
    ((Params*)value)->ret = tid;

    // Читаем параметр переданный в поток
    int val = ((Params*)value)->value;
    for (int i = 0; i < val; i++) {
        printf("[worker %d]: %d\n", tid, i);
        // Засыпаем
        thrd_sleep(&(struct timespec){.tv_nsec = 1000 * 10 * 1}, NULL);
    }
    printf("[worker %d]: finish\n", tid);
    
    return EXIT_SUCCESS;
}



int main(int argc, char* argv[]) {

    // Создаем структуры для данных потоков
    Params par;
    par.value = 15;
    Params par_2;
    par_2.value = 15;

    pthread_t worker_id;
    pthread_attr_t worker_attr;
    pthread_t worker_id_2;
    pthread_attr_t worker_attr_2;

    // Инициализируем аттрибуты потоков
    pthread_attr_init(&worker_attr);
    pthread_attr_init(&worker_attr_2);

    // Запускаем потоки
    pthread_create(&worker_id, &worker_attr, worker, &par);
    pthread_create(&worker_id_2, &worker_attr_2, worker, &par_2);
  
    thrd_sleep(&(struct timespec){.tv_nsec = 1000 * 10 * 5}, NULL);
  
    printf("WORKER 1: tid = %d\n", par.ret);
    printf("WORKER 2: tid = %d\n", par_2.ret);

    for (int i = 0; i < 10; i++) {
        printf("[main]: %d\n", i);
        thrd_sleep(&(struct timespec){.tv_nsec = 1000 * 10 * 5}, NULL);
    }
    printf("[main]: finish\n");

    // Ждем завершения потоков
    void* ret;
    void* ret_2;
    pthread_join(worker_id, &ret);
    pthread_join(worker_id_2, &ret_2);

    return EXIT_SUCCESS;
}

