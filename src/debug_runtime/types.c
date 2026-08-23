/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/types.c
 *
 * PURPOSE:
 *   Implement stable DAP runtime diagnostic text.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/types.h"

const char *umi_debug_runtime_message_kind_text(
    UmiDebugRuntimeMessageKind kind)
{
    switch (kind) {
        case UMI_DEBUG_RUNTIME_MESSAGE_REQUEST: return "request";
        case UMI_DEBUG_RUNTIME_MESSAGE_RESPONSE: return "response";
        case UMI_DEBUG_RUNTIME_MESSAGE_EVENT: return "event";
        case UMI_DEBUG_RUNTIME_MESSAGE_UNKNOWN:
        default: return "unknown";
    }
}

const char *umi_debug_runtime_adapter_state_text(
    UmiDebugRuntimeAdapterState state)
{
    switch (state) {
        case UMI_DEBUG_RUNTIME_ADAPTER_STOPPED: return "stopped";
        case UMI_DEBUG_RUNTIME_ADAPTER_STARTING: return "starting";
        case UMI_DEBUG_RUNTIME_ADAPTER_INITIALIZING: return "initializing";
        case UMI_DEBUG_RUNTIME_ADAPTER_READY: return "ready";
        case UMI_DEBUG_RUNTIME_ADAPTER_RUNNING: return "running";
        case UMI_DEBUG_RUNTIME_ADAPTER_PAUSED: return "paused";
        case UMI_DEBUG_RUNTIME_ADAPTER_STOPPING: return "stopping";
        case UMI_DEBUG_RUNTIME_ADAPTER_FAILED: return "failed";
        default: return "unknown";
    }
}
