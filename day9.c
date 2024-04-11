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
      fprintf(stderr, "tail does not have manhatten distance less than or equal to 1 from head\n"); \
      exit(3);\
}


int *get_tail_pos(int*, int** , int, int, int*, int*, int*, int*);
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
    size_t tail_coords_cap = 32;
    size_t tail_coords_len = 0;
    int **tail_coords = (int**) malloc(tail_coords_cap * sizeof(int*));
    int *tail = calloc(2, sizeof(int));
    int *head = calloc(2, sizeof(int));
    tail_coords[tail_coords_len++] = tail;
    int row_min = 0, row_max = 0, col_min = 0, col_max = 0;

    while (fscanf(fp, "%c %d\n", &direction, &steps) != EOF)
    {
        printf("direction: %c\n", direction);
        printf("steps: %d\n", steps);
        int d_row, d_col;

        switch (direction)
        {
            case 'L':
                d_row = 0;
                d_col = -1;
                break;
            case 'R':
                d_row = 0;
                d_col = 1;
                break;
            case 'U':
                d_row = -1;
                d_col = 0;
                break;
            case 'D':
                d_row = 1;
                d_col = 0;
                break;
            default:
                fprintf(stderr, "%s:%s:%d invalid direction character read from file\n", __FILE__, __FUNCTION__, __LINE__);
                exit(5);
        }

        for (int i = 0; i < steps; i++)
        {
            int *next_tail = get_tail_pos(tail, &head, d_row, d_col, &row_min, &row_max, &col_min, &col_max);
            if (next_tail)
            {
                if (tail_coords_len == tail_coords_cap)
                {
                    tail_coords_cap *= 2;
                    int **new_tail_coords = (int**) realloc(tail_coords, tail_coords_cap*sizeof(int*));
                    if (!new_tail_coords)
                    {
                        fprintf(stderr, "%s:%s:%d error reallocating buffer for tail coordinates\n", __FILE__, __FUNCTION__, __LINE__);
                        exit(6);
                    }

                    tail_coords = new_tail_coords;
                }

                tail_coords[tail_coords_len++] = next_tail;
                tail = next_tail;
            }

            printf("head: %d, %d\n", head[0], head[1]);
            printf("tail: %d, %d\n", tail[0], tail[1]);
        }
    }

    if (ferror(fp) || !feof(fp))
    {
        fprintf(stderr, "%s:%s:%d error reading file\n", __FILE__, __FUNCTION__, __LINE__);
        exit(4);
    }

    printf("row_min, row_max: %d, %d\n", row_min, row_max);
    printf("col_min, col_max: %d, %d\n", col_min, col_max);

    // populate the grid
    int row_offset = row_min < 0 ? -row_min : 0;
    int col_offset = col_min < 0 ? -col_min: 0;
    int m = row_offset + row_max + 1;
    int n = col_offset + col_max + 1;

    int **visited = (int**) malloc(m * sizeof(int*));
    for (int i = 0; i < m; i++)
        visited[i] = calloc(n, sizeof(int));

    int total = 0;
    for (size_t i = 0; i < tail_coords_len; i++)
    {
        int row = tail_coords[i][0];
        int col = tail_coords[i][1];


        if (!visited[row_offset + row][col_offset + col])
        {
            visited[row_offset + row][col_offset + col]++;
            total++;
        }
    }

    printf("\n-----------------------------------------\n");
    printf("Total positions visited by the tail: %d\n", total);

    free(head);
    head = NULL;

    for (size_t i = 0; i < tail_coords_len; i++)
    {
        free(tail_coords[i]);
        tail_coords[i] = NULL;
    }

    free(tail_coords);
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
    if (dist <= 1 || (dist == 2 && iabs(tail[0], (*head)[0]) == 1 && iabs(tail[1], (*head)[1]) == 1))
        return NULL;

    int *neo_tail = (int*) malloc(2 * sizeof(int));
    if (dist == 2 && iabs(tail[0], (*head)[0]) == 2)
    {
        neo_tail[0] = tail[0] + d_row;
        neo_tail[1] = tail[1];
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

    ASSERT((manhatten_dist(neo_tail[0], neo_tail[1], (*head)[0], (*head)[1]) <= 1));

    *row_min = min(*row_min, neo_tail[0]);
    *row_max = max(*row_max, neo_tail[0]);
    *col_min = min(*col_min, neo_tail[1]);
    *col_max = max(*col_max, neo_tail[1]);
    return neo_tail;
}