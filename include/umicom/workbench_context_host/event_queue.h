/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_host/event_queue.h
 *
 * PURPOSE:
 *   Retain bounded lifecycle and routing events for decoupled workbench observers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_HOST_EVENT_QUEUE_H
#define UMICOM_WORKBENCH_CONTEXT_HOST_EVENT_QUEUE_H
#include "umicom/workbench_context_host/event.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_WORKBENCH_CONTEXT_HOST_MAX_EVENTS 512U
/**
 * Represent the workbench context host event queue data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchContextHostEventQueue {
    UmiWorkbenchContextHostEvent *items;
    size_t count;
    size_t capacity;
    uint64_t dropped_count;
    uint64_t revision;
} UmiWorkbenchContextHostEventQueue;
/**
 * Initialise workbench context host event queue from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_host_event_queue_init(UmiWorkbenchContextHostEventQueue *queue);
/**
 * Release or reset state held by workbench context host event queue so the same storage
 * can be reused safely.
 */
void umi_workbench_context_host_event_queue_destroy(UmiWorkbenchContextHostEventQueue *queue);
/**
 * Provide the workbench context host event queue push operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_context_host_event_queue_push(
    UmiWorkbenchContextHostEventQueue *queue,const UmiWorkbenchContextHostEvent *event);
/**
 * Provide the workbench context host event queue pop operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_context_host_event_queue_pop(
    UmiWorkbenchContextHostEventQueue *queue,UmiWorkbenchContextHostEvent *out_event);
/**
 * Release or reset state held by workbench context host event queue so the same storage
 * can be reused safely.
 */
void umi_workbench_context_host_event_queue_clear(UmiWorkbenchContextHostEventQueue *queue);
#ifdef __cplusplus
}
#endif
#endif
