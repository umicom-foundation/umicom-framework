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
/* Admit event text and complete batches before changing shared observation storage. The former implementation is retained for owner review. */
#if 0
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

#endif
static UmiStatus event_queue_check(const UmiAiCodingEventQueue *queue)
{
    if (queue == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (queue->head >= UMI_AI_CODING_RUNTIME_EVENT_CAPACITY ||
        queue->count > UMI_AI_CODING_RUNTIME_EVENT_CAPACITY)
        return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}

/* Preflight every record before appending any event. Sequence numbers belong
 * to the producer and are copied unchanged, including zero or repeated values. */
/* Share read-only admission with composed observation publication so all owners can be checked before any retention changes. The former implementation is retained for owner review. */
#if 0
UmiStatus UmiAiCodingEventQueuePushBatch(UmiAiCodingEventQueue *queue,
    const UmiAiCodingEvent *events, size_t event_count)
{
    UmiStatus status = event_queue_check(queue);
    if (status != UMI_STATUS_OK) return status;
    if (events == NULL && event_count != 0U) return UMI_STATUS_INVALID_ARGUMENT;
    if (event_count > UMI_AI_CODING_RUNTIME_EVENT_CAPACITY - queue->count)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    for (size_t i = 0U; i < event_count; ++i) {
        status = UmiAiCodingEventValidateText(&events[i]);
        if (status != UMI_STATUS_OK) return status;
    }
    for (size_t i = 0U; i < event_count; ++i) {
        size_t index = (queue->head + queue->count) % UMI_AI_CODING_RUNTIME_EVENT_CAPACITY;
        queue->items[index] = events[i];
        ++queue->count;
    }
    return UMI_STATUS_OK;
}

#endif
/* Read-only admission shares exactly the checks used by the mutator.
 * Serialize the owner until the matching operation completes; this is not a reservation. */
UmiStatus UmiAiCodingEventQueueCheckPushBatch(const UmiAiCodingEventQueue *queue,
    const UmiAiCodingEvent *events, size_t event_count)
{
    UmiStatus status = event_queue_check(queue);
    if (status != UMI_STATUS_OK) return status;
    if (events == NULL && event_count != 0U) return UMI_STATUS_INVALID_ARGUMENT;
    if (event_count > UMI_AI_CODING_RUNTIME_EVENT_CAPACITY - queue->count)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    for (size_t i = 0U; i < event_count; ++i) {
        status = UmiAiCodingEventValidateText(&events[i]);
        if (status != UMI_STATUS_OK) return status;
    }
    return UMI_STATUS_OK;
}

UmiStatus UmiAiCodingEventQueuePushBatch(UmiAiCodingEventQueue *queue,
    const UmiAiCodingEvent *events, size_t event_count)
{
    UmiStatus status = UmiAiCodingEventQueueCheckPushBatch(queue, events, event_count);
    if (status != UMI_STATUS_OK) return status;
    for (size_t i = 0U; i < event_count; ++i) {
        size_t index = (queue->head + queue->count) % UMI_AI_CODING_RUNTIME_EVENT_CAPACITY;
        queue->items[index] = events[i];
        ++queue->count;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_ai_coding_event_queue_push(
    UmiAiCodingEventQueue *queue,
    const UmiAiCodingEvent *event)
{
    return UmiAiCodingEventQueuePushBatch(queue, event, 1U);
}

/*
 * Provide the ai coding event queue pop operation used by this module and its client
 * applications.
 */
/* Provide bounded consumer batches with unchanged results and queue state on refusal while retaining single-pop empty semantics. The former implementation is retained for owner review. */
#if 0
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

#endif
/* Read admission finishes before either the consumer's output or the queue
 * changes. The bounded copy count prevents multiplication and index overflow. */
UmiStatus UmiAiCodingEventQueuePopBatch(UmiAiCodingEventQueue *queue,
    UmiAiCodingEvent *events, size_t capacity, size_t *out_count)
{
    UmiStatus status = event_queue_check(queue);
    if (status != UMI_STATUS_OK) return status;
    if (out_count == NULL || (events == NULL && capacity != 0U))
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t count = queue->count < capacity ? queue->count : capacity;
    for (size_t i = 0U; i < count; ++i) {
        size_t index = (queue->head + i) % UMI_AI_CODING_RUNTIME_EVENT_CAPACITY;
        status = UmiAiCodingEventValidateText(&queue->items[index]);
        if (status != UMI_STATUS_OK) return status;
    }
    for (size_t i = 0U; i < count; ++i) {
        size_t index = (queue->head + i) % UMI_AI_CODING_RUNTIME_EVENT_CAPACITY;
        events[i] = queue->items[index];
        memset(&queue->items[index], 0, sizeof queue->items[index]);
    }
    queue->head = (queue->head + count) % UMI_AI_CODING_RUNTIME_EVENT_CAPACITY;
    queue->count -= count;
    *out_count = count;
    return UMI_STATUS_OK;
}

UmiStatus umi_ai_coding_event_queue_pop(
    UmiAiCodingEventQueue *queue,
    UmiAiCodingEvent *out_event)
{
    size_t copied = 0U;
    UmiStatus status = UmiAiCodingEventQueuePopBatch(queue, out_event, 1U, &copied);
    return status != UMI_STATUS_OK ? status :
        copied == 0U ? UMI_STATUS_NOT_FOUND : UMI_STATUS_OK;
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
