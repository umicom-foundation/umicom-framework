/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_event/queue.h
 *
 * PURPOSE:
 *   Provide a bounded dynamic FIFO for normalised real workbench interaction events.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_EVENT_QUEUE_H
#define UMICOM_WORKBENCH_CONTEXT_EVENT_QUEUE_H
#include "umicom/workbench_context_event/event.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiWorkbenchContextEventQueue {
    UmiWorkbenchContextEvent *items;
    size_t count;
    size_t capacity;
    uint64_t dropped_count;
    uint64_t revision;
} UmiWorkbenchContextEventQueue;
void umi_workbench_context_event_queue_init(UmiWorkbenchContextEventQueue *queue);
void umi_workbench_context_event_queue_destroy(UmiWorkbenchContextEventQueue *queue);
UmiStatus umi_workbench_context_event_queue_push(
    UmiWorkbenchContextEventQueue *queue,const UmiWorkbenchContextEvent *event);
UmiStatus umi_workbench_context_event_queue_pop(
    UmiWorkbenchContextEventQueue *queue,UmiWorkbenchContextEvent *out_event);
const UmiWorkbenchContextEvent *umi_workbench_context_event_queue_peek(
    const UmiWorkbenchContextEventQueue *queue);
void umi_workbench_context_event_queue_clear(UmiWorkbenchContextEventQueue *queue);
#ifdef __cplusplus
}
#endif
#endif
