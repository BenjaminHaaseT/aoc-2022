//
// Created by benjamin haase on 4/13/24.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>

void render_pixel(int reg, int cycle)
{
    if (reg - 1 == (cycle % 40) || reg == (cycle % 40) || reg + 1 == (cycle % 40))
        printf("#");
    else
        printf(".");
}

void increment_cycle(int *cycle)
{
    (*cycle)++;
    if (*cycle % 40 == 0)
        printf("\n");
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "USAGE: day10 <FILE>\n");
        exit(1);
    }

    char *fname = argv[1];
    FILE *fp = fopen(fname, "r");
    if (!fp)
    {
        fprintf(stderr, "%s:%s:%d unable to open file %s\n", __FILE__, __FUNCTION__, __LINE__, fname);
        exit(2);
    }

    int cycle = 0;
    int reg = 1;
    char instruction[5];
    int val;
    int signal = 0;

    while (fscanf(fp, "%s %d\n", instruction, &val) != EOF)
    {
        if (!strcmp(instruction, "noop"))
        {
            cycle++;
            if (cycle == 20 || cycle == 60 || cycle == 100 || cycle == 140 || cycle == 180 || cycle == 220)
            {
                signal += cycle * reg;
            }
        } else
        {
            cycle++;
            if (cycle == 20 || cycle == 60 || cycle == 100 || cycle == 140 || cycle == 180 || cycle == 220)
            {
                signal += cycle * reg;
            }
            cycle++;
            if (cycle == 20 || cycle == 60 || cycle == 100 || cycle == 140 || cycle == 180 || cycle == 220)
            {
                signal += cycle * reg;
            }
            reg += val;
        }
    }

    if (ferror(fp) || !feof(fp))
    {
        fprintf(stderr, "%s:%s:%d error reading file\n", __FILE__, __FUNCTION__, __LINE__);
        exit(3);
    }

    printf("signal sum: %d\n", signal);
    rewind(fp);

    cycle = 0;
    reg = 1;

    while (fscanf(fp, "%s %d\n", instruction, &val) != EOF)
    {
        render_pixel(reg, cycle);
        increment_cycle(&cycle);
        if (!strcmp(instruction, "addx"))
        {
            render_pixel(reg, cycle);
            increment_cycle(&cycle);
            reg += val;
        }
    }


    return EXIT_SUCCESS;
}
