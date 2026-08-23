/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_coding_runtime/event_queue.h
 *
 * PURPOSE:
 *   Retain observable coding-agent events without coupling the runtime to GTK,
 *   web sockets or any particular UI/event loop.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_RUNTIME_EVENT_QUEUE_H
#define UMICOM_AI_CODING_RUNTIME_EVENT_QUEUE_H
#include "umicom/ai_coding_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiAiCodingEventQueue UmiAiCodingEventQueue;
UmiStatus umi_ai_coding_event_queue_create(
    UmiAiCodingEventQueue **out_queue);
void umi_ai_coding_event_queue_destroy(UmiAiCodingEventQueue *queue);
UmiStatus umi_ai_coding_event_queue_push(
    UmiAiCodingEventQueue *queue,
    const UmiAiCodingEvent *event);
UmiStatus umi_ai_coding_event_queue_pop(
    UmiAiCodingEventQueue *queue,
    UmiAiCodingEvent *out_event);
size_t umi_ai_coding_event_queue_count(
    const UmiAiCodingEventQueue *queue);
#ifdef __cplusplus
}
#endif
#endif
