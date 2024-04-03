//
// Created by benjamin haase on 3/29/24.
//
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>

int find_max_cal(FILE *fp);
int read_cur_cal(FILE *fp);
int find_top_3_cal(FILE *fp);

int main(int argc, char **argv)
{
    if (argc == 1)
    {
        fprintf(stderr, "usage: day1 FILE\n");
        exit(1);
    }

    char *fname = argv[1];
    FILE *fp = fopen(fname, "r");

    if (!fp)
    {
        fprintf(stderr, "unable to open file %s\n", fname);
        exit(1);
    }

    int max_cal = find_max_cal(fp);
    if (max_cal < 0)
        exit(1);

    printf("elf with most calories: %d\n", max_cal);
    rewind(fp);

    int top_3_cal = find_top_3_cal(fp);
    printf("top 3 elves calories: %d\n", top_3_cal);


    return EXIT_SUCCESS;
}

int find_max_cal(FILE *fp)
{
    int max_cal_so_far = 0;
    int cur_cal;

    while (!feof(fp) && !ferror(fp))
    {
        cur_cal = read_cur_cal(fp);
        max_cal_so_far = max_cal_so_far > cur_cal ? max_cal_so_far : cur_cal;
    }

    if (ferror(fp))
        return -1;

    return max_cal_so_far;
}

int cmp(const void *a, const void *b)
{
    return *(int *)a - *(int *)b;
}

int find_top_3_cal(FILE *fp)
{
    size_t calories_buf_sz = 64;
    size_t calories_buf_idx = 0;
    int *calories_buf = malloc(calories_buf_sz * sizeof(int));

    while (!feof(fp))
    {
        if (calories_buf_idx == calories_buf_sz)
        {
            calories_buf_sz *= 2;
            int *new_calories_buf = realloc(calories_buf, calories_buf_sz * sizeof(int));
            if (!new_calories_buf)
            {
                fprintf(stderr, "%s:%s:%d error reallocating buffer\n", __FILE__, __FUNCTION__, __LINE__);
                return -1;
            }

            calories_buf = new_calories_buf;
        }

        calories_buf[calories_buf_idx++] = read_cur_cal(fp);
    }

    // check we successfully read the entire file and not encountered any errors
    if (ferror(fp) || !feof(fp))
        return -1;

    // resize buffer
    int *new_calories_buf = realloc(calories_buf, calories_buf_idx * sizeof(int));
    if (!new_calories_buf)
    {
        fprintf(stderr, "%s:%s:%d error reallocating buffer\n", __FILE__, __FUNCTION__, __LINE__);
        return -1;
    }

    calories_buf = new_calories_buf;
    qsort(calories_buf, calories_buf_idx, sizeof(int), cmp);

    return calories_buf[calories_buf_idx - 1] + calories_buf[calories_buf_idx - 2] + calories_buf[calories_buf_idx - 3];
}

int read_cur_cal(FILE *fp)
{
    int total_cal = 0;
    int cur_char;

    while ((cur_char = fgetc(fp)) != EOF)
    {
        int cur_cal = 0;
        while (cur_char != '\n' && cur_char != EOF)
        {
            cur_cal *= 10;
            cur_cal += cur_char % '0';
            cur_char = fgetc(fp);
        }

        if (ferror(fp))
        {
            fprintf(stderr, "%s:%s:%d error reading from file\n", __FILE__, __FUNCTION__, __LINE__);
            return -1;
        }

        if (!cur_cal)
            break;

        total_cal += cur_cal;
    }

    if (ferror(fp))
    {
        fprintf(stderr, "%s:%s:%d error reading from file\n", __FILE__, __FUNCTION__, __LINE__);
        return -1;
    }

    return total_cal;
}