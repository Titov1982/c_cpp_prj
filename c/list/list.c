// #include <stdint.h>
#include <stdio.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "list.h"


// Создание нового списка
List list_new() {
    List l = {
        .head = NULL,
        .tail = NULL,
        .size = 0,

        .list_cmp_node = NULL,
        .print_node = NULL,

        .list_get_len = list_get_len,
        .print_list = print_list,
        .list_add = list_add,
        .list_del = list_del,
        .list_del_all = list_del_all,
        .list_find_node = list_find_node,

    };
    return l;
}

// Получить кол-во элементов в списке
size_t list_get_len(List *self) {
    return self->size;
}

// Добавить элемент в список
bool list_add(List *self, void *value, size_t value_size) {
    if (self != NULL) {
        if (value == NULL) return false; // error
        Node *nd = (Node*) malloc(sizeof(Node));
        if (nd == NULL) return false; // error 
        
        void *val = malloc(value_size);
        memcpy(val, value, value_size);
        nd->value = val;

        if (self->size  == 0) {
            nd->prev = NULL;
            nd->next = NULL;
            self->head = nd;
            self->tail = nd;
        } else {
            self->tail->next = nd;
            nd->prev = self->tail;
            self->tail = nd;
            nd->next = NULL;
        }

        self->size++;
        return true;

    }
    return false; // error
}

// Получить указатель на элемент списка по его значению
Node* list_find_node(List *self, void *value) {
    if (self != NULL) {
        Node *ptr_nd = self->head;
        while (ptr_nd != NULL) {
            if (self->list_cmp_node(ptr_nd->value, value)) {
                return ptr_nd;
            }
            ptr_nd = ptr_nd->next;
        }        
    }
    return NULL;
}

// Удалть элемент из списка по значению
bool list_del(List *self, void *value) {
    if (self != NULL) {
        Node *ptr_nd = list_find_node(self, value);
        if (ptr_nd != NULL) {
            if (ptr_nd->prev == NULL) {
                self->head = ptr_nd->next;
                ptr_nd->next->prev = NULL;
            } else if (ptr_nd->next == NULL) {
                self->tail = ptr_nd->prev;
                ptr_nd->prev->next = NULL;
            } else {
                ptr_nd->prev->next = ptr_nd->next;
                ptr_nd->next->prev = ptr_nd->prev;
            }
            free(ptr_nd->value);
            free(ptr_nd);
            
            self->size--;
            return true;
        }
    }
    return false;
}

// Удалить все элементы списка, но оставить привязку пользовательских методов 
void list_del_all(List *self) {
    if (self == NULL) return;
    if (self->head == NULL) return;
    Node *current = self->head;
    while (current->next != NULL) {
        current = current->next;
        free(current->prev->value);
        free(current->prev);
        current->prev = NULL;
        self->size--;
    }
    if (self->size == 1) {
        free(current->value);
        free(current);
        self->head = NULL;
        self->tail = NULL;
        self->size--;
    }
    return;
}


// Полностью очистить список (с привязкой методов)
void list_clear(List *self) {
    if (self == NULL) return;
    list_del_all(self);
    self->list_cmp_node = NULL;
    self->print_list = NULL;
    return;
}


// Вывести на экран значения всего списка
void print_list(List *self) {
    Node *ptr_nd = self->head;
    for (; ptr_nd != NULL; ptr_nd = ptr_nd->next) {
        self->print_node(ptr_nd);
    }
}


