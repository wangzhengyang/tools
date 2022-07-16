#include "hash.h"

void hlist_head_init(struct hlist_head* head)
{
    head->first = (void*)0;
}

void hlist_node_init(struct hlist_node* node)
{
    node->next = node->pprev = (void*)0;
}

int hlist_unhashed(const struct hlist_node* node)
{
    return !node->pprev;
}

int hlist_empty(const struct hlist_head* head)
{
    return !head->first;
}

void hlist_delete(struct hlist_node* node)
{
    struct hlist_node* next, ** pprev;

    if (!hlist_unhashed(node))
    {
        next = node->next;
        pprev = node->pprev;
        *pprev = next;

        if (next)
        {
            next->pprev = pprev;
        }

        hlist_node_init(node);
    }
}

void hlist_add_head(struct hlist_node* node, struct hlist_head* head)
{
    struct hlist_node* first = head->first;

    node->next = first;

    if (first)
    {
        first->pprev = &node->next;
    }

    head->first = node;
    node->pprev = &head->first;
}

void hlist_add_before(struct hlist_node* node, struct hlist_node* next)
{
    node->pprev = next->pprev;
    node->next = next;
    next->pprev = &node->next;
    *(node->pprev) = node;
}

void hlist_add_after(struct hlist_node* node, struct hlist_node* prev)
{
    node->next = prev->next;
    node->pprev = &prev->next;
    prev->next = node;

    if (node->next)
    {
        node->next->pprev = &node->next;
    }
}

void hlist_move(struct hlist_head* old, struct hlist_head* other)
{
    other->first = old->first;

    if (old->first)
    {
        old->first->pprev = &other->first;
    }

    old->first = (void*)0;
}