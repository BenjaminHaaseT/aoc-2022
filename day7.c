//
// Created by benjamin haase on 4/4/24.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <errno.h>
#include <limits.h>


typedef struct {
    char *name;
    struct DirNode *parent;
    struct DirNode **children;
    size_t children_sz;
    size_t idx;
    unsigned long long sz;
} DirNode;

typedef struct {
    DirNode **buf;
    size_t cap;
    size_t len;
} DirNodeStack;

void init_dir_node(DirNode *dn, DirNode *parent, char *name);
void display_dir_node(DirNode *dn);
DirNode *find_child(DirNode *root, char *target);
void add_child(DirNode *root, DirNode *child);
unsigned long long walk_tree(DirNode *root, unsigned long long *res);
unsigned long long total_tree_size(DirNode *root);
unsigned long long find_min_directory(DirNode *root, unsigned long long total_size, unsigned long long *min_size);

char *read_line(FILE *fp, size_t *len);
char *get_node_name(char *buf);
void get_command(char *buf, char **cmd, char **target);


void init_dir_node_stack(DirNodeStack *stack);
bool is_empty(DirNodeStack *stack);
void push(DirNodeStack *stack, DirNode *node);
DirNode *pop(DirNodeStack *stack);
DirNode *top(DirNodeStack *stack);


//int populate_dir_tree(FILE *fp, DirNode *root);



int main(int argc, char **argv)
{
    if (argc == 1)
    {
        fprintf(stderr, "usage: day7 <FILE>\n");
        exit(1);
    }

    char *fname = argv[1];
    FILE *fp = fopen(fname, "r");

    if (!fp)
    {
        fprintf(stderr, "unable to open file %s\n", fname);
        exit(1);
    }

    // Read first line
    size_t buf_sz;
    char *buf = read_line(fp, &buf_sz);
    if (!buf)
    {
        fprintf(stderr, "error reading from file \n");
        exit(1);
    }

    char *root_name = get_node_name(buf);
    DirNode *root = (DirNode*) malloc(sizeof(DirNode));
    init_dir_node(root, NULL, root_name);

    DirNodeStack stack;
    init_dir_node_stack(&stack);
    push(&stack, root);
    DirNode *cur = root;

    while ((buf = read_line(fp, &buf_sz)))
    {
        if (buf[0] == '$')
        {
            char *cmd;
            char *target;
            get_command(buf, &cmd, &target);

            if (cmd[0] == 'c')
            {
                if (!strcmp(target, ".."))
                {
                    (void) pop(&stack);
                    cur = top(&stack);
                }
                else
                {
                    DirNode *next = find_child(cur, target);
                    if (!next)
                    {
                        fprintf(stderr, "%s:%s:%d error locating child %s in node %s\n", __FILE__, __FUNCTION__, __LINE__, target, cur->name);
                        exit(1);
                    }

                    push(&stack, next);
                    cur = next;
                }
            }
        }
        else if (buf[0] == 'd')
        {

            char *temp = strtok(buf, " ");
            (void) temp;
            char *dir_name = strtok(NULL, " ");

            DirNode *child = (DirNode *) malloc(sizeof(DirNode));
            init_dir_node(child, cur, dir_name);

            // add the new node to cur
            add_child(cur, child);
        }
        else if (isdigit(buf[0]))
        {

            unsigned long long fsize = strtoull(buf, NULL, 10);
            if (!fsize || errno == ERANGE)
            {
                fprintf(stderr, "%s:%s:%d error converting file size\n", __FILE__, __FUNCTION__, __LINE__);
                exit(1);
            }

            cur->sz += fsize;
        }
    }

    unsigned long long res = 0;
    (void) walk_tree(root, &res);

    printf("%llu\n", res);

    unsigned long long total_size = total_tree_size(root);
    unsigned long long dir = ULONG_LONG_MAX;
    (void) find_min_directory(root, total_size, &dir);

    printf("%llu\n", dir);

    return EXIT_SUCCESS;
}

char *read_line(FILE *fp, size_t *len)
{
    *len = 32;
    char *buf = (char*) malloc(*len);
    size_t buf_idx = 0;
    int cur_char;

    while ((cur_char = fgetc(fp)) != EOF)
    {
        if (buf_idx == *len)
        {
            *len *= 2;
            char *new_buf = (char*) realloc(buf, *len);
            if (!new_buf)
            {
                fprintf(stderr, "%s:%s:%d error reallocating buffer\n", __FILE__, __FUNCTION__, __LINE__);
                return NULL;
            }
            buf = new_buf;
        }

        if (cur_char == '\n')
            break;

        buf[buf_idx++] = (char) cur_char;
    }

    if (ferror(fp))
    {
        fprintf(stderr, "%s:%s:%d error reading file\n", __FILE__, __FUNCTION__, __LINE__);
        return NULL;
    }
    else if (!buf_idx)
    {
        return NULL;
    }

    char *new_buf = (char*) realloc(buf, buf_idx + 1);
    if (!new_buf)
    {
        fprintf(stderr, "%s:%s:%d error reallocating buffer\n", __FILE__, __FUNCTION__, __LINE__);
        return NULL;
    }

    buf = new_buf;
    buf[buf_idx] = '\0';
    *len = buf_idx;
    return buf;
}

void init_dir_node(DirNode *dn, DirNode *parent, char *name)
{
//    size_t name_len = strlen(name);
//    dn->name = (char*) malloc(name_len + 1);
//    strncpy(dn->name, name, name_len);
//    dn->name[name_len] = '\0';
    dn->name = name;
    dn->parent = (struct DirNode *) parent;
    dn->children_sz = 16;
    dn->children = (struct DirNode **) malloc(16 * sizeof(struct DirNode *));
    dn->idx = 0;
    dn->sz = 0;
}

DirNode *find_child(DirNode *root, char *target)
{
    for (size_t i = 0; i < root->idx; i++)
    {
        char *name = ((DirNode *) (root->children[i]))->name;
        if (!strcmp(name, target))
            return ((DirNode *) (root->children[i]));
    }
    return NULL;
}

void add_child(DirNode *root, DirNode *child)
{
    if (root->idx == root->children_sz)
    {
        root->children_sz *= 2;
        struct DirNode **new_children = (struct DirNode**) realloc(root->children, root->children_sz);
        if (!new_children)
        {
            fprintf(stderr, "%s:%s:%d error reallocating buffer\n", __FILE__, __FUNCTION__, __LINE__);
            return;
        }

        root->children = new_children;
    }

    root->children[root->idx++] = (struct DirNode *) child;
}

unsigned long long walk_tree(DirNode *root, unsigned long long *res)
{
    unsigned long long root_sz = root->sz;
    for (size_t i = 0; i < root->idx; i++)
    {
        root_sz += walk_tree((DirNode*) (root->children[i]), res);
    }

    if (root_sz <= 100000)
        *res += root_sz;

    return root_sz;
}

unsigned long long total_tree_size(DirNode *root)
{
    unsigned long long res = root->sz;
    for (size_t i = 0; i < root->idx; i++)
    {
        res += total_tree_size((DirNode *) root->children[i]);
    }
    return res;
}

unsigned long long find_min_directory(DirNode *root, unsigned long long total_size, unsigned long long *min_size)
{
    unsigned long long res = root->sz;
    for (size_t i = 0; i < root->idx; i++)
        res += find_min_directory((DirNode *) root->children[i], total_size, min_size);

    if (70000000 + res - total_size >= 30000000 && res < *min_size)
        *min_size = res;

    return res;
}

void display_dir_node(DirNode *dn)
{
    printf("name: %s\nsize: %llu\nparent: %p\nchildren size: %zu\n", dn->name, dn->sz, dn->parent, dn->children_sz);
}

void init_dir_node_stack(DirNodeStack *stack)
{
    stack->cap = 32;
    stack->buf = (DirNode**) malloc(32 * sizeof(DirNode*));
    stack->len = 0;
}

bool is_empty(DirNodeStack *stack)
{
    return !stack->len;
}

void push(DirNodeStack *stack, DirNode *node)
{
    if (stack->len == stack->cap)
    {
        stack->cap *= 2;
        DirNode **new_buf = (DirNode**) realloc(stack->buf, stack->cap * sizeof(DirNode*));
        if (!new_buf)
        {
            fprintf(stderr, "%s:%s:%d error reallocating buffer\n", __FILE__, __FUNCTION__, __LINE__);
            return;
        }

        stack->buf = new_buf;
    }

    stack->buf[stack->len++] = node;
}

DirNode *pop(DirNodeStack *stack)
{
    if (is_empty(stack))
        return NULL;
    DirNode *res = stack->buf[stack->len - 1];
    stack->buf[--stack->len] = NULL;
    return res;
}

DirNode *top(DirNodeStack *stack)
{
    if (!is_empty(stack))
    {
        DirNode* top = stack->buf[stack->len - 1];
        return top;
    }
    return NULL;
}

char *get_node_name(char *buf)
{
    char *tok1 =  strtok(buf, " ");
    (void) tok1;
    char *tok2 = strtok(NULL, " ");
    (void) tok2;
    char *tok3 = strtok(NULL, " ");
    return tok3;
}

void get_command(char *buf, char **cmd, char **target)
{
    char *tok1 = strtok(buf, " ");
    (void) tok1;
    *cmd = strtok(NULL, " ");
    *target = strtok(NULL, " ");
}




