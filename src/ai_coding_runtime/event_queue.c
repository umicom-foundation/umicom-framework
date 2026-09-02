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

/*
 * Initialise ai coding event queue from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_ai_coding_event_queue_create(
    UmiAiCodingEventQueue **out_queue)
{
    UmiAiCodingEventQueue *queue;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_queue == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_queue = NULL;

    queue = (UmiAiCodingEventQueue *)calloc(1U, sizeof(*queue));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (queue == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    *out_queue = queue;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by ai coding event queue so the same storage can be reused
 * safely.
 */
void umi_ai_coding_event_queue_destroy(UmiAiCodingEventQueue *queue)
{
    free(queue);
}

/*
 * Provide the ai coding event queue push operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_coding_event_queue_push(
    UmiAiCodingEventQueue *queue,
    const UmiAiCodingEvent *event)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (queue == NULL || event == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
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

/*
 * Provide the ai coding event queue pop operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_coding_event_queue_pop(
    UmiAiCodingEventQueue *queue,
    UmiAiCodingEvent *out_event)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (queue == NULL || out_event == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (queue->count == 0U) return UMI_STATUS_NOT_FOUND;

    *out_event = queue->items[queue->head];
    (void)memset(&queue->items[queue->head], 0, sizeof(queue->items[0]));
    queue->head =
        (queue->head + 1U) % UMI_AI_CODING_RUNTIME_EVENT_CAPACITY;
    queue->count -= 1U;
    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by ai coding event queue without changing their
 * state.
 */
size_t umi_ai_coding_event_queue_count(
    const UmiAiCodingEventQueue *queue)
{
    return queue != NULL ? queue->count : 0U;
}
