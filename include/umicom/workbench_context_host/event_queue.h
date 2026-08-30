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
typedef struct UmiWorkbenchContextHostEventQueue {
    UmiWorkbenchContextHostEvent *items;
    size_t count;
    size_t capacity;
    uint64_t dropped_count;
    uint64_t revision;
} UmiWorkbenchContextHostEventQueue;
void umi_workbench_context_host_event_queue_init(UmiWorkbenchContextHostEventQueue *queue);
void umi_workbench_context_host_event_queue_destroy(UmiWorkbenchContextHostEventQueue *queue);
UmiStatus umi_workbench_context_host_event_queue_push(
    UmiWorkbenchContextHostEventQueue *queue,const UmiWorkbenchContextHostEvent *event);
UmiStatus umi_workbench_context_host_event_queue_pop(
    UmiWorkbenchContextHostEventQueue *queue,UmiWorkbenchContextHostEvent *out_event);
void umi_workbench_context_host_event_queue_clear(UmiWorkbenchContextHostEventQueue *queue);
#ifdef __cplusplus
}
#endif
#endif
