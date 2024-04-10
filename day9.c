//
// Created by benjamin haase on 4/10/24.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define ASSERT(condition) \
if (!condition)           \
{                         \
      fprintf(stderr, "tail does not have manhatten distance of 1 from head"); \
      exit(3);\
\
}



int *get_tail_pos(int*, int**, int, int, int*, int*, int*, int*);
int max(int a, int b);
int min(int a, int b);
int iabs(int a, int b);
int manhatten_dist(int x1, int y1, int x2, int y2);

int main(int argc, char **argv)
{
    if (argc == 1)
    {
        fprintf(stderr, "usage: day9: <FILE>\n");
        exit(1);
    }

    char *fname = argv[1];
    FILE *fp = fopen(fname, "r");
    if (!fp)
    {
        fprintf(stderr, "%s:%s:%d unable to open file %s\n", __FILE__, __FUNCTION__, __LINE__, fname);
        exit(2);
    }

    char direction;
    int steps;

    while (fscanf(fp, "%c %d\n", &direction, &steps) != EOF)
    {
        printf("direction: %c\n", direction);
        printf("steps: %d\n", steps);
    }

    if (ferror(fp) || !feof(fp))
    {
        fprintf(stderr, "%s:%s:%d error reading file\n", __FILE__, __FUNCTION__, __LINE__);
        exit(4);
    }

    return 0;
}

int max(int a, int b)
{
    return a >= b ? a : b;
}

int min(int a, int b)
{
    return a <= b ? a : b;
}

int iabs(int a, int b)
{
    return a - b >= 0 ? a - b : b - a;
}

int manhatten_dist(int x1, int y1, int x2, int y2)
{
    return iabs(x1, x2) + iabs(y1, y2);
}


int *get_tail_pos(int* tail, int** head, int d_row, int d_col, int* row_min, int* row_max, int* col_min, int* col_max)
{
    (*head)[0] += d_row;
    (*head)[1] += d_col;
    int dist = manhatten_dist(tail[0], tail[1], (*head)[0], (*head)[1]);
    if (dist == 1 || (dist == 2 && iabs(tail[0], (*head)[0]) == 1 && iabs(tail[1], (*head)[1]) == 1))
        return NULL;

    int *neo_tail = (int*) malloc(2 * sizeof(int));
    if (dist == 2 && iabs(tail[0], (*head)[0]) == 2)
    {
        neo_tail[0] = tail[0] + d_row;
        neo_tail[0] = tail[1];
    }
    else if (dist == 2 && iabs(tail[1], (*head)[1]) == 2)
    {
        neo_tail[0] = tail[0];
        neo_tail[1] = tail[1] + d_col;
    }
    else
    {
        if ((*head)[0] > tail[0])
            neo_tail[0] = tail[0] + 1;
        else
            neo_tail[0] = tail[0] - 1;
        if ((*head)[1] > tail[1])
            neo_tail[1] = tail[1] + 1;
        else
            neo_tail[1] = tail[1] - 1;
    }

    ASSERT(manhatten_dist(neo_tail[0], neo_tail[1], (*head)[0], (*head)[1]));

    *row_min = min(*row_min, neo_tail[0]);
    *row_max = max(*row_max, neo_tail[0]);
    *col_min = min(*col_min, neo_tail[1]);
    *col_max = max(*col_max, neo_tail[1]);
    return neo_tail;
}