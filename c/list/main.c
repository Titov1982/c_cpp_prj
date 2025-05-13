#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "list.h"



// void srnd() {srand(time(NULL));}


bool eq_node_value_int32_t(void *val1, void *val2) {
    if (*((int32_t*)val1) == *((int32_t*)val2)) return true;
    else return false;
}

bool eq_node_value_str(void *val1, void *val2) {
    char *v1 = (char*) val1;
    char *v2 = (char*) val2;
    if (strcmp(v1, v2) == 0) return true;
    else return false;
}

void print_str(Node *ptr_nd) {
    printf("%s\n", (char*) ptr_nd->value);
}

void print_int32_t(Node *ptr_nd) {
    printf("%" PRId32 "\n", *((int32_t*)ptr_nd->value));
}


int main(void) {

    //srnd();
    
    // Создаем список
    List  list = list_new();
    // Устанавливаем функцию сравнения элементов списка
    list.list_cmp_node = eq_node_value_str;
    // Устанавливаем функцию печати элементов списка
    list.print_node = print_str;

    char *s1 = "a";
    char *s2 = "b";
    char *s3 = "c";
    char *s4 = "d";
    if (list.list_add(&list, s1, strlen(s1) + 1) == false) return 1;
    if (list.list_add(&list, s2, strlen(s2) + 1) == false) return 1;
    if (list.list_add(&list, s3, strlen(s3) + 1) == false) return 1;
    if (list.list_add(&list, s4, strlen(s4) + 1) == false) return 1;
    printf("List size = %zu\n", list.list_get_len(&list));
    list.print_list(&list);
    
    if (list.list_del(&list, s2) == false) return 1;
    if (list.list_del(&list, s4) == false) return 1;
    printf("List size = %zu\n", list.list_get_len(&list));
    list.print_list(&list);
    
    char *s5 = "e";
    if (list.list_add(&list, s5, strlen(s5) + 1) == false) return 1;
    printf("List size = %zu\n", list.list_get_len(&list));
    list.print_list(&list);
    
    list.list_del_all(&list);
    printf("List size = %zu\n", list.list_get_len(&list));

    for (size_t i = 0; i < 15; i++) {
        char buf[16] = "str_val_";
        char b[5] = {0};
        snprintf(b, sizeof(b), "%zu", i);
        strcat(buf, b);

        if (list.list_add(&list, buf, strlen(buf)) == false) return 1;
    }    
    printf("List size = %zu\n", list.list_get_len(&list));
    list.print_list(&list);

    char* v = "str_val_10"; // найдет
    //char* v = "b"; // не найдет
    Node nd = {
        .prev = NULL,
        .next = NULL,
        .value = v,
    };
    Node *finded_node = list.list_find_node(&list, nd.value); 
    if (finded_node != NULL) print_str(finded_node);

    list.list_del_all(&list);
    
    return 0;
}
