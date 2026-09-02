/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_event/queue.h
 *
 * PURPOSE:
 *   Provide a bounded dynamic FIFO for normalised real workbench interaction events.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_EVENT_QUEUE_H
#define UMICOM_WORKBENCH_CONTEXT_EVENT_QUEUE_H
#include "umicom/workbench_context_event/event.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the workbench context event queue data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchContextEventQueue {
    UmiWorkbenchContextEvent *items;
    size_t count;
    size_t capacity;
    uint64_t dropped_count;
    uint64_t revision;
} UmiWorkbenchContextEventQueue;
/**
 * Initialise workbench context event queue from caller-provided values so later operations
 * receive a known state.
 */
void umi_workbench_context_event_queue_init(UmiWorkbenchContextEventQueue *queue);
/**
 * Release or reset state held by workbench context event queue so the same storage can be
 * reused safely.
 */
void umi_workbench_context_event_queue_destroy(UmiWorkbenchContextEventQueue *queue);
/**
 * Provide the workbench context event queue push operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_context_event_queue_push(
    UmiWorkbenchContextEventQueue *queue,const UmiWorkbenchContextEvent *event);
/**
 * Provide the workbench context event queue pop operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_context_event_queue_pop(
    UmiWorkbenchContextEventQueue *queue,UmiWorkbenchContextEvent *out_event);
/**
 * Provide the workbench context event queue peek operation used by this module and its
 * client applications.
 */
const UmiWorkbenchContextEvent *umi_workbench_context_event_queue_peek(
    const UmiWorkbenchContextEventQueue *queue);
/**
 * Release or reset state held by workbench context event queue so the same storage can be
 * reused safely.
 */
void umi_workbench_context_event_queue_clear(UmiWorkbenchContextEventQueue *queue);
#ifdef __cplusplus
}
#endif
#endif
