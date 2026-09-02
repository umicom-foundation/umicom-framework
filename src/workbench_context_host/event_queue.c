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

/* Provide the grow operation used by this module and its client applications. */
static UmiStatus grow(UmiWorkbenchContextHostEventQueue *queue)
{
    UmiWorkbenchContextHostEvent *items;
    size_t next;

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (queue->count < queue->capacity) return UMI_STATUS_OK;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
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
    /* Apply this branch only when its contract condition is satisfied. */
    if (next > UMI_WORKBENCH_CONTEXT_HOST_MAX_EVENTS) {
        next = UMI_WORKBENCH_CONTEXT_HOST_MAX_EVENTS;
    }
    items = (UmiWorkbenchContextHostEvent *)realloc(
        queue->items, next * sizeof(queue->items[0]));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (items == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
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

/*
 * Initialise workbench context host event queue from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_host_event_queue_init(
    UmiWorkbenchContextHostEventQueue *queue)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (queue == NULL) return;
    memset(queue, 0, sizeof(*queue));
    queue->revision = 1U;
}

/*
 * Release or reset state held by workbench context host event queue so the same storage
 * can be reused safely.
 */
void umi_workbench_context_host_event_queue_destroy(
    UmiWorkbenchContextHostEventQueue *queue)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (queue == NULL) return;
    free(queue->items);
    memset(queue, 0, sizeof(*queue));
}

/*
 * Provide the workbench context host event queue push operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_context_host_event_queue_push(
    UmiWorkbenchContextHostEventQueue *queue,
    const UmiWorkbenchContextHostEvent *event)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (queue == NULL || event == NULL ||
        event->structure_size != sizeof(*event)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = grow(queue);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    queue->items[queue->count++] = *event;
    ++queue->revision;
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench context host event queue pop operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_context_host_event_queue_pop(
    UmiWorkbenchContextHostEventQueue *queue,
    UmiWorkbenchContextHostEvent *out_event)
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
    *out_event = queue->items[0];
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
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

/*
 * Release or reset state held by workbench context host event queue so the same storage
 * can be reused safely.
 */
void umi_workbench_context_host_event_queue_clear(
    UmiWorkbenchContextHostEventQueue *queue)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (queue == NULL) return;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (queue->items != NULL && queue->capacity > 0U) {
        memset(
            queue->items,
            0,
            queue->capacity * sizeof(queue->items[0]));
    }
    queue->count = 0U;
    ++queue->revision;
}
