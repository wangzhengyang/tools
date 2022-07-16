#ifndef HASH_H
#define HASH_H

struct hlist_node
{
    struct hlist_node* next, ** pprev;
};

struct hlist_head
{
    struct hlist_node* first;
};

void hlist_head_init(struct hlist_head* head);

void hlist_node_init(struct hlist_head* node);

int hlist_unhashed(const struct hlist_node* node);

int hlist_empty(const struct hlist_head* head);

void hlist_delete(struct hlist_node* node);

void hlist_add_head(struct hlist_node* node, struct hlist_head* head);

void hlist_add_before(struct hlist_node* node, struct hlist_node* next);

void hlist_add_after(struct hlist_node* node, struct hlist_node* prev);

void hlist_move(struct hlist_head* old, struct hlist_head* other);

#define hlist_entry(ptr, type, member)  (type*)((char*)(ptr) - (unsigned long)&(((type*)0)->member))

#define hlist_for_each(pos, head) \
    for(pos = (head)->first; pos; pos = pos->next)

#define hlist_for_each_safe(pos, n, head) \
    for(pos = (head)->first, n = pos->next; pos; pos = n, n = pos->next)

#endif