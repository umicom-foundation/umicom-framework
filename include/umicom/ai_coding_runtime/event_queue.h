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
#ifdef __cplusplus
}
#endif
#endif
