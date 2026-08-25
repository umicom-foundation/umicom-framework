/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/native_web/types.h
 *
 * PURPOSE:
 *   Define stable capacities, browser transports, event kinds, patch kinds and shared native-web helpers.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_INCLUDE_UMICOM_FRONTEND_NATIVE_WEB_TYPES_H
#define UMICOM_INCLUDE_UMICOM_FRONTEND_NATIVE_WEB_TYPES_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_NATIVE_WEB_API_VERSION 1U
#define UMI_NATIVE_WEB_ID_CAPACITY 128U
#define UMI_NATIVE_WEB_TEXT_CAPACITY 512U
#define UMI_NATIVE_WEB_VALUE_CAPACITY 1024U
#define UMI_NATIVE_WEB_MARKUP_CAPACITY 16384U
#define UMI_NATIVE_WEB_CSS_CAPACITY 8192U
#define UMI_NATIVE_WEB_MAX_ATTRIBUTES 16U
#define UMI_NATIVE_WEB_MAX_ELEMENTS 256U
#define UMI_NATIVE_WEB_MAX_PATCHES 512U
#define UMI_NATIVE_WEB_MAX_EVENTS 256U
#define UMI_NATIVE_WEB_MAX_RENDERERS 96U
#define UMI_NATIVE_WEB_MAX_ASSETS 32U

typedef enum UmiNativeWebTransportKind {
    UMI_NATIVE_WEB_TRANSPORT_NONE = 0,
    UMI_NATIVE_WEB_TRANSPORT_WEBSOCKET = 1,
    UMI_NATIVE_WEB_TRANSPORT_SSE = 2,
    UMI_NATIVE_WEB_TRANSPORT_POLLING = 3
} UmiNativeWebTransportKind;

typedef enum UmiNativeWebPatchKind {
    UMI_NATIVE_WEB_PATCH_SET_TEXT = 1,
    UMI_NATIVE_WEB_PATCH_SET_ATTRIBUTE = 2,
    UMI_NATIVE_WEB_PATCH_REMOVE_ATTRIBUTE = 3,
    UMI_NATIVE_WEB_PATCH_INSERT_ELEMENT = 4,
    UMI_NATIVE_WEB_PATCH_REMOVE_ELEMENT = 5,
    UMI_NATIVE_WEB_PATCH_REPLACE_HTML = 6,
    UMI_NATIVE_WEB_PATCH_FOCUS = 7
} UmiNativeWebPatchKind;

typedef enum UmiNativeWebEventKind {
    UMI_NATIVE_WEB_EVENT_CLICK = 1,
    UMI_NATIVE_WEB_EVENT_INPUT = 2,
    UMI_NATIVE_WEB_EVENT_CHANGE = 3,
    UMI_NATIVE_WEB_EVENT_KEY = 4,
    UMI_NATIVE_WEB_EVENT_FOCUS = 5,
    UMI_NATIVE_WEB_EVENT_BLUR = 6,
    UMI_NATIVE_WEB_EVENT_DRAG_START = 7,
    UMI_NATIVE_WEB_EVENT_DRAG_OVER = 8,
    UMI_NATIVE_WEB_EVENT_DROP = 9,
    UMI_NATIVE_WEB_EVENT_COMMAND = 10,
    UMI_NATIVE_WEB_EVENT_CONTEXT = 11
} UmiNativeWebEventKind;

typedef enum UmiNativeWebCapabilityFlag {
    UMI_NATIVE_WEB_CAP_DOM_PATCH = 1ULL << 0,
    UMI_NATIVE_WEB_CAP_WEBSOCKET = 1ULL << 1,
    UMI_NATIVE_WEB_CAP_SSE = 1ULL << 2,
    UMI_NATIVE_WEB_CAP_POLLING = 1ULL << 3,
    UMI_NATIVE_WEB_CAP_KEYBOARD = 1ULL << 4,
    UMI_NATIVE_WEB_CAP_POINTER = 1ULL << 5,
    UMI_NATIVE_WEB_CAP_DRAG_DROP = 1ULL << 6,
    UMI_NATIVE_WEB_CAP_ACCESSIBILITY = 1ULL << 7,
    UMI_NATIVE_WEB_CAP_THEME = 1ULL << 8,
    UMI_NATIVE_WEB_CAP_DENSITY = 1ULL << 9,
    UMI_NATIVE_WEB_CAP_RESPONSIVE = 1ULL << 10
} UmiNativeWebCapabilityFlag;

/* Copy a bounded UTF-8 byte string and always NUL terminate on success. */
UmiStatus umi_native_web_copy_text(char *destination, size_t capacity, const char *source);
/* Return the stable wire name for a browser transport. */
const char *umi_native_web_transport_text(UmiNativeWebTransportKind kind);

#ifdef __cplusplus
}
#endif
#endif
