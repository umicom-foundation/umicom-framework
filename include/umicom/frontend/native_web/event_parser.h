/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/native_web/event_parser.h
 *
 * PURPOSE:
 *   Parse the compact browser event protocol into validated semantic UI events.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_INCLUDE_UMICOM_FRONTEND_NATIVE_WEB_EVENT_PARSER_H
#define UMICOM_INCLUDE_UMICOM_FRONTEND_NATIVE_WEB_EVENT_PARSER_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/frontend/native_web/ui_event.h"
/* Parse a compact kind|target|name|value event message emitted by the tiny browser runtime. */
UmiStatus umi_native_web_event_parse(const char *message, uint64_t sequence, UmiNativeWebUiEvent *out_event);

#ifdef __cplusplus
}
#endif
#endif
