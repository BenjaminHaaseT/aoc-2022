//
// Created by benjamin haase on 4/2/24.
//
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    size_t len;
    char** multi_stack;
    size_t* col_len;
    size_t* col_idx;
} MultiStack;

MultiStack init_mstack(size_t len);
int pop_at(MultiStack* mstack, size_t i);
int push_at(MultiStack* mstack, char c, size_t i);
void rmstack(MultiStack* mstack);
int move_crates(MultiStack* mstack, int amt, size_t src, size_t dst);
size_t read_line(FILE* fp, char** buf, size_t* buf_size);
void parse_line(MultiStack* mstack, char* buf, size_t buf_size);


int main(int argc, char** argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "usage: day5 <FILE>\n");
        exit(1);
    }

    char* fname = argv[1];
    FILE* fp = fopen(fname, "r");
    if (!fp)
    {
        fprintf(stderr, "unable to open file\n");
        exit(1);
    }

    size_t buf_size = 0;
    char* buf;
    MultiStack mstack = init_mstack(9);

    while (read_line(fp, &buf, &buf_size))
    {
        printf("%s\n", buf);
        if (buf[0] == '[')
        {
            // Now parse the line
            parse_line(&mstack, buf, buf_size);
        }

        free(buf);
    }

    // reverse multi stack
    rmstack(&mstack);

        for (size_t i = 0; i < mstack.len; i++)
    {
        printf("%zu: %c", i + 1, mstack.multi_stack[i][0]);
        for (size_t j = 1; j < mstack.col_idx[i]; j++)
            printf(", %c", mstack.multi_stack[i][j]);
        printf("\n");
    }

    char tok1[4];
    char tok2[4];
    char tok3[4];
    int quantity = 0, idx1 = 0, idx2 = 0;
    while (fscanf(fp, "%s %d %s %d %s %d\n", tok1, &quantity, tok2, &idx1, tok3, &idx2) != EOF)
    {
        printf("quantity: %d, idx1: %d idx2: %d\n", quantity, idx1 - 1, idx2 - 1);
        printf("col1 idx: %zu\n", mstack.col_idx[idx1 - 1]);
        printf("col2 idx: %zu\n", mstack.col_idx[idx2 - 1]);
        if (move_crates(&mstack, quantity, (size_t) (idx1 - 1), (size_t) (idx2 - 1)))
            exit(1);
    }

    char res[10];
    size_t idx = 0;
    for (size_t i = 0; i < mstack.len; i++)
    {
        if (mstack.col_idx[i])
        {
            res[idx++] = mstack.multi_stack[i][mstack.col_idx[i] - 1];
        }
    }

    res[idx] = '\0';
    printf("%s\n", res);

    return EXIT_SUCCESS;
}


MultiStack init_mstack(size_t len)
{
    char** multi_stack = (char**) malloc(len * sizeof(char*));
    for (size_t i = 0; i < len; i++)
        multi_stack[i] = (char*) malloc(16);

    size_t* multi_stack_col_len  = (size_t*) malloc(len * sizeof(size_t));
    for (size_t i = 0; i < len; i++)
        multi_stack_col_len[i] = 16;

    size_t* multi_stack_col_idx = (size_t*) calloc(len, sizeof(size_t));
    return (MultiStack) {
        .len=len,
        .multi_stack=multi_stack,
        .col_len=multi_stack_col_len,
        .col_idx=multi_stack_col_idx
    };
}

int push_at(MultiStack* mstack, char c, size_t i)
{
    if (mstack->col_len[i] == mstack->col_idx[i])
    {
        mstack->col_len[i] *= 2;
        char* new_col = (char*) realloc(mstack->multi_stack[i], mstack->col_len[i]);
        if (!new_col)
        {
            fprintf(stderr, "%s:%s:%d error reallocating buffer\n", __FILE__, __FUNCTION__, __LINE__);
            return 1;
        }

        mstack->multi_stack[i] = new_col;
    }
    mstack->multi_stack[i][mstack->col_idx[i]++] = c;
    return 0;
}

int pop_at(MultiStack* mstack, size_t i)
{
    if (!mstack->col_idx[i])
        return -1;
    char res = mstack->multi_stack[i][--mstack->col_idx[i]];
    mstack->multi_stack[i][mstack->col_idx[i]] = '\0';
    return res;
}

int move_crates(MultiStack* mstack, int amt, size_t src, size_t dst)
{
    int c;
    while (amt--)
    {
        if ((c = pop_at(mstack, src)) < 0)
        {
            fprintf(stderr, "%s:%s:%d error popping from multi-stack\n", __FILE__, __FUNCTION__, __LINE__);
            return 1;
        }
        if (push_at(mstack, (char) c,  dst))
            return 1;
    }

    return 0;
}

void rmstack(MultiStack* mstack)
{
    for (size_t i = 0; i < mstack->len; i++)
    {
        size_t col_len = mstack->col_idx[i];
        for (size_t j = 0; j < col_len / 2; j++)
        {
            char temp = mstack->multi_stack[i][j];
            mstack->multi_stack[i][j] = mstack->multi_stack[i][col_len - 1 - j];
            mstack->multi_stack[i][col_len - 1 - j] = temp;
        }
    }
}

void parse_line(MultiStack* mstack, char* buf, size_t buf_size)
{
    // We know the format of the data will always have chars starting at index 1 + 4k
    for (size_t i = 1; i < buf_size; i += 4)
    {
        if (isalpha(buf[i]))
        {
            push_at(mstack, buf[i], (i - 1) / 4);
        }
    }
}

size_t read_line(FILE* fp, char** buf, size_t* buf_size)
{
    size_t buf_idx = 0;
    *buf_size = 32;
    *buf = (char*) malloc(*buf_size);
    int cur_char;

    while ((cur_char = fgetc(fp)) != EOF)
    {
        if (buf_idx == *buf_size)
        {
            *buf_size *= 2;
            char* new_buf = (char*) realloc(*buf, *buf_size);
            if (!new_buf)
            {
                fprintf(stderr, "%s:%s:%d error reallocating buffer\n", __FILE__, __FUNCTION__, __LINE__);
                return 0;
            }

            *buf = new_buf;
        }

        // Break once new line has been found
        if (cur_char == '\n')
            break;

        (*buf)[buf_idx++] = (char) cur_char;
    }

    if (ferror(fp))
    {
        fprintf(stderr, "%s:%s:%d error reading from file\n", __FILE__, __FUNCTION__, __LINE__);
        return 0;
    }
    else if (!buf_idx)
    {
        return 0;
    }

    char* new_buf = (char*) realloc(*buf, buf_idx + 1);
    if (!new_buf)
    {
        fprintf(stderr, "%s:%s:%d error reallocating buffer\n", __FILE__, __FUNCTION__, __LINE__);
        return 0;
    }

    *buf = new_buf;
    (*buf)[buf_idx] = '\0';
    *buf_size = buf_idx;
    return buf_idx;
}
