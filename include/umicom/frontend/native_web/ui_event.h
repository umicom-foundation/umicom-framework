/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/native_web/ui_event.h
 *
 * PURPOSE:
 *   Define browser-originated semantic UI events with no direct DOM ownership in applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_INCLUDE_UMICOM_FRONTEND_NATIVE_WEB_UI_EVENT_H
#define UMICOM_INCLUDE_UMICOM_FRONTEND_NATIVE_WEB_UI_EVENT_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/frontend/native_web/types.h"
/**
 * Represent the native web ui event data shared with callers of this public contract.
 */
typedef struct UmiNativeWebUiEvent { UmiNativeWebEventKind kind; char target_id[UMI_NATIVE_WEB_ID_CAPACITY]; char name[UMI_NATIVE_WEB_ID_CAPACITY]; char value[UMI_NATIVE_WEB_VALUE_CAPACITY]; uint64_t sequence; } UmiNativeWebUiEvent;
/* Initialise one semantic browser event. */
UmiStatus umi_native_web_ui_event_init(UmiNativeWebUiEvent *event, UmiNativeWebEventKind kind, const char *target_id, const char *name, const char *value, uint64_t sequence);

#ifdef __cplusplus
}
#endif
#endif
