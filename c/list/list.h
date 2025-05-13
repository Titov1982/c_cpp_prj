#ifndef __LIST__H__
#define __LIST__H__

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


//typedef bool (*list_cmp_node)(void*, void*);
//typedef void (*print_func)(void*) ;

// Структура элемента списка
typedef struct node {
    struct node *prev;
    struct node *next;
    void* value;
} Node;

// Структура списка
typedef struct list {
    Node *tail;
    Node *head;
    size_t size;

    // Определяемые пользователем методы (зависят от типа объектов списка)
    bool (*list_cmp_node)(void*, void*);
    void (*print_node)(Node*);

    // Методы списка
    size_t (*list_get_len)(struct list*);
    bool (*list_add)(struct list*, void*, size_t); 
    Node* (*list_find_node)(struct list*, void*);
    bool (*list_del)(struct list*, void*);
    void (*list_del_all)(struct list*);
    void (*list_clear)(struct list*);
    void (*print_list)(struct list*); 
} List;


// Создание нового списка
List list_new();
// Получить кол-во элементов в списке
size_t list_get_len(List *self); 
// Добавить элемент в список
bool list_add(List *self, void *value, size_t value_size); 
// Получить указатель на элемент списка по его значению
Node* list_find_node(List *self, void *value);
// Удалить элемент из списка по значению
bool list_del(List *self, void *value);
// Удалить все элементы списка, но оставить привязку пользовательских методов 
void list_del_all(List *self);
// Полностью очистить список (с привязкой методов)
void list_clear(List *self); 
// Вывести на экран значения всего списка
void print_list(List *self); 

#endif
