/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/native_web/types.c
 *
 * PURPOSE:
 *   Define stable capacities, browser transports, event kinds, patch kinds and shared native-web helpers.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/frontend/native_web/types.h"

#include <string.h>

UmiStatus umi_native_web_copy_text(char *destination, size_t capacity, const char *source)
{
    size_t length;
    if (destination == NULL || capacity == 0U || source == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    length = strlen(source);
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

const char *umi_native_web_transport_text(UmiNativeWebTransportKind kind)
{
    switch (kind) {
        case UMI_NATIVE_WEB_TRANSPORT_WEBSOCKET: return "websocket";
        case UMI_NATIVE_WEB_TRANSPORT_SSE: return "sse";
        case UMI_NATIVE_WEB_TRANSPORT_POLLING: return "polling";
        default: return "none";
    }
}

