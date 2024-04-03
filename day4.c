//
// Created by benjamin haase on 4/1/24.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

char* read_line(FILE*, size_t*);
void get_range_endpoints(char* range, long long* start, long long* end);

int main(int argc, char** argv)
{
    // Ensure file has been passed
    if (argc < 2)
    {
        fprintf(stderr, "usage: day4 <FILE>\n");
        exit(1);
    }

    // attempt to open file
    char* fname = argv[1];
    FILE* fp = fopen(fname, "r");

    if (!fp)
    {
        fprintf(stderr, "unable to open file %s\n", fname);
        exit(1);
    }

    char* line;
    size_t line_len = 0;
    int count1 = 0;
    int count2 = 0;


    while ((line = read_line(fp, &line_len)))
    {
        char* range1 = strtok(line, ",");
        char* range2 = strtok(NULL, ",");

        long long range1_start, range1_end;
        get_range_endpoints(range1, &range1_start, &range1_end);

        long long range2_start, range2_end;
        get_range_endpoints(range2, &range2_start, &range2_end);

        if (errno == ERANGE)
        {
            exit(1);
        }

        if (
                (range1_start <= range2_start && range2_end <= range1_end) ||
                (range2_start <= range1_start && range1_end <= range2_end))
        {
            count1++;
        }

        if (
                (range1_start <= range2_start && range2_start <= range1_end) ||
                (range2_start <= range1_start && range1_start <= range2_end)
                )
        {
            count2++;
        }
    }

    printf("count1: %d\n", count1);
    printf("count2: %d\n", count2);

    return EXIT_SUCCESS;
}

char* read_line(FILE* fp, size_t* buf_len)
{
    *buf_len = 32;
    size_t buf_idx = 0;
    char* buf = (char*) malloc(*buf_len);
    int cur_char = 0;

    while ((cur_char = fgetc(fp)) != EOF)
    {
        if (buf_idx == *buf_len)
        {
            *buf_len *= 2;
            char* new_buf = realloc(buf, *buf_len);

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
        fprintf(stderr, "%s:%s:%d error reading from file\n", __FILE__, __FUNCTION__, __LINE__);
        return NULL;
    }
    else if (!buf_idx)
    {
        return NULL;
    }

    char* new_buf = realloc(buf, buf_idx + 1);
    if (!new_buf)
    {
        fprintf(stderr, "%s:%s:%d error reallocating buffer\n", __FILE__, __FUNCTION__, __LINE__);
        return NULL;
    }

    buf = new_buf;
    buf[buf_idx] = '\0';
    *buf_len = buf_idx;
    return buf;
}

void get_range_endpoints(char* range, long long* start, long long* end)
{
    char* start_s = strtok(range, "-");
    char* start_s_end_ptr;
    *start = strtoll(start_s, &start_s_end_ptr, 10);
    if (*start_s_end_ptr != '\0' || errno == ERANGE)
    {
        fprintf(stderr, "%s:%s:%d error converting to long long\n", __FILE__, __FUNCTION__, __LINE__);
        return;
    }


    char* end_s = strtok(NULL, "-");
    char* end_s_end_ptr;
    *end = strtoll(end_s, &end_s_end_ptr, 10);
    if (*end_s_end_ptr != '\0' || errno == ERANGE)
    {
        fprintf(stderr, "%s:%s:%d error converting to long long\n", __FILE__, __FUNCTION__, __LINE__);
        return;
    }
}
