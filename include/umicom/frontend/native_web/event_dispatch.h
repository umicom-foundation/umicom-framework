/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/native_web/event_dispatch.h
 *
 * PURPOSE:
 *   Dispatch semantic UI events through registered Framework callbacks.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_INCLUDE_UMICOM_FRONTEND_NATIVE_WEB_EVENT_DISPATCH_H
#define UMICOM_INCLUDE_UMICOM_FRONTEND_NATIVE_WEB_EVENT_DISPATCH_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/frontend/native_web/ui_event.h"
typedef UmiStatus (*UmiNativeWebEventHandler)(void *context, const UmiNativeWebUiEvent *event);
typedef struct UmiNativeWebEventDispatchEntry { UmiNativeWebEventKind kind; UmiNativeWebEventHandler handler; void *context; } UmiNativeWebEventDispatchEntry;
typedef struct UmiNativeWebEventDispatcher { UmiNativeWebEventDispatchEntry entries[32]; size_t count; } UmiNativeWebEventDispatcher;
/* Initialise an empty semantic event dispatcher. */
void umi_native_web_event_dispatch_init(UmiNativeWebEventDispatcher *dispatcher);
/* Register one handler per event kind. */
UmiStatus umi_native_web_event_dispatch_register(UmiNativeWebEventDispatcher *dispatcher, UmiNativeWebEventKind kind, UmiNativeWebEventHandler handler, void *context);
/* Dispatch one event to its registered handler. */
UmiStatus umi_native_web_event_dispatch_one(UmiNativeWebEventDispatcher *dispatcher, const UmiNativeWebUiEvent *event);

#ifdef __cplusplus
}
#endif
#endif
