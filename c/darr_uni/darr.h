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
 * size_one_elem - размер одного элемента массива
 */
typedef struct darr_s {
  size_t size;
  size_t cap;
  void* arr;
  size_t size_one_elem;
} darr;




int     darr_create(darr* arr, size_t size_one_elem);   // Создать динамический массив
int     darr_add(darr* arr, void* elem);                // Добавить элемент
int     darr_destroy(darr* arr);                        // Уничтожить массив
int     darr_del(darr* arr, size_t idx);                // Удалить элемент по индексу
void*   darr_get(darr* arr, size_t idx);                // Прлучить элемент по индексу









#endif
