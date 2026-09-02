/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/event_queue.c
 *
 * PURPOSE:
 *   Implement the heap-owned bounded FIFO for asynchronous DAP events.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/event_queue.h"
#include <stdlib.h>
#include <string.h>

struct UmiDebugRuntimeEventQueue {
    UmiDebugRuntimeEnvelope items[UMI_DEBUG_RUNTIME_MAX_EVENTS];
    size_t head;
    size_t count;
    uint64_t revision;
};

/*
 * Initialise debug runtime event queue from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_debug_runtime_event_queue_create(
    UmiDebugRuntimeEventQueue **out_queue)
{
    UmiDebugRuntimeEventQueue *queue;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_queue == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_queue = NULL;

    queue = (UmiDebugRuntimeEventQueue *)calloc(1U, sizeof(*queue));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (queue == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    queue->revision = 1U;
    *out_queue = queue;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by debug runtime event queue so the same storage can be
 * reused safely.
 */
void umi_debug_runtime_event_queue_destroy(
    UmiDebugRuntimeEventQueue *queue)
{
    free(queue);
}

/*
 * Provide the debug runtime event queue push operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_runtime_event_queue_push(
    UmiDebugRuntimeEventQueue *queue,
    const UmiDebugRuntimeEnvelope *event)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (queue == NULL || event == NULL ||
        event->kind != UMI_DEBUG_RUNTIME_MESSAGE_EVENT) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (queue->count >= UMI_DEBUG_RUNTIME_MAX_EVENTS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    index = (queue->head + queue->count) % UMI_DEBUG_RUNTIME_MAX_EVENTS;
    queue->items[index] = *event;
    queue->count += 1U;
    queue->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the debug runtime event queue pop operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_runtime_event_queue_pop(
    UmiDebugRuntimeEventQueue *queue,
    UmiDebugRuntimeEnvelope *out_event)
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
    queue->head = (queue->head + 1U) % UMI_DEBUG_RUNTIME_MAX_EVENTS;
    queue->count -= 1U;
    queue->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by debug runtime event queue without changing
 * their state.
 */
size_t umi_debug_runtime_event_queue_count(
    const UmiDebugRuntimeEventQueue *queue)
{
    return queue != NULL ? queue->count : 0U;
}
