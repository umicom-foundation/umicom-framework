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

UmiStatus umi_debug_runtime_event_queue_create(
    UmiDebugRuntimeEventQueue **out_queue)
{
    UmiDebugRuntimeEventQueue *queue;

    if (out_queue == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_queue = NULL;

    queue = (UmiDebugRuntimeEventQueue *)calloc(1U, sizeof(*queue));
    if (queue == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    queue->revision = 1U;
    *out_queue = queue;
    return UMI_STATUS_OK;
}

void umi_debug_runtime_event_queue_destroy(
    UmiDebugRuntimeEventQueue *queue)
{
    free(queue);
}

UmiStatus umi_debug_runtime_event_queue_push(
    UmiDebugRuntimeEventQueue *queue,
    const UmiDebugRuntimeEnvelope *event)
{
    size_t index;

    if (queue == NULL || event == NULL ||
        event->kind != UMI_DEBUG_RUNTIME_MESSAGE_EVENT) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (queue->count >= UMI_DEBUG_RUNTIME_MAX_EVENTS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    index = (queue->head + queue->count) % UMI_DEBUG_RUNTIME_MAX_EVENTS;
    queue->items[index] = *event;
    queue->count += 1U;
    queue->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_debug_runtime_event_queue_pop(
    UmiDebugRuntimeEventQueue *queue,
    UmiDebugRuntimeEnvelope *out_event)
{
    if (queue == NULL || out_event == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (queue->count == 0U) return UMI_STATUS_NOT_FOUND;

    *out_event = queue->items[queue->head];
    (void)memset(&queue->items[queue->head], 0, sizeof(queue->items[0]));
    queue->head = (queue->head + 1U) % UMI_DEBUG_RUNTIME_MAX_EVENTS;
    queue->count -= 1U;
    queue->revision += 1U;
    return UMI_STATUS_OK;
}

size_t umi_debug_runtime_event_queue_count(
    const UmiDebugRuntimeEventQueue *queue)
{
    return queue != NULL ? queue->count : 0U;
}
