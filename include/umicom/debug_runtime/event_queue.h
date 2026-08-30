/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/event_queue.h
 *
 * PURPOSE:
 *   Retain asynchronous DAP events while synchronous invocations wait for their
 *   responses. Storage is opaque/heap-owned because one envelope may contain a
 *   large JSON payload and must never consume the caller's thread stack.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_EVENT_QUEUE_H
#define UMICOM_DEBUG_RUNTIME_EVENT_QUEUE_H
#include "umicom/debug_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDebugRuntimeEventQueue UmiDebugRuntimeEventQueue;

UmiStatus umi_debug_runtime_event_queue_create(
    UmiDebugRuntimeEventQueue **out_queue);
void umi_debug_runtime_event_queue_destroy(
    UmiDebugRuntimeEventQueue *queue);
UmiStatus umi_debug_runtime_event_queue_push(
    UmiDebugRuntimeEventQueue *queue,
    const UmiDebugRuntimeEnvelope *event);
UmiStatus umi_debug_runtime_event_queue_pop(
    UmiDebugRuntimeEventQueue *queue,
    UmiDebugRuntimeEnvelope *out_event);
size_t umi_debug_runtime_event_queue_count(
    const UmiDebugRuntimeEventQueue *queue);

#ifdef __cplusplus
}
#endif
#endif
