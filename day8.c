//
// Created by benjamin haase on 4/8/24.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

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
int scenic_score(int **grid, size_t row_len, size_t col_len, size_t row, size_t col, FILE *fp);
void read_scenic_score_output();


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

    // for keeping track of what cells we have visited
    int **visited = (int **) malloc(row_len * sizeof(int *));
    for (size_t i = 0; i < row_len; i++)
        visited[i] = calloc(col_len, sizeof(int));

    int res = 0;
    for (size_t i = 0; i < row_len; i++)
    {
        int forward_res = search_row_forward(grid, &visited, row_len, col_len, i);
        int reverse_res = search_row_reverse(grid, &visited, row_len, col_len, i);
        res += forward_res + reverse_res;
    }

    for (size_t j = 0; j < col_len; j++)
        res += search_col_forward(grid, &visited, row_len, col_len, j) +
                search_col_reverse(grid, &visited, row_len, col_len, j);

    printf("total number of trees visible from edges: %d\n", res);

    int max_score = INT_MIN;
    FILE *fp_out = fopen("output.txt", "w");

    if (!fp_out)
    {
        fprintf(stderr, "%s:%s:%d error opening output file\n", __FILE__, __FUNCTION__, __LINE__);
        exit(4);
    }

    for (size_t i = 1; i < row_len - 1; i++)
    {
        for (size_t j = 1; j < col_len - 1; j++)
        {
            fprintf(fp_out, "(%zu, %zu): %d\n", i, j, grid[i][j]);
            int cur_score = scenic_score(grid, row_len, col_len, i, j, fp_out);

            if (cur_score > max_score)
                max_score = cur_score;
        }
    }

    printf("The maximum scenic score: %d\n", max_score);





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
        if (is_empty(&s) || top(&s) < grid[row][col])
        {
            push(&s, grid[row][col]);
            if (!((*visited)[row][col]))
            {
                (*visited)[row][col] = 1;
                res++;
            }
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
        if (is_empty(&s) || top(&s) < grid[row][col])
        {
            push(&s, grid[row][col]);
            if (!((*visited)[row][col]))
            {
                (*visited)[row][col] = 1;
                res++;
            }
        }
    }

    if (is_empty(&s) || top(&s) < grid[row][0])
    {
        push(&s, grid[row][0]);
        if (!((*visited)[row][0]))
        {
            (*visited)[row][0] = 1;
            res++;
        }
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
        if (is_empty(&s) || top(&s) < grid[row][col])
        {
            push(&s, grid[row][col]);
            if (!((*visited)[row][col]))
            {
                (*visited)[row][col] = 1;
                res++;
            }
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
        if (is_empty(&s) || top(&s) < grid[row][col])
        {
            push(&s, grid[row][col]);
            if (!((*visited)[row][col]))
            {
                (*visited)[row][col] = 1;
                res++;
            }
        }
    }

    if (is_empty(&s) || top(&s) < grid[0][col])
    {
        push(&s, grid[0][col]);
        if (!((*visited)[0][col]))
        {
            res++;
            (*visited)[0][col]++;
        }
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

int scenic_score(int **grid, size_t row_len, size_t col_len, size_t row, size_t col, FILE *fp)
{
    if (row == 0 || col == 0 || row == row_len - 1 || col == col_len - 1)
        return 0;

    int score = 1;

    size_t row_idx = row - 1;
    while (row_idx && grid[row_idx][col] < grid[row][col])
        row_idx--;

    int temp_score = (int) (row - row_idx);
    score *= temp_score;

    row_idx = row + 1;
    while (row_idx < row_len - 1 && grid[row_idx][col] < grid[row][col])
        row_idx++;

    temp_score = (int) (row_idx - row);
    score *= temp_score;

    size_t col_idx = col - 1;
    while (col_idx && grid[row][col_idx] < grid[row][col])
        col_idx--;

    temp_score = (int) (col - col_idx);
    score *= temp_score;

    col_idx = col + 1;
    while (col_idx < col_len - 1 && grid[row][col_idx] < grid[row][col])
        col_idx++;

    temp_score = (int) (col_idx - col);
    score *= temp_score;

    fprintf(fp, "\ttotal score: %d\n", score);

    return score;
}
