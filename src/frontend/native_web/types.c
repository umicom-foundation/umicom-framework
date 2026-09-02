/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/native_web/types.c
 *
 * PURPOSE:
 *   Define stable capacities, browser transports, event kinds, patch kinds and shared native-web helpers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/frontend/native_web/types.h"

#include <string.h>

/*
 * Provide the native web copy text operation used by this module and its client
 * applications.
 */
UmiStatus umi_native_web_copy_text(char *destination, size_t capacity, const char *source)
{
    size_t length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U || source == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    length = strlen(source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

/*
 * Provide the native web transport text operation used by this module and its client
 * applications.
 */
const char *umi_native_web_transport_text(UmiNativeWebTransportKind kind)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (kind) {
        case UMI_NATIVE_WEB_TRANSPORT_WEBSOCKET: return "websocket";
        case UMI_NATIVE_WEB_TRANSPORT_SSE: return "sse";
        case UMI_NATIVE_WEB_TRANSPORT_POLLING: return "polling";
        default: return "none";
    }
}

