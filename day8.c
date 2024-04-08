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
void push(stack *s, int val);
int top(stack *s);
int is_empty(stack *s);

int *read_line(FILE *fp, size_t *buf_sz);
int search_row_forward(int **grid, int ***visited, size_t row_len, size_t col_len, size_t row);
int search_row_reverse(int **grid, int ***visited, size_t row_len, size_t col_len, size_t row);
int search_col_forward(int **grid, int ***visited, size_t row_len, size_t col_len, size_t col);
int search_col_reverse(int **grid, int ***visited, size_t row_len, size_t col_len, size_t col);


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

//    for (size_t i = 0; i < row_len; i++)
//    {
//        printf("%d", grid[i][0]);
//        for (size_t j = 1; j < col_len; j++)
//        {
//            printf(", %d", grid[i][j]);
//        }
//
//        printf("\n");
//    }

    // for keeping track of what cells we have visited
    int **visited = (int **) malloc(row_len * sizeof(int *));
    for (size_t i = 0; i < row_len; i++)
        visited[i] = calloc(col_len, sizeof(int));

    int res = 0;
    for (size_t i = 0; i < row_len; i++)
    {
        int forward_res = search_row_forward(grid, &visited, row_len, col_len, i);
        int reverse_res = search_row_reverse(grid, &visited, row_len, col_len, i);
        printf("row: %zu\n", i);
        printf("\tforward result: %d\n", forward_res);
        printf("\treverse result: %d\n", reverse_res);
    }

    for (size_t j = 0; j < col_len; j++)
        res += search_col_forward(grid, &visited, row_len, col_len, j) +
                search_col_reverse(grid, &visited, row_len, col_len, j);

    printf("total number of trees visible from edges: %d\n", res);

    for (size_t i = 0; i < row_len; i++)
    {
        printf("%d", visited[i][0]);
        for (size_t j = 1; j < col_len; j++)
        {
            printf(", %d", visited[i][j]);
        }
        printf("\n");
    }

    return EXIT_SUCCESS;
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

int search_row_forward(int **grid, int ***visited, size_t row_len, size_t col_len, size_t row)
{
    stack s;
    init_stack(&s);
    int res = 0;
    for (size_t col = 0; col < col_len; col++)
    {
        if ((row == 0 || col == 0 || row == row_len - 1 || col == col_len - 1))
        {
            (*visited)[row][col] = 1;
            res++;
            push(&s, grid[row][col]);
        }
    }
    destroy_stack(&s);
    return res;
}

int search_row_reverse(int **grid, int ***visited, size_t row_len, size_t col_len, size_t row)
{
    stack s;
    init_stack(&s);
    int res = 0;
    for (size_t col = col_len - 1; col > 0; col--)
    {
        if ((is_empty(&s) || top(&s) < grid[row][col]) && !((*visited)[row][col]))
        {
            (*visited)[row][col] = 1;
            res++;
            push(&s, grid[row][col]);
        }
    }

    if ((is_empty(&s) || top(&s) < grid[row][0]) && !((*visited)[row][0]))
    {
        (*visited)[row][0] = 1;
        res++;
        push(&s, grid[row][0]);
    }

    destroy_stack(&s);
    return res;
}

int search_col_forward(int **grid, int ***visited, size_t row_len, size_t col_len, size_t col)
{
    stack s;
    init_stack(&s);
    int res = 0;
    for (size_t row = 0; row < row_len; row++)
    {
        if ((is_empty(&s) || top(&s) < grid[row][col]) && !((*visited)[row][col]))
        {
            (*visited)[row][col] = 1;
            res++;
            push(&s, grid[row][col]);
        }
    }
    destroy_stack(&s);
    return res;
}

int search_col_reverse(int **grid, int ***visited, size_t row_len, size_t col_len, size_t col)
{
    stack s;
    init_stack(&s);
    int res = 0;
    for (size_t row = row_len - 1; row > 0; row--)
    {
        if ((is_empty(&s) || top(&s) < grid[row][col]) && !((*visited)[row][col]))
        {
            (*visited)[row][col] = 1;
            res++;
            push(&s, grid[row][col]);
        }
    }

    if ((is_empty(&s) || top(&s) < grid[0][col]) && !((*visited)[0][col]))
    {
        (*visited)[0][col] = 1;
        res++;
        push(&s, grid[0][col]);
    }

    destroy_stack(&s);
    return res;
}

void init_stack(stack *s)
{
    s->cap = 32;
    s->len = 0;
    s->buf = (int*) malloc(s->cap * sizeof(int));
}

int is_empty(stack *s)
{
    return !s->len;
}

int top(stack *s)
{
    return s->buf[s->len - 1];
}

void push(stack *s, int val)
{
    if (s->len == s->cap)
    {
        // reallocate buffer
        s->cap *= 2;
        int *new_buf = (int*) realloc(s->buf, s->cap * sizeof(int));
        if (!new_buf)
        {
            fprintf(stderr, "%s:%s:%d error reallocating stack buffer\n", __FILE__, __FUNCTION__, __LINE__);
            return;
        }

        s->buf = new_buf;
    }

    s->buf[s->len++] = val;
}

void destroy_stack(stack *s)
{
    free(s->buf);
}