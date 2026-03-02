#include "darr.h"

#include <stdio.h>


/**
 * Определения для примеров использования массива
 */
#define TYPE_INT                    int
#define TYPE_SPECIFIER_INT          "%d"

#define TYPE_LONG_DOUBLE            long double
#define TYPE_SPECIFIER_LONG_DOUBLE  "%Lf"

int main(void)
{
  darr new_arr = {0};

/**
 * Проверка динамического массива с элементами "int"
 */
#if 0  
  if (darr_create(&new_arr, sizeof(TYPE_INT)) > 0) return EXIT_FAILURE;
  
  // Емкость и размер нового массива
  printf("cap: %zu\tsize: %zu\n", new_arr.cap, new_arr.size);

  // Заполняем массив значениями  
  for (int i = 0; i < 1500000000; i++) {
    TYPE_INT buf = (i + 1) * 10; 
    int res = darr_add(&new_arr, &buf);
    if (res == 1) {
      perror("darr_add: realloc вернул NULL\n");
      exit(1);
    }
  }

  // Емкость и размер наполненного массива
  printf("cap: %zu\tsize: %zu\n", new_arr.cap, new_arr.size);
  printf("-------------\n");

  // getchar();

  // Получение и вывод элементов массива
  // for (size_t i = 0; i < new_arr.size; i++) {
  //   printf("i: %zu data: " TYPE_SPECIFIER_INT "\n", i, *(TYPE_INT*)darr_get(&new_arr, i));
  // }  
#endif



/**
 * Проверка динамического массива с элементами "long double"
 */
#if 1  
  if (darr_create(&new_arr, sizeof(TYPE_LONG_DOUBLE)) > 0) return EXIT_FAILURE;
  
  // Емкость и размер нового массива
  printf("cap: %zu\tsize: %zu\n", new_arr.cap, new_arr.size);
  // Заполняем массив значениями  
  for (size_t i = 0; i < 450000000; i++) {
    TYPE_LONG_DOUBLE buf = (i + 1) * 10; 
    int res = darr_add(&new_arr, &buf);
    if (res == 1) {
      perror("darr_add: realloc вернул NULL\n");
      exit(1);
    }
  }

  // Емкость и размер наполненного массива
  printf("cap: %zu\tsize: %zu\n", new_arr.cap, new_arr.size);
  printf("-------------\n");

  // getchar();

  // Получение и вывод элементов массива
  // for (size_t i = 0; i < new_arr.size; i++) {
  //   printf("i: %zu data: " TYPE_SPECIFIER_LONG_DOUBLE "\n", i, *(TYPE_LONG_DOUBLE*)darr_get(&new_arr, i));
  // }  
#endif



  // Удаляем все элементы из массива начиная с нулевого
  // while (new_arr.size > 0) {
  //   darr_del(&new_arr, 0);
  //   // printf("cap: %zu\tsize: %zu\n", new_arr.cap, new_arr.size);
  // }

  // Удаляем все элементы из массива начиная с конца 
  while (new_arr.size > 0) {
    darr_del(&new_arr, new_arr.size - 1);
  }

  // Емкость и размер опусташенного массива
  printf("-------------\n");
  printf("cap: %zu\tsize: %zu\n", new_arr.cap, new_arr.size);

  // Удаляем массив, очищаем память
  darr_destroy(&new_arr);


  return EXIT_SUCCESS;
}
