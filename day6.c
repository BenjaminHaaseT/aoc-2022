//
// Created by benjamin haase on 4/3/24.
//
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

typedef struct {
    char* buf;
    size_t len;
    size_t head;
    size_t tail;
} CircularQueue;

void init_cq(CircularQueue *cq, size_t len);
void release_cq(CircularQueue* cq);
void enqueue(CircularQueue* cq, char c);
char dequeue(CircularQueue* cq);
void print_arr(int *arr, int arr_sz);
bool packet_found(const int freq[128]);
bool msg_packet_found(const int freq[26]);

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "usage: day6 <FILE>\n");
        exit(1);
    }

    char* fname = argv[1];
    FILE* fp = fopen(fname, "r");
    if (!fp)
    {
        fprintf(stderr, "unable to open file %s\n", fname);
        exit(1);
    }

    int freq[128] = {0};
    CircularQueue cq;
    init_cq(&cq, 4);
    int count = 0;
    int cur_char;

    while ((cur_char = fgetc(fp)) != EOF)
    {
        if (cur_char != '\n')
        {
            enqueue(&cq, (char) cur_char);
            freq[cur_char]++;
            count++;

            if (count >= 4 && packet_found(freq))
                break;
            else if (count >= 4)
            {
                char c = dequeue(&cq);
                freq[(size_t) c]--;
            }
        }
    }

    if (ferror(fp))
    {
        fprintf(stderr, "%s:%s:%d error reading from file\n", __FILE__, __FUNCTION__, __LINE__);
        exit(1);
    }

    printf("start of packet char count: %d\n", count);
    release_cq(&cq);
    rewind(fp);

    int msg_freq[26] = {0};
    count = 0;
    CircularQueue msg_cq;
    init_cq(&msg_cq, 14);

    while ((cur_char = fgetc(fp)) != EOF)
    {
        enqueue(&msg_cq, (char) cur_char);
        msg_freq[(size_t) (cur_char % 'a')]++;
        count++;


        if (count >= 14 && msg_packet_found(msg_freq))
            break;
        else if (count >= 14)
        {
            char c = dequeue(&msg_cq);
            msg_freq[(size_t) (c % 'a')]--;
        }
    }

    if (ferror(fp))
    {
        fprintf(stderr, "%s:%s:%d error reading from file\n", __FILE__, __FUNCTION__, __LINE__);
        exit(1);
    }

    printf("start of message char count: %d\n", count);
    fclose(fp);

    return EXIT_SUCCESS;
}


void init_cq(CircularQueue *cq, size_t len)
{
    cq->len = len;
    cq->buf = (char *) malloc(len);
    cq->head = 0;
    cq->tail = 0;
}

void release_cq(CircularQueue* cq)
{
    free(cq->buf);
}

void enqueue(CircularQueue* cq, char c)
{
    cq->buf[cq->tail++] = c;
    cq->tail %= cq->len;
}

char dequeue(CircularQueue* cq)
{
    char res = cq->buf[cq->head];
    cq->buf[cq->head++] = '\0';
    cq->head %= cq->len;
    return res;
}

bool packet_found(const int freq[128])
{
    for (int i = 0; i < 128; i++)
        if (freq[i] > 1)
            return false;
    return true;
}

bool msg_packet_found(const int freq[26])
{
    for (int i = 0; i < 26; i++)
        if (freq[i] > 1)
            return false;
    return true;
}

void print_arr(int *arr, int arr_sz)
{
    if (arr_sz == 0)
    {
        printf("[]\n");
        return;
    }

    printf("[%d", arr[0]);
    for (int i = 1; i < arr_sz; i++)
        printf(", %d", arr[i]);
    printf("]\n");
}