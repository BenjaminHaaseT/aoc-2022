//
// Created by benjamin haase on 3/31/24.
//
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char* read_line(FILE* fp, size_t* buf_size);
char find_badge(char*, size_t, char*, size_t, char*, size_t);
void populate_table(unsigned char[128], const char*, size_t);

int main(int argc, char** argv)
{
    if (argc == 1)
    {
        fprintf(stderr, "usage: day3 <FILE>");
        exit(1);
    }

    char* fname = argv[1];
    FILE* fp = fopen(fname, "r");

    if (!fp)
    {
        fprintf(stderr, "unable to open file %s\n", fname);
        exit(1);
    }

    char* line;
    size_t len = 0;

    int sum = 0;

    while ((line = read_line(fp, &len)))
    {
        if (len % 2 != 0)
        {
            fprintf(stderr, "%s:%s:%d error reading line from file\n", __FILE__, __FUNCTION__, __LINE__);
            exit(1);
        }

        char* half1 = line;
        char* half2 = (line + (len / 2));

        char* shared = strpbrk(half1, half2);
        if (!shared)
        {
            fprintf(stderr, "%s:%s:%d error finding common character in line %s\n", __FILE__, __FUNCTION__, __LINE__, line);
            exit(1);
        }

        if (65 <= *shared && *shared <= 90)
            sum += (*shared % 'A') + 27;
        else
            sum += (*shared % 'a') + 1;

        free(line);
    }

    if (!feof(fp) || ferror(fp))
        exit(1);

    printf("sum of priorities: %d\n", sum);

    // Now find sum of badge priorities
    rewind(fp);

    char* line1;
    size_t len1 = 0;

    char* line2;
    size_t len2 = 0;

    char* line3;
    size_t len3 = 0;

    sum = 0;

    while (
            ((line1 = read_line(fp, &len1)) &&
            (line2 = read_line(fp, &len2)) &&
            (line3 = read_line(fp, &len3))))
    {
        char badge = find_badge(line1, len1, line2, len2, line3, len3);
        if (65 <= badge && badge <= 90)
            sum += (badge % 'A') + 27;
        else
            sum += (badge % 'a') + 1;

        free(line1);
        free(line2);
        free(line3);
    }

    printf("sum of badge priorities: %d\n", sum);
    return EXIT_SUCCESS;
}

char* read_line(FILE* fp, size_t* buf_size)
{
    // Allocate buffer
    *buf_size = 32;
    size_t buf_idx = 0;
    char* buf = (char*) malloc(*buf_size);
    int cur_char;

//    printf("Before readline loop");

    while ((cur_char = fgetc(fp)) != EOF)
    {
//        printf("%c\n", cur_char);
        if (buf_idx == *buf_size)
        {
            *buf_size *= 2;
            char* new_buf = (char*) realloc(buf, *buf_size);
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
        return NULL;

    char* new_buf = (char*) realloc(buf, buf_idx + 1);
    if (!new_buf)
    {
        fprintf(stderr, "%s:%s:%d error reallocating buffer\n", __FILE__, __FUNCTION__, __LINE__);
        return NULL;
    }

    buf = new_buf;
    buf[buf_idx] = '\0';
    *buf_size = buf_idx;
    return buf;
}



char find_badge(char* buf1, size_t len1, char* buf2, size_t len2, char* buf3, size_t len3)
{
    unsigned char table1[128] = {0};
    unsigned char table2[128] = {0};
    unsigned char table3[128] = {0};

    populate_table(table1, buf1, len1);
    populate_table(table2, buf2, len2);
    populate_table(table3, buf3, len3);

    char found = '\0';

    for (size_t i = 0; i < 128; i++)
        if (table1[i] && table2[i] && table3[i])
        {
            found = (char) i;
            break;
        }

    return found;
}

void populate_table(unsigned char table[128], const char* buf, size_t len)
{
    for (size_t i = 0; i < len; i++)
        if (!table[(int) buf[i]])
            table[(int) buf[i]]++;
}
