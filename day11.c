#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <ctype.h>
#include <limits.h>

typedef struct {
    int *buf;
    size_t cap;
    size_t head;
    size_t tail;
} queue_t;

void queue_t_init(queue_t *q)
{
    q->buf = (int*)malloc(16*sizeof(int));
    q->cap = 16;
    q->head = 0;
    q->tail = 0;
}

void queue_t_enqueue(queue_t *q, int val)
{
    q->buf[q->tail++] = val;
    q->tail %= q->cap;
    if (q->tail == q->head)
    {
        // reallocate the queue
        size_t new_cap = q->cap * 2;
        int *new_buf = (int*)malloc(new_cap*sizeof(int));
        if (!new_buf)
        {
            fprintf(stderr, "%s:%s:%d error reallocating buffer for queue\n", __FILE__, __FUNCTION__, __LINE__);
            exit(1);
        }
        // copy elements from current buffer to new buffer
        size_t i = 0;
        new_buf[i++] = q->buf[q->head++];
        q->head %= q->cap;
        while (q->head != q->tail)
        {
            new_buf[i++] = q->buf[q->head++];
            q->head %= q->cap;
        }
        free(q->buf);
        q->buf = new_buf;
        q->cap = new_cap;
        q->head = 0;
        q->tail = i; 
    }
}

int queue_t_dequeue(queue_t *q)
{
    int res = q->buf[q->head];
    q->buf[q->head++] = (int) '\0';
    q->head %= q->cap;
    return res;
}

bool queue_t_is_empty(queue_t *q)
{
    return q->head == q->tail;
}

typedef struct {
    queue_t *items;
    char *op;
    size_t op_len;
    int test;
    size_t test_len;
    size_t *adj;
} monkey_info_t;


char *read_line(FILE *fp, size_t *sz)
{
    *sz = 16;
    size_t len = 0;
    char *buf = (char*)malloc(*sz);
    int cur_char;

    while ((cur_char = fgetc(fp)) != EOF)
    {
        if (len == *sz)
        {
            *sz *= 2;
            char *new_buf = (char*)realloc(buf, *sz);
            if (!new_buf)
            {
                fprintf(stderr, "%s:%s:%d error reallocating buffer: %s\n", __FILE__, __FUNCTION__, __LINE__, strerror(errno));
                free(buf);
                return NULL;
            }
            buf = new_buf;
        }
        if (cur_char == '\n')
            break;
        buf[len++] = (char) cur_char;
    }

    if (ferror(fp))
    {
        fprintf(stderr, "%s:%s:%d error reading from file: %s\n", __FILE__, __FUNCTION__, __LINE__, strerror(errno));
        return NULL;
    }
    else if (!len)
    {
        return NULL;
    }

    char *new_buf = (char*)realloc(buf, len + 1);
    if (!new_buf)
    {
        fprintf(stderr, "%s:%s:%d error reallocating buffer: %s\n", __FILE__, __FUNCTION__, __LINE__, strerror(errno));
        free(buf);
        return NULL;
    }
    buf = new_buf;
    buf[len] = '\0';
    *sz = len;
    return buf;
}

int parse_items(queue_t *q, char *items)
{
    for (char *tok = strtok(items, ","); tok; tok = strtok(NULL, ","))
    {
        char *end;
        int item = (int) strtol(tok, &end, 10);
        if (!strcmp(end, tok))
        {
            fprintf(stderr, "%s:%s:%d error parsing items\n", __FILE__, __FUNCTION__, __LINE__);
            return 1;
        }
        queue_t_enqueue(q, item);
    }
    return 0;
}

int read_monkey(FILE *fp, monkey_info_t *m)
{
    char *line;
    size_t line_sz;
    // read the first line, then discard
    line = read_line(fp, &line_sz);
    if (!line)
        return 1;
    free(line);

    // parse items
    line = read_line(fp, &line_sz);
    char *label = strtok(line, ":");
    (void) label;
    // get remaining items in the string
    char *items = strtok(NULL, ":");
    m->items = (queue_t *)malloc(sizeof(queue_t));
    queue_t_init(m->items);
    if (parse_items(m->items, items))
    {
        fprintf(stderr, "%s:%s:%d unable to parse monkey items\n", __FILE__, __FUNCTION__, __LINE__);
        return 1;
    }

    // parse operation
    line = read_line(fp, &line_sz);
    label = strtok(line, "=");
    (void) label;
    char *operation = strtok(NULL, "=");
    if (!operation)
    {
        fprintf(stderr, "%s:%s:%d unable to parse monkey operation\n", __FILE__, __FUNCTION__, __LINE__);
        return 1;
    }
    if (operation[0] == ' ')
        operation++;
    m->op = operation;
    m->op_len = strlen(operation);

    // parse test criteria
    line = read_line(fp, &line_sz);
    label = strtok(line, ":");
    (void) label;
    char *test = strtok(NULL, ":");
    if (!test)
    {
        fprintf(stderr, "%s:%s:%d unable to parse monkey test criteria\n", __FILE__, __FUNCTION__, __LINE__);
        return 1;
    }
    if (test[0] == ' ')
        test++;

    for (char *tok = strtok(test, " "); tok; tok = strtok(NULL, " "))
    {
        char *end;
        int test_val = (int) strtol(tok, &end, 10);
        if (end != tok)
        {
            m->test = test_val;
            break;
        }
    }

    m->test_len = strlen(test);

    // parse adjacent monkeys
    m->adj = (size_t*)malloc(2 * sizeof(size_t));
    line = read_line(fp, &line_sz);
    if (!isdigit(line[line_sz - 1]))
    {
        fprintf(stderr, "%s:%s:%d invalid data format detected, unable to parse adjacent monkeys\n", __FILE__, __FUNCTION__, __LINE__);
        return 1;
    }

    size_t adj1 = (size_t) (line[line_sz - 1] % 48);
    free(line);
    line = read_line(fp, &line_sz);
    if (!line)
    {
        fprintf(stderr, "%s:%s:%d unable to parse file correctly\n", __FILE__, __FUNCTION__, __LINE__);
        return 1;
    }
    if (!isdigit(line[line_sz - 1]))
    {
        fprintf(stderr, "%s:%s:%d invalid data format detected, unable to parse adjacent monkeys\n", __FILE__, __FUNCTION__, __LINE__);
        return 1;
    }

    size_t adj2 = (size_t) (line[line_sz - 1] % 48);
    free(line);
    m->adj[0] = adj1;
    m->adj[1] = adj2;

    // read line break
    line = read_line(fp, &line_sz);
    (void)line;

    return 0;
}

int monkey_info_t_operation(char *op, size_t len, int item)
{
    char *op_buf = (char*)malloc(len + 1);
    strncpy(op_buf, op, len);
    op_buf[len] = '\0';

    char *operand1 = strtok(op_buf, " ");
    char *operation = strtok(NULL, " ");
    char *operand2 = strtok(NULL, " ");
    int operand1_val;
    int operand2_val;
    if (!strcmp(operand1, "old"))
        operand1_val = item;
    else
        operand1_val = (int) strtol(operand1, NULL, 10);
    if (!strcmp(operand2, "old"))
        operand2_val = item;
    else
        operand2_val = (int) strtol(operand2, NULL, 10);

    if (!strcmp(operation, "+"))
        return operand1_val + operand2_val;
    else if (!strcmp(operation, "-"))
        return operand1_val - operand2_val;
    else if (!strcmp(operation, "*"))
        return operand1_val * operand2_val;
    else
        return operand1_val / operand2_val;
}

int monkey_turn(monkey_info_t monkeys[], size_t sz, size_t idx)
{
    int count = 0;
    while (!queue_t_is_empty(monkeys[idx].items))
    {
        int cur_item = queue_t_dequeue(monkeys[idx].items);
        if (idx == 0)
        {
            printf("monkey0 item: %d\n", cur_item);
        }
        int new_item = monkey_info_t_operation(monkeys[idx].op, monkeys[idx].op_len, cur_item) / 3;
        if (idx == 0)
        {
            printf("monkey0 item: %d, new_item: %d, test: %d\n", cur_item, new_item, monkeys[idx].test);
        }
        size_t adj;
        if (new_item % monkeys[idx].test == 0)
            adj = monkeys[idx].adj[0];
        else
            adj = monkeys[idx].adj[1];
        queue_t_enqueue(monkeys[adj].items, new_item);
        count++;
    }
    return count;
}


int main(int argc, char **argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "USAGE: day11 <FILE>\n");
        exit(1);
    }

    char *fname = argv[1];
    FILE *fp = fopen(fname, "r");
    if (!fp)
    {
        fprintf(stderr, "unable to open file %s\n", fname);
        exit(2);
    }

    monkey_info_t monkeys[8];
    for (size_t i = 0; i < 8; i++)
    {
        if (read_monkey(fp, monkeys + i))
            exit(3);
    }

    if (ferror(fp) || !feof(fp))
    {
        fprintf(stderr, "%s:%s:%d error reading file\n", __FILE__, __FUNCTION__, __LINE__);
        exit(4);
    }

    for (size_t i = 0; i < 8; i++)
    {
        printf(
                "monkey[%d]: op: %s, test: %d, adj: %zu, %zu\n",
                (int) i,  monkeys[i].op, monkeys[i].test, monkeys[i].adj[0], monkeys[i].adj[1]
                );

        printf("items: ");
        for (size_t j = 0; j < monkeys[i].items->tail; j++)
        {
            printf("%d ", monkeys[i].items->buf[j]);
        }

        printf("\n");
    }
    printf("success\n");

    int max1 = INT_MIN, max2 = INT_MIN;
    int counts[8] = {0};
    for (int i = 0; i < 20; i++)
    {
        for (size_t j = 0; j < 8; j++)
        {
            int count = monkey_turn(monkeys, 8, j);
            counts[j] += count;
        }
    }

    for (size_t i = 0; i < 8; i++)
    {
        if (counts[i] > max1)
        {
            max2 = max1;
            max1 = counts[i];
        }
        else if (counts[i] > max2)
        {
            max2 = counts[i];
        }
    }

    printf("monkey business: %d\n", max1 * max2);

    return 0;
}