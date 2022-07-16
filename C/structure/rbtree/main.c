#include "rbtree1.h"
#include <stdio.h>

struct mytype
{
    struct rb_node node;
    int value;
};

struct rb_root root = { .rb_node = (void*)0 };

struct mytype* my_search(struct rb_root* root, int value)
{
    struct rb_node* node = root->rb_node;

    if (!node)
    {
        return (void*)0;
    }

    while (node)
    {
        struct mytype* data = rb_entry(node, struct mytype, node);

        if (value < data->value)
        {
            node = node->rb_left;
        }
        else if (value > data->value)
        {
            node = node->rb_right;
        }
        else
        {
            return data;
        }
    }
}

int my_insert(struct rb_root* root, struct mytype* data)
{
    struct rb_node** new = &(root->rb_node), * parent = (void*)0;

    while (*new)
    {
        struct mytype* this = rb_entry(*new, struct mytype, node);
        parent = *new;
        if (data->value < this->value)
        {
            new = &((*new)->rb_left);
        }
        else if (data->value > this->value)
        {
            new = &((*new)->rb_right);
        }
        else
        {
            return -1;
        }
    }

    rb_link_node(&data->node, parent, new);
    rb_insert_color(&data->node, root);
    return 0;
}

struct mytype datas[5] = {
    {.value = 10},
    {.value = 35},
    {.value = 2},
    {.value = 100},
    {.value = 43},
};

int main()
{
    for (int i = 0; i < 5; i++)
    {
        rb_node_init(&datas[i].node);
        my_insert(&root, &datas[i]);
    }

    struct rb_node* tmp = rb_first(&root);
    while (tmp)
    {
        struct mytype* data = rb_entry(tmp, struct mytype, node);
        printf("value:%d\r\n", data->value);
        tmp = rb_next(tmp);
    }
    return 0;
}
