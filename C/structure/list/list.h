#ifndef LIST_H
#define LIST_H

struct list_head
{
    struct list_head* next, * prev;
};

void list_init(struct list_head* list);

void list_node_init(struct list_head* list);

void list_add(struct list_head* head, struct list_head* node);

void list_add_tail(struct list_head* head, struct list_head* node);

void list_delete(struct list_head* node);

void list_replace(struct list_head* old, struct list_head* replace);

void list_move(struct list_head* node, struct list_head* head);

void list_move_tail(struct list_head* node, struct list_head* head);

int list_is_last(const struct list_head* node, const struct list_head* head);

int list_empty(const struct list_head* head);

#define list_entry(ptr, type, member) (type*)((char*)(ptr) - (unsigned long)&(((type*)0)->member))

#define list_for_each(pos, head) \
    for(pos = (head)->next; pos != head; pos = pos->next)

#define list_for_each_prev(pos, head) \
    for(pos = (head)->prev; pos != head; pos = pos->prev)

#define list_for_each_safe(pos, n, head) \
    for(pos = (head)->next, n = pos->next; pos != head; pos = n, n = pos->next)

#define list_for_each_prev_safe(pos, n, head) \
    for(pos = (head)->prev, n = pos->prev; pos != head; pos = n, n = pos->prev)

#endif