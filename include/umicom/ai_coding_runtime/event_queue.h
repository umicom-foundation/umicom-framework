/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_coding_runtime/event_queue.h
 *
 * PURPOSE:
 *   Retain observable coding-agent events without coupling the runtime to GTK,
 *   web sockets or any particular UI/event loop.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_RUNTIME_EVENT_QUEUE_H
#define UMICOM_AI_CODING_RUNTIME_EVENT_QUEUE_H
#include "umicom/ai_coding_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ai coding event queue data shared with callers of this public contract.
 */
typedef struct UmiAiCodingEventQueue UmiAiCodingEventQueue;
/**
 * Initialise ai coding event queue from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_ai_coding_event_queue_create(
    UmiAiCodingEventQueue **out_queue);
/**
 * Release or reset state held by ai coding event queue so the same storage can be reused
 * safely.
 */
void umi_ai_coding_event_queue_destroy(UmiAiCodingEventQueue *queue);
/**
 * Provide the ai coding event queue push operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_coding_event_queue_push(
    UmiAiCodingEventQueue *queue,
    const UmiAiCodingEvent *event);
/** Append the complete batch or leave the queue unchanged. Text arrays must be
 * terminated; empty strings and scalar metadata are preserved. A zero count
 * accepts NULL events. Capacity is checked before input traversal. No sequence
 * is assigned or interpreted. Keep input records stable and separate from the
 * queue's owned storage; serialize every operation on this owner. */
UmiStatus UmiAiCodingEventQueuePushBatch(UmiAiCodingEventQueue *queue,
    const UmiAiCodingEvent *events, size_t event_count);

/** Remove up to capacity oldest events. Empty or zero-capacity reads return OK
 * with zero copied; NULL events is allowed only at zero capacity. Every failure
 * preserves queue, output records and out_count. out_count is required, separate
 * storage; all output must be separate from queue storage. The caller supplies
 * real writable capacity and serializes access. This does not start a worker. */
UmiStatus UmiAiCodingEventQueuePopBatch(UmiAiCodingEventQueue *queue,
    UmiAiCodingEvent *events, size_t capacity, size_t *out_count);

/**
 * Provide the ai coding event queue pop operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_coding_event_queue_pop(
    UmiAiCodingEventQueue *queue,
    UmiAiCodingEvent *out_event);
/**
 * Return the number of records represented by ai coding event queue without changing their
 * state.
 */
size_t umi_ai_coding_event_queue_count(
    const UmiAiCodingEventQueue *queue);
/* Predict the matching batch mutation without changing this owner.
 * A successful check is valid only while the owner and inputs remain unchanged
 * on the serialized owner thread. It reserves no capacity and acquires no lock. */
UmiStatus UmiAiCodingEventQueueCheckPushBatch(const UmiAiCodingEventQueue *queue,
    const UmiAiCodingEvent *events, size_t event_count);

#ifdef __cplusplus
}
#endif
#endif
