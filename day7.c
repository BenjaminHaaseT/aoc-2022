//
// Created by benjamin haase on 4/4/24.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


typedef struct {
    char *name;
    struct DirNode *parent;
    struct DirNode **children;
    size_t children_sz;
    unsigned long long sz;
} DirNode;

void init_dir_node(DirNode *dn, DirNode *parent, char *name);
void display_dir_node(DirNode *dn);
size_t read_line(FILE *fp, char **buf, size_t *buf_sz);
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

    DirNode root;
    init_dir_node(&root, NULL, "\\");

    printf("name: %s\nparent: %p\nchildren_sz: %zu\n", root.name, root.parent, root.children_sz);
    display_dir_node(&root);


    return EXIT_SUCCESS;
}

size_t read_line(FILE *fp, char **buf, size_t *buf_sz)
{
    size_t buf_idx = 0;
    int cur_char;

    while ((cur_char = fgetc(fp)) != EOF)
    {
        if (buf_idx == *buf_sz)
        {
            *buf_sz *= 2;
            char *new = (char*) realloc(*buf, *buf_sz);
            if (!new)
            {
                fprintf(stderr, "%s:%s:%d error reallocating buffer\n", __FILE__, __FUNCTION__, __LINE__);
                return 0;
            }

            *buf = new;
        }

        if (cur_char == '\n')
            break;

        (*buf)[buf_idx++] = (char) cur_char;
    }

    if (ferror(fp))
    {
        fprintf(stderr, "%s:%s:%d error reading from file\n", __FILE__, __FUNCTION__, __LINE__);
        return 0;
    } else if (!buf_idx)
        return 0;

    if (buf_idx == *buf_sz)
    {
        char *new = (char*) realloc(*buf, buf_idx + 1);
        if (!new)
        {
            fprintf(stderr, "%s:%s:%d error reallocating buffer\n", __FILE__, __FUNCTION__, __LINE__);
            return 0;
        }
        *buf = new;
    }

    (*buf)[buf_idx] = '\0';
    *buf_sz = buf_idx;
    return buf_idx;
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
    dn->sz = 0;
}

void display_dir_node(DirNode *dn)
{
    printf("name: %s\nsize: %llu\nparent: %p\nchildren size: %zu\n", dn->name, dn->sz, dn->parent, dn->children_sz);
}

