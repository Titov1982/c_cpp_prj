#ifndef __LIST__H__
#define __LIST__H__

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


typedef bool (*list_cmp_node)(void*, void*);
typedef void (*print_func)(void*) ;

// Структура элемента списка
struct node {
    struct node *prev;
    struct node *next;
    void* value;
};

// Структура списка
struct list {
    struct node *tail;
    struct node *head;
    size_t size;
    bool (*list_cmp_node_func)(void*, void*);
    void (*print_func)(struct node*);
};


// Создание нового списка
struct list list_new();
// Получить кол-во элементов в списке
size_t list_get_len(struct list *list); 
// Добавить элемент в список
bool list_add(struct list *list, void *value, size_t value_size); 
// Получить указатель на элемент списка по его значению
struct node* list_find_node(struct list *list, void *value);
// Удалть элемент из списка по значению
bool list_del(struct list *list, void *value);
// Очистить список
void list_del_all(struct list *list);
// Вывести на экран значения всего списка
void list_print(struct list *list); 

#endif
