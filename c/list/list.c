// #include <stdint.h>
#include <stdio.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "list.h"


// Создание нового списка
struct list list_new() {
    struct list l = {
        .head = NULL,
        .tail = NULL,
        .size = 0,
        .list_cmp_node_func = NULL,
    };
    return l;
}

// Получить кол-во элементов в списке
size_t list_get_len(struct list *list) {
    return list->size;
}

// Добавить элемент в список
bool list_add(struct list *list, void *value, size_t value_size) {
    if (list != NULL) {
        struct node *nd = (struct node*) malloc(sizeof(struct node));
        if (nd == NULL) return false; // error
        
        if (value == NULL) return false; // error
        void *val = malloc(value_size);
        memcpy(val, value, value_size);
        nd->value = val;

        if (list->size  == 0) {
            nd->prev = NULL;
            nd->next = NULL;
            list->head = nd;
            list->tail = nd;
        } else {
            list->tail->next = nd;
            nd->prev = list->tail;
            list->tail = nd;
            nd->next = NULL;
        }

        list->size++;
        return true;

    }
    return false; // error
}

// Получить указатель на элемент списка по его значению
struct node* list_find_node(struct list *list, void *value) {
    if (list != NULL) {
        struct node *ptr_nd = list->head;
        while (ptr_nd != NULL) {
            //if (ptr_nd->value == value) {
            if (list->list_cmp_node_func(ptr_nd->value, value)) {
                return ptr_nd;
            }
            ptr_nd = ptr_nd->next;
        }        
    }
    return NULL;
}

// Удалть элемент из списка по значению
bool list_del(struct list *list, void *value) {
    if (list != NULL) {
        struct node *ptr_nd = list_find_node(list, value);
        if (ptr_nd != NULL) {
            if (ptr_nd->prev == NULL) {
                list->head = ptr_nd->next;
                ptr_nd->next->prev = NULL;
            } else if (ptr_nd->next == NULL) {
                list->tail = ptr_nd->prev;
                ptr_nd->prev->next = NULL;
            } else {
                ptr_nd->prev->next = ptr_nd->next;
                ptr_nd->next->prev = ptr_nd->prev;
            }
            free(ptr_nd->value);
            free(ptr_nd);
            
            list->size--;
            return true;
        }
    }
    return false;
}

// Очистить список
void list_del_all(struct list *list) {
    if (list == NULL) return;
    if (list->head == NULL) return;
    struct node *current = list->head;
    while (current->next != NULL) {
        current = current->next;
        free(current->prev->value);
        free(current->prev);
        current->prev = NULL;
        list->size--;
    }
    if (list->size == 1) {
        free(current->value);
        free(current);
        list->head = NULL;
        list->tail = NULL;
        list->list_cmp_node_func = NULL;
        list->size--;
    }
    return;
}

// Вывести на экран значения всего списка
/*void list_print(struct list *list) {
    struct node *ptr_nd = list->head;
    for (; ptr_nd != NULL; ptr_nd = ptr_nd->next) {
        printf("%" PRId32 "\n", *((int32_t*)ptr_nd->value));
    }
}
*/
void list_print(struct list *list) {
    struct node *ptr_nd = list->head;
    for (; ptr_nd != NULL; ptr_nd = ptr_nd->next) {
        list->print_func(ptr_nd);
    }
}


