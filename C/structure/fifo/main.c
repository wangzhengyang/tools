#include "fifo.h"
#include <stdio.h>

struct msg
{
    int value1;
    int value2;
};

int main()
{
    struct msg msg1;
    struct msg recv;
    msg1.value1 = 1;
    msg1.value2 = 2;

    struct fifo* q = fifo_init(sizeof(struct msg) * 10);
    printf("the q size:%d\r\n", fifo_size(q));
    fifo_in(q, &msg1, sizeof(struct msg));
    printf("the q left:%d\r\n", fifo_avail(q));
    fifo_out(q, &recv, sizeof(struct msg));
    printf("data %d %d\r\n", recv.value1, recv.value2);
    return 0;
}