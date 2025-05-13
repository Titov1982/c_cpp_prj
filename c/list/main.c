// #include <stdint.h>
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

void print_str(struct node *ptr_nd) {
    printf("%s\n", (char*) ptr_nd->value);
}

void print_int32_t(struct node *ptr_nd) {
    printf("%" PRId32 "\n", *((int32_t*)ptr_nd->value));
}


int main(void) {

    //srnd();
    struct list list = list_new();
    list.list_cmp_node_func = eq_node_value_str;
    list.print_func = print_str;

    char *s1 = "a";
    char *s2 = "b";
    char *s3 = "c";
    char *s4 = "d";
    if (list_add(&list, s1, strlen(s1) + 1) == false) return 1;
    if (list_add(&list, s2, strlen(s2) + 1) == false) return 1;
    if (list_add(&list, s3, strlen(s3) + 1) == false) return 1;
    if (list_add(&list, s4, strlen(s4) + 1) == false) return 1;
    printf("List size = %zu\n", list_get_len(&list));
    list_print(&list);
    
    if (list_del(&list, s2) == false) return 1;
    if (list_del(&list, s4) == false) return 1;
    printf("List size = %zu\n", list_get_len(&list));
    list_print(&list);
    
    char *s5 = "e";
    if (list_add(&list, s5, strlen(s5) + 1) == false) return 1;
    printf("List size = %zu\n", list_get_len(&list));
    list_print(&list);
    
    list_del_all(&list);
    printf("List size = %zu\n", list_get_len(&list));
    list_print(&list);



    return 0;
}
