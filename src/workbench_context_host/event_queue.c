/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_host/event_queue.c
 *
 * PURPOSE:
 *   Implement bounded event retention with oldest-event eviction under pressure.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_host/event_queue.h"

#include <stdlib.h>
#include <string.h>

static UmiStatus grow(UmiWorkbenchContextHostEventQueue *queue)
{
    UmiWorkbenchContextHostEvent *items;
    size_t next;

    if (queue->count < queue->capacity) return UMI_STATUS_OK;
    if (queue->capacity >= UMI_WORKBENCH_CONTEXT_HOST_MAX_EVENTS) {
        memmove(
            &queue->items[0],
            &queue->items[1],
            (queue->count - 1U) * sizeof(queue->items[0]));
        --queue->count;
        ++queue->dropped_count;
        return UMI_STATUS_OK;
    }

    next = queue->capacity == 0U ? 32U : queue->capacity * 2U;
    if (next > UMI_WORKBENCH_CONTEXT_HOST_MAX_EVENTS) {
        next = UMI_WORKBENCH_CONTEXT_HOST_MAX_EVENTS;
    }
    items = (UmiWorkbenchContextHostEvent *)realloc(
        queue->items, next * sizeof(queue->items[0]));
    if (items == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    if (next > queue->capacity) {
        memset(
            items + queue->capacity,
            0,
            (next - queue->capacity) * sizeof(items[0]));
    }
    queue->items = items;
    queue->capacity = next;
    return UMI_STATUS_OK;
}

void umi_workbench_context_host_event_queue_init(
    UmiWorkbenchContextHostEventQueue *queue)
{
    if (queue == NULL) return;
    memset(queue, 0, sizeof(*queue));
    queue->revision = 1U;
}

void umi_workbench_context_host_event_queue_destroy(
    UmiWorkbenchContextHostEventQueue *queue)
{
    if (queue == NULL) return;
    free(queue->items);
    memset(queue, 0, sizeof(*queue));
}

UmiStatus umi_workbench_context_host_event_queue_push(
    UmiWorkbenchContextHostEventQueue *queue,
    const UmiWorkbenchContextHostEvent *event)
{
    UmiStatus status;
    if (queue == NULL || event == NULL ||
        event->structure_size != sizeof(*event)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = grow(queue);
    if (status != UMI_STATUS_OK) return status;
    queue->items[queue->count++] = *event;
    ++queue->revision;
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_context_host_event_queue_pop(
    UmiWorkbenchContextHostEventQueue *queue,
    UmiWorkbenchContextHostEvent *out_event)
{
    if (queue == NULL || out_event == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (queue->count == 0U) return UMI_STATUS_NOT_FOUND;
    *out_event = queue->items[0];
    if (queue->count > 1U) {
        memmove(
            &queue->items[0],
            &queue->items[1],
            (queue->count - 1U) * sizeof(queue->items[0]));
    }
    --queue->count;
    memset(&queue->items[queue->count], 0, sizeof(queue->items[0]));
    ++queue->revision;
    return UMI_STATUS_OK;
}

void umi_workbench_context_host_event_queue_clear(
    UmiWorkbenchContextHostEventQueue *queue)
{
    if (queue == NULL) return;
    if (queue->items != NULL && queue->capacity > 0U) {
        memset(
            queue->items,
            0,
            queue->capacity * sizeof(queue->items[0]));
    }
    queue->count = 0U;
    ++queue->revision;
}
