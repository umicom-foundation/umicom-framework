/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_runtime/event_queue.c
 *
 * PURPOSE:
 *   Implement a bounded heap-owned FIFO for agent state and evidence events.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_runtime/event_queue.h"

#include <stdlib.h>
#include <string.h>

struct UmiAiCodingEventQueue {
    UmiAiCodingEvent items[UMI_AI_CODING_RUNTIME_EVENT_CAPACITY];
    size_t head;
    size_t count;
};

UmiStatus umi_ai_coding_event_queue_create(
    UmiAiCodingEventQueue **out_queue)
{
    UmiAiCodingEventQueue *queue;

    if (out_queue == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_queue = NULL;

    queue = (UmiAiCodingEventQueue *)calloc(1U, sizeof(*queue));
    if (queue == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    *out_queue = queue;
    return UMI_STATUS_OK;
}

void umi_ai_coding_event_queue_destroy(UmiAiCodingEventQueue *queue)
{
    free(queue);
}

UmiStatus umi_ai_coding_event_queue_push(
    UmiAiCodingEventQueue *queue,
    const UmiAiCodingEvent *event)
{
    size_t index;

    if (queue == NULL || event == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (queue->count >= UMI_AI_CODING_RUNTIME_EVENT_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    index =
        (queue->head + queue->count) %
        UMI_AI_CODING_RUNTIME_EVENT_CAPACITY;
    queue->items[index] = *event;
    queue->count += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_ai_coding_event_queue_pop(
    UmiAiCodingEventQueue *queue,
    UmiAiCodingEvent *out_event)
{
    if (queue == NULL || out_event == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (queue->count == 0U) return UMI_STATUS_NOT_FOUND;

    *out_event = queue->items[queue->head];
    (void)memset(&queue->items[queue->head], 0, sizeof(queue->items[0]));
    queue->head =
        (queue->head + 1U) % UMI_AI_CODING_RUNTIME_EVENT_CAPACITY;
    queue->count -= 1U;
    return UMI_STATUS_OK;
}

size_t umi_ai_coding_event_queue_count(
    const UmiAiCodingEventQueue *queue)
{
    return queue != NULL ? queue->count : 0U;
}
