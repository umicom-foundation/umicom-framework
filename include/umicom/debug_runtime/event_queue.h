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

/**
 * Represent the debug runtime event queue data shared with callers of this public
 * contract.
 */
typedef struct UmiDebugRuntimeEventQueue UmiDebugRuntimeEventQueue;

/**
 * Initialise debug runtime event queue from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_debug_runtime_event_queue_create(
    UmiDebugRuntimeEventQueue **out_queue);
/**
 * Release or reset state held by debug runtime event queue so the same storage can be
 * reused safely.
 */
void umi_debug_runtime_event_queue_destroy(
    UmiDebugRuntimeEventQueue *queue);
/**
 * Provide the debug runtime event queue push operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_runtime_event_queue_push(
    UmiDebugRuntimeEventQueue *queue,
    const UmiDebugRuntimeEnvelope *event);
/**
 * Provide the debug runtime event queue pop operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_runtime_event_queue_pop(
    UmiDebugRuntimeEventQueue *queue,
    UmiDebugRuntimeEnvelope *out_event);
/**
 * Return the number of records represented by debug runtime event queue without changing
 * their state.
 */
size_t umi_debug_runtime_event_queue_count(
    const UmiDebugRuntimeEventQueue *queue);

#ifdef __cplusplus
}
#endif
#endif
