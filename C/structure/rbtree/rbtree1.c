#include "rbtree1.h"


struct rb_node* rb_parent(const struct rb_node* node)
{
    return (struct rb_node*)(node->rb_parent_color & ~3);
}

int rb_color(const struct rb_node* node)
{
    return node->rb_parent_color & 1;
}

int rb_is_red(const struct rb_node* node)
{
    return !rb_color(node);
}

int rb_is_black(const struct rb_node* node)
{
    return !rb_color(node);
}

void rb_set_red(struct rb_node* node)
{
    node->rb_parent_color &= ~1;
}

void rb_set_black(struct rb_node* node)
{
    node->rb_parent_color |= 1;
}

void rb_set_parent(struct rb_node* child, struct rb_node* parent)
{
    child->rb_parent_color = (child->rb_parent_color & 3) | (unsigned long)parent;
}

void rb_set_color(struct rb_node* node, int color)
{
    node->rb_parent_color = (node->rb_parent_color & ~1) | color;
}

void rb_root_init(struct rb_root* root)
{
    root->rb_node = (void*)0;
}

void rb_node_init(struct rb_node* node)
{
    rb_set_parent(node, node);
}

int rb_root_is_empty(const struct rb_root* root)
{
    return root->rb_node == (void*)0;
}

int rb_node_is_empty(const struct rb_node* node)
{
    return rb_parent(node) == node;
}

void rb_link_node(struct rb_node* node, struct rb_node* parent, struct rb_node** rb_link)
{
    node->rb_parent_color = (unsigned long)parent;
    node->rb_left = node->rb_right = (void*)0;
    *rb_link = node;
}

static void rb_rotate_left(struct rb_node* node, struct rb_root* root)
{
    struct rb_node* right = node->rb_right;
    struct rb_node* parent = rb_parent(node);

    if ((node->rb_right = right->rb_left))
    {
        rb_set_parent(right->rb_left, node);
    }

    right->rb_left = node;

    rb_set_parent(right, parent);

    if (parent)
    {
        if (node == parent->rb_left)
        {
            parent->rb_left = right;
        }
        else
        {
            parent->rb_right = right;
        }
    }
    else
    {
        root->rb_node = right;
    }

    rb_set_parent(node, right);
}

static void rb_rotate_right(struct rb_node* node, struct rb_root* root)
{
    struct rb_node* left = node->rb_left;
    struct rb_node* parent = rb_parent(node);

    if ((node->rb_left = left->rb_right))
    {
        rb_set_parent(left->rb_right, node);
    }

    left->rb_right = node;

    rb_set_parent(left, parent);

    if (parent)
    {
        if (node == parent->rb_right)
        {
            parent->rb_right = left;
        }
        else
        {
            parent->rb_left = left;
        }
    }
    else
    {
        root->rb_node = left;
    }

    rb_set_parent(node, left);
}

void rb_insert_color(struct rb_node* node, struct rb_root* root)
{
    struct rb_node* parent, * gparent, * uncle, * tmp;

    while ((parent = rb_parent(node)) && rb_is_red(parent))
    {
        gparent = rb_parent(parent);

        if (parent == gparent->rb_left)
        {
            uncle = gparent->rb_right;
            if (uncle && rb_is_red(uncle))
            {
                rb_set_black(uncle);
                rb_set_black(parent);
                rb_set_red(gparent);
                node = gparent;
                continue;;
            }

            if (parent->rb_right == node)
            {
                rb_rotate_left(parent, root);
                tmp = parent;
                parent = node;
                node = tmp;
            }

            rb_set_black(parent);
            rb_set_red(gparent);
            rb_rotate_right(gparent, root);
        }
        else
        {
            uncle = gparent->rb_left;
            if (uncle && rb_is_red(uncle))
            {
                rb_set_black(uncle);
                rb_set_black(parent);
                rb_set_red(gparent);
                node = gparent;
                continue;
            }

            if (parent->rb_left == node)
            {
                rb_rotate_right(parent, root);
                tmp = parent;
                parent = node;
                node = tmp;
            }

            rb_set_black(parent);
            rb_set_red(gparent);
            rb_rotate_left(gparent, root);
        }
    }

    rb_set_black(root->rb_node);
}

static void rb_erase_color(struct rb_node* node, struct rb_node* parent, struct rb_root* root)
{
    struct rb_node* other;

    while ((!node || rb_is_black(node)) && node != root->rb_node)
    {
        if (parent->rb_left == node)
        {
            other = parent->rb_right;
            if (rb_is_red(other))
            {
                rb_set_black(other);
                rb_set_red(parent);
                rb_rotate_left(parent, root);
                other = parent->rb_right;
            }
            if ((!other->rb_left || rb_is_black(other->rb_left)) &&
                (!other->rb_right || rb_is_black(other->rb_right)))
            {
                rb_set_red(other);
                node = parent;
                parent = rb_parent(node);
            }
            else
            {
                if (!other->rb_right || rb_is_black(other->rb_right))
                {
                    rb_set_black(other->rb_left);
                    rb_set_red(other);
                    rb_rotate_right(other, root);
                    other = parent->rb_right;
                }
                rb_set_color(other, rb_color(parent));
                rb_set_black(parent);
                rb_set_black(other->rb_right);
                rb_rotate_left(parent, root);
                node = root->rb_node;
                break;
            }
        }
        else
        {
            other = parent->rb_left;
            if (rb_is_red(other))
            {
                rb_set_black(other);
                rb_set_red(parent);
                rb_rotate_right(parent, root);
                other = parent->rb_left;
            }
            if ((!other->rb_left || rb_is_black(other->rb_left)) &&
                (!other->rb_right || rb_is_black(other->rb_right)))
            {
                rb_set_red(other);
                node = parent;
                parent = rb_parent(node);
            }
            else
            {
                if (!other->rb_left || rb_is_black(other->rb_left))
                {
                    rb_set_black(other->rb_right);
                    rb_set_red(other);
                    rb_rotate_left(other, root);
                    other = parent->rb_left;
                }
                rb_set_color(other, rb_color(parent));
                rb_set_black(parent);
                rb_set_black(other->rb_left);
                rb_rotate_right(parent, root);
                node = root->rb_node;
                break;
            }
        }
    }

    if (node)
    {
        rb_set_black(node);
    }
}

void rb_erase(struct rb_node* node, struct rb_root* root)
{
    struct rb_node* child, * parent, * old, * left;
    int color;

    if (!node->rb_left)
    {
        child = node->rb_right;
    }
    else if (!node->rb_right)
    {
        child = node->rb_left;
    }
    else
    {
        old = node;
        node = node->rb_right;
        while ((left = node->rb_left) != (void*)0)
        {
            node = left;
        }

        if (rb_parent(old))
        {
            if (rb_parent(old)->rb_left == old)
            {
                rb_parent(old)->rb_left = node;
            }
            else
            {
                rb_parent(old)->rb_right = node;
            }
        }
        else
        {
            root->rb_node = node;
        }

        child = node->rb_right;
        parent = rb_parent(node);
        color = rb_color(node);

        if (parent == old)
        {
            parent = node;
        }
        else
        {
            if (child)
            {
                rb_set_parent(child, parent);
            }
            parent->rb_left = child;

            node->rb_right = old->rb_right;
            rb_set_parent(old->rb_right, node);
        }

        node->rb_parent_color = old->rb_parent_color;
        node->rb_left = old->rb_left;
        rb_set_parent(old->rb_left, node);

        goto color;
    }

    parent = rb_parent(node);
    color = rb_color(node);

    if (child)
    {
        rb_set_parent(child, parent);
    }

    if (parent)
    {
        if (parent->rb_left == node)
        {
            parent->rb_left = child;
        }
        else
        {
            parent->rb_right = child;
        }
    }
    else
    {
        root->rb_node = child;
    }

color:
    if (color == RB_BLACK)
    {
        rb_erase_color(child, parent, root);
    }
}

struct rb_node* rb_next(const struct rb_node* node)
{
    struct rb_node* parent;

    if (rb_parent(node) == node)
    {
        return (void*)0;
    }

    if (node->rb_right)
    {
        node = node->rb_right;
        while (node->rb_left)
        {
            node = node->rb_left;
        }
        return (struct rb_node*)node;
    }

    while ((parent = rb_parent(node)) && node == parent->rb_right)
    {
        node = parent;
    }

    return parent;
}

struct rb_node* rb_prev(const struct rb_node* node)
{
    struct rb_node* parent;

    if (rb_parent(node) == node)
    {
        return (void*)0;
    }

    if (node->rb_left)
    {
        node = node->rb_left;
        while (node->rb_right)
        {
            node = node->rb_right;
        }

        return (struct rb_node*)node;
    }

    while ((parent = rb_parent(node)) && node == parent->rb_left)
    {
        node = parent;
    }

    return parent;
}

struct rb_node* rb_first(const struct rb_root* root)
{
    struct rb_node* n;

    n = root->rb_node;

    if (!n)
    {
        return (void*)0;
    }

    while (n->rb_left)
    {
        n = n->rb_left;
    }

    return n;
}

struct rb_node* rb_last(const struct rb_root* root)
{
    struct rb_node* n;

    n = root->rb_node;

    if (!n)
    {
        return (void*)0;
    }

    while (n->rb_right)
    {
        n = n->rb_right;
    }

    return n;
}

void rb_replace_node(struct rb_node* old, struct rb_node* replace, struct rb_root* root)
{
    struct rb_node* parent = rb_parent(old);

    if (parent)
    {
        if (old == parent->rb_left)
        {
            parent->rb_left = replace;
        }
        else
        {
            parent->rb_right = replace;
        }
    }
    else
    {
        root->rb_node = replace;
    }

    if (old->rb_left)
    {
        rb_set_parent(old->rb_left, replace);
    }

    if (old->rb_right)
    {
        rb_set_parent(old->rb_right, replace);
    }

    *replace = *old;
}