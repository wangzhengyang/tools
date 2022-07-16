#include "list.h"


void list_init(struct list_head* list)
{
    list->next = list->prev = list;
}

void list_node_init(struct list_head* list)
{
    list->next = list->prev = (void*)0;
}

void list_add(struct list_head* head, struct list_head* node)
{
    struct list_head* next = head->next;
    node->prev = head;
    node->next = next;
    head->next = node;
    next->prev = node;
}

void list_add_tail(struct list_head* head, struct list_head* node)
{
    struct list_head* prev = head->prev;
    node->prev = prev;
    node->next = head;
    head->prev = node;
    prev->next = node;
}

void list_delete(struct list_head* node)
{
    struct list_head* prev = node->prev;
    struct list_head* next = node->next;
    prev->next = next;
    next->prev = prev;
}

void list_replace(struct list_head* old, struct list_head* replace)
{
    replace->next = old->next;
    replace->prev = old->prev;
    replace->next->prev = replace;
    replace->next->prev = replace;
}

void list_move(struct list_head* node, struct list_head* head)
{
    list_delete(node);
    list_add(node, head);
}

void list_move_tail(struct list_head* node, struct list_head* head)
{
    list_delete(node);
    list_add_tail(node, head);
}

int list_is_last(const struct list_head* node, const struct list_head* head)
{
    return node->next == head;
}

int list_empty(const struct list_head* head)
{
    return head->next == head;
}