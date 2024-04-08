//
// Created by benjamin haase on 4/8/24.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    int *buf;
    size_t cap;
    size_t len;
} stack;

void init_stack(stack *s);
void destroy_stack(stack *s);
void push(stack *s);
int pop(stack *s);
int is_empty(stack *s);

int *read_line(FILE *fp, size_t *buf_sz);

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "usage: day8 <FILE>\n");
        exit(1);
    }

    char *fname = argv[1];
    FILE *fp = fopen(fname, "r");
    if (!fp)
    {
        fprintf(stderr, "unable to open file %s\n", fname);
        exit(2);
    }

    // read the numbers into a grid
    size_t row_cap = 256;
    size_t grid_idx = 0;
    int **grid = (int **) malloc(row_cap * sizeof(int*));
    int *row;
    size_t row_sz;
    size_t row_len = 0;
    size_t col_len = 0;

    while ((row = read_line(fp, &row_sz)))
    {
        if (grid_idx == row_cap)
        {
            row_cap *= 2;
            int **new_grid = (int **) realloc(grid, row_cap * sizeof(int*));
            if (!new_grid)
            {
                fprintf(stderr, "%s:%s:%d error reallocating new grid\n", __FILE__, __FUNCTION__, __LINE__);
                exit(3);
            }

            grid = new_grid;
        }

        grid[grid_idx++] = row;
        // check for setting the length of the columns
        if (!col_len)
            col_len = row_sz;
    }

    row_len = grid_idx;

    for (size_t i = 0; i < row_len; i++)
    {
        printf("%d", grid[i][0]);
        for (size_t j = 1; j < col_len; j++)
        {
            printf(", %d", grid[i][j]);
        }

        printf("\n");
    }
}

int *read_line(FILE *fp, size_t *buf_sz)
{
    size_t buf_idx = 0;
    *buf_sz = 32;
    int *buf = (int *) malloc(*buf_sz * sizeof(int));
    int cur_char;

    while ((cur_char = fgetc(fp)) != EOF)
    {
        if (buf_idx == *buf_sz)
        {
            *buf_sz *= 2;
            int *new_buf = (int *) realloc(buf, *buf_sz * sizeof(int));
            if (!new_buf)
            {
                fprintf(stderr, "%s:%s:%d error reallocating buffer\n", __FILE__, __FUNCTION__, __LINE__);
                return NULL;
            }

            buf = new_buf;
        }

        if (cur_char == '\n')
            break;

        buf[buf_idx++] = cur_char % '0';
    }

    if (ferror(fp))
    {
        fprintf(stderr, "%s:%s:%d error reading from file\n", __FILE__, __FUNCTION__, __LINE__);
        return NULL;
    }
    else if (!buf_idx)
    {
        // we are at the end of the file
        return NULL;
    }

    int *resized_buf = (int *) realloc(buf, buf_idx * sizeof(int));
    if (!resized_buf)
    {
        fprintf(stderr, "%s:%s:%d error reallocating buffer\n", __FILE__, __FUNCTION__, __LINE__);
        return NULL;
    }

    buf = resized_buf;
    *buf_sz = buf_idx;
    return buf;
}