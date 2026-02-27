#include "darr.h"

#include <stdio.h>



int main(void)
{
  darr new_arr = {0};

  if (darr_create(&new_arr, sizeof(int)) > 0) return EXIT_FAILURE;
  
  // Емкость и размер нового массива
  printf("cap: %zu\tsize: %zu\n", new_arr.cap, new_arr.size);
  // Заполняем массив значениями  
  for (int i = 0; i < 10000000; i++) {
    int res = darr_add(&new_arr, (i + 1) * 10);
    if (res == 1) {
      perror("darr_add: realloc вернул NULL\n");
      exit(1);
    }
  }

  // darr_print(&new_arr);

  // Емкость и размер наполненного массива
  printf("cap: %zu\tsize: %zu\n", new_arr.cap, new_arr.size);
  printf("-------------\n");

  // Удаляем все элементы из массива начиная с нулевого
  // while (new_arr.size > 0) {
  //   darr_del_elem(&new_arr, 0);
  //   // printf("cap: %zu\tsize: %zu\n", new_arr.cap, new_arr.size);
  // }

  // Удаляем все элементы из массива начиная с конца 
  while (new_arr.size > 0) {
    darr_del_elem(&new_arr, new_arr.size - 1);
  }

  // darr_del_elem(&new_arr, 0);
  // darr_print(&new_arr);
  
  // printf("-------------\n");
  // Емкость и размер опусташенного массива
  printf("cap: %zu\tsize: %zu\n", new_arr.cap, new_arr.size);

  darr_destroy_array(&new_arr);


  return EXIT_SUCCESS;
}
