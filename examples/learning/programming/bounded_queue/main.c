/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/learning/programming/bounded_queue/main.c
 *
 * PURPOSE:
 *   A bounded queue and backpressure.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

enum { QUEUE_CAPACITY=3 };
typedef struct Queue { int values[QUEUE_CAPACITY]; size_t head, count; } Queue;
static bool Push(Queue *queue, int value)
{
    if (queue==NULL || queue->head>=QUEUE_CAPACITY || queue->count>=QUEUE_CAPACITY) return false;
    queue->values[(queue->head+queue->count)%QUEUE_CAPACITY]=value;
    ++queue->count; return true;
}
static bool Pop(Queue *queue, int *out)
{
    if (queue==NULL || out==NULL || queue->head>=QUEUE_CAPACITY ||
        queue->count==0U || queue->count>QUEUE_CAPACITY) return false;
    *out=queue->values[queue->head];
    queue->head=(queue->head+1U)%QUEUE_CAPACITY; --queue->count; return true;
}
int main(void)
{
    Queue queue={0}; int value=99;
    if (Pop(&queue,&value) || value!=99) return EXIT_FAILURE;
    if (!Push(&queue,10) || !Push(&queue,20) || !Push(&queue,30) || Push(&queue,40))
        return EXIT_FAILURE;
    if (!Pop(&queue,&value) || value!=10 || !Push(&queue,40)) return EXIT_FAILURE;
    const int expected[]={20,30,40};
    for (size_t index=0U; index<3U; ++index)
        if (!Pop(&queue,&value) || value!=expected[index]) return EXIT_FAILURE;
    if (Pop(&queue,&value)) return EXIT_FAILURE;
    puts("FIFO and wraparound checked; full queue refused extra input");
    puts("PASS: bounded_queue");
    return EXIT_SUCCESS;
}
