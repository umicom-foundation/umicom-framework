/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/event.h
 *
 * PURPOSE:
 *   Define bounded frontend event records so adapters can translate native input into
 *   typed Framework events before invoking commands.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_EVENT_H
#define UMICOM_UI_EVENT_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ui/types.h"

#ifdef __cplusplus
extern "C" {
#endif


#define UMI_UI_EVENT_PAYLOAD_CAPACITY 1024U
#define UMI_UI_EVENT_QUEUE_MAX 512U
/**
 * List the named ui event kind values accepted by this public contract.
 */
typedef enum UmiUiEventKind { UMI_UI_EVENT_ACTIVATE=1, UMI_UI_EVENT_CHANGE=2, UMI_UI_EVENT_CLOSE=3, UMI_UI_EVENT_FOCUS=4, UMI_UI_EVENT_SELECTION=5, UMI_UI_EVENT_CUSTOM=6 } UmiUiEventKind;
/**
 * Represent the ui event data shared with callers of this public contract.
 */
typedef struct UmiUiEvent { uint64_t event_id; UmiUiEventKind kind; char source_id[UMI_UI_ID_CAPACITY]; char command_id[UMI_UI_ID_CAPACITY]; char payload[UMI_UI_EVENT_PAYLOAD_CAPACITY]; uint64_t timestamp_ns; } UmiUiEvent;
/**
 * Represent the ui event queue data shared with callers of this public contract.
 */
typedef struct UmiUiEventQueue UmiUiEventQueue;
/**
 * Initialise ui event queue from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_ui_event_queue_create(UmiUiEventQueue **out_queue);
/**
 * Release or reset state held by ui event queue so the same storage can be reused safely.
 */
void umi_ui_event_queue_destroy(UmiUiEventQueue *queue);
/**
 * Provide the ui event push operation used by this module and its client applications.
 */
UmiStatus umi_ui_event_push(UmiUiEventQueue *queue, const UmiUiEvent *event, uint64_t *out_event_id);
/**
 * Provide the ui event pop operation used by this module and its client applications.
 */
UmiStatus umi_ui_event_pop(UmiUiEventQueue *queue, UmiUiEvent *out_event);
/**
 * Return the number of records represented by ui event without changing their state.
 */
size_t umi_ui_event_count(const UmiUiEventQueue *queue);
/**
 * Release or reset state held by ui event so the same storage can be reused safely.
 */
void umi_ui_event_clear(UmiUiEventQueue *queue);

#ifdef __cplusplus
}
#endif

#endif
