/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/native_web/event_queue.h
 *
 * PURPOSE:
 *   Queue browser UI events in bounded FIFO order for Framework-side dispatch.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_INCLUDE_UMICOM_FRONTEND_NATIVE_WEB_EVENT_QUEUE_H
#define UMICOM_INCLUDE_UMICOM_FRONTEND_NATIVE_WEB_EVENT_QUEUE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/frontend/native_web/ui_event.h"
typedef struct UmiNativeWebEventQueue { UmiNativeWebUiEvent items[UMI_NATIVE_WEB_MAX_EVENTS]; size_t head; size_t count; uint64_t dropped; } UmiNativeWebEventQueue;
/* Initialise an empty bounded event queue. */
void umi_native_web_event_queue_init(UmiNativeWebEventQueue *queue);
/* Push one event or report capacity exhaustion. */
UmiStatus umi_native_web_event_queue_push(UmiNativeWebEventQueue *queue, const UmiNativeWebUiEvent *event);
/* Pop the oldest event. */
UmiStatus umi_native_web_event_queue_pop(UmiNativeWebEventQueue *queue, UmiNativeWebUiEvent *out_event);

#ifdef __cplusplus
}
#endif
#endif
