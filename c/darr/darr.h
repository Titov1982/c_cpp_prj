#ifndef DARR_H_
#define DARR_H_

#include <stdlib.h>


/**
 * Емкость массива по умолчанию
 */
#define DARR_CAPACITY 5

/**
 * Структура описывающая динамический массив.
 * size - текущее число элементов в массиве
 * cap  - емкость массива. Кол-во элементов массива для которых выделена память.
 * arr  - указатель на начало массива выделенной памяти
 */
typedef struct darr_s {
  size_t size;
  size_t cap;
  int* arr;
} darr;




int   darr_create(darr* arr, size_t size_one_elem);
int   darr_add(darr* arr, int elem);
void  darr_print(darr* arr);
int   darr_destroy(darr* arr);
int   darr_del(darr* arr, size_t idx);
int   darr_get(darr* arr, size_t idx);









#endif
