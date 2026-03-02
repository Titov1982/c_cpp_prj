#include "darr.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>



/**
 * Создание нового динамического массива
 * return:
 * 0 - ошибок нет
 * 1 - ошибка  malloc
 */
int darr_create(darr* arr, size_t size_one_elem)
{
  arr->cap = DARR_CAPACITY;
  arr->arr = (int*) malloc(arr->cap * size_one_elem);
  if (arr->arr == NULL) return 1;
  arr->size = 0;
  return 0;
}

/**
 * Добавление элемента в массив
 * return:
 * 0 - ошибок нет
 * 1 - ошибка - не произошло перевыделение памяти при расширении емкости массива (realloc)
 */
int darr_add(darr* arr, int elem)
{
  // Если кол-во элементов меньше емкости
  if (arr->size < arr->cap) {
    arr->arr[arr->size] = elem;
    arr->size++;
    return 0;
  }

  // Если емкость достигнута, то увеличиваем ее в 2 раза
  int* new_ptr = realloc(arr->arr, arr->cap * sizeof(int) * 2);
  if (new_ptr == NULL) return 1;
  arr->arr = new_ptr;
  // Устанавливаем новую емкость
  arr->cap = arr->cap * 2;

  // Добавлчем новый элемент
  arr->arr[arr->size] = elem;
  // Устанавливаем новый размер
  arr->size++;
  return 0;
}

/**
 * Распечатка значений всего массива
 */
void darr_print(darr* arr)
{
  for (size_t i = 0; i < arr->size; i++) {
    printf("i: %zu data: %d\n", i, arr->arr[i]);
  }
}

/**
 *  Полное удаление массива
 *  return:
 *  0 - ошибок нет
 *  1 - массив уже очищен
 */
int darr_destroy(darr* arr)
{
  if (arr->arr != NULL) {
    free((void*)(arr->arr));
    arr->arr = NULL;
    arr->size = 0;
    arr->cap = DARR_CAPACITY;
    return 0;
  }
  return 1;
}

/**
 * Удаление элемента по индексу
 * return:
 * 0 - ошибок нет
 * 1 - ошибка - не произошло перевыделение памяти при уменьшении емкости массива (realloc)
 * 2 - ошибка удаления элемента в пустом массиве
 * 3 - ошибка удаления элемента с индексом превышающим массив
 */
int darr_del(darr* arr, size_t idx)
{
  if (arr->size == 0) return 2;
  if (idx >= arr->size) return 3;
  // Получаем адрес удаляемого элемента
  int* ptr = (arr->arr) + idx;
  // Перемещаем все элементы после удаленного влево
  memmove(ptr, ptr + 1, (arr->size - idx - 1) * sizeof(int));
  arr->size--;

  if (arr->size <= (arr->cap / 2) && (arr->size >= 5)) {
    int* new_ptr = realloc(arr->arr, (arr->cap * sizeof(int) / 2));
    if (new_ptr == NULL) return 1;
    arr->arr = new_ptr;
    // Устанавливаем новую емкость
    arr->cap = arr->cap / 2;
  }

  return 0;
}

/**
 * Получение значения по индексу
 * return:
 * int - значение по индексу
 */
int darr_get(darr* arr, size_t idx)
{
  return *(arr->arr + idx);
}
