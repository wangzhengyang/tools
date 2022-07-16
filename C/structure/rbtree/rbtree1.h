#ifndef RBTREE_H
#define RBTREE_H

struct rb_node
{
    unsigned long rb_parent_color;
#define RB_RED 0
#define RB_BLACK 1
    struct rb_node* rb_right;
    struct rb_node* rb_left;
}__attribute__((aligned(sizeof(long))));

struct rb_root
{
    struct rb_node* rb_node;
};

#define rb_entry(ptr, type, member) (type*)((char*)(ptr) - (unsigned long)&(((type*)0)->member))

struct rb_node* rb_parent(const struct rb_node* node);

int rb_color(const struct rb_node* node);

int rb_is_red(const struct rb_node* node);

int rb_is_black(const struct rb_node* node);

void rb_set_red(struct rb_node* node);

void rb_set_black(struct rb_node* node);

void rb_set_parent(struct rb_node* child, struct rb_node* parent);

void rb_set_color(struct rb_node* node, int color);

void rb_root_init(struct rb_root* root);

void rb_node_init(struct rb_node* node);

int rb_root_is_empty(const struct rb_root* root);

int rb_node_is_empty(const struct rb_node* node);

void rb_link_node(struct rb_node* node, struct rb_node* parent, struct rb_node** rb_link);

void rb_insert_color(struct rb_node* node, struct rb_root* root);

void rb_erase(struct rb_node* node, struct rb_root* root);

struct rb_node* rb_next(const struct rb_node* node);

struct rb_node* rb_prev(const struct rb_node* node);

struct rb_node* rb_first(const struct rb_root* root);

struct rb_node* rb_last(const struct rb_root* root);

void rb_replace_node(struct rb_node* old, struct rb_node* replace, struct rb_root* root);


#endif